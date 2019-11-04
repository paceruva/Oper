#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
//#include <syscall.h>

int EMPLEADO = 0;
int HACKER = 1;

pthread_mutex_t mutex;
pthread_cond_t	hacker_queue;
pthread_cond_t	empleado_queue;
pthread_cond_t	waiting_to_row;

//sem_t impresion;
// sem_t hackerQueue;
// sem_t serfQueue;
// pthread_barrier_t barrier;
int waiting_hackers = 0;
int assigned_hackers = 0;
int waiting_empleados = 0;
int assigned_empleados = 0;
int in_boat = 0;
int botes = 0;

// void board(int ran) {
//   sem_wait(&impresion);
//   printf("El proceso con tid = %d es un %s y abordó el bote.\n", syscall(SYS_gettid), (ran==0?"hacker":"serf"));
//   sem_post(&impresion);
// }

void board(int tipo)
{
	if(tipo == HACKER)
	{
		printf("Hacker Boarding\n");
	}
	else
	{
		printf("Empleado Boarding\n");
	}
}
// void rowBoat() {
//   printf("El proceso con tid = %d es es el capitan del bote %d.\n\n", syscall(SYS_gettid), botes);
//   ++botes;
// }

void row_boat()
{
	printf("~~Rowing boat~~\n");
}

void hacker()
{
	pthread_mutex_lock(&mutex);
		++waiting_hackers;

		while(assigned_hackers == 0)
		{
			if(waiting_hackers >= 2 && in_boat + assigned_hackers + assigned_empleados < 4)
			{
				waiting_hackers -= 2;
				assigned_hackers += 2;
				pthread_cond_signal(&hacker_queue);
			}
			else
			{
				pthread_cond_wait(&hacker_queue, &mutex);
			}
		}

		--assigned_hackers;
		board(HACKER);
		++in_boat;
		if(in_boat == 4)
		{
			pthread_cond_broadcast(&waiting_to_row);
			row_boat();
			in_boat = 0;
			pthread_cond_signal(&hacker_queue);
			pthread_cond_signal(&hacker_queue);
			pthread_cond_signal(&empleado_queue);
			pthread_cond_signal(&empleado_queue);
		}
		else
		{
			pthread_cond_wait(&waiting_to_row, &mutex);
		}
	pthread_mutex_unlock(&mutex);
}

void empleado()
{
	pthread_mutex_lock(&mutex);
		++waiting_empleados;

		while(assigned_empleados == 0)
		{
			if(waiting_empleados >= 2 && in_boat + assigned_empleados + assigned_empleados < 4)
			{
				waiting_empleados -= 2;
				assigned_empleados += 2;
				pthread_cond_signal(&empleado_queue);
			}
			else
			{
				pthread_cond_wait(&empleado_queue, &mutex);
			}
		}

		--assigned_empleados;
		board(EMPLEADO);
		++in_boat;
		if(in_boat == 4)
		{
			pthread_cond_broadcast(&waiting_to_row);
			row_boat();
			in_boat = 0;
			pthread_cond_signal(&empleado_queue);
			pthread_cond_signal(&empleado_queue);
			pthread_cond_signal(&hacker_queue);
			pthread_cond_signal(&hacker_queue);
		}
		else
		{
			pthread_cond_wait(&waiting_to_row, &mutex);
		}
	pthread_mutex_unlock(&mutex);
}

int main()
{
	srand(time(NULL));
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&hacker_queue, NULL);
	pthread_cond_init(&waiting_to_row, NULL);

	int people_count = 100;
	pthread_t thread_vector[people_count];
	int random = 0;
	for(int current = 0; current < people_count; ++current)
	{
		random = rand() % 2;

		if(random == 0)
		{	
			pthread_create(&thread_vector[current], NULL, (void*)hacker, NULL);
		}
		else
		{
			pthread_create(&thread_vector[current], NULL, (void*)empleado, NULL);
		}
	}

	for(int current = 0; current < people_count; current++)
	{
		pthread_join(thread_vector[current], NULL);
	}
}
