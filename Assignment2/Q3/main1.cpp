#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
// map><filename<inode_id,number of blocks >>
map<string,int> inode_pointer;
map<int,int> disk_pointer;
set<int> inode_number;
set<string> name;
string root="root/";
void create_file(string filename,string content)
{
    name.insert(filename);
    int inode_num;
    while(1)
    {
        inode_num=rand()%1000;
        if(inode_number.find(inode_num)==inode_number.end())
        {
            inode_number.insert(inode_num);
            break;
        }
    }
    
    inode_pointer.insert({filename,inode_num});
    string node_name=to_string(inode_num);
    const char *content_c=content.c_str();
    int x;
    x=content.length();
    
    int j=0;
    int i=1;
    
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
    disk_pointer.insert({inode_num,i-1});
    cout<<"file created :\n";
    cout<<"name :"<<filename<<"\ninode number :"<<inode_num<<"\ndisk size : "<<i-1<<"\n";

}
void rename_file(string old_name,string new_name)
{
    inode_pointer.insert({new_name,inode_pointer[old_name]});
    inode_pointer.erase(old_name);
    for(auto x:name)
    {
        if(x==old_name)
        {
            name.erase(x);
            break;
        }
    }
    name.insert(new_name);

}
void delete_file(string filename)
{
    cout<<"deleted filename :"<<filename<<"\n";
    int inode_num=inode_pointer[filename];
    int block_size=disk_pointer[inode_num];
    string del_filename;
    for(int i=1;i<=block_size;i++)
    {
        del_filename=root+to_string(inode_num)+"_"+to_string(i)+".txt";
        const char *name=del_filename.c_str();
        int status=remove(name);
        
    }
    {
        cout<<block_size<<" Block deleted\n";
    }
    inode_pointer.erase(filename);
}
void print_file()
{
    for(auto i:inode_pointer)
    {
        cout<<i.first<<"\t";
    }
    cout<<"\n";
}

void content_display(string filename)
{
    int inode_po=inode_pointer[filename];
    int block=disk_pointer[inode_po];
    for(int i=1;i<=block;i++)
    {
        string file_name=root+to_string(inode_po)+"_"+to_string(i)+".txt";
        const char *name=file_name.c_str();
        
        FILE *f=fopen(name,"r");
        char output[100];
        fgets(output,100,f);
        cout<<output;
        fclose(f);
    }
    cout<<"\n";
}
void read_meta_data()
{
    cout<<"Meta data reading........... \n";
    FILE *f=fopen("root/meta_data.txt","r");
    if(!f)
        return;
    char file_name[1000];
    int inode;
    int size;
    while(!feof(f))
    {
        fscanf(f,"%s %d %d\n",file_name,&inode,&size);
        inode_pointer.insert({file_name,inode});
        disk_pointer.insert({inode,size});
        inode_number.insert(inode);
        name.insert(file_name);
    }
    fclose(f);
}
void create_meta_data()
{
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
    srand(0);
    int d=mkdir("root",0777);
    if(d==-1)
    {
        cout<<"Root directory already available :\n";
    }

    read_meta_data();


    string command;
    while(1)
    {   
        cout<<"Enter command : ";
        getline(std::cin, command);
        vector<string> token;
        string word="";
        int i=0;
        for(auto i:command)
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
        
        if(token[0]=="mf")
        {
            string file_name="";
            string content="";
            file_name=token[1];
            int bit=0;
            for(auto i:command)
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
            if(name.find(file_name)!=name.end())
            {
                cout<<"File already present :\n";
                continue;
            }
            name.insert(file_name);
            create_file(file_name,content);

            create_meta_data();
        }
        else if(token[0]=="df")
        {
            string file_name=token[1];

            if(name.find(file_name)==name.end())
            {
                cout<<"File is not available :\n";
                continue;
            }
            delete_file(file_name);
            cout<<file_name<<" deleted:\n";
            create_meta_data();
        }
        else if(token[0]=="rf")
        {
            if(name.find(token[1])==name.end())
            {
                cout<<"File is not available :\n";
                continue;
            }
            rename_file(token[1],token[2]);
            cout<<"file renamed :\n";
            create_meta_data();
        }
        else if(token[0]=="pf")
        {

            if(name.find(token[1])==name.end())
            {
                cout<<"File is not available :\n";
                continue;
            }
            content_display(token[1]);
        }
        else if(token[0]=="ls")
        {
            print_file();
        }       
        else if(token[0]=="end")
        {
            break;
        }
        else if(token[0]=="print_data")
        {
            for(auto x :name)
            {
                cout<<x<<"\n";
            }
            for(auto x:inode_number)
            {
                cout<<x<<"\n";
            }
            for(auto i:inode_pointer)
            {
                cout<<i.first<<"\t"<<i.second<<"\n";
            }

        }
    }
    create_meta_data();
}