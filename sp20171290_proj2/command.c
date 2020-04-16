#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "20171290.h"
#include "slice.h"
#include "command.h"
#include "subfunc.h"
#define addrend 1048575

//init the 1Mbyte virtual memory
void init_memory(){
   int i,j;
   for(i=0;i<65536;i++){
      for(j=0;j<16;j++){
		 mem[i][j]=0;
      }
   }
}

//function called when the command is 'help' or 'h'
//show all possible commands 
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
   printf("assemble filename\n");
   printf("type filename\n");
   printf("symbol\n");
}

//function called when the command is 'dir' or 'd'
//show all files in the current directory.
//add * for execution file and / for directory at the end of the file name.
void dir_func(){
   DIR *directory;
   struct stat buf;
   struct dirent *file = NULL;
   directory = opendir("./");   //get whole file information in current directory.
   if(directory!=NULL){
      while((file=readdir(directory))!=NULL){
        int length=strlen(file->d_name);
		stat(file->d_name, &buf);
		if(S_ISDIR(buf.st_mode)){	//if the file is directory
		   	printf("%s/\n", file->d_name);
		}
		else if(length>=4 && subcmp(length-4, length, file->d_name, ".out")){ 
			//if the file is execution file (check if the file ends with '.out')
	   		printf("%s*\n", file->d_name);
		}
     	else printf("%s\n", file->d_name);
      }
      closedir(directory);
   }
}

//function called when the command is 'quit' or 'q'
//free all the dynamically allocated memory
void quit_func(){
	//free history linked list
	while(history_head!=NULL){
		str_node *temp=history_head;
		history_head=history_head->ptr;
		free(temp);
	}
	//free opcode hast table linked list
	for(int i=0;i<20;i++){
		op_node *walk=hash_table[i];
		while(walk!=NULL){
			op_node *temp=walk;
			walk=walk->ptr;
			free(temp);
		}
	}
}

//funcion called when the command is 'history' or 'hi'
//by visiting all nodes in the linked list that store all input commands,
//print command history.
void history_func(){
    str_node* walk=history_head;

    while(walk!=NULL){
		printf("%d %s\n", walk->index, walk->sentence);
        walk=walk->ptr;
    }
}

//function called when the command is 'dump' or 'du'
//print the memory within the range mentioned in command.
void dump_func(int startidx, int endidx){
	int startx=startidx/16;
	int starty=startidx%16;
	int i, temp, upper, lower;
	char addr[5]={'0','0','0','0','0'};
	int address=startx;
	int endx=endidx/16;
	int endy=endidx%16;
	while(address<=endx){
	temp=address;
    	int index=3;
		//print the address of the memory with hexadecimal
        while(temp!=0){
        	int rest=temp%16;
            temp/=16;
            if(rest<10) addr[index--]=rest+'0';
            else addr[index--]='A'+(rest-10);
        }
        for(i=0;i<5;i++){
            printf("%c", addr[i]);
            addr[i]='0';
        }
        printf(" ");
		//print the memory of first line
        if(address==startx){
            for(i=0;i<16;i++){
                if((i<starty)||((startx==endx)&&i>endy) ){
                    printf("   "); continue;  //don't print the out of range memory
                }
                upper=(mem[address][i]>>4);	//find the upper half bits of the memory
		   		if(upper<10) printf("%c", upper+'0');
                else printf("%c", 'A'+(upper-10));
                lower=(mem[address][i]&15); //find the lower half bits of the memory
                if(lower<10) printf("%c ", lower+'0');
                else printf("%c ", 'A'+(lower-10));
			}
        }
		//print the middle part of the memory
	    else if((startx<address)&&(address<endx)){
			for(i=0;i<16;i++){
        		upper=(mem[address][i]>>4);  //find the upper half bits of the memory
		    	if(upper<10) printf("%c", upper+'0');
            	else printf("%c", 'A'+(upper-10));
            	lower=(mem[address][i]&15);  //find the lower half bits of the memory
		    	if(lower<10) printf("%c ", lower+'0');
            	else printf("%c ", 'A'+(lower-10));
        	}
	    }
		//print the memory of last line
	    else{
			for(i=0;i<16;i++){
            	if(i>endy){
                	printf("   "); continue;  //don't print the out of range memory
                }
                upper=(mem[address][i]>>4);  //find the upper half bits of the memory
                if(upper<10) printf("%c", upper+'0');
                else printf("%c", 'A'+(upper-10));
                lower=(mem[address][i]&15);  //find the lower half bits of the memory
                if(lower<10) printf("%c ", lower+'0');
                else printf("%c ", 'A'+(lower-10));
            }
	    }
	    printf(";");
		//print the character of the memory
        for(i=0;i<16;i++){
			//print '.' if there are no ascii codes matched with the value or out of range of required memory part.
			if(mem[address][i]<20||mem[address][i]>126||(address==startx&&i<starty)||(address==endx&&i>endy)) printf(".");
            else printf("%c",mem[address][i]);
        }
        printf("\n");
        address++;
	}
}

//function called when the command is 'edit' or 'e'
//change the given memory value into the given value.
void edit_func(){
    int x=start/16;
    int y=start%16;
    mem[x][y]=value;
}

//function called when the command is 'fill' or 'f'
//change the given memory range value into the given value.
void fill_func(){
    int startx=start/16;
    int starty=start%16;
    int endx=end/16;
    int endy=end%16;
    int i, j;
    for(i=startx;i<=endx;i++){
		for(j=0;j<16;j++){
	   		if((i==startx&&j<starty)||(i==endx&&j>endy)) continue;
	    	mem[i][j]=value;
		}
    }
}

//function called when the command is 'opcodelist'
//print all the opcode information stored in the hash table
void opcode_list_func(){
    int i;
    for(i=0;i<20;i++){
		printf("%d : ", i);
		if(hash_table[i]==NULL)	{ printf("\n");  continue; }
		op_node* walk=hash_table[i];
		while(walk!=NULL){
	    	printf("[%s, %s]",walk->command, walk->hexa);
	    	if(walk->ptr!=NULL) printf(" -> ");
	    	walk=walk->ptr;
		}
		printf("\n"); 
    }
}

//function called when the command is 'opcode'
//find out the opcode of the given assembly language
op_node* opcode_func(char* command){
   	int index=(command[0]-'A')%20;
  	op_node* walk=hash_table[index];
   	while(walk!=NULL){
		if(strcmp(walk->command,command)==0 && strlen(command)==strlen(walk->command)){
	  		break;
       	}
       	walk=walk->ptr;
   	}
   	return walk;
}

int type_filename_func(char* filename){
	FILE *fp=fopen(filename, "r");
	if(fp==NULL) return -1;	
	char c;
	//print the text of the file
	while((c=fgetc(fp))!=EOF){
		printf("%c",c);
	}
	return 0;
}

void symbol_func(){
	if(symbol_list==NULL) printf("Empty Symbols\n");
	else{
		symb_node* walk=symbol_list;
		while(walk!=NULL){
			printf("\t %-30s %04X\n", walk->symbol, walk->addr);
			walk=walk->ptr;
		}
	}
}
