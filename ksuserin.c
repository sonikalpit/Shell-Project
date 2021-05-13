/*
Kalpit Soni
110032443
*/
#include "smallsh.h"
#include<unistd.h>
#include<string.h>
#include<signal.h>

//Signal handler for alram installed below to display whether user has not entered enter key within 10 seconds
void signalHndlr() {
	printf("\nPress Enter key else you will get annoyed \n");
}


// command arg1 arg2 arg3
static char inpbuf[MAXBUF], tokbuf[2 * MAXBUF],
*ptr = inpbuf, *tok = tokbuf;


int inarg(char c);

int userin(char *p)
{
	//Set the handler for alarm
	signal(SIGALRM,signalHndlr);
	int c, count;
	ptr = inpbuf;
	tok = tokbuf;
	
	//Initial alarm called....
	alarm(10);

    //asci value of enter key is 10 so checking it. and then alarm will be called after 10 seconds.
	if(c == 10) {
		alarm(10);
	}

	// getting current working directory
	char dir[512];
	p = strcat(getcwd(dir,sizeof(dir)), "$");//strcat((getcwd(dir,sizeof(dir)),"$"));
	printf("%s", p);

	count = 0;
	while (1)
	{
		if ((c = getchar()) == EOF)
			return(EOF);
		if (count < MAXBUF)
			inpbuf[count++] = c;
		if (c == '\n' && count < MAXBUF)
		{
			inpbuf[count] = '\0';
			return count;
		}
		
		if (c == '\n') 
		{
			printf("smallsh: input line too long\n");
			count = 0;
			printf("%s ", p);
		}
	}

}

int gettok(char **outptr)
{
	int type;
	
	*outptr = tok;
	
	while (*ptr == ' ' || *ptr == '\t')
		ptr++;
	
	*tok++ = *ptr;
	
	switch (*ptr++) {
	case '\n':
		type = EOL;
		break;
	case '&':
		type = AMPERSAND;
		break;
	case ';':
		type = SEMICOLON;
		break;
	default:
		type = ARG;
		
		while (inarg(*ptr))
			*tok++ = *ptr++;
	}
	*tok++ = '\0';
	return type;
}

//****************************************************************************************
static char special[] = { ' ', '\t', '&', ';', '\n', '\0' };
int inarg(char c)
{
	char *wrk;
	for (wrk = special; *wrk; wrk++)
	{
		if (c == *wrk)
			return (0);
	}
	
	return (1);
}