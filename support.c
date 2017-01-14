#include<pthread.h>

#include<unistd.h>

#include<stdio.h>

#include<stdlib.h>

pthread_mutex_tx,wsem;

pthread_ttid;

intreadcount;

voidintialize()

{

        pthread_mutex_init(&x,NULL);

        pthread_mutex_init(&wsem,NULL);

        readcount=0;

}

void * reader (void * param)

{

        intwaittime;

        waittime = rand() % 5;

        printf("\nReader is trying to enter");

        pthread_mutex_lock(&x);

        readcount++;

        if(readcount==1)

                pthread_mutex_lock(&wsem);

        printf("\n%d Reader is inside ",readcount);

        pthread_mutex_unlock(&x);

        sleep(waittime);

        pthread_mutex_lock(&x);

        readcount--;

        if(readcount==0)

                pthread_mutex_unlock(&wsem);

        pthread_mutex_unlock(&x);

        printf("\nReader is Leaving");

}

void * writer (void * param)

{

        intwaittime;

        waittime=rand() % 3;

        printf("\nWriter is trying to enter");

        pthread_mutex_lock(&wsem);

        printf("\nWrite has entered");

        sleep(waittime);

        pthread_mutex_unlock(&wsem);

        printf("\nWriter is leaving");

        sleep(30);

        exit(0);

}
