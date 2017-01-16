#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <string>
using namespace std;

int read_count = 0;
pthread_mutex_t lock_mutex;
pthread_cond_t readerWait,readerEntryWait;
sem_t* bufferSem;
int isWrite = 0, DataBuffer;

void *reader( void *process_number );
void *writer( void *process_number );
double times( double prob, double mean );

int main() {
	srand( time( NULL ) );
	int r = 0, w = 0, produce_r = 1, produce_w = 1, count = 0;
	double mean_r = 1, mean_w = 3, defer_r = 0, defer_w = 0;
	void *temp;
	string order = "";
	vector<pthread_t> readers, writers;
	bufferSem = sem_open( "/semBuffer", O_CREAT, 0644, 1 );
	sem_init( bufferSem, 0, 1 );
	readers.push_back( 0 );
	while( 1 ){
		produce_r += (int)round( times( (double)(rand()%1001)/1000, mean_r ) );
		for( int a = r; a < produce_r; a++ ) readers.push_back( a );
		for( ;r < produce_r; r++ ){
			// create and start a reader and check if it was successful
			pthread_create( &readers[r], NULL, reader, &r );

			// ensure that the reader thread is not killed when the main thread completes
			pthread_detach( readers[r] );
		}

		produce_w += (int)round( times( (double)(rand()%3001)/1000, mean_w ) );
		for( int a = w; a < produce_w; a++ ) writers.push_back( a );
		for( ;w < produce_w; w++ ){
			// create and start a write process and check if it was successful
			pthread_create( &writers[w], NULL, writer, &w );

			// ensure that the writer thread is not killed when the main thread completes
			pthread_detach( writers[w] );
		}

		sleep( 1 );
		if( ( ++count )%10 == 0 ){
			sleep( 3 );
			cout << "Enter exit to terminate" << endl;
			cin >> order;
			if( order == "exit" ) return 0;
		}
		//printf( "%d\n", count );
	}

	pthread_exit( 0 ); // thread safe exit of main thread
}

void *reader( void *process_number ) {
	int number = *( int* ) process_number;
	printf( "Reader %d entered.\n", number, read_count );

	pthread_mutex_lock(&lock_mutex);
	
	while( isWrite ){
		printf( "Reader %d is waiting for writer...\n", number );
		pthread_cond_wait( &readerWait, &lock_mutex );
	}
	
	while( read_count >= 3 ){
		printf( "Reader %d is waiting fo avaliable entry...\n", number );
		pthread_cond_wait( &readerEntryWait, &lock_mutex );
	}
	
	if( read_count == 0 ) sem_wait( bufferSem );
	
	read_count++;
	pthread_mutex_unlock( &lock_mutex ); // release the lock
	
	// simulate reading of resource
	
	printf( "Reader %d started reading\n", number );
	
	sleep( 1 );
	
	printf( "Reader %d finished reading\n",number );
	pthread_mutex_lock( &lock_mutex ); // acquire the lock

	// decrement read count and release read write lock if this is the last reader
	read_count--;
	
	pthread_cond_broadcast( &readerEntryWait );
	
	if ( read_count == 0 ) sem_post( bufferSem );
	pthread_mutex_unlock( &lock_mutex ); // release the lock

	printf( "Reader %d exited\n", number );
	pthread_exit( NULL ); // thread safe exit

	return NULL;
}

void *writer( void *process_number ) {
	int number = *( int* ) process_number;
	
	printf( "Writer %d entered.\n", number );
	printf( "Writer %d waiting.\n", number );
	isWrite++;
	sem_wait( bufferSem );
	int print = ++DataBuffer;
	
	// simulate writing to resource
	
	sleep( 1 );

	printf( "Writer %d updated to %d\n", number, print );
	isWrite--;
	
	pthread_cond_broadcast( &readerWait );
	
	printf( "Writer %d exited\n", number );
	sem_post( bufferSem );

	pthread_exit( NULL ); // thread safe exit

	return NULL;
}

double times( double prob, double mean ) {
	return log( ( double )( prob/mean ) ) / ( mean*-1 );
}
