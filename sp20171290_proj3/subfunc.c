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

	int i;
	for(i=0;i<20;i++){
		hash_table[i]=NULL;
	}
	progaddr = 0;
	before_prog_length = 0;
	current_prog_addr = 0;
	execution_addr = 0;
	estab_head = NULL;
	estab_tail = NULL;
	refer_head = NULL;
	refer_tail = NULL;
	bpoint_head = NULL;
	bpoint_tail = NULL;
	totalLength = 0;	
	A=0, X=0, L=0, PC=0, B=0, S=0, T=0;
	loc=0;
	compare=0;
}

//To get recent assemble file symbol, free the symbol list made before.
void free_symbol(){
	while(symbol_list!=NULL){
		symb_node *temp=symbol_list;
		symbol_list=symbol_list->ptr;
		free(temp);
	}
}

estab* find_progname(char* name){
	estab* walk=estab_head;
	while(walk!=NULL){
		if(strcmp(walk->control, name)==0){
			 break;
		}
		walk=walk->ptr;
	}
	return walk;
}

estab* find_progsymb(char* name){
	estab* walk=estab_head;
	while(walk!=NULL){
		if(strcmp(walk->symbol, name)==0) break;
		walk=walk->ptr;
	}
	return walk;
}

refer* find_refer(char* number){
	refer* walk = refer_head;
	while(walk!=NULL){
		if(strcmp(walk->number, number)==0) break;
		walk=walk->ptr;
	}
	return walk;
}

bpoint* find_bpoint(int number){
	bpoint* walk= bpoint_head;
	while(walk!=NULL){
		if(walk->breakpoint == number) break;
		walk=walk->ptr;
	}
	return walk;
}

void free_estab(){
	while(estab_head!=NULL){
		estab* temp = estab_head;
		estab_head = estab_head->ptr;
		free(temp);
	}
	estab_head=NULL;
	estab_tail=NULL;
	before_prog_length=0;
}

void print_bp(){
	printf("\tbreakpoint\n");
	printf("\t----------\n");

	bpoint* walk = bpoint_head;
	while(walk!=NULL){
		printf("\t%X\n", walk->breakpoint);
		walk=walk->ptr;
	}
}

void free_bp(){
	while(bpoint_head!=NULL){
		bpoint* walk = bpoint_head;
		bpoint_head = bpoint_head->ptr;
		free(walk);
		walk=walk->ptr;
	}
	bpoint_head=NULL;
	bpoint_tail=NULL;
}

void print_registers(boolean end, int bp){
	printf("A : %06X   X : %06X\n", A, X);
	printf("L : %06X  PC : %06X\n", L, PC);
	printf("B : %06X   S : %06X\n", B, S);
	printf("T : %06X\n", T);
	if(end==true){
		printf("\t End Program\n");
	}
	else{
		printf("\t Stop at checkpoint[%X]\n", bp);
	}
}

op_node* find_opinst(char* hexa){
	int i;
	op_node* walk = NULL;
	for(i=0;i<20;i++){
		boolean findInst = false;
		walk= hash_table[i];
		while(walk!=NULL){
			if(strcmp(walk->hexa, hexa)==0){
				findInst = true;
				break;
			}
			walk=walk->ptr;
		}
		if(findInst == true) break;
	}
	return walk;
}

void set_register(int reg, int value){
	if(reg==0){ A=value;}
	if(reg==1){ X=value;}
	if(reg==2){ L=value;}
	if(reg==8){ PC=value;}
	if(reg==3){ B=value;}
	if(reg==4){ S=value;}
	if(reg==5){ T=value;}
}

int get_register(int reg){
	if(reg==0) return A;
	if(reg==1) return X;
	if(reg==2) return L;
	if(reg==8) return PC;
	if(reg==3) return B;
	if(reg==4) return S;
	if(reg==5) return T;
	return -1;
}

void init_reg(){
	int i, j;
	for(i=0;i<65536;i++){
		for(j=0;j<16;j++){
			reg[i][j]=0;
		}
	}
}
