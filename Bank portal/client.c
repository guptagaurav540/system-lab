#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include <unistd.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<string.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#define MAXPW 32


#define MAXLINE 50

int fd;
struct sockaddr_in srv;
char buf[1024],buf_r[1024];
int nbytes,newfd;
void print_customer_menu()
{
    system("clear");   
    printf("┌───────────────────────────────────────────────────┐\n");
    printf("│░░░░░░░░░░░░░░░░░ CUSTOMER PORTAL ░░░░░░░░░░░░░░░░░│\n");
    printf("├───────────────────────────────────────────────────┤\n");
    printf("│1. Balance Enquiry                                 │\n");
    printf("│2. Transaction History                             │\n");
    printf("│3. Mini Statment                                   │\n");
    printf("│4. Logout                                          │\n");
    printf("└───────────────────────────────────────────────────┘\n");
}
void print_police_menu()
{
    system("clear");
    printf("┌───────────────────────────────────────────────────┐\n");
    printf("│░░░░░░░░░░░░░░░░░ POLICE PORTAL ░░░░░░░░░░░░░░░░░░░│\n");
    printf("├───────────────────────────────────────────────────┤\n");
    printf("│1. Balance Enquiry for all users                   │\n");
    printf("│2. Balance Enquiry for specific user               │\n");
    printf("│3. Transaction History for specific user           │\n");
    printf("│4. Mini statement for specific user                │\n");
    printf("│5. Logout                                          │\n");
    printf("└───────────────────────────────────────────────────┘\n");
}
void print_admin_menu()
{
    system("clear");
    printf("┌───────────────────────────────────────────────────┐\n");
    printf("│░░░░░░░░░░░░░░░░░ ADMIN PORTAL ░░░░░░░░░░░░░░░░░░░░│\n");
    printf("├───────────────────────────────────────────────────┤\n");
    printf("│1. Credit                                          │\n");
    printf("│2. Debit                                           │\n");
    printf("│3. Transfer                                        │\n");
    printf("│4. Add New User                                    │\n");
    printf("│5. Logout                                          │\n");
    printf("└───────────────────────────────────────────────────┘\n");
}
void ministatment(char user_id[])
{
    int data_availability=-1;
    if(read(fd,&data_availability,sizeof(data_availability))<0)
    {
        perror("Error while receiving data availability\n");
    }
    if(data_availability ==-1)
    {
        printf("Account number %s does not exist.\n",user_id);
        return;
    }
    else if(data_availability == -2)
    {
        printf("This option will be available after making transactions.\n");
        return;
    }
    else
    {
        printf("┌────────────────────────────────────────────────────────────────────┐\n");
        printf("│ %14s %8s %10s %4s %10s %3s %10s  │\n","Date"," ","Type"," ","Amount"," ","Balance");
        printf("├────────────────────────────────────────────────────────────────────┤\n");
    }
    while(1)
    {
        // int data_availability=1;
        
        char *buff=(char *)malloc(1024*sizeof(char));
        bzero(buff,1024);
        int count=read(fd,buff,1024);
        if(count<0)
        {
            perror("Error while receiving mini statement\n");
            exit(1);
        }
        
        int ack=1;
        printf("|  %s   |\n",buff);
        if(write(fd,&ack,sizeof(int))<0)
        {
            perror("2.error in ack sending ");
        }
        free(buff);
        if(read(fd,&data_availability,sizeof(data_availability))<0)
        {
            perror("Error while receiving data availability\n");
        }
        if(data_availability==-1)
            break;
        
    }
    printf("└────────────────────────────────────────────────────────────────────┘\n");

}
void print_balance(char user_id[],int balance)
{
    printf("┌───────────────────────────────────┐\n");
    printf("│            USER BALANCE           │\n");
    printf("├───────────────────────────────────┤\n");
    printf("│ %s %15s    │\n","Account Number","Balance");
    printf("│ %11s    %15d    │\n",user_id,balance);
    printf("└───────────────────────────────────┘\n");
}
void all_balance()
{
    printf("┌───────────────────────────────────┐\n");
    printf("│            BALANCESHEET           │\n");
    printf("├───────────────────────────────────┤\n");
    
    printf("│ %s %15s    │\n","Account Number","Balance");

    while(1)
    {
        int data_availability=1;
        if(read(fd,&data_availability,sizeof(data_availability))<0)
        {
            perror("Error in recv data availabilty");
        }
        else{
            
        }
        if(data_availability==-1)
            break;
        char *buff=(char *)malloc(1024*sizeof(char));
        bzero(buff,1024);
        int count=read(fd,buff,1024);
        if(count<0)
        {
            perror("error in recv ministm");
            exit(1);
        }

        int ack=1;
        printf("│%s    │\n",buff);
        if(write(fd,&ack,sizeof(int))<0)
        {
            perror("2.error in ack sending ");
        }        
        free(buff);
        
    }
    printf("└───────────────────────────────────┘\n");
}

void sigintHandler(int sig_num)
{
    printf("\n Exiting the program\n");
    exit(1);
}
ssize_t getpasswd (char **pw, size_t sz, int mask, FILE *fp)
{
    if (*pw == NULL) {              /* reallocate if no address */
        void *tmp = realloc (*pw, sz * sizeof **pw);
        if (!tmp)
            return -1;
        memset (tmp, 0, sz);    /* initialize memory to 0   */
        *pw =  (char*) tmp;
    }

    size_t idx = 0;         /* index, number of chars in read   */
    int c = 0;

    struct termios old_kbd_mode;    /* orig keyboard settings   */
    struct termios new_kbd_mode;

    if (tcgetattr (0, &old_kbd_mode)) { /* save orig settings   */
        fprintf (stderr, "%s() error: tcgetattr failed.\n", __func__);
        return -1;
    }   /* copy old to new */
    memcpy (&new_kbd_mode, &old_kbd_mode, sizeof(struct termios));

    new_kbd_mode.c_lflag &= ~(ICANON | ECHO);  /* new kbd flags */
    new_kbd_mode.c_cc[VTIME] = 0;
    new_kbd_mode.c_cc[VMIN] = 1;

    if (tcsetattr (0, TCSANOW, &new_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    /* read chars from fp, mask if valid char specified */
    while (((c = fgetc (fp)) != '\n' && c != EOF && idx < sz - 1) ||
            (idx == sz - 1 && c == 127))
    {
        if (c != 127) {
            if (31 < mask && mask < 127)    /* valid ascii char */
                fputc (mask, stdout);
            (*pw)[idx++] = c;
        }
        else if (idx > 0) {         /* handle backspace (del)   */
            if (31 < mask && mask < 127) {
                fputc (0x8, stdout);
                fputc (' ', stdout);
                fputc (0x8, stdout);
            }
            (*pw)[--idx] = 0;
        }
    }
    (*pw)[idx] = 0; /* null-terminate   */

    /* reset original keyboard  */
    if (tcsetattr (0, TCSANOW, &old_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    if (idx == sz - 1 && c != '\n') /* warn if pw truncated */
        fprintf (stderr, " (%s() warning: truncated at %zu chars.)\n",
                __func__, sz - 1);

    return idx; /* number of chars in passwd    */
}



void customer_function(char user_id[])
{
    int option;
    char choice= 'Y';
    do
    {
        print_customer_menu();
        printf("Enter your choice :");
        scanf("%d",&option);
        getc(stdin);
        int count;
        count=write(fd,&option,sizeof(int));
        if(count<0){
            printf("Error in sending option \n");
            exit(1);
        }
        system("clear");
        if(option==1)
        {
          // Balance Enquiry
            int balance;

            count=read(fd,&balance,sizeof(int));
            if(count<0)
            {
              printf("Error in reciving balance \n");
                exit(1);
            }
            else
            {
                int i=0;
                char user_id_modified[500];
                strcpy(user_id_modified,user_id);
                while(user_id_modified[i]!='\n')
                {
                    i++;
                }
                user_id_modified[i]='\0';
                print_balance(user_id_modified,balance);
            }
          
        }
        else if(option==2 || option == 3)
        {
            ministatment(user_id);
        }
        else if(option == 4)
        {
            exit(1);
        }
        else
            printf("Invalid option, please try again...\n");
      
        printf("Do you want to continue?(Y/N): ");

        scanf("%c", &choice);
        
    }
    while(choice  == 'Y' || choice  == 'y');



}
void admin_function()
{
    int option;
    int count;
    char choice='N';
    do
    {
        option = 6;
        print_admin_menu();
        char *user_id=(char*)malloc(500*sizeof(char));
        char *user_id_2=(char*)malloc(500*sizeof(char));
        bzero(user_id,500);
        printf("Enter Option :  ");
        scanf("%d",&option);
        getc(stdin);
        if(option > 5 || option < 1)
        {
            printf("Incorrect option, please try again...\n");
            printf("Do you want to continue?(Y/N): ");
            scanf("%c", &choice);
            continue;
        }
        int i=0;
        count=write(fd,&option,sizeof(option));
        
        
        if(count<0)
        {
            perror("Sending option error in admin function\n");
            exit(1);
        }

        if(option == 1 || option == 2)
        {
            printf("Enter user_id :  ");
            fgets(user_id,500,stdin);
            while(user_id[i]!='\n')
            {
                i++;
            }
            user_id[i]='\0';
            count=write(fd,user_id,500);
            if(count<0)
            {
                perror("Sending user_id error in admin function\n");
                exit(1);
            }
            int success;
            count=read(fd,&success,sizeof(success));
            if(success == 0)
            {
                printf("Account number invalid please check...\n");
                printf("Do you want to continue?(Y/N): ");
                scanf("%c", &choice);
                continue;
            }


            int amount = -1;
            printf("Enter amount  : ");
            scanf("%d",&amount);

            while (amount < 0)
            {
                print_admin_menu();
                printf("Enter Option :  %d\n",option);
                printf("Enter user_id :  %s\n",user_id);
                printf("Amount negative please Re-enter amount: ");
                scanf("%d",&amount);
            }
            getc(stdin);

            count=write(fd,&amount,sizeof(amount));
            if(count<0)
            {
                perror("Error while sending amount to the admin function\n");
                exit(1);
            }

            count=read(fd,&success,sizeof(success));
            if(success == 1)
            printf("Transaction successful !!\n");
            else if(success==0)printf("Update unsuccessful invalid user id !!\n");
            else printf("Account number %s has insufficient balance transaction cannot be made.. \n",user_id);

        }
        else if(option == 3)
        {

            printf("Enter user id to send money from :  ");
            fgets(user_id,500,stdin);
            printf("Enter user id to send money to :  ");
            fgets(user_id_2,500,stdin);
            while(user_id[i]!='\n')
            {
                i++;
            }
            user_id[i]='\0';
            i = 0;
            while(user_id_2[i]!='\n')
            {
                i++;
            }
            user_id_2[i]='\0';
            count=write(fd,user_id,500);
            if(count<0)
            {
                perror("Sending user_id error in admin function\n");
                exit(1);
            }
            count=write(fd,user_id_2,500);
            if(count<0)
            {
                perror("Sending user_id_2 error in admin function\n");
                exit(1);
            }
            int success;
            count=read(fd,&success,sizeof(success));
            if(success == 0)
            {
                printf("\nAccount number invalid please check...\n");
                continue;
            }

            int amount = -1;
            printf("Enter amount: ");
            scanf("%d",&amount);

            while (amount < 0)
            {
                print_admin_menu();
                printf("Enter Option :  %d\n",option);
                printf("Enter user_id :  %s\n",user_id);
                printf("Amount negative please Re-enter amount: ");
                scanf("%d",&amount);
            }
            getc(stdin);

            count=write(fd,&amount,sizeof(amount));
            if(count<0)
            {
                perror("Error while sending amount to the admin function\n");
                exit(1);
            }


            count=read(fd,&success,sizeof(success));
            if(success == 1)
            printf("Transaction successful !!\n");
            else if(success==0)printf("Update unsuccessful invalid user id !!\n");
            else printf("Account number %s has insufficient balance transaction cannot be made.. \n",user_id);

        }
        else if(option == 4)
        {
            printf("Enter account number of new user :  ");
            fgets(user_id,500,stdin);
            while(user_id[i]!='\n')
            {
                i++;
            }
            user_id[i]='\0';

            count=write(fd,user_id,500);
            if(count<0)
            {
                perror("Sending account number error in admin function\n");
                exit(1);
            }
            int success;
            count=read(fd,&success,sizeof(success));
            if(count<0)
            {
                perror("Sending account number error in admin function\n");
                exit(1);
            }
            
            if(success == 0)
            {
                printf("Account already exists please check...\n");
                printf("Do you want to continue?(Y/N): ");
                scanf("%c", &choice);
                continue;
            }
            else
            {
                printf("Enter password of new user : ");
                char password[500];
                fgets(password,500,stdin);
                i = 0;
                while(password[i]!='\n')
                {
                    i++;
                }
                password[i]='\0';
                int type;
                //Change %d to %c after type format change in login file
                printf("Enter user type : ");
                scanf("%d",&type);
                getc(stdin);
                count=write(fd,password,500);
                count=write(fd,&type,sizeof(type));
                count=read(fd,&success,sizeof(success));
                if(success == 1)
                {
                    printf("Account Created.\n");
                }
                else
                {
                    printf("Account creation unsuccessfull.\n");
                }

            }
        }
        else
        {
            exit(1);
        }
        free(user_id);
        printf("Do you want to continue?(Y/N): ");
        scanf("%c", &choice);
        
    }
    while(choice  == 'Y' || choice  == 'y');
}
void police_function()
{

    int option;
  
    int count;
    char choice;
  
    do
    {
        print_police_menu();
        char *user_id=(char*)malloc(500*sizeof(char));
        bzero(user_id,500);
        printf("Enter Option :  ");
        scanf("%d",&option);
        getc(stdin);
        if(option > 5 || option < 1)
        {
            printf("Incorrect option, please try again...\n");
            printf("Do you want to continue?(Y/N): ");
            scanf("%c", &choice);
            continue;
        }
        count=write(fd,&option,sizeof(int));
        if(count<0)
        {
            perror("Error while sending selected option.\n");
            exit(1);
        }
        
        if(option == 2 || option == 3 || option == 4)
        {
            printf("Enter user_id :  ");
            fgets(user_id,500,stdin);
            int i=0;
            while(user_id[i]!='\n')
            {
                i++;
            }
            user_id[i]='\0';

            count=write(fd,user_id,500);
            if(count<0)
            {
                perror("Sending user_id error in police function\n");
                exit(1);
            }
        }
        int balance;
        system("clear");
        switch(option)
        {
            case 1:
                all_balance();
                break;
            case 2:
                count=recv(fd,&balance,4,0);
                if(count<0)
                {
                    perror("\nBalance reciving error in police function ");
                }
                else
                {
                    if(balance == -1)
                    {
                        printf("User doesn't exist.\n");
                    }
                    else
                    {
                        print_balance(user_id,balance);
                    }
                }
                break;
            case 3:
                ministatment(user_id);
                break;
            case 4:
                ministatment(user_id);
                break;
            case 5:
                exit(1);
                break;
            default:
                printf("Incorrect option, please try again\n");
                continue;
        }
        free(user_id);
        printf("Do you want to continue?(Y/N): ");

        scanf("%c", &choice);
        
    }
    while(choice  == 'Y' || choice  == 'y');
}


int main(int argc, char *argv[])
{
    //For clearing the screen
    signal(SIGINT, sigintHandler);
    system("clear");
    if((fd=socket(AF_INET,SOCK_STREAM, 0))<0)
    {
        perror("Error in socket creation :\n");
        exit(1);
    }else
        printf("[+] Socket Created :\n");

    bzero((char *)&srv,sizeof(srv));
    srv.sin_family=AF_INET;
    srv.sin_port=htons(atoi(argv[2]));
    srv.sin_addr.s_addr= inet_addr(argv[1]);

    if(connect(fd,(struct sockaddr*) &srv,sizeof(srv))<0)
    {
        perror("Failed to connect to server :\n");
        exit(1);
    }
    else
        printf("[+] Connected to server.\n");
    //  login check 



    char *user_id=(char*)malloc(500*sizeof(char));

    char *password=(char*)malloc( 500*sizeof(char));
    printf("\n𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣  LOGIN PAGE 𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣𝌣 \n\a");
    // printf("\nૐ ૐ ૐ ૐ ૐ ૐ ૐ 𝐋𝐎𝐆𝐈𝐍 𝐏𝐀𝐆𝐄 ૐ ૐ ૐ ૐ ૐ ૐ \n\a");
    //printf("\n░░░░░░░░░░░░░░░░░░ LOGIN PAGE ░░░░░░░░░░░░░░░░░░\n\a");
    printf("Enter User ID :");
    fgets(user_id,500,stdin);
    //strcpy(user_id,"214101019\n");
    //scanf("%s",user_id);
    int count;
    if((count=write(fd,user_id,strlen(user_id)))<0)
    {
        perror("error in sending user name ");
        exit(1);
    }
    else{
        int size=sizeof(int);
       // printf("%d bytre send user_name\n",count);
        int ack=1;
        if((count=read(fd,&ack,size))<0)
        {
            perror("1. error in recv ack :");
        }
        
    }
    // printf("Type password :");
    // fgets(password,MAXLINE,stdin);


    char pw[MAXPW] = {0};
    char *p = pw;
    FILE *fp = stdin;
    ssize_t nchr = 0;

    printf ( "Enter Password :");
    nchr = getpasswd (&p, MAXPW, '*', fp);
    
    strcpy(password,p);
    
    int i=0;
    while(password[i]!='\0')
    {
        i++;
    }
    
    password[i]='\n';

    //strcpy(password,"00000\n");
    if((count=write(fd,password,strlen(password)))<0)
    {
        perror("Error in sending password ");
        exit(1);
    }
    else
    {
        int ack=1;
        if(read(fd,&ack,sizeof(ack))<0)
        {
            perror("error in recv ack :");
        }
    }
    int user_type;
    if((count=read(fd,&user_type,sizeof(user_type)))<0)
    {
        perror("Error in recv user type");
        exit(1);
    }
    else{
        
        int ack=1;
        if(write(fd,&ack,sizeof(ack))<0)
        {
            perror("error in sending ack :");
        }
        //For clearing the screen
        system("clear");
        if(user_type==0)
        {
            printf("\n░░░░░░░░░░░░░░░░░░ CLIENT PORTAL ░░░░░░░░░░░░░░░░░░\n\a");
            customer_function(user_id);
        }
        else if(user_type==1)
        {
            printf("\n░░░░░░░░░░░░░░░░░░ POLICE PORTAL ░░░░░░░░░░░░░░░░░░\n\a");
            police_function();
        }
        else if(user_type==2)
        {   
            printf("\n░░░░░░░░░░░░░░░░░░ ADMIN PORTAL ░░░░░░░░░░░░░░░░░░\n\a");
            admin_function();

        }
        else{
            printf("Login Credentials invalid... \n");
            exit(0);
        }

    }

   close(fd);
}
