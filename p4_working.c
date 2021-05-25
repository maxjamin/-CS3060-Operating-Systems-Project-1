#include <stdio.h>
#include<stdlib.h>
#include<pthread.h>

/* Created by: Ben Smith
 * Class: CS3060 X01 summer 2021 B1
 * Assignment: Project 4
 */

#define variables_buffer_size 10
#define factors_buffer_size 10 

typedef struct Number_of_factors 
{
	int base_number;
	int prime_factors[factors_buffer_size];
	int sizeof_array;

} Number_of_factors;

pthread_mutex_t variables;
pthread_mutex_t factors;

pthread_cond_t empty_variable_buffer;
pthread_cond_t full_variable_buffer;
pthread_cond_t empty_factors_buffer;
pthread_cond_t full_factors_buffer;

int number_of_variables;
int place_in_vars;

int place_in_args;

int *array_variables; //for producer
Number_of_factors  *array_factors; //for consumer

void consumer()
{

}

int insert_item(int item)
{
	pthread_mutex_lock(&variables);
	if(place_in_vars == variables_buffer_size-1)
		return -1;
	array_variables[place_in_vars] = item;
	place_in_vars = (place_in_vars + 1) % variables_buffer_size;
	pthread_mutex_unlock(&variables);

}

int remove_top_item()
{
	pthread_mutex_lock(&variables);
	if(place_in_vars < 0)
		return -1;
	array_variables[place_in_vars] = 0;
	place_in_vars = (place_in_vars - 1) % variables_buffer_size;
	pthread_mutex_unlock(&variables);
}

void *producer(void *ptr)
{
	while(1)
	{


		/*/printf("producer waiting for full buffer\n");
		pthread_mutex_lock(&variables);
		pthread_cond_wait(&full_variable_buffer, &variables);
		pthread_mutex_unlock(&variables);*/

		//printf("producer Reading buffer\n");

		while(place_in_vars >= 0)
		{
			pthread_mutex_lock(&variables);
			pthread_cond_wait(&full_variable_buffer, &variables);
			printf("Value: %d place: %d \n", array_variables[place_in_vars], place_in_vars );
			pthread_mutex_unlock(&variables);
			remove_top_item();
		}

		pthread_mutex_lock(&variables);
		pthread_cond_signal(&empty_variable_buffer);
		pthread_mutex_unlock(&variables);
	}
}

int main(int argc, char *argv[])
{
	array_variables = calloc(1, sizeof(int)*variables_buffer_size);
	array_factors = calloc(1, sizeof(Number_of_factors)
					*factors_buffer_size);

	pthread_t child;
	pthread_mutex_init(&variables, NULL);
	pthread_mutex_init(&factors, NULL);

	pthread_cond_init(&empty_variable_buffer, NULL);
	pthread_cond_init(&full_variable_buffer, NULL);

	pthread_create(   &child
					, NULL
					, producer
					, NULL);

	place_in_vars = 0;
	place_in_args = 1;

	printf("Main: (0) ");
	sleep(2);

	while(place_in_args < argc)
	{
		printf("Main: (0) \n");

		pthread_mutex_lock(&variables);
		//printf("Main waiting for empty buffer\n");
		if(place_in_args != 1)
			pthread_cond_wait(&empty_variable_buffer, &variables);
		pthread_mutex_unlock(&variables);


		while((place_in_args != argc))
		{
			printf("Main: (1) %d\n", atoi(argv[place_in_args]));
			if(insert_item(atoi(argv[place_in_args])) != -1)
				break;

			place_in_args++;
		}
		//printf("test\n");
		pthread_mutex_lock(&variables);
		pthread_cond_signal(&full_variable_buffer); //signal to producer that array_variables is ready;
		pthread_mutex_unlock(&variables);
		//printf("test end\n");
	}



	pthread_join(child, NULL);
}
