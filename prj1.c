#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef enum boolean{false, true} boolean;

typedef struct str_node{
    int index;
    char sentence[150];
    struct str_node* ptr;
}str_node;

str_node *history_head = NULL;
str_node *history_tail = NULL;
int historynum = 1;

void push(char* str, int index){
    str_node* new = (str_node*)malloc(sizeof(str_node));
    new->index=historynum++;
    strncpy(new->sentence, str, index);
    new->ptr=NULL;
    if(history_head==NULL){
	history_head=new;
    }
    if(history_tail!=NULL){
	history_tail->ptr=new;
    }
    history_tail=new;
}

boolean subcmp(int start, int end, char* str1, char* str2){
    int i;
    boolean ret=true;
    for(i=start;i<end;i++){
        if(str1[i]!=str2[i-start]) ret=false;
    }
    return ret;
}

char sentence[150];

void help_func(){
   printf("h[elp]\n");
   printf("d[ir]\n");
   printf("q[uit]\n");
   printf("hi[story]\n");
   printf("du[mp] [start, end]\n");
   printf("e[dit] address, value\n");
   printf("f[ill] start, end, value\n");
   printf("reset\n");
   printf("opcode mnemonic\n");
   printf("opcodelist\n");
}

void dir_func(){
   DIR *directory;
   struct stat buf;
   struct dirent *file = NULL;
   directory = opendir("./");
   if(directory!=NULL){
      while((file=readdir(directory))!=NULL){
        int length=strlen(file->d_name);
	stat(file->d_name, &buf);
	if(S_ISDIR(buf.st_mode)){
	   printf("%s/\n", file->d_name);
	}
	else if(length>=4 && subcmp(length-4, length, file->d_name, ".out")){
	   printf("%s*\n", file->d_name);
	}
     	else printf("%s\n", file->d_name);
      }
      closedir(directory);
   }
}

void history_func(){
    str_node* walk=history_head;
    while(walk!=NULL){
	printf("%d %s\n", walk->index, walk->sentence);
        walk=walk->ptr;
    }
}

void dump_func(){
    printf("dump\n");
}
void main(){

    while(1){
    	printf("sicsim>");
    	char c;
	int index=0;
    	while((c=getchar())!='\n'){
       	   sentence[index++]=c;
    	}
	if((strncmp(sentence, "help",4)==0)||((strncmp(sentence, "h",1)==0)&&index==1)){
	    push(sentence, index);
	    help_func();
	}
	else if((strncmp(sentence, "dir",3)==0)||((strncmp(sentence,"d",1)==0)&&index==1)){
	    push(sentence, index);
            dir_func();
	}
	else if((strncmp(sentence, "quit", 4)==0)||(strncmp(sentence, "q", 1)==0)){
	    break;
	}
	else if((strncmp(sentence, "history", 7)==0)||(strncmp(sentence, "hi", 2)==0)){
            push(sentence, index);
            history_func();
        }
	else if((strncmp(sentence, "dump", 4)==0)||(strncmp(sentence, "du", 2)==0)){
	    push(sentence, index);
	    dump_func();
	}
    }
}
