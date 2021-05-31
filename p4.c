#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

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

int counter_in_factors;
int place_in_factors;
int place_out_factors;

int counter_in_vars;
int place_in_vars;
int place_out_vars;

int place_in_args;
int sizeof_input;

int *array_variables; //for producer
Number_of_factors  *array_factors; //for consumer

void *consumer(void *ptr)
{
	while(counter_in_factors > 0 || counter_in_vars > 0)
	{

		pthread_mutex_lock(&factors); //lock_mutex

			//check if buffer is empty
			if(counter_in_factors == 0)
				pthread_cond_wait(&full_factors_buffer, &factors);

			printf("%d: ",  array_factors[place_out_factors].base_number);
			for(int array_index = 0; array_index < array_factors[place_out_factors].sizeof_array; array_index++)
			{
				printf("%d ", array_factors[place_out_factors].prime_factors[array_index] );
			}
			printf("\n");

			place_out_factors = (place_out_factors + 1) % factors_buffer_size;
			counter_in_factors--;
			pthread_cond_signal(&empty_factors_buffer);

		pthread_mutex_unlock(&factors); //unlock_mutex

	}

	return 0;
}

int trial_divison(void *number_of_factors)
{
	if(number_of_factors)
	{
		Number_of_factors * ptr = (struct Number_of_factors *)
									number_of_factors;	
		int factor = 2;
		int number = ptr->base_number;
		while(number > 1)
		{
			if(number % factor == 0)
			{
				(*ptr).prime_factors[(*ptr).sizeof_array] = factor;
				(*ptr).sizeof_array++;

				number /= factor;
			}
			else
				factor++;
		}
		return 0;
	}
}

void *producer(void *ptr)
{
	while(counter_in_vars > 0 || place_in_args < sizeof_input) {

		pthread_mutex_lock(&variables); //lock mutex
			//check if buffer is empty
			if(counter_in_vars == 0)
				pthread_cond_wait(&full_variable_buffer, &variables);

			/************************************************************************/
			pthread_mutex_lock(&factors); //lock_mutex

				//check if buffer is full
				if(counter_in_factors == factors_buffer_size-1)
					pthread_cond_wait(&empty_factors_buffer, &factors);

				//add values to array_factors for consumer
				array_factors[place_in_factors].base_number = array_variables[place_out_vars];
				array_factors[place_in_factors].sizeof_array = 0;

				//factor and add to sublist in array_factor strut
				trial_divison(&array_factors[place_in_factors]);
				place_in_factors = (place_in_factors + 1) % factors_buffer_size;
				counter_in_factors++;

				//signal that something has been added to the buffer
				pthread_cond_signal(&full_factors_buffer);
			pthread_mutex_unlock(&factors); //lock_mutex
			/*************************************************************************/


			place_out_vars = (place_out_vars + 1) % variables_buffer_size;
			counter_in_vars--;
			pthread_cond_signal(&empty_variable_buffer);
		pthread_mutex_unlock(&variables); //unlock mutex

	}
	return 0;
}

int main(int argc, char *argv[])
{
	array_variables = calloc(1, sizeof(int)*variables_buffer_size);
	array_factors = calloc(1, sizeof(Number_of_factors)
					*factors_buffer_size);

	if(argc <= 1)
	{
		printf("Usage:./p4 <number to factor>...\n");
		return 1;
	}

	pthread_t child, consumer_child;
	pthread_mutex_init(&variables, NULL);
	pthread_mutex_init(&factors, NULL);

	pthread_cond_init(&empty_variable_buffer, NULL);
	pthread_cond_init(&empty_factors_buffer, NULL);

	pthread_cond_init(&full_variable_buffer, NULL);
	pthread_cond_init(&full_factors_buffer, NULL);

	pthread_create(   &child 			// create producer thread.
					, NULL
					, producer
					, NULL);

	pthread_create(   &consumer_child 	// create consumer thread.
					, NULL
					, consumer
					, NULL);

	place_in_vars = 0;
	place_out_vars = 0;

	place_out_factors = 0;
	place_in_factors = 0;

	place_in_args = 1;
	counter_in_vars = 0;
	counter_in_factors = 0;
	sizeof_input = argc;

	while(place_in_args < argc)
	{
		pthread_mutex_lock(&variables); //lock_mutex
		
			//check if buffer is full
			while(counter_in_vars == variables_buffer_size-1)
				pthread_cond_wait(&empty_variable_buffer, &variables);

			array_variables[place_in_vars] = atoi(argv[place_in_args]);
			place_in_vars = (place_in_vars + 1) % variables_buffer_size;

			counter_in_vars++;
			place_in_args++;

			//signal that something has been added to the buffer
			pthread_cond_signal(&full_variable_buffer);

		pthread_mutex_unlock(&variables); //unlock_mutex
	
	}

	pthread_join(consumer_child, NULL);
	pthread_join(child, NULL);
}
