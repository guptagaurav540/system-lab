#include<vector>
#include<string>
#include<iostream>	
#include<fstream>
#include<sstream>
#include<semaphore.h>
#include<pthread.h>
#include<map>
#include<unistd.h>


using namespace std;

//NOTE**
//here process is analogous to parties(P0-P(n-1))
//and resources are analogous to items (plate,bowl,cutlery,etc.)

sem_t resources;
pthread_mutex_t lock;
int N,M;

vector<int>available;                  //stores the available resources
vector<vector<int>>allocated;          //stores the resources allocated to each process
vector<vector<int>>max_need;           //stores the maximum requirement of each process
vector<int>safe;                       //stores the safe sequence
vector<vector<int>>c_need;             //stores the current need of each process
vector<bool>done;                      //indicates whether execution of process is done or not
map<int,int>mp;                        //mapping of safe sequence and process                       

bool read_data()
{
    //this function reads the input.txt file
    string dat;
    ifstream fptr;
    int x;
    fptr.open("input.txt");
    if(!fptr)
        {
            cout<<"Could not open file !!"<<endl;
            return false;
        }
    else
    {
        getline(fptr,dat);
        istringstream iss(dat);
        cout<<"\n\n";
        int k=0;
        while(iss >> x)
        {
            if(k==0)
                N = x;
            else M = x;
            k++;
        }
        getline(fptr,dat);
        istringstream avl(dat);
        while(avl >> x)
        {
            available.push_back(x);
        }
        for(int j=0;j<N;j++)
        {
            getline(fptr,dat);
            istringstream all(dat);
            vector<int>v;
            while(all >> x)
            {
                 v.push_back(x);
            }
            allocated.push_back(v);
        }
        for(int j=0;j<N;j++)
        {
            getline(fptr,dat);
            istringstream need(dat);
            vector<int>v;
            while(need >> x)
            {
                 v.push_back(x);
            }
            max_need.push_back(v);
        }
        return true;
    }
}

int allocation_possible(vector<int>temp_available,vector<vector<int>>current_need,vector<bool>processed)
{
    //this function checks if there is a process 'i' whose excecution is not completed yet and for which resource allocation is possible
    //i.e. finds a process i for which resource requirement <= available
    //this is done by looping from 0 to N-1

    int i=0;
    for(int i=0;i<N;i++)
    {
        if(processed[i])
        {
            for(int j=0;j<M;j++)
            {
                if(current_need[i][j] > temp_available[j])    //allocation not possible        
                    break;
                if(j == M-1)                                 //allocation possible
                    return i;
                
            }

        }
    }
    return -1;
}

bool safe_sequence()
{
    //this function return true if safe sequence else returns false

    vector<vector<int>> current_need;     //stores the current need of resources of each process
    vector<bool> processed(N,true);       //stores processing info of each process
    vector<int>temp_available(available.begin(),available.end());
    int t=0;
    while(true)
    {
        //calculating current need for each process
        for(int row=0;row<N;row++)
        {
            vector<int>v;
            for(int col=0;col<M;col++)
            {
                v.push_back(max_need[row][col] - allocated[row][col]);
            }
            current_need.push_back(v);
        }
    
        //find process for which allocation of resource  is possible
        int is = allocation_possible(temp_available,current_need,processed);

        if(is != -1)
        {
            for(int i=0;i<M;i++)
            {
                temp_available[i] += allocated[is][i];
                /*allocated[is][i] = 0;
                current_need[is][i] = 0;*/
            }
            processed[is] = false;
            safe.push_back(is+1);
            mp.insert({is,t});
            t++;
        }
        else
        {
            // is==-1 thus further allocation is not possible with available resources
            // check if processing for any process is still left
            // if yes then safe sequence not possible 
            bool flag = true;
            for(int i=0;i<N;i++)
            {
                if(processed[i] == true)
                {
                    flag = false;
                    break;
                }
            }
            return flag;
        }
    }
}

void *allocate_resource(void *arg)
{
    int i = *((int *) arg);
   
    i--; 
    bool flag = true;
    while(flag)
    {
        pthread_mutex_lock(&lock);
        //mutex here is used to check if the execution of the process before the current process is done or not
        bool check = true;
        if(mp[i]!=0 && done[mp[i]-1]==false)
        {
            check = false;
        }
        else if(mp[i] == 0 && done[mp[i]]==false)
            check = true;
        if(check == true)
            flag = false;
        else
            {
               //if previous process safe sequence if not done then unlock mutex
                pthread_mutex_unlock(&lock);
            }
    }
    flag = true;
    done[mp[i]] = true;
            
    sem_wait(&resources);  
    //semaphore here is used to access resources
    //access the critical section
    cout<<"\n-->Process "<<i+1<<"\n";
    cout<<"Allocated : ";
    for(int j=0;j<M;j++)
    {
        cout<<allocated[i][j]<<" ";
    }
    cout<<"\n";
    cout<<"Needed    : ";
    for(int j=0;j<M;j++)
    {
        cout<<c_need[i][j]<<" ";
    }
    cout<<"\n";
    cout<<"Available : ";
    for(int j=0;j<M;j++)
    {
        cout<<available[j]<<" ";
    }
    cout<<"\n";
    cout<<"Resource Allocated..\n";
    cout<<"Resource Released...\n";
    //incrementing available resources
    for(int j=0;j<M;j++)
    {
        available[j]+=allocated[i][j];
    }
    cout<<"Now Available : ";
    for(int j=0;j<M;j++)
    {
        cout<<available[j]<<" ";
    }
    cout<<endl;

    //exit critical section
    cout<<"Exited process "<<i+1<<endl;
    sem_post(&resources);
    pthread_mutex_unlock(&lock);           
    //sleep(10);
 }


int main()
{
    sem_init(&resources,0,1);
    //reading input  file 
    bool read = read_data();    
    if(!read)
        return 0;         
    if(safe_sequence())
    {
        //safe sequence found
        cout<<"Safe Sequence Found : ";
        for(int i=0;i<safe.size();i++)
            cout<<safe[i]<<" ";
        cout<<"\n";
    }
    else
    {
        //safe sequence not found
        cout<<"Safe Sequence not found\n\n"<<endl;
        return 0;
    }
    pthread_t pro[N];
    //calculating need for each proceses
    for(int row=0;row<N;row++)
        {
            vector<int>v;
            for(int col=0;col<M;col++)
            {
                v.push_back(max_need[row][col] - allocated[row][col]);
            }
            c_need.push_back(v);
        }
    
    for(int i=0;i<N;i++)
        done.push_back(false);

    //creating threads for each process
    for(int i=0;i<N;i++)
    {
        pthread_create(&pro[safe[i]-1],NULL,allocate_resource,&safe[i]);
    }
    //waiting for all threads to finish
    for(int i=0;i<N;i++)
    {
        pthread_join(pro[safe[i]-1],NULL);
    }

    cout<<"\nAll Processes Finished\n\n";

    pthread_mutex_destroy(&lock);
    sem_destroy(&resources);
    
    return 0;
}



/*
testcases

5 3
1 1 2
0 1 0
2 0 0
3 0 2
2 1 1
0 0 2
7 5 3
3 2 2
9 0 2
2 2 2
4 3 3



5 3
0 0 0
1 1 2
2 1 2
4 0 1
0 2 0
1 1 2
4 3 3
3 2 2
9 0 2
7 5 3
1 1 2
*/

