#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
// map><filename<inode_id,number of blocks >>
map<string,int> inode_pointer;  // mapping of name of file and inode_pointer 
map<int,int> disk_pointer;      // mapping of inode pointer ande disk size 
set<int> inode_number;                     // store the used inode pointers 
set<string> name;               // store the file names 
string root="root/";



// create file 
void create_file(string filename,string content)
{
    name.insert(filename);              // insert file name in name 
    int inode_num;
    while(1)                            // randomly genrate the inode pointer 
    {
        inode_num=rand()%1000;
        if(inode_number.find(inode_num)==inode_number.end())
        {
            inode_number.insert(inode_num);        // insert the inode number 
            break;
        }
    }
    
    inode_pointer.insert({filename,inode_num});     // map inode number and file name 
    string node_name=to_string(inode_num);
    const char *content_c=content.c_str();
    int x;
    x=content.length();
    
    int j=0;
    int i=1;
    
    // divide string in 4 byte chunks and store it in file
    /*
    let file name is abc and inode number is 580 and size of abc is 11B
    files created 
    580_1.txt   4B
    580_2.txt   4B
    580_3.txt   3B
    */
    while(j<x)
    {
        string file_name=root+node_name+"_"+to_string(i)+".txt";
        const char *name=file_name.c_str();
        FILE *f=fopen(name,"w");
        if(j+4<=x)
            fwrite(content_c+j,sizeof(char),4,f);
        else{
            fwrite(content_c+j,sizeof(char),x-j,f);
        }
        fclose(f);
        i++;
        j=j+4;
    }
    disk_pointer.insert({inode_num,i-1});               // insert inode number and num,ber of disk used 
    cout<<"file created :\n";
    cout<<"name :"<<filename<<"\ninode number :"<<inode_num<<"\ndisk size : "<<i-1<<"\n";

}


// rename the file 
void rename_file(string old_name,string new_name)
{
    inode_pointer.insert({new_name,inode_pointer[old_name]});       // insert new name and inode number 
    inode_pointer.erase(old_name);                                  // delete old name entry 
    for(auto x:name)                                                // delete name from set  
    {
        if(x==old_name)
        {
            name.erase(x);
            break;
        }
    }
    name.insert(new_name);                                          // insert name in set

}
void delete_file(string filename)                                   // delete file 
{
    cout<<"deleted filename :"<<filename<<"\n";
    int inode_num=inode_pointer[filename];                      // inode n
    int block_size=disk_pointer[inode_num];
    string del_filename;
    // delete all blocs 
    for(int i=1;i<=block_size;i++)
    {
        del_filename=root+to_string(inode_num)+"_"+to_string(i)+".txt";
        const char *name=del_filename.c_str();
        int status=remove(name);
    }

   const char *name2=filename.c_str();
    for(auto x:name)                                                // delete name from set  
    {
        if(x==name2)
        {
            name.erase(x);
            break;
        }
    }
    {
        cout<<block_size<<" Block deleted\n";
    }
    
    // delete entry from inode pointer 
    inode_pointer.erase(filename);                             // delte name and inode number from inode_pointer 
}
void print_file()                                               // print nfile 
{
    for(auto i:inode_pointer)
    {
        cout<<i.first<<"\t"<<i.second<<"\n";
    }
    cout<<"\n";
}
// display content of file 
void content_display(string filename)                       
{

    int inode_po=inode_pointer[filename];       // find inode number of file 
    int block=disk_pointer[inode_po];           // find num,ber of blocks for the file is avaiolable 

    string output_string="";
    for(int i=1;i<=block;i++)
    {
        string file_name=root+to_string(inode_po)+"_"+to_string(i)+".txt";      // block name 
        const char *name=file_name.c_str();
        
        FILE *f=fopen(name,"r");        // open block
        char output[100];
        fgets(output,100,f);            // read blocks 
        output_string+=output;          // concatenate string 
        fclose(f);                      //close 
    }
    cout<<output_string;
        
    cout<<"\n";
}

// read meta data from file 
void read_meta_data()
{
    cout<<"Meta data reading........... \n";
    FILE *f=fopen("root/meta_data.txt","r");            // open meta_data.txt
    if(!f)  
        return;
    char file_name[1000];
    int inode;
    int size;
    while(!feof(f))
    {   
        // read meta data file 
        fscanf(f,"%s %d %d\n",file_name,&inode,&size);
        // insert filename and inode pointer 
        inode_pointer.insert({file_name,inode});
        // insert inode pointer and number of blocks required 
        disk_pointer.insert({inode,size});
        // insert file name and inode in set 
        inode_number.insert(inode);
        name.insert(file_name);
    }
    fclose(f);
}
void create_meta_data()
{
    // update meta data file 
    FILE *f=fopen("root/meta_data.txt","w");
    for(auto i:inode_pointer)
    {
        const char *file_name=i.first.c_str();
        int inode=i.second;
        int size=disk_pointer[inode];
        fprintf(f,"%s %d %d\n",file_name,inode,size);
    }
    fclose(f);
}

int main()
{
    srand(time(0));
    int d=mkdir("root",0777);                                   // create root directory if not present
    if(d==-1)
    {
        cout<<"Root directory already available :\n";
    }

    read_meta_data();                                           // read meta data from file from meta_data.txt


    string command;                             
    while(1)
    {   
        cout<<"Enter command : ";                   
        getline(std::cin, command);                             // take command from    
        vector<string> token;       
        string word="";
        for(auto i:command)                                     // divide string in tokens 
        {
            if(i==' ')
            {
                token.push_back(word);
                word="";
            }
            else{
                word+=i;
            }
        }
        token.push_back(word);                                 
        
        if(token[0]=="mf")                                      // create new file 
        {
            string file_name="";
            string content="";
            file_name=token[1];
            int bit=0;
            for(auto i:command)                                 // divide file name and input astring 
            {
                if(i=='\"' && bit==0)
                {
                    bit=1;
                    continue;
                }
                else if(i=='\"' && bit==1)
                {
                    bit=0;
                }
                if(bit==1)
                {
                    content+=i;
                }
            }
            if(name.find(file_name)!=name.end())                // seach in map if file id available or not if file is not available create new file 
            {
                cout<<"File already present :\n";
                continue;
            }
            name.insert(file_name);                             // insert file name in map 
            create_file(file_name,content);                     // create file 

            create_meta_data();                                 // update meta_data.txt
        }
        else if(token[0]=="df")                                 // delete file 
        {
            string file_name=token[1];

            if(name.find(file_name)==name.end())                // search file is available or not 
            {
                cout<<"File is not available :\n";
                continue;
            }
            delete_file(file_name);                             // delete file 
            cout<<file_name<<" deleted:\n";
            create_meta_data();                                 // update meta data 
        }
        else if(token[0]=="rf")                                 // rename file 
        {
            if(name.find(token[1])==name.end())                 // find file 
            {
                cout<<"File is not available :\n";
                continue;
            }
            rename_file(token[1],token[2]);                     // rename function 
            cout<<"file renamed :\n";
            create_meta_data();                                    // update meta_data.txt
        }
        else if(token[0]=="pf")                                 // print file data 
        {

            if(name.find(token[1])==name.end())                 // search file is available or not 
            {
                cout<<"File is not available :\n";
                continue;
            }
            content_display(token[1]);                          // display content of file 
        }
        else if(token[0]=="ls")                                 // show all files avauilable 
        {
            print_file();
        }       
        else if(token[0]=="end")
        {
            break;
        }
        
        
    }
    create_meta_data();                             // update meta data 
}