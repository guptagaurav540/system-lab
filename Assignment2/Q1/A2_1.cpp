#include<bits/stdc++.h>
#include<fstream>

using namespace std;

vector<int>cost(3,0);
vector<int>a_room(3,0);                              //available rooms
int requests=0;
vector<char>r_room;                                  //requested rooms
vector<int>earnings(3,0);                            //earning done by each manager
vector<int>denied(3,0);                              //requests denied
vector<int>money_wasted_v;                           //stores loss due to rejection of genuine request


void aman()
{

    vector<int>rooms(a_room.begin(),a_room.end());   //stores currently available rooms
    vector<char>room_assigned(requests,'N');         //keeps track of room assigned for each request. N indicates no room assigned
    int earning = 0;
    int doublee=0,triple=0;                          //doublee stores the numer of double rooms alloted triple stores the numbner of triple rooms alloted to double or single occupancy requested rooms
    int rejected_requests=0;                         //stores the number of genuine rejected requests
    for(int i=0;i<r_room.size();i++)
    {
        //check if triple occupancy room present or not
        if(rooms[2]!=0)
            {
                rooms[2]--;                             //decrement #available triple occupancy rooms
                room_assigned[i] = 'T';
                if(r_room[i]=='S')
                {
                    triple++;                           //increment as triple occupancy room alloted for single occupancy request 
                    earning+=cost[0];
                }
                else if(r_room[i]=='D')
                {
                    triple++;                           //increment as triple occupancy room alloted for double occupancy request
                    earning+=cost[1];
                }
                else 
                    earning+=cost[2];
            }
            else if(r_room[i] == 'S')
                {
                    //check if single occupancy rooms available
                    if(rooms[0]!=0)
                        {
                            rooms[0]--;                 //decrement #single occupancy room
                            room_assigned[i] == 'S';
                            earning+=cost[0];
                        }
                        else if(rooms[1]!=0)
                        {
                            rooms[1]--;                 //decrement #double occupancy room
                            room_assigned[i] = 'D';
                            earning += cost[0];
                            doublee++;
                        }
               }
               else if(r_room[i] == 'D')
               {
                   //check if double occupancy room available or not
                   if(rooms[1]!=0)
                        {
                            rooms[1]--;
                            room_assigned[i] = 'D';
                            earning += cost[1];
                        }
                        else if(rooms[1]==0 && doublee>0)       
                        {
                            //we enter this loop if double occupancy room is currently unavailble 
                            //as a double occupancy room has been alloted for a single occupancy room request
                            doublee--;          
                            rejected_requests++;
                            money_wasted_v[0] += 9000;
                        }
               }
               else if(r_room[i] == 'T' && rooms[2]==0 && triple>0)
               {
                   //we enter this loop if triple occupancy room is currently unavailable 
                   //as triple occupancy room has been alloted for a double or single occupancy room request
                   triple--;
                   rejected_requests++;
                   money_wasted_v[0] += 12500;
               }
                

    }
    // for(int i=0;i<room_assigned.size();i++)
    //     cout<<room_assigned[i]<<" ";
    // cout<<endl;
    // cout<<"earnings = "<<earning<<endl;
    earnings[0] = earning;
    denied[0] = rejected_requests;
}

void raj()
{
    vector<int>rooms(a_room.begin(),a_room.end());    //maintains a copy of currently available rooms
    vector<char>room_assigned(requests,'N');          //keeps track of room assigned for each request. N indicates no room assigned
    int earning = 0;
    int single=0,doublee=0,triple=0;                  //doublee stores count of double occupancy room alloted for single occupancy reques
    int rejected_requests=0;
    for(int i=0;i<r_room.size();i++)
    {
        
        if(r_room[i] == 'S')
        {
            //checks if triple occupancy room available
            if(rooms[2] != 0)
               {
                    rooms[2]--;
                    room_assigned[i] = 'T';
                    earning += cost[0];
                    triple++;
               }
            else if(rooms[1] !=0)
            {
                //checks if double occupancy room available 
                rooms[1]--;
                room_assigned[i] = 'D';
                earning += cost[0];
                doublee++;
            }
            else if(rooms[0] != 0)
            {
                //checks if triple occupancy room available
                rooms[0]--;
                room_assigned[i] = 'S';
                earning += cost[0];
            }
        }
        else if(r_room[i] == 'D')
        {
            //checks if triple occupancy room available 
            if(rooms[2] != 0)
               {
                    rooms[2]--;
                    room_assigned[i] = 'T';
                    earning += cost[1];
                    triple++;
               }
               //checks if double occupancy room available
            else if(rooms[1] !=0)
            {
                rooms[1]--;
                room_assigned[i] = 'D';
                earning += cost[1];
            }
            //checks if double occupancy not available due to allocation for single occupancy 
            else if(rooms[1] == 0 && doublee>0)
            {
                doublee--;
                rejected_requests++;
                money_wasted_v[1] += 9000;
            }

        }
        else if(r_room[i] == 'T')
        {
            //checks if triple occupancy room avialable 
            if(rooms[2] != 0)
               {
                    rooms[2]--;
                    room_assigned[i] = 'T';
                    earning += cost[2];
               }
               //checks if triple occupancy room not available due to allotment for single/double allotment request
               else if(rooms[2] == 0 && triple>0)
               {
                   triple--;
                   rejected_requests++;
                   money_wasted_v[1] += 12500;
               }
        }
    }
    // for(int i=0;i<room_assigned.size();i++)
    //     cout<<room_assigned[i]<<" ";
    // cout<<endl;
    // cout<<"earnings = "<<earning<<endl;
    earnings[1] = earning;
    denied[1] = rejected_requests;
}


void alok()
{
    vector<int>rooms(a_room.begin(),a_room.end()); //maintains a copy of currently available rooms
    vector<char>room_assigned(requests,'N');         //keeps track of room assigned for each request. N indicates no room assigned
    int earning = 0;
    int single=0,doublee=0,triple=0;
    int rejected_requests=0;
    for(int i=0;i<r_room.size();i++)
    {
        if(r_room[i] == 'S')
        {
            //checks if single occupancy available or not
            if(rooms[0] != 0)
               {
                   rooms[0]--;
                   room_assigned[i] = 'S';
                   earning += cost[0];
                    
               }
               //checks if double occupancy available
            else if(rooms[1] !=0)
            {
                rooms[1]--;
                room_assigned[i] = 'D';
                earning += cost[0];
                doublee++;
            }
            //checks if triple occupancy available
            else if(rooms[2] != 0)
            {
                    rooms[2]--;
                    room_assigned[i] = 'T';
                    earning += cost[0];
                    triple++;
            }
        }
        else if(r_room[i] == 'D')
        {
            //checks if double occupancy available
            if(rooms[1] != 0)
               {
                   rooms[1]--;
                   room_assigned[i] = 'D';
                   earning += cost[1];
               }
               //checks if triple occupancy availble
            else if(rooms[2] !=0)
            {
                   rooms[2]--;
                   room_assigned[i] = 'T';
                   earning += cost[1];
                   triple++;
            }
          //checks if double occupancy not available due to its allotment to single occupancy
            else if(rooms[1] == 0 && doublee>0)
            {
                doublee--;
                rejected_requests++;
                money_wasted_v[2] += 9000;
            }
        }
        else if(r_room[i] == 'T')
        {
            //checks if triple occupacy avialable 
            if(rooms[2] != 0)
               {
                    rooms[2]--;
                    room_assigned[i] = 'T';
                    earning += cost[2];
               }
               //checks if triple occupancy not availble due to allotment to single/double
               else if(rooms[2] == 0 && triple>0)
               {
                   triple--;
                   rejected_requests++;
                   money_wasted_v[2] += 12500;
               }
    
        }
        
    }
    // for(int i=0;i<room_assigned.size();i++)
    //     cout<<room_assigned[i]<<" ";
    // cout<<endl;
    // cout<<"earnings = "<<earning<<endl;
    earnings[2] = earning;
    denied[2] = rejected_requests;
}


void write_to_file()
{
    //writes to output.txt file
    FILE *fpointer = fopen("output.txt","w");
    fprintf(fpointer,"-----------------------OUTPUT FILE------------------------------------------\n\n");
    fprintf(fpointer,"Performance of Aman = %d\n",earnings[0]);
    fprintf(fpointer,"Performance of Raj  = %d\n",earnings[1]);
    fprintf(fpointer,"Performance of Alok = %d\n\n",earnings[2]);
    fprintf(fpointer,"-----------------------------------------------------------------------\n\n");
    if(earnings[0]>earnings[1] && earnings[0]>earnings[2])
        fprintf(fpointer,"Best manager is : Aman\n\n");
    else if(earnings[1]>earnings[2] && earnings[1]>earnings[2])
        fprintf(fpointer,"Best manager is : Raj\n\n");
    else if(earnings[2]>earnings[0] && earnings[2]>earnings[1])
        fprintf(fpointer,"Best manager is : Alok\n\n");
    else if(earnings[0] == earnings[1] && earnings[0]>earnings[2])
        fprintf(fpointer,"Best manager is : Aman or Raj\n\n");
    else if(earnings[0] == earnings[2] && earnings[0]>earnings[1])
        fprintf(fpointer,"Best manager is : Aman or Alok\n\n");
    else if(earnings[2] == earnings[1] && earnings[1]>earnings[0])
        fprintf(fpointer,"Best manager is : Raj or Alok\n\n");
    else 
        fprintf(fpointer,"Best manager is : Aman or Raj or Alok\n\n");

    fprintf(fpointer,"\n---------------------------------------------------------------\n");

    fprintf(fpointer,"Money wasted by Aman = %d\n",money_wasted_v[0]);
    fprintf(fpointer,"Money wasted by Raj  = %d\n",money_wasted_v[1]);
    fprintf(fpointer,"Money wasted by Alok = %d\n",money_wasted_v[2]);

    fprintf(fpointer,"\n---------------------------------------------------------------\n");

    fprintf(fpointer,"Requests denied by Aman = %d\n",denied[0]);
    fprintf(fpointer,"Requests denied by Raj  = %d\n",denied[1]);
    fprintf(fpointer,"Requests denied by Alok = %d\n",denied[2]);

    fprintf(fpointer,"\n-------------------------END FILE-------------------------------\n");

    

    fclose(fpointer);

}

void best_manager()
{
    //finds best manager
    if(earnings[0]>earnings[1] && earnings[0]>earnings[2])
        cout<<"Best manager is : Aman"<<endl;
    else if(earnings[1]>earnings[2] && earnings[1]>earnings[2])
        cout<<"Best manager is : Raj"<<endl;
    else if(earnings[2]>earnings[0] && earnings[2]>earnings[1])
        cout<<"Best manager is : Alok"<<endl;
    else if(earnings[0] == earnings[1] && earnings[0]>earnings[2])
        cout<<"Best manager is : Aman or Raj"<<endl;
    else if(earnings[0] == earnings[2] && earnings[0]>earnings[1])
        cout<<"Best manager is : Aman or Alok"<<endl;
    else if(earnings[2] == earnings[1] && earnings[1]>earnings[0])
        cout<<"Best manager is : Raj or Alok"<<endl;
    else 
        cout<<"Best manager is : Aman or Raj or Alok"<<endl;
    cout<<endl;
}

void indi_performance()
{
    //prints individual performance(Earnings) of each manager
    cout<<"Performance of Aman = "<<earnings[0]<<endl;
    cout<<"Performance of Raj  = "<<earnings[1]<<endl;
    cout<<"Performance of Alok = "<<earnings[2]<<endl;
    cout<<endl;
}

void money_wasted()
{
    //prints amount of money wasted by each manager
    cout<<"Money wasted by Aman = "<<money_wasted_v[0]<<endl;
    cout<<"Money wasted by Raj  = "<<money_wasted_v[1]<<endl;
    cout<<"Money wasted by Alok = "<<money_wasted_v[2]<<endl;
    cout<<endl;
}


void requests_denied()
{
    //prints #genuine requests denied by user
    cout<<"Requests denied by Aman = "<<denied[0]<<endl;
    cout<<"Requests denied by Raj  = "<<denied[1]<<endl;
    cout<<"Requests denied by Alok = "<<denied[2]<<endl;
    cout<<endl;
}


int main()
{
    cost[0] = 5000;
    cost[1] = 9000;
    cost[2] = 12500;

    string dat;
    ifstream fptr;

    fptr.open("input.txt");
    //Reading input file
    if(!fptr)
        cout<<"Could not open file !!"<<endl;
    else
    {
        int testcases = 1;
        int r;
        cout<<"\n---------------------------------------------------------------------------------------\n\n";
        while(getline(fptr,dat))
        {
            if(dat == " ")break;
            
            cout<<"Testcase #"<<testcases<<"\n\n";
            testcases++; 
            istringstream iss(dat);
            int i=0;
            while(iss >> r)
            {
                a_room[i++] = r;
            }
            getline(fptr,dat);
            istringstream is(dat);
            is >> requests;
            getline(fptr,dat);
            istringstream  sp(dat);
            char c;
            while(sp >> c)
            {
                r_room.push_back(c);
            }
            for(int t=0;t<=2;t++)
                money_wasted_v.push_back(0);
            
            aman();
            raj();
            alok();
            best_manager();
            indi_performance();
            money_wasted();
            requests_denied();
            write_to_file();
            r_room.clear();
            money_wasted_v.clear();
            cout<<"\n---------------------------------------------------------------------------------------\n\n";
        }
    }


    return 0;
}


