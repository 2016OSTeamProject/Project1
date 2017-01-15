#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <string>
using namespace std;

int read_count = 0;
pthread_mutex_t read_write_mutex, lock_mutex;

void *reader(void *process_number);
void *writer(void *process_number);
double poisson( int k, double mean );
double exponential( int k, double mean );
double times( double prob, double mean );
double density( int k, double mean );

int main() {
	srand( time( NULL ) );
	int r = 0, w = -1, produce_r = 1, produce_w = 0, count = 0, temp;
	double mean_r = 2, mean_w = 1, defer_r = 0, defer_w = 0;
	string order = "";
	vector<pthread_t> readers, writers;
	readers.push_back( 0 );
	while( order != "exit" ){

		if( r != 0 ){
			produce_r += (int)times( (double)(rand()%1000)/1000, mean_r );
		}
		for( int a = r; a < produce_r; a++ ) readers.push_back( a );
		if ( r >= 0 ){
			for( ;r < produce_r; r++ ){
				// create and start a reader and check if it was successful
				pthread_create(&readers[r], NULL, reader, &r);

				// ensure that the reader thread is not killed when the main thread completes
				pthread_detach(readers[r]);
				count++;
				if( count%10 == 0 ){
					cout << "Enter exit to terminate" << endl;
					cin >> order;
				}
			}
		}

		produce_w += (int)times( (double)(rand()%1000)/1000, mean_w );
		if( produce_w > 0 && w < 0 ) w++;
		for( int a = w; a < produce_w; a++ ) writers.push_back( a );
		if ( w >= 0 ){
			for( ;w < produce_w; w++ ){
				// create and start a write process and check if it was successful
				pthread_create(&writers[w], NULL, writer, &w);

				// ensure that the writer thread is not killed when the main thread completes
				pthread_detach(writers[w]);
				count++;
				if( count%10 == 0 ){
					cout << "Enter exit to terminate" << endl;
					cin >> order;
				}
			}
		}

		sleep( 1 );

	}

	pthread_exit(0); // thread safe exit of main thread
}

void *reader(void *process_number) {
	int *number = (int*) process_number;
	// acquire the lock
	pthread_mutex_lock(&lock_mutex);

	// increment the read count can obtain read write lock if this is the first reader
	read_count++;
	if (read_count == 1)
		pthread_mutex_lock(&read_write_mutex);

	pthread_mutex_unlock(&lock_mutex); // release the lock

	// simulate reading of resource
	printf("Reader #%d readed\n", *number);
	printf("Reader #%d exited\n", *number);

	pthread_mutex_lock(&lock_mutex); // acquire the lock

	// decrement read count and release read write lock if this is the last reader
	read_count--;
	if (read_count == 0) {
		pthread_mutex_unlock(&read_write_mutex);
	}

	pthread_mutex_unlock(&lock_mutex); // release the lock
	pthread_exit(NULL); // thread safe exit

	return NULL;
}

void *writer(void *process_number) {
	int *number = (int*) process_number;
	printf("Writer #%d waiting\n", *number);

	pthread_mutex_lock(&read_write_mutex); // acquire the read write lock

	// simulate writing to resource
	printf("Writer #%d updated\n", *number);
	printf("Writer #%d exited\n", *number);

	pthread_mutex_unlock(&read_write_mutex); // release the read write lock

	pthread_exit(NULL); // thread safe exit

	return NULL;
}

double poisson( int k, double mean ) {
	double p = exp(-mean);
	p *= pow( mean, ( double )k );
	for ( int i=0 ; i<k ; i++ ) p /= (i+1);
	return p;
}

double exponential( int k, double mean ) {
	return mean * exp( (-1) * mean * k );
}

double times( double prob, double mean ) {
	return log( ( double )( prob/mean ) ) / ( mean*-1 );
}

double density( int k, double mean ) {
	return ( double )exp( ( double )k*-1 / mean ) / mean;
}
