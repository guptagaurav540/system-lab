#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include <unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <signal.h>




#define MAXLINE 50

struct sockaddr_in srv;
struct sockaddr_in cli,srv_addr;
int newfd;
unsigned int cli_len ;


pid_t child_pd;

//Semaphore
sem_t *mutex;

//For handeling Ctrl + C
void sigintHandler(int sig_num)
{
    printf("\n Destroying semaphore with CTRL + C\n");
    sem_close(mutex);
    sem_unlink("semaphore");
    exit(1);
}
//Function to validate credentials
int login_check(char user_id[],char password[])
{
	FILE *f1;
    
    
    int user_type;
	char user_id_c[20],password_c[10];
	int i;
	while(user_id[i]!='\n')
    {
        i++;
    }
    user_id[i]='\0';
    i=0;
    while(password[i]!='\n')
    {
        i++;
    }
    password[i]='\0';
   
    f1=fopen("login_data.txt","r");
	while(fscanf(f1,"%s %s %d\n",user_id_c,password_c,&user_type)!=EOF)
	{
        //printf("%s %s %s %s\n",user_id,user_id_c,password,password_c);
		if(strcmp(user_id,user_id_c)==0 && strcmp(password,password_c)==0)
		{
            //printf("return value %d",user_type);
			return(user_type);
			break;
		}

	}
	return(-1);
}

//Functoin to check whether account number exists or not
int account_check(char user_id[])
{
	FILE *f1;
    
    
    int user_type;
	char user_id_c[20],password_c[10];
	int i;
   
    f1=fopen("login_data.txt","r");
	while(fscanf(f1,"%s %s %d\n",user_id_c,password_c,&user_type)!=EOF)
	{
        //printf("%s %s %s %s\n",user_id,user_id_c,password,password_c);
		if(strcmp(user_id,user_id_c)==0)
		{
            printf("return value %d",user_type);
			return(user_type);
			break;
		}

	}
    fclose(f1);
	return(-1);
}

//Function to find balance of an user
int balanceEnquiry(char user_id[])
{
    printf("balance function\n");
    FILE *fptr;
    fptr = fopen("Balance.txt","r");
    if(!fptr)
    {
        perror("Balance .txt not exist");
    }
    int balance;
    char user[1000];
    while(fscanf(fptr,"%s %d\n",user,&balance)!=EOF )
    {
        if(strcmp(user,user_id)==0)
        {
            fclose(fptr);
            printf("balance return %d \n",balance);
            return balance;
        }
    }
    fclose(fptr);
    return -1;
}

//Function to update the mini statement
void updateminiministatement(char account_number[1000])
{
    FILE *read, *write;
    char new_acc_no[500] = "mini_";
    strcat(new_acc_no,account_number);

    read = fopen(account_number,"r");
    write = fopen(new_acc_no,"w");

    int lines = 0;
    char temp[500];
    while(!feof(read))
    {
        fgets(temp,500,read);
        lines++;
    }
    lines--;
    rewind(read);
    while(lines > 5)
    {
        fgets(temp,500,read);
        lines--;
    }
    while(lines > 0)
    {
        fgets(temp,500,read);
        fputs(temp,write);
        lines--;
    }

    printf("No of lines in transaction history : %d\n", lines);
    fclose(write);
    fclose(read);


}

//Function to upadte transaction history
void updateMiniStatement(char account_number[1000],int amount,int updated_balance,char type)
{
    //int type says credit or debit

    FILE *fptr;
    //printf("available balance = %d\n",available_balance)
    fptr = fopen(account_number,"a");
    time_t t;
    time(&t);
    char *tt = ctime(&t);
    int i=0;
    while(tt[i]!='\n')
    {
        if(tt[i]==' ')
            tt[i] = '-';
        i++;
    }
    tt[i]='\0';
    int a=amount;
    if(a<0)
        a = -1*a;
    if(type == 'C')
        fprintf(fptr,"%s C %d %d\n",tt,a,updated_balance);
    else fprintf(fptr,"%s D %d %d\n",tt,a,updated_balance);
    fclose(fptr);
}

//Function to update balance
void updateBalance(char account_number[500],int available_balance,int u_balance)
{
    FILE *fptr,*temp;
    temp = fopen("temp.txt","w");
    fptr = fopen("Balance.txt","r+");
    if(fptr==NULL)
        printf("error opening file\n");
    
    char content[100];
    int balance = balanceEnquiry(account_number);

    char b[100];
    bool flag = true;
    int current_balance=0;
    
    while(fscanf(fptr,"%s %d",content,&current_balance)!=EOF)
    {
        //printf("%s\n",content);
        if(strcmp(content,account_number)==0)
        {
            printf("available balance of %s is = %d\n",account_number,available_balance);
            printf("updated  balance  of %s is = %d\n",account_number,u_balance);
            flag = false;
            char ch ;
           // ch = getc(fptr);
            fprintf(temp,"%s %d\n",content,u_balance);
            break;
        }
        else{
            fprintf(temp,"%s %d\n",content,current_balance);
            
        }
    }
    while(fscanf(fptr,"%s %d",content,&current_balance)!=EOF)
    {
        fprintf(temp,"%s %d\n",content,current_balance);
    }

    
    fclose(temp);
    fclose(fptr);
    rename("Balance.txt","Backup.txt");
    rename("temp.txt","Balance.txt");

}

//Function to handel credit/debit
int swCreditDebit(char a_number[500],int amount,char type)
{
    sem_wait(mutex);
    int a_available_balance = balanceEnquiry(a_number);
    
    // sleep(1); 
    //invalid user
    if(a_available_balance == -1)
        return 0;
    //insufficient balance
    if(a_available_balance < amount && type == 'D')
        return 2;
    
    char a_account_number[500];
    strcpy(a_account_number,a_number);
    strcat(a_account_number,".txt");
    printf("Account number rec: %s\n",a_number);

    printf("Account number: %s\n",a_account_number);
    
    if(type == 'C')
    {
        updateMiniStatement(a_account_number,amount,a_available_balance+amount,'C');
        updateminiministatement(a_account_number);
        updateBalance(a_number,a_available_balance,a_available_balance+amount);
    }
    else if(type == 'D')
    {
        updateMiniStatement(a_account_number,amount,a_available_balance-amount,'D');
        updateminiministatement(a_account_number);
        updateBalance(a_number,a_available_balance,a_available_balance-amount);
    }

    sem_post(mutex);
    return 1;


} 

//Function to send transactin history to client
void ministatment(char user_id[])   
{

    //Date debit/credit amount updated_balance Date
    char file_name[1024];
    char txt[]=".txt";

    
    strcpy(file_name,user_id);
    strcat(file_name,txt);
    
    FILE *file_pointer=fopen(file_name,"r");

    int data_availability=-1;
    if(file_pointer==NULL)
    {
        printf("No Transaction History available...\n");
        data_availability = -2;
        if(write(newfd,&data_availability,sizeof(int))<0)
        {
            perror("error in sending data availability ");
            exit(1);
        }
        return;
    }
    char date[501];
    char transaction_type;
    int amount;
    int balance;

    
    int i=3;
    while(fscanf(file_pointer,"%s %c %d %d",date,&transaction_type,&amount,&balance)!=EOF)
    {
        data_availability=1;
        if(write(newfd,&data_availability,sizeof(int))<0)
        {
            perror("error in sending data availability ");
            exit(1);
        }
        else{
            
        }
    

        char *buff=(char *)malloc(1024*sizeof(char));
        bzero(buff,1024);
        sprintf(buff, "%20s\t %c %5s %10d\t%10d", date, transaction_type," ", amount, balance);  
            int count=write(newfd,buff,500);
            if(count<0)
            {
                perror("error in sending ministm");
                exit(1);
            }
            int ack;
            if(read(newfd,&ack,sizeof(ack))<0)
            {
                perror("2. errror in recv ack");
                exit(1);
            }
            printf("%s\n",buff);
        free(buff);
    }

    data_availability=-1;
    if(write(newfd,&data_availability,sizeof(int))<0)
    {
        perror("error in sending data availability ");
        exit(1);
    }
    fclose(file_pointer);

}

//Function to send mini statment to client
void miniministatment(char user_id[])   
{

    //Date debit/credit amount updated_balance Date
    char file_name[1024] = "mini_";
    char txt[]=".txt";

    
    strcat(file_name,user_id);
    strcat(file_name,txt);
    printf("%s", file_name);
    FILE *file_pointer=fopen(file_name,"r");

    int data_availability=-1;
    if(file_pointer==NULL)
    {
        printf("No mini statement available...");
        data_availability = -2;
        if(write(newfd,&data_availability,sizeof(int))<0)
        {
            perror("error in sending data availability ");
            exit(1);
        }
        return;
    }
    char date[501];
    char transaction_type;
    int amount;
    int balance;

    
    int i=3;
    while(fscanf(file_pointer,"%s %c %d %d",date,&transaction_type,&amount,&balance)!=EOF)
    {
        data_availability=1;
        if(write(newfd,&data_availability,sizeof(int))<0)
        {
            perror("error in sending data availability ");
            exit(1);
        }
        else{
            
        }
    

        char *buff=(char *)malloc(1024*sizeof(char));
        bzero(buff,1024);
        sprintf(buff, "%20s\t %c %5s %10d\t%10d", date, transaction_type," ", amount, balance);  
            int count=write(newfd,buff,500);
            if(count<0)
            {
                perror("error in sending ministm");
                exit(1);
            }
            int ack;
            if(read(newfd,&ack,sizeof(ack))<0)
            {
                perror("2. errror in recv ack");
                exit(1);
            }
            printf("%s\n",buff);
        free(buff);
    }

    data_availability=-1;
    if(write(newfd,&data_availability,sizeof(int))<0)
    {
        perror("error in sending data availability ");
        exit(1);
    }
    fclose(file_pointer);

}

//Functin to send balance of all users to client
void all_balance()
{


    FILE *file_pointer=fopen("Balance.txt","r");
    if(file_pointer==NULL)
    {
        printf("file not exist");
        exit(1);
    }
    char account_no[501];
    int balance;

    int data_availability=-1;
    int i=3;
    while(fscanf(file_pointer,"%s %d",account_no,&balance)!=EOF)
    {
        data_availability=1;
        if(write(newfd,&data_availability,sizeof(int))<0)
        {
            perror("error in sending data availability ");
            exit(1);
        }
    

        char *buff=(char *)malloc(1024*sizeof(char));
        bzero(buff,1024);
        sprintf(buff, "   %9s %18d", account_no, balance);  
            int count=write(newfd,buff,500);
            if(count<0)
            {
                perror("error while sending balance of every user");
                exit(1);
            }
            int ack;
            if(read(newfd,&ack,sizeof(ack))<0)
            {
                perror("2. errror in recv ack");
                exit(1);
            }
            printf("%s\n",buff);
        free(buff);
    }

    data_availability=-1;
    if(write(newfd,&data_availability,sizeof(int))<0)
    {
        perror("error in sending data availability ");
        exit(1);
    }
    fclose(file_pointer);

}

//Function to handle client with type as 0 ie. customer
void customer_function(char *user_id)
{
    int option = 4;
    int count;
    while(1)
    {
        option = 4;
        count=read(newfd,&option,sizeof(option));
        if(count<0)
        {
            perror("error in option recv");
            exit(1);
        }
        printf("Option =%d\n",option);
        
        if(option==1)
        {
            int balance=balanceEnquiry(user_id);
            count=write(newfd,&balance,sizeof(balance));
            if(count<0)
            {
                perror("error in sending balance ");
                exit(1);
            }

        }
        else if(option==2)
        {
            ministatment(user_id);
        }
        else if(option==3)
        {
            miniministatment(user_id);
        }
        else
        {
            break;
        }
        
    }
}

//Function to transfer balance from one account to another
int creditDebit(char a_number[1000],char b_number[1000],int amount,char type)
{
    //type C implies credit
    //type D implies debit

    //if return value is 1 then update is successful else if 0 then update is unsuccessful else if return value is 2 then account b has insufficient balance
    //here check if both user account number  are valid or not and return 0

    sem_wait(mutex);
    int a_available_balance = balanceEnquiry(a_number);
    int b_available_balance = balanceEnquiry(b_number);

    if(type == 'C')
        if(b_available_balance < amount)
        {
            sem_post(mutex);
            return 2;
        }
    
    char a_account_number[1000],b_account_number[1000];
    strcpy(a_account_number,a_number);
    strcpy(b_account_number,b_number);
    
    strcat(a_account_number,".txt");        //user 1 ministatement 
    strcat(b_account_number,".txt");        //user 2 ministatement

    int a_u_balance = a_available_balance+amount;
    int b_u_balance = b_available_balance-amount;

    //updating mini_statement
    updateMiniStatement(a_account_number,amount,a_u_balance,'C');
    updateminiministatement(a_account_number);
    updateMiniStatement(b_account_number,amount,b_u_balance,'D');
    updateminiministatement(b_account_number);


    //updating Balance.txt
    
    updateBalance(a_number,a_available_balance,a_u_balance);
    updateBalance(b_number,b_available_balance,b_u_balance);
    
    sem_post(mutex);
    return 1;

}

//Function to handle client with type as 2 ie. admin
void admin_function()
{
    int option=1;
    int count;
    while(option!=5)
    {
        option = 5;
        if((count=read(newfd,&option,sizeof(int)))<0)
        {
            perror("Option recv error in police function ");
            exit(1);
        }

        // printf("1.Credit 2.Debit 3.Exit\n");
        printf("Updated Option: %d\n", option);
        // scanf("%d",&option);
        if(option==1 || option == 2)
        {
            char a_account_number[500];
            int amount;

            read(newfd,a_account_number,500);

            int success = 0;
            if(balanceEnquiry(a_account_number) == -1)
            {
                printf("Account number invalid please check...\n");
                count=write(newfd,&success,sizeof(success));
                continue;
            }
            else
            {
                success = 1;
                count=write(newfd,&success,sizeof(success));
            }

            read(newfd,&amount,sizeof(amount));
            printf("a_account_no: %s\n", a_account_number);
            
            printf("Amount: %d\n", amount);
            

            if(option == 1)
                success = swCreditDebit(a_account_number,amount,'C');
            else
                success = swCreditDebit(a_account_number,amount,'D');

            count=write(newfd,&success,sizeof(success));

            if(success == 1)
                printf("Update successful !!\n");
            else if(success == 0)printf("Update unsuccessful invalid user id !!\n");
            else printf("Account number %s has insufficient balance transaction cannot be made.. \n",a_account_number);
        }
        else if(option==3)
        {
                char a_account_number[1000],b_account_number[1000];
                int amount;
                int success = 0;
                read(newfd,a_account_number,500);
                printf("Bank_Account_Number to be  debited is: %s",a_account_number);
                read(newfd,b_account_number,500);
                printf("Bank_Account_Number to be credited is: %s",b_account_number);
                
                //Check account number
                if(balanceEnquiry(a_account_number) == -1 || balanceEnquiry(b_account_number) == -1)
                {
                    printf("Account number invalid please check...\n");
                    count=write(newfd,&success,sizeof(success));
                    continue;
                }
                else
                {
                    success = 1;
                    count=write(newfd,&success,sizeof(success));
                }

                read(newfd,&amount,sizeof(amount));
                printf("Amount is: %d",amount);
                success = creditDebit(b_account_number,a_account_number,amount,'C');
                count=write(newfd,&success,sizeof(success));
                if(success == 1)
                    printf("Update successful !!\n");
                else if(success==0)printf("Update unsuccessful invalid user id !!\n");
                else printf("Account number %s has insufficient balance transaction cannot be made.. \n",a_account_number);
        }
        else if(option==4)
        {
            
            char a_account_number[500];

            read(newfd,a_account_number,500);
            printf("Account no: %s\n",a_account_number);
            int success = 0;
            if(account_check(a_account_number) != -1)
            {
                printf("Account already exists...\n");
                count=write(newfd,&success,sizeof(success));
                continue;
            }
            else
            {
                success = 1;
                count=write(newfd,&success,sizeof(success));
            }
            char password[500];
            int type;
            read(newfd,password,500);
            read(newfd,&type,sizeof(type));
            FILE *fptr;
            if(type == 0)
            {
                fptr = fopen("Balance.txt","a+");
                fprintf(fptr,"%s 0\n",a_account_number);
                fclose(fptr);
            }
            fptr = fopen("login_data.txt","a+");
            fprintf(fptr,"%s %s %d\n",a_account_number,password,type);
            fclose(fptr);
            success = 1;
            count=write(newfd,&success,sizeof(success));
        }
        else if(option==5)
        {
            printf("Exiting..\n");
            // break;
        }
    }
}

//Function to handle client with type as 1 ie. police
void police_function()
{
    int option;
    while(1)
    {
        option = 5;
        int count;
        if((count=read(newfd,&option,sizeof(int)))<0)
        {
            perror("Option recv error in police function ");
            exit(1);
        }
        if(option>4)
        {
            break;
        }
        char user_id[500];
        if(option != 1)
        {
            if((count=read(newfd,user_id,500))<0)
            {
                perror("user id recv error in police function ");
                exit(1);
            }
        }
        //If option is 1 call all_balance to send balance of all users to client
        if(option==1)
        {
            all_balance();
        }
        //If option is 2 send balance of specific user to client
        else if(option==2)
        {
            int balance=balanceEnquiry(user_id);
            if((count=write(newfd,&balance,sizeof(balance)))<0)
            {
                perror("error in balance sending in  police function");
                exit(1);
            }
        }
        //if option is 3 send transaction history of a specific user to client
        else if(option==3)
        {
            ministatment(user_id);
        }
        //if option is 4 mini statement of a specific user to client
        else if(option==4)
        {
            miniministatment(user_id);
        }
    }
}

//Main function which will be the driver function for whole program
int main(int argc, char *argv[])
{
    signal(SIGINT, sigintHandler);
    // mutex = sem_open(SNAME, O_CREAT | O_EXCL, 0644, 1);
    mutex = sem_open("semaphore", O_CREAT | O_EXCL, 0644, 1);

    int fd;
    if((fd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("[-] Socket Creation failed \n");
        exit(1);
    }
    else{   
        printf("[+] Socket created\n");
    }
    bzero((char *)&srv,sizeof(srv));

    srv.sin_family=AF_INET;
    srv.sin_port=htons(atoi(argv[1]));
    // bind a client may connect to any of server addresses

    srv.sin_addr.s_addr=INADDR_ANY;
    if(bind(fd,(struct sockaddr*) &srv,sizeof(srv))<0)
    {
        perror("[-]Binding failed");
        exit(1);
    }
    else{
        printf("[+] Binding Complete\n");
    }
    if(listen(fd,10)<0)
    {
        perror("[-] Error in Listen :\n");
        exit(1);
    }
    else{
        printf("[+] Socket listening For incoming Connection...\n");
    }
    cli_len=sizeof(cli);

    while(1)
    {
        newfd=accept(fd,(struct sockaddr*) &cli, &cli_len);
        
        if((child_pd=fork())==0){


            if(newfd<0)
            {
                perror("[-] Error in accept\n");
                exit(1);
            }
            else{
                printf("[+] Connection accepted from IP : %s, Port : %d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
            }

            char *user_id=(char*)malloc( 500*sizeof(char));
            char *password=(char *)malloc(500*sizeof(char));
            int count=0;
            if((count=read(newfd,user_id,500))<0)
            {
                perror("erorr in recv user_id");
            }
            else{
                int ack=1;
                if((count=write(newfd,&ack,sizeof(ack)))<0)
                {
                    perror("1. error in sending ack :");
                }
            }

            if((count=read(newfd,password,500))<0)
            {
                perror("eroor in recv password ");
            }
            else{
                int ack=1;
                if(write(newfd,&ack,sizeof(ack))<0)
                {
                    perror("error in sending ack :");
                }
            }
            printf("Requesting to login User id : %s\n",user_id);
            int user_type=login_check(user_id,password);
            if((count=write(newfd,&user_type,sizeof(user_type)))<0)
            {
                perror("Error in sendind user_type ");
            }
            else{
                int ack=1;
                if(read(newfd,&ack,sizeof(ack))<0)
                {
                    perror("error in sending ack :");
                }

                else if(user_type==0)
                {
                    printf("%s login as Customer\n",user_id);
                    customer_function(user_id);
                }
                else if(user_type==1)
                {
                    printf("%s login as ploice\n",user_id);

                    police_function();
                }
                else if(user_type==2)
                {
                    printf("%s login as Admin\n",user_id);
                    admin_function();
                }
                else{
                    printf("Incorrect credentials ..\n ");
                }
            }
        }
    }
    close(fd);               
}