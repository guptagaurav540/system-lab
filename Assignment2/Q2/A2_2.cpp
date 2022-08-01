#include<bits/stdc++.h>

using namespace std;

vector<int>subjects(3,0);   //stores #subjects available per course index 0 is for BTech,1 for MTech3 for Phd
vector<int>tray_size(3,0);  //stores tray size for each course type 
vector<string>request;      //stores request sequence
vector<string>trayB;        //tray for course B.Tech
vector<string>trayM;        //tray for course M.Tech
vector<string>trayP;        //tray for course Phd
vector<int>search_dep(3,0); //stores #times book depo is visited for each course
vector<int>lfu;
vector<int>BSubjects;       //stores count of request made for each subject in course BTech
vector<int>MSubjects;       //stores count of request made for each subject in course MTech
vector<int>PSubjects;       //stores count of request made for each subject in course Phd
vector<string>q3;           //stores result of question 3 
FILE *fpointer;


bool read_input()
{
    string dat;
    ifstream fptr;

    fptr.open("issue.txt");
    //Reading input file
    if(!fptr)
       {
         cout<<"Could not open file !!"<<endl;
         return false;
       }
    else
    {
        int testcases = 1;
        int r;
        cout<<"\n---------------------------------------------------------------------------------------\n\n";
        //while(getline(fptr,dat))
        //{
            getline(fptr,dat);

            // if(dat == " ")
            //     return true;
            
            //cout<<"Testcase #"<<testcases<<"\n\n";
            //testcases++; 
            istringstream iss(dat);
            int i=0;
            while(iss >> r)
            {
                subjects[i++] = r;
            }
            getline(fptr,dat);
            istringstream is(dat);
            i=0;
            while(is >> r)
            {
                tray_size[i++] = r;
            }
            getline(fptr,dat);
            istringstream sp(dat);
            string str="";
            while(sp >> str)
            {
                request.push_back(str);
            }


            cout<<"Subjects per Course : ";
            for(int i=0;i<subjects.size();i++)
            {
                cout<<subjects[i]<<" ";
            }
            cout<<"\nTray size : ";
            for(int i=0;i<tray_size.size();i++)
                cout<<tray_size[i]<<" ";
            cout<<"\nRequests : ";
            for(int i=0;i<request.size();i++)
                cout<<request[i]<<" ";
            cout<<"\n\n";
            cout<<"----------------------------------------------------------------------------------\n";
            fptr.close();

}
return true;
}

void initialize_subjects()
{
    //initializing subject vectors
    for(int i=0;i<subjects[0];i++)
        BSubjects.push_back(0);
    for(int i=0;i<subjects[1];i++)
        MSubjects.push_back(0);
    for(int i=0;i<subjects[2];i++)
        PSubjects.push_back(0);
    //initializing lfu vector
    for(int i=0;i<subjects[1];i++)
        lfu.push_back(0);

}

void issueBTech(string subject)
{
    //First In First Out
    //check if book is available in tray 
    if( trayB.end() != find (trayB.begin(),trayB.end(), subject))
    {
        trayB.erase(find (trayB.begin(),trayB.end(), subject));
    }
    else
    {
        search_dep[0]++;                    //as book not available increment search_depo value incremented
        if(trayB.size()==tray_size[0])
            trayB.erase(trayB.begin());     //remove the book which entereed the tray first
        trayB.push_back(subject);           //insert the new book into the tray
    }

}

void issueMTech(string subject)
{
    //Least Frequently Used
    lfu[subject[1]-'1']++;                               //increment the frequency of the requested book
    //check if element present in the tray
    if( trayM.end() != find (trayM.begin(),trayM.end(), subject))
    {
        trayM.erase(find(trayM.begin(),trayM.end(), subject));  
    }
    else
    {
        search_dep[1]++;                                //increment search value
        if(trayM.size() == tray_size[1])
            {
                //find the element with least frequncy
                int mini=INT_MAX,min_index=0,loc;
                for(int j=0;j<trayM.size();j++)
                {
                    if(mini < (lfu[trayM[j][1]-'1']))     //compare the frequency of all the elements present in the tray
                        {
                            mini = lfu[trayM[j][1]-'1'];
                            min_index = trayM[j][1]-'1';
                            loc = j;                      //stores tray index of the book with minimum frequency
                        }
                }
                trayM[loc] = subject;                     //replacing the least frequency book with the requested one
            }
        else
            trayM.push_back(subject);                       //insert the new requested book in the tray
    }
}

void issuePhd(string subject)
{
    //Least Recently Used
    //search if element present in the tray
    if( trayP.end() != find (trayP.begin(),trayP.end(), subject))
    {
        trayP.erase(find (trayP.begin(),trayP.end(), subject));
    }
    else
    {
        search_dep[2]++;
        //check if tray is full
        if(trayP.size() == tray_size[2])
            trayP.erase(trayP.begin());         //first book will be least recently used one as we are inserting the latest one at the last
        trayP.push_back(subject);               //insert new element in the tray
    }
}

void print_tray(vector<string>trayname)
{
    //Print tray element
    for(int i=0;i<trayname.size();i++)
        cout<<trayname[i]<<" ";
    cout<<"\n\n";
}

void issue()
{
    //this function read the requested book and calls the respective function
    for(int i=0;i<request.size();i++)
    {
        if(request[i][0] == 'B')
            {
                if(request[i][1]-'0' <= subjects[0]) 
                {
                    BSubjects[request[i][1]-'1']++;
                    issueBTech(request[i]);
                }
                else{
                    //invalid subject
                    cout<<"Subject "<<request[i]<<" is invalid\n\n";
                }
            }
        else if(request[i][0] == 'M')
            {
                if(request[i][1]-'0' <= subjects[1]) 
                {
                    MSubjects[request[i][1]-'1']++;
                    issueMTech(request[i]);
                }
                else cout<<"Subject "<<request[i]<<" is invalid\n\n";
            }
        else if(request[i][0] == 'P')
            {
                if(request[i][1]-'0' <= subjects[2])
                {
                PSubjects[request[i][1]-'1']++;
                issuePhd(request[i]);
                }
            else cout<<"Subject "<<request[i]<<" is invalid\n\n";
            }
    }
}

int find_max(vector<int> CourseSubject)
{
    //find the subject which was requested the most for each course
    int max_index = 0,max = INT_MIN;
    for(int i=0;i<CourseSubject.size();i++)
    {
        if(max < CourseSubject[i])
        {
            max = CourseSubject[i];
            max_index = i;
        }
    }
    return max_index+1;
}

void book_in_tray(vector<string>trayC,vector<int>CourseSubject)
{
    //finds book_id and subject_id of the books in respective tray
    for(int i=0;i<trayC.size();i++)
    {
        int x =  CourseSubject[trayC[i][1]-'1']+1;
        char c = '0' + x;
        string str = trayC[i] + '_' + c;
        string s = "Book_id = "+str+" Subject_id = "+trayC[i];
        //cout<<"s = "<<s<<endl;
        cout<<"Book_id = "<<str<<" Subject_id = "<<trayC[i]<<endl;
        q3.push_back(s);
    }
    cout<<endl;
}

void compare()
{
    //this function prints the output
    fpointer = fopen("output2.txt","w");
    fprintf(fpointer,"-----------------------OUTPUT FILE----------------------------------\n\n");
    //book depo searches
    cout<<"Book Depo visits for BTech book issual = "<<search_dep[0]<<endl;
    cout<<"Book Depo visits for MTech book issual = "<<search_dep[1]<<endl;
    cout<<"Book Depo visits for Phd   book issual = "<<search_dep[2]<<endl;
    cout<<"\nNumber of times the librarian goes to search entire book depo = "<<search_dep[0]+search_dep[1]+search_dep[2]<<"\n\n";
    cout<<"\n----------------------------------------------------------------------------------\n";
    
    fprintf(fpointer,"Book Depo visits for BTech book issual = %d \n",search_dep[0]);
    fprintf(fpointer,"Book Depo visits for MTech book issual = %d \n",search_dep[1]);
    fprintf(fpointer,"Book Depo visits for Phd   book issual = %d \n",search_dep[2]);
    fprintf(fpointer,"Number of times the librarian goes to search entire book depo = %d\n",search_dep[0]+search_dep[1]+search_dep[2]);

    fprintf(fpointer,"-----------------------------------------------------------------------\n\n");

    cout<<"\n";

    //max subject request for each course
    cout<<"BTech maximum request for subject = B"<<find_max(BSubjects)<<endl;
    cout<<"MTech maximum request for subject = M"<<find_max(MSubjects)<<endl;
    cout<<"Phd   maximum request for subject = P"<<find_max(PSubjects)<<endl;
    cout<<"\n----------------------------------------------------------------------------------\n";
    

    fprintf(fpointer,"BTech maximum request for subject = B%d \n",find_max(BSubjects));
    fprintf(fpointer,"MTech maximum request for subject = M%d \n",find_max(MSubjects));
    fprintf(fpointer,"Phd   maximum request for subject = P%d \n",find_max(PSubjects));
    fprintf(fpointer,"-----------------------------------------------------------------------\n\n");

    cout<<"\n";

    //books left in tray
    cout<<"Books left in BTech Tray\n\n";
    book_in_tray(trayB,BSubjects);
    cout<<"\n----------------------------------------------------------------------------------\n";
    cout<<"Books left in MTech Tray\n\n";
    book_in_tray(trayM,MSubjects);
    cout<<"\n----------------------------------------------------------------------------------\n";
    cout<<"Books left in Phd Tray\n\n";
    book_in_tray(trayP,PSubjects);
    cout<<"\n----------------------------------------------------------------------------------\n";
    cout<<endl;

    fprintf(fpointer,"Books left in BTech Tray\n\n");
    int j=0;
    for(int j=0;j<trayB.size();j++)
        {for(int k=0;k<q3[j].size();k++)
        fprintf(fpointer,"%c",q3[j][k]);
        fprintf(fpointer,"\n");}
        fprintf(fpointer,"\n");
        fprintf(fpointer,"-----------------------------------------------------------------------\n\n");

    fprintf(fpointer,"Books left in MTech Tray\n\n");
    for(j;j<trayB.size()+trayM.size();j++)
    {for(int k=0;k<q3[j].size();k++)
        fprintf(fpointer,"%c",q3[j][k]);
        fprintf(fpointer,"\n");}
    fprintf(fpointer,"\n");
    fprintf(fpointer,"-----------------------------------------------------------------------\n\n");

    fprintf(fpointer,"Books left in Phd Tray\n\n");
    for(j;j<trayB.size()+trayM.size()+trayP.size();j++)
    {for(int k=0;k<q3[j].size();k++)
        fprintf(fpointer,"%c",q3[j][k]);
        fprintf(fpointer,"\n");}
    fprintf(fpointer,"\n");
    fprintf(fpointer,"----------------------------END FILE---------------------------------------\n");

    fclose(fpointer);

}



int main()
{
    
    if(!read_input())
    return 0;
    initialize_subjects();
    issue();
    compare();

    return 0;


}


// input format
// first line number of subject in each course
// second line number of books per tray
// subject id requests