#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <syscall.h>

sem_t mutex;
sem_t impresion;
sem_t hackerQueue;
sem_t serfQueue;
pthread_barrier_t barrier;
int hackersCount;
int serfsCount;
int ran;
int botes = 0;

void board() {
  sem_wait(&impresion);
  printf("El proceso con tid = %d es un %s y abordó el bote.\n", syscall(SYS_gettid), (ran==0?"hacker":"serf"));
  sem_post(&impresion);
}

void rowBoat() {
  printf("El proceso con tid = %d es es el capitan del bote %d.\n\n", syscall(SYS_gettid), botes);
  ++botes;
}

void hackers() {
  int capitan = 0;
  sem_wait(&mutex);
  hackersCount++;
  if(hackersCount == 4) {
    sem_post(&hackerQueue);
    sem_post(&hackerQueue);
    sem_post(&hackerQueue);
    sem_post(&hackerQueue);
    hackersCount = 0;
    capitan = 1;
  } else {
    if (hackersCount == 2 && serfsCount == 2) {
    sem_post(&hackerQueue);
    sem_post(&hackerQueue);
    sem_post(&serfQueue);
    sem_post(&serfQueue);
    serfsCount -= 2;
    hackersCount = 0;
    capitan = 1;
    } else {
      sem_post(&mutex);
    }
  }
  sem_wait(&hackerQueue);

  board();
  pthread_barrier_wait(&barrier);

  if(capitan) {
    rowBoat();
    sem_post(&mutex);
  }
}

void serfs() {
  int capitan = 0;
  sem_wait(&mutex);
  serfsCount++;
  if(serfsCount == 4) {
    sem_post(&serfQueue);
    sem_post(&serfQueue);
    sem_post(&serfQueue);
    sem_post(&serfQueue);
    serfsCount = 0;
    capitan = 1;
  } else {
    if (serfsCount == 2 && hackersCount == 2) {
    sem_post(&serfQueue);
    sem_post(&serfQueue);
    sem_post(&hackerQueue);
    sem_post(&hackerQueue);
    hackersCount -= 2;
    serfsCount = 0;
    capitan = 1;
    } else {
      sem_post(&mutex);
    }
  }
  sem_wait(&serfQueue);

  board();
  pthread_barrier_wait(&barrier);

  if(capitan) {
    rowBoat();
    sem_post(&mutex);
  }
}

int main() {
  srand(time(NULL));
  sem_init(&mutex, 0, 1);
  sem_init(&impresion, 0, 1);
  sem_init(&hackerQueue, 0, 0);
  sem_init(&serfQueue, 0, 0);
  pthread_barrier_init (&barrier, NULL, 4);
  while(botes < 10) {
    ran = rand()%2;
    void (*funciones[])() = {&hackers, &serfs};
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, funciones[ran], NULL);
  }
}
