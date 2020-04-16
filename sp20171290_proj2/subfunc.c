#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "20171290.h"
#include "subfunc.h"

//construct hast table with the information in 'opcode.txt'
void build_hash_table(){
   FILE *fp=fopen("opcode.txt", "r");
   if(!fp){
	  //error : no file to read
      printf("There is no opcode.txt\n");
      opcode_file_error=true;
      return;	
   }
   while(1){
		//construct hash table.
		op_node* node=(op_node*)malloc(sizeof(op_node));
		if(fscanf(fp, "%s %s %s\n", node->hexa, node->command, node->format)==EOF) break;
		(node->hexa)[2]='\0'; (node->command)[strlen(node->command)]=(node->format)[strlen(node->format)]='\0';
		int index=(node->command[0]-'A')%20;  //hash function
		node->ptr=hash_table[index];
		hash_table[index]=node;
   }
   fclose(fp);
}

//add correct command into the history linked list
void push(char* str, int index){
    str_node* new = (str_node*)malloc(sizeof(str_node));
    new->index=historynum++;
    strncpy(new->sentence, str, index);
    (new->sentence)[index]='\0';
	new->ptr=NULL;
	 if(history_head==NULL){
		history_head=new;
	}
	if(history_tail!=NULL){
		history_tail->ptr=new;
	}
	history_tail=new;
}

//check if the two strings are same
boolean subcmp(int start, int end, char* str1, char* str2){
    int i;
    boolean ret=true;
    for(i=start;i<end;i++){
        if(str1[i]!=str2[i-start]) ret=false;
    }
    return ret;
}

//init the global variable when the program is initiated.
void init_global_variable(){
	history_head=NULL;
	history_tail=NULL;
	historynum=1;
	startaddress=0;
	opcode_file_error=false;
	parameter_error=false;
	symbol_list=NULL;
	list_head=NULL;
	list_tail=NULL;
	program_length=0;
	mod_head=NULL;
	mod_tail=NULL;
	base_reg=0;
}

//To get recent assemble file symbol, free the symbol list made before.
void free_symbol(){
	symb_node *walk=symbol_list;
	while(walk!=NULL){
		symb_node *temp=walk;
		walk=walk->ptr;
		free(temp);
	}
}
