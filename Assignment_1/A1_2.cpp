#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<iostream>
#include<semaphore.h>
#include<time.h>
#include<map>
#include<queue>
using namespace std;

sem_t write_semaphore,read_semaphore,helper_sem;    // semaphore 
pthread_mutex_t mutex;                              // mutex for read count increment ans decrement 
int read_count=0;                                   // number of reader currently reading
map <int,int> operation; // priority queue which store the operation <token_number,operation_type>

pthread_t w_thread;                     //write thread 
int *token;             

pthread_t *r_thread;                    // read thread
map<int,int> token_digits;              // map user id vs token number 
     

// read operation
void *read_card(void *arg)
{
    int user_id=*((int *) arg);         // user id (friend id)
    pthread_mutex_lock(&mutex);         // mutex lock for increasing the read count 
    if(read_count==0)                   // check read count is 0 if 0 menas it is first reader do wait for write semaphore 
        sem_wait(&write_semaphore);     // write semaphore is lock means someone is writing so wait till writing is not completed
    read_count++;                       // increment the read count 
    pthread_mutex_unlock(&mutex);       // unlock the mutex 
    /*reading starts */
    //critical section
    cout<<"User Id : "<<user_id<<" reading"<<endl;
    sleep(2);
    //coming out of critical section
  
    /*reading complete */

    pthread_mutex_lock(&mutex);         // lock mutex for decreasing the read count 
        read_count--;                   // decerase the read count 
    if(read_count==0)                   // if read count is 0 menas so any one can write 
        sem_post(&write_semaphore);     // unlock the write semaphore 
    pthread_mutex_unlock(&mutex);       // unlock the mutex 
}


// write operation 
void *write_card(void *arg)
{

    int user_id=*((int *) arg);    // user id 
    sem_wait(&write_semaphore);     // waiting for write semaphore if semaphore is available lock otherwise wait 
    /*writing opeartion starts */
    
    cout<<"User Id : "<<user_id<<" Writing \n";
    // critical section 
    sleep(2);
  
    cout<<"User Id : "<<user_id<<" Writing completed\n";
    /*writing complete */
    
    sem_post(&write_semaphore);     // unlock semaphore 
}




// handle the request
/*
helper function remove a operation from priority queue and create a threade for exection
we are using priority queue so that the friend which have minimum token number can read or write first

*/ 
void *helper(void *arg)
{

    while(1)
    {
        // (operation is a priority queue which contain the operation sequence )
        // opearion size > 0 means there is some opeartion which is waiting on queue
        if(operation.size()>0)          // oeration is a queue which store the requests 
        {
            auto x=operation.begin();   // pick the one who have lowest tag 
            if(x->second==0)            // operation type 0 menas write 
            {
                // write operation  
                if(read_count==0)       // if read count is 0 means a friend can write 
                {
                    sem_wait(&helper_sem);  
                    // create a write thread 
                    pthread_create(&w_thread,NULL,write_card,&token_digits[x->first]);
                    
                    // delete the operation from priority queue which is completed 
                    operation.erase(x->first);
                    sem_post(&helper_sem);
                }
            }            
            else
            {
                // read operation 
                int value;
                sem_getvalue(&write_semaphore,&value);      // get the write semaphore value 
                if(value==1)                                // value 1 means any one can read 
                {
                    pthread_create(&r_thread[token_digits[x->first]],NULL,read_card,&token_digits[x->first]);
                    // delete the operation from priority queue which is completed 
                    operation.erase(x->first);    
                }
            }
        }
    }
}


// count the number of user reading 
void *count_print(void *arg)
{
    while(1){
    cout<<"Read count :";
    cout<<read_count<<"\n";
    sleep(3);
    }
}


// operation genrate 
// this function genrate type of process for users and insert in priority queue
void *operation_genrator(void *i)
{
    int n = *((int *) i);           // number of friends 
   // int n=10;
    int x=0;
    while(1)
    {
        int user_id=rand()%(n);        // genrate random user id (0 to n-1);
        int operation_type=rand()%2;   // genrate random operation type 0 menas write 1 means read 
       
        if(operation.count(token[user_id])==0){     // if friend already make a request or not if friend already make a request then no dublicate request will inserted 
            operation[token[user_id]]=operation_type;
             x++;
            sleep(1);
        }   
        if(x==100){
            sleep(3);
            x=0;
        }
    }
}

int main()
{
    // input taken for number of friend 
    cout<<"Enter the number of friends: ";
    int *n=new int();                           // n is number of friend 
    cin>>*n;                    
    
    token=new int[*n];                          // array storen token number of ith friend 
    r_thread=new pthread_t[*n];                 // read thread array
    
    sem_init(&write_semaphore,0,1);             // initialization of write semaphore (Binary)
    sem_init(&read_semaphore,0,*n);             // initialization of read semaphore 
    sem_init(&helper_sem,0,1);                  // helper semaphore 
    pthread_mutex_init(&mutex,NULL);            


    srand(time(0));             
    // random unique three digit token number genration  
    for(int i = 0; i<*n; i++){
        int x=(rand()%900)+100;
        if(token_digits.count(x)!=0)
        {
            i--;
            continue;
        }
        token_digits[x]=i;
        token[i]=x;
    } 

    pthread_t operation_create_thread_id;
    // thread creation
    // this thread create operations and type of operation read or write 
    pthread_create(&operation_create_thread_id,NULL,operation_genrator,n);
    
    // helper thread used for ordering the sequence of operations 
    pthread_t temp;
    pthread_create(&temp,NULL, helper,NULL);


    pthread_t count_th;
    pthread_create(&count_th,NULL,count_print,NULL);
    
    
    pthread_join(count_th,NULL);

    // join threads (waiting for completion)
    pthread_join(operation_create_thread_id,NULL);
    pthread_join(temp,NULL);
    for(int i=0;i<*n;i++)
        pthread_join(r_thread[i],NULL);
    pthread_join(w_thread,NULL);
     
    return 0;

}
