/*
Kalpit Soni
110032443
*/
#include "smallsh.h"
#include<unistd.h>
#include<string.h>

char dir[512];
char *prompt;

int userin(char *p);
void procline(void);

int main()
{
	prompt = strcat(getcwd(dir,sizeof(dir)), "$");
	while (userin(prompt) != EOF)
		procline();
}
