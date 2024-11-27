// Andrew Ramsey
// rams0040
// cs105

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h> // Required for sleep (was throwing errors without)

#define BUFFER_SIZE 15

char buffer[BUFFER_SIZE]; // Creates buffer with 15 positions
int in_index = 0; // Producer will use to write chars
int out_index = 0; // Consumer will use to read chars

sem_t empty;        // Counts empty slots
sem_t full;         // Counts full slots
sem_t buffer_lock;  // Binary semaphore to sync processes

void* producer(void* arg) 
{
    char newChar;
    FILE* fp;

    fp = fopen("mytest.dat", "r"); // Given code to open the file, also throws error if file isn't opened
    if (!fp) 
    {
        perror("Error opening file");
        return NULL;
    }

    while (fscanf(fp, "%c", &newChar) != EOF) // Continue to read characters until end of file
    {
        sem_wait(&empty); // Wait for an empty slot
        sem_wait(&buffer_lock); // Lock buffer

        // Adds character to buffer
        buffer[in_index] = newChar; 
        in_index = (in_index + 1) % BUFFER_SIZE; // Modulus because the buffer is circular, when index reaches last pos it wraps back to 0

        sem_post(&buffer_lock); // Unlock the buffer
        sem_post(&full);        // Signal a full slot
    }

    // After placing last char in buffer, puts * in buffer (signals end)
    sem_wait(&empty);
    sem_wait(&buffer_lock);
    buffer[in_index] = '*';
    in_index = (in_index + 1) % BUFFER_SIZE;
    sem_post(&buffer_lock);
    sem_post(&full);

    fclose(fp);
    return NULL;
}

void* consumer(void* arg) 
{
    char ch;

    while (1) // Infinite loop, not using while(ch!='*') since ch needs to be synchronized as below and it would break anyway
    {
        sem_wait(&full);        // Wait for a full slot
        sem_wait(&buffer_lock); // Lock the buffer

        // Remove ch from buffer
        ch = buffer[out_index];
        out_index = (out_index + 1) % BUFFER_SIZE;

        sem_post(&buffer_lock); // Unlock the buffer
        sem_post(&empty);       // Signal an empty slot

        if (ch == '*') 
        {
            break;
        }

        printf("%c", ch);
        fflush(stdout);
        sleep(1); // One second sleep so consumer runs slower than producer
    }

    return NULL;
}

int main() {
    // Initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&buffer_lock, 0, 1); // Binary semaphore 

    pthread_t producer_thread;
    pthread_t consumer_thread;
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Clean up semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&buffer_lock);

    printf("\nEnd of simulation.\n");
    return 0;
}
