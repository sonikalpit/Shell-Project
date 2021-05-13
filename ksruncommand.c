/*
Kalpit Soni
110032443
*/
#include "smallsh.h"
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include<errno.h>
#include <stdbool.h>
#include<fcntl.h>

//commands variables!
char **cmd1=NULL;
char **cmd2=NULL;
char **cmd3=NULL;
char **cmd4=NULL;

//pipe to transfer
int p[2];
// For storing count of total elements in buf
int count;
// to check whether command is | or not
bool pipeSymbol = false;
// to check whether command is > or not
bool redirSymbol = false;
// to check whether command is calc or not
bool calcDec = false;


//for terminating the shell
void termProg() 
{
	exit(0);
}
//for the number of argument passed with command
void cmdnum(int c) {
	count = c;
}

//Changing directory
void changeDir(const char *argv) {
	int st;
	if(argv == NULL){
		st = chdir(getenv("HOME"));
	} else {
		st = chdir(argv);
	}
	
	if(st == -1) {
		switch (errno)
		{
			case ENAMETOOLONG:
				perror("Path is so long");
				break;
			case ENOTDIR:
				perror("Path does not exist");
				break;
			case ENOENT: 
				perror("No File or Directory");
				break; 
			case EACCES:
				perror("Permission is denied");
				break;
			case EIO:
				perror("IO Error");
				break;
		}
	}
	
	
}

void childFunction(char *ex)
{
    int temp,resScan;
	int op1, op2;
	int sum,difference,mult;
    char op;
    float division;
    char buf[500];
     
    write(1,"Child: ",8); 
    temp = sscanf(ex, "%d %c %d", &op1, &op, &op2);  
     
    sleep(1);
    
    if (temp < 3)    
	    exit(50);
     
    if (op == '/' && op2 == 0)
        exit(100);
     
    switch (op) 
    {

		case '+':
            sum = op1+op2;
            resScan=sprintf(buf,"\nResult : %d %c %d = %d\n", op1, op, op2, sum);
            write(1,buf,resScan);
            break;	 
        case '-':
            difference = op1-op2;
            resScan=sprintf(buf,"\n>Result : %d %c %d = %d\n", op1, op, op2,difference);
            write(1,buf,resScan);
            break;    
        case '/':
            division = op1 / op2;
            resScan=sprintf(buf,"\n>Result : %d %c %d = %f\n", op1, op, op2, division);
            write(1,buf,resScan);
            break;
        case '*':
            mult = op1 * op2;
            resScan=sprintf(buf,"\n>Result : %d %c %d = %d\n", op1, op, op2, mult);
            write(1,buf,resScan);
            break;
             
        default:
            exit(200);
            break;
    }
    exit(0);
     
}

// Calculator function - gets only expression and then child will perform the same thing
void calculator() {

	char expr[500];
    int st;
    pid_t pid;
    write(1,"Calculator: \n",14);
     
    while(1)    
    {
        write(1,"Enter the arithmetic statement(ex. 5*5): \n",40);
        
        
       read(0,expr,15);
         
        pid = fork();  
         
        if (pid == -1)  
        {
            perror("Error while forking..\n");
            exit(0);
        }
         
        else if (pid == 0)    
            childFunction(expr);
        
        else
        {
            if (pid > 0)  
            {
                if(strstr(expr,"exit") != NULL) {
                    exit(1);
                }
                write(1,"\nChild Has been created\n",25); 
                wait(&st);  
                 
                if (WEXITSTATUS(st) == 50)  
                    write(1,"Wrong Statement\n",17);
                 
                else if (WEXITSTATUS(st) == 100)    
                    write(1,"Divided by zero\n",17);
                 
                else
                    if (WEXITSTATUS(st) == 200) 
                        write(1,"Wrong Operator\n",16);
            }
        }
         
    }
}

int runcommand(char **cline, int where)
{
	pid_t pid;
	int st;

	//---
	char buf[MAXBUF];
	int fd;


	// Code for checking if command entered is cd or not
	// If command entered is cd then check if any argument is passed or not
	if(strcmp(cline[0],"cd") == 0) {
		if(cline[1] == 0){
			changeDir(NULL);
		}
		else 
		{ 
			changeDir(cline[1]);
		}
	}

	else {
		// appending words to one variable
		for(int i=0;i<count;i++) {
			strcat(buf, " ");
			strcat(buf,cline[i]);
		}
		// checking whether buf has | or not.
		if(strstr(buf,"|") != NULL ) 
		{
			pipeSymbol = true;
			// Splitting two commands between | and white space. 
			char *arg1 = strtok(buf,"|");
			char *arg2 = strtok(NULL,"|");		
			char *temp1   = strtok (arg1, " ");
			// realloc is used to dynamically generate based on the args
			int p = 0;
			while (temp1) {
				cmd1 = realloc(cmd1, sizeof (char*) * ++p);
				if (cmd1 == NULL)
					exit (-1);
				cmd1[p-1] = temp1;
				temp1 = strtok (NULL, " ");
			}
			//NULL has been added!
			cmd1 = realloc(cmd1, sizeof (char*) * (p+1));
			cmd1[p] = 0;
			//same as above for cmd2
			char *temp2 = strtok(arg2, " ");
			int j = 0;
			while (temp2) {
				cmd2 = realloc (cmd2, sizeof (char*) * ++j);
				if (cmd2 == NULL)
					exit (-1);
				cmd2[j-1] = temp2;
				temp2 = strtok (NULL, " ");
			}
			cmd2 = realloc (cmd2, sizeof (char*) * (j+1));
			cmd2[j] = 0;
		}


		//exit if quit or exit are entered
		if(strcmp(cline[0],"exit") == 0 || strcmp(cline[0],"quit") == 0) {
			termProg();
		}


		//same as above but for > symbol
		if(strstr(buf,">") != NULL) {
			redirSymbol = true;
			char *arg1 = strtok(buf,">");
			char *arg2 = strtok(NULL,">");		
			char *temp1   = strtok (arg1, " ");
			int l = 0;
			while (temp1) {
				cmd3 = realloc (cmd3, sizeof (char*) * ++l);
				if (cmd3 == NULL)
					exit (-1);
				cmd3[l-1] = temp1;
				temp1 = strtok (NULL, " ");
			}
			cmd3 = realloc (cmd3, sizeof (char*) * (l+1));
			cmd3[l] = 0;
			char *temp2   = strtok(arg2, " ");
			int k = 0;
			while (temp2) {
				cmd4 = realloc (cmd4, sizeof (char*) * ++k);
				if (cmd4 == NULL)
					exit (-1);
				cmd4[k-1] = temp2;
				temp2 = strtok (NULL, " ");
			}
		}

		if(strcmp(cline[0],"calculator") == 0) {
			calcDec = true;
		}
	
	
		//Child is doing its execution of commands
		switch (pid = fork()) {
			case -1:
				perror("Error....");
				return (-1);
			case 0:
				if(pipeSymbol == true) {
					int pid1,st;
					if(pipe(p) == -1) {
						perror("Error piping \n");
					}
					pid1 = fork();
					if(pid1 == -1) {
						perror("Error while creating process \n");
					}
					//for >
					if(pid1 == 0){
						//piping
						dup2(p[1], 1);
						close(p[0]);
						close(p[1]);
						execvp(cmd1[0],cmd1);
						perror(*cmd1);
						exit(3);
					}
					//for |
					if(pid1 > 0) {
						waitpid(pid1,&st,0);
						//piping
						dup2(p[0], 0);
						close(p[0]);
						close(p[1]);
						execvp(cmd2[0],cmd2);
						perror(*cmd2);
						free(cmd1);
						free(cmd2);
						memset(buf,0,MAXBUF);
						exit(2);
					}
				}
				else if(pipeSymbol == false && redirSymbol == false && calcDec == false) { 
					execvp(cline[0], cline);
					perror(*cline);
					exit(1);
				}
				else if(redirSymbol == true) {
					int fd = open(cmd4[0],O_WRONLY|O_CREAT|O_TRUNC,0777);
					if(fd == -1) {
						perror("Error Opening File\n");
					}
					dup2(fd,1);
					execvp(cmd3[0],cmd3);
					close(fd);
					free(cmd3);
					free(cmd4);
					memset(buf,0,MAXBUF);
					exit(3);
				} 
				else if(calcDec == true) {
					calculator();
					exit(4);
				}
		}
	}

	//code for parents
	if (where == BACKGROUND)
	{
		printf("[process id %d]\n", pid);
		return (0);
	}
	
	if (waitpid(pid, &st, 0) == -1)
		return (-1);
	else{
		pipeSymbol = false;
		redirSymbol = false;
		calcDec = false;
		memset(buf,0,MAXBUF);
		return (st);
	}
}