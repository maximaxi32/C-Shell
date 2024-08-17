#include  <stdio.h>
#include  <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
void controlc(int sig) {
  printf("\nforeground process terminated\n");
}
bool bgindicator=false;

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
  //SIGINT handling
  signal(SIGINT,controlc);
  //to keep track of background processes
  bool bgp[64]={false};
  int bgpid[64];
  memset(bgpid,-1,sizeof(bgpid));
  

	while(1) {			
    /*

for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
*/
     //reap background processes
     if(bgindicator) {
      int k=0;      //to set the value of bgindicator
      for(i=0;i<64;i++) {
        
        if(bgp[i]) {
          k=-1;
          int r=waitpid(bgpid[i],NULL,WNOHANG);
          if(r==bgpid[i]) {
            printf("Shell: Background process with PID %d finished \n",r);
            bgpid[i]=-1;bgp[i]=false;
            
          }
        }
      }
      if(k==0) {
        bgindicator=false;
      }
     }

     //to check if background process invoked in this iteration
    bool bgcurrent=false;
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them
         // START

    
	   if(tokens[0]==NULL) {
		continue;
	   }
      int j=1;
      //bg process invoke detection
      while(tokens[j]) {
        if(strcmp(tokens[j],"&")==0) {
          bgindicator=true;
          bgcurrent=true;
          break;
        }
        j++;
      }
     
         //cd handling,incorrect commands also detected
     if(strcmp(tokens[0],"cd")==0) {
                
                     /* if(tokens[2]) {
                      printf("Shell:Incorrect\n");
                     } */
  

                  if(tokens[2]) {
                    printf("Incorrect cd command\n");
                  }
                  else if(!tokens[1]) {
                    printf("Mention directory\n");
                  }
                else if(strcmp(tokens[1],"..")==0) {
                    chdir("..");
                }
                else {
                    int k=chdir(tokens[1]);
                    if(k==-1) {
                        printf("Shell:Incorrect\n");
                    }
                }
                continue;
            }
          //handled exit
      else if(strcmp(tokens[0],"exit")==0) {
           if(bgindicator) {
            for(i=0;i<64;i++) {
              if(bgp[i]) {
                kill(bgpid[i],SIGKILL);
                waitpid(bgpid[i],NULL,0);
                bgp[i]=false;
                bgpid[i]=-1;
              }
            }
           }
           printf("Shell Exiting\n");
           for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
       exit(0);
      }
 
         //handle background processes
      else if(bgcurrent) {
        
       
         
        char **tokens1 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
        /*for(i=0;i<strlen(line)-1;i++) {
            line[i]=line1[i];
              
        }  */
       
       int j=0;
       
       while(strcmp(tokens[j],"&")) {
        tokens1[j]=tokens[j];
        j++;
       }
       // tokens1=tokenize(line1);
           int ret=fork();
            //fork failure
           if(ret<0) {
            fprintf(stderr,"Fork failed\n");
           }
           //child
           else if(ret==0) {
            setpgid(0,0);
               execvp(tokens1[0],tokens1);
               printf("Invalid Command.Try Something Simpler?\n");
            exit(1);

           }
           //parent
           else {
               
               for(i=0;i<64;i++) {

                if(bgp[i]) {
                  continue;
                }
                else {
                  bgp[i]=true;
                  bgpid[i]=ret;
                  break;
                }
                
               }
           }
      }     


           //foreground process execution
            else {
        int ret=fork();

     //fork failure
       if(ret<0) {
            fprintf(stderr,"Fork failed\n");
        }
     //child
       else if(ret==0) {
            execvp(tokens[0],tokens);
            
             
              //printf("%s\n",tokens[0]);
            printf("Invalid Command.Try Something Simpler?\n");
            exit(1);    

        }
        //parent
        else {
            int wc=waitpid(ret,NULL,0);
        }
            }
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
