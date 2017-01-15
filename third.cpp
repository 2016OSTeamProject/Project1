#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <pthread.h>
#include <vector>
using namespace std;

int read_count = 0;
pthread_mutex_t read_write_mutex, lock_mutex;

void *reader(void *process_number);
void *writer(void *process_number);
double poisson( int k, double mean );

int main() {
        srand( time( NULL ) );
        int r, w, produce_r = 0, produce_w = 0;
        double mean_r = 2, mean_w = 0.2;
        vector<pthread_t> readers, writers;
        while( 1 ){

                // loop to create and start the reader and writer process
                for ( r = 0; r < produce_r; r++) {
                        // create and start a reader and check if it was successful
                        pthread_create(&readers[r], NULL, reader, &r);

                        pthread_detach(readers[r]); // ensure that the reader thread is not killed when the main thread completes

                        // start a writer process for every 5th reader process
                }
                for ( w = 0; w < produce_w; w++ ) {
                        // create and start a write process and check if it was successful
                        pthread_create(&writers[w], NULL, writer, &w);

                        // ensure that the writer thread is not killed when the main thread completes
                        pthread_detach(writers[w++]);
                }

        }

        pthread_exit(0); // thread safe exit of main thread
}

void *reader(void *process_number) {
        // acquire the lock
        pthread_mutex_lock(&lock_mutex);

        // increment the read count can obtain read write lock if this is the first reader
        read_count++;
        if (read_count == 1)
                pthread_mutex_lock(&read_write_mutex);

        pthread_mutex_unlock(&lock_mutex); // release the lock

        // simulate reading of resource
        printf("Reader #%d readed\n", process_number);
        printf("Reader #%d exited\n", process_number);

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
        printf("Writer #%d waiting\n", process_number);

        pthread_mutex_lock(&read_write_mutex); // acquire the read write lock

        // simulate writing to resource
        printf("Writer #%d updated\n", process_number);
        printf("Writer #%d exited\n", process_number);

        pthread_mutex_unlock(&read_write_mutex); // release the read write lock

        pthread_exit(NULL); // thread safe exit

        return NULL;
}

double poisson( int k, double mean ) {
        double p = exp(-mean);
        p *= pow( mean, k );
        for ( int i=0 ; i<k ; i++ ) p /= (i+1);
        return p;
}
