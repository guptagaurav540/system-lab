#include<bits/stdc++.h>
#include<time.h>

using namespace std;

vector<int>GChart;          //to store Gantt Chart
vector<int>burst;           //to store burst time
int N_Dishes;               //to store number of dishes

class Dishes{
    public:
        int Pid;
        int Arrival_time;
        int Burst_time;
        int Finish_time;
        void print_dish_details();
        void calculate_avgWaitingTime();
        void print_Gantt_Chart();
}p[1000];


void Dishes::print_Gantt_Chart()
{
    //this function prints the Gnatt Chart
    cout<<"Gantt Chart : \n";
    int t=0;
    int min_arr_time=INT_MAX;
    for(int i=0;i<N_Dishes;i++)
    {
        min_arr_time = min(min_arr_time,p[i].Arrival_time);
    }
    t = min_arr_time;
    for(int i=0;i<GChart.size();i++)
    {
        cout<<"|P"<<GChart[i]<<"("<<t;
        while(i<GChart.size() && GChart[i]==GChart[i+1])
        {
            i++;t++;
        }  
        t++; 
        cout<<"-"<<t<<")";
    }
    cout<<"|\n\n";
    cout<<"-----------------------------------------------------------------------------------------------------\n\n";
}

void Dishes::calculate_avgWaitingTime()
{
    // This function performs the task of scheduling the ordered dishes 
    // using "SHORTEST REMAINING TIME FIRST ALGORITHM"

    int i=0,shortest_burst_time=INT_MAX,shortest_job=0,time=0;

    //shortest_burst_time keeps track of shortest burst time after each time slot
    //shotest_job stores the smallest job id of the job with shortest_burst_time
    bool flag = false;
    int complete=0;
    while(complete != N_Dishes)
    {
        for(int i=0;i<N_Dishes;i++)
        {
            if(p[i].Arrival_time<=time && p[i].Burst_time<=shortest_burst_time && p[i].Burst_time>0)
            {
                if(p[i].Burst_time == shortest_burst_time)
                {
                    //breaking tie w.r.t arrival time 
                    if(p[i].Arrival_time<p[shortest_job].Arrival_time)  
                    {
                        shortest_job = i;
                        shortest_burst_time = p[i].Burst_time;
                    }
                    else if(p[i].Arrival_time == p[shortest_job].Arrival_time)
                    {
                        //breaking tie w.r.t id if arrival time is same
                        if(p[i].Pid<p[shortest_job].Pid)
                        {
                            shortest_job = i;
                            shortest_burst_time = p[i].Burst_time;
                        }
                        
                    }
                }
                else
                {
                    shortest_job = i;
                    shortest_burst_time = p[i].Burst_time;
                }
                flag = true;
        }
        }

        time++;
        if(!flag)
            continue;
        else
        {
            GChart.push_back(shortest_job);
            //decrement burst time of the shortest job
            p[shortest_job].Burst_time--;
            shortest_burst_time = p[shortest_job].Burst_time;
            if(shortest_burst_time == 0)
               { 
                    cout<<"Completed Execution of job "<<p[shortest_job].Pid<<" at time = "<<time<<endl;
                    shortest_burst_time = INT_MAX;
                    complete++;
                    p[shortest_job].Finish_time = time;
                    flag = false;
                    //set flag back to false so as to ensure that if no other process are present then we can simply continue
               }
               
        }
        
    }
}

void Dishes::print_dish_details()
{
    //This function is used to print the process details and average waiting time
    //like process id , arrival time, burst time, finish time , waiting time..
    //waiting time = finish_time-arrival_time-burst_time

    double avg_waiting_time=0;
    cout<<"\n\n";
    cout<<"-------------------------------------------------------\n";
    cout<<"id \t arr \t burst \t finish\t waiting time"<<endl;
    cout<<"-------------------------------------------------------\n";
        for(int i=0;i<N_Dishes;i++)
        {
            cout<<p[i].Pid<<" \t "<<p[i].Arrival_time<<" \t "<<burst[i]<<" \t "<<p[i].Finish_time<<" \t "<<p[i].Finish_time-p[i].Arrival_time-burst[i]<<endl;
            avg_waiting_time+=p[i].Finish_time-p[i].Arrival_time-burst[i];       
        }
        cout<<"\n\n";
        avg_waiting_time/=N_Dishes;
        cout<<"Average Waiting Time = "<<avg_waiting_time<<"\n\n";

}

int main()
{
    Dishes pro; 
    int time,i;
    srand(10);
    int testcases=0;

    string dat;
    ifstream fptr;

    fptr.open("arrival.txt");
    //Reading input file
    if(!fptr)
        cout<<"Could not open file !!"<<endl;
    else
    {
        testcases=1;
        while(getline(fptr,dat))
        {
            if(dat == " ")break;
            istringstream iss(dat);
            N_Dishes = 0;
            i=0;
                 cout<<"Testcase #"<<testcases<<"\n\n";
                testcases++; 
                while(iss >> time)
                {
                    N_Dishes++;
                    p[i].Pid = i;
                    p[i].Arrival_time = time;
                    int bur = (rand()%8)+1;
                    p[i].Burst_time = bur;
                    burst.push_back(bur);
                    i++;
                }
            cout<<"Number of dishes = "<<N_Dishes<<"\n\n";
            //calculate average waiting time
            pro.calculate_avgWaitingTime();
            //printing dish details
            pro.print_dish_details();
            //printing Gantt Chart
            pro.print_Gantt_Chart();
            
            GChart.clear();
            burst.clear();
        }
    }
    return 0;  
}


























