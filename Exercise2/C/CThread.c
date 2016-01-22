#include<stdio.h>
#include<pthread.h>


pthread_mutex_t lock;

int i = 0;

void* threadFunctionOne()
{

  pthread_mutex_lock(&lock);

  int k = 0;

  for (k = 0; k < 1000000; k++)
  {
    
    i++;
  }

  pthread_mutex_unlock(&lock);

  return NULL;

}


void* threadFunctionTwo()
{

  pthread_mutex_lock(&lock);
  int k = 0;

  for (k = 0; k < 1000000; k++)
  {
    i--;
  }

  pthread_mutex_unlock(&lock);

  return NULL;

}


int main()
{
  pthread_t threadOne, threadTwo;
  pthread_create(&threadOne, NULL, threadFunctionOne, NULL);
  pthread_create(&threadTwo, NULL, threadFunctionTwo, NULL);

  pthread_join(threadOne, NULL);
  pthread_join(threadTwo, NULL);

  pthread_mutex_destroy(&lock);
  printf("%d\n", i);

  return 0;

}
