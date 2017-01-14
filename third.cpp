#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <vector>
//#include <poisson_distribution>
using namespace std;

int read_count = 0;
pthread_mutex_t read_write_mutex, lock_mutex;

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

int main() {
        int r, w;
        vector<pthread_t> readers, writers;
        /*
        // loop to create and start the reader and writer process
        for ( r = 0, w = 0; r < 10; r++) {
                // create and start a reader and check if it was successful
                pthread_create(&readers[r], NULL, reader, &r);

                pthread_detach(readers[r]); // ensure that the reader thread is not killed when the main thread completes

                // start a writer process for every 5th reader process
                if (r % 5 == 0) {
                        // create and start a write process and check if it was successful
                        pthread_create(&writers[w], NULL, writer, &w);

                        // ensure that the writer thread is not killed when the main thread completes
                        pthread_detach(writers[w++]);
                }
        }*/

        pthread_exit(0); // thread safe exit of main thread
}
