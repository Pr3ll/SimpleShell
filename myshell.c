#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_PATH 3

int main(int argc, char *argv[]){
	char line[256], pathPtr[MAX_PATH][25];
	char command[25], comNloc[30];
	char str[1]; 
	char *comOpt[10], optTmp[10];
	int pathFd;
	int found, status;
	
	int pth_index1 = 0, pth_index2=0;

	/* reset pathPtr array */
	for(int a = 0; a<MAX_PATH; a++){
		memset(pathPtr[a], 0, 25);
	}
	
	/* .myshell file open */
	if((pathFd = open(".myshell", O_RDONLY)) == -1){
		perror(argv[1]);
		exit(3);
	}
	
	/* read .myshell file and find PATH route */
	while(read(pathFd, str, 1) > 0){
		if(str[0] == '='){break;}
	}
	while(read(pathFd, str, 1) > 0){
		if(str[0] == ':'){
			pth_index2 = 0;
			pth_index1++; 
			continue;
		}
		pathPtr[pth_index1][pth_index2] = str[0];
		pth_index2++;
	}
	/*printf("%s\n%s\n%s\n", pathPtr[0], pathPtr[1], pathPtr[2]);*/

	/* loop start */
	while(1){
		/* variable and array reset */
		found = 0;
		memset(command, 0, 25);
		memset(optTmp, 0, 10);
		
		command[0] = '/';
		
		printf("%% ");
		fgets(line, 255, stdin);
		
		for(int b=0; b<10; b++){
			comOpt[b] = (char *)malloc(64);
		}
		
		/* extract command in "line" */
		for(int c=0; c<strlen(line); c++){
			if(line[c]==' ' || line[c] == '\0' || line[c] == '\n'){
				break;
			}
			command[c+1] = line[c];
		}
		
		/* split options by ' ' and save options to comOpt array */
		for(int d=0, index1=0, index2=0; d<strlen(line); d++){
			if(line[d]==' '){
				strcpy(comOpt[index1], optTmp);
				memset(optTmp, 0, 10);
				index1++;
				index2 = 0;				
				continue;
			}
			else if(line[d] == '\0' || line[d] == '\n'){
				strcpy(comOpt[index1], optTmp);
				comOpt[index1+1] = (char *)NULL;
				memset(optTmp, 0, 10);
				break;
			}
			optTmp[index2] = line[d];
			index2++;
		}
	
		/* find command in pathPtr path */
		for(int e=0; e<MAX_PATH; e++){
			found = 0;
			strcpy(comNloc, pathPtr[e]);
			strcat(comNloc, command);
			
			if(access(comNloc, F_OK) == 0){
				found = 1;
				break;
			}
		}
			
		/* execute command with option or prints "not found" message */
		if(found == 1){
			if(fork() == 0){
				execv(comNloc, comOpt);
				exit(1);
			}
			wait(&status);
		}
		else{
			printf("command not found\n");
		}
	}
}
