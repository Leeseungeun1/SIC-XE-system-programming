#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "20171290.h"
#include "pass1.h"
#include "subfunc.h"
#include "command.h"

boolean find_symbol(char* symbol){
	boolean ret=false;
	symb_node* walk=symbol_list;
	while(walk!=NULL){
		if(strcmp(walk->symbol, symbol)==0) ret=true;
		walk=walk->ptr;
	}
	return ret;
}

void push_symbol(char* symbol, int address){
	symb_node* new=(symb_node*)malloc(sizeof(symb_node));
	strcpy(new->symbol,symbol);
	new->addr=address;
	new->ptr=NULL;
	if(symbol_list==NULL||strcmp(symbol_list->symbol, symbol)>0){
		new->ptr=symbol_list;
		symbol_list=new;
		return;
	}
	symb_node* walk=symbol_list;
	int flag=1;
	while(flag){
		if(strcmp(walk->symbol, symbol)<0 && walk->ptr==NULL){
			walk->ptr=new;
			flag=0;
		}
		else if(strcmp(walk->symbol, symbol)<0&&strcmp((walk->ptr)->symbol,symbol)>0){
			new->ptr=walk->ptr;
			walk->ptr=new;
			flag=0;
		}
		else walk=walk->ptr;
	}
	
}

int pass_one(char* filename){
	FILE *fp=fopen(filename, "r");
	if(fp==NULL) return -1;
	if(symbol_list!=NULL) free_symbol();
	char temp[200];
	int line=0, address=0,i, number, index,size, ret=0;
	while(1){
		fgets(temp, sizeof(temp), fp);
		temp[strlen(temp)-1]='\0';
		size=strlen(temp);
		line+=5;
		list_node* new=(list_node*)malloc(sizeof(list_node));
		new->line=line; new->ptr=NULL;
		index=0; number=0;
		if(temp[0]=='.'){	
			strcpy(new->assembly[0], ".");
			new->argnum=1;
		}
		else{
			for(i=0;i<size;i++){
				if(i==0&&temp[0]==' '){
					strcpy(new->assembly[0], "-"); number++;
				}
				else if(temp[i]==' '&&index!=0){
					new->assembly[number++][index]='\0'; index=0;
				}
				else if(temp[i]!=' '){
					new->assembly[number][index++]=temp[i];
				}
			}
			new->assembly[number++][index]='\0';
			new->argnum=number;
			new->loc=address;
			if(strcmp(new->assembly[1], "START")==0){
				strcpy(program_title, new->assembly[0]);  //store the program title
				//save a starting address
				size=strlen(new->assembly[2]);
				int sixteen=1;
				for(i=size-1;i>=0;i--){
					address+=((new->assembly[2])[i]-'0')*sixteen;
					sixteen=sixteen*16;
				}
				//initialize LOCCTR to starting address
				new->loc=address;
			}
			else {
				//if there is a symbol in the LABEL field
				 if(strcmp(new->assembly[0],"-")!=0){
					//check the existence of symbol
					if(find_symbol(new->assembly[0])==true){
						assemble_error=2;
						ret=new->line;
					}
					else{
						push_symbol(new->assembly[0], address);
					}
				}
				op_node* found;
				if(new->assembly[1][0]=='+'){			
					found=opcode_func(new->assembly[1]+1);
				}
				else found=opcode_func(new->assembly[1]);
				if(found!=NULL){
					if(strcmp(found->format,"1")==0) address+=1;
					if(strcmp(found->format,"2")==0) address+=2;
					else{
						if(new->assembly[1][0]=='+') address+=4;
						else address+=3;
					}
				} 
				else if (strcmp(new->assembly[1], "END")!=0){
					if(strcmp(new->assembly[1], "WORD")==0) address+=3;
					else if(strcmp(new->assembly[1], "RESW")==0){
						size=strlen(new->assembly[2]);
						int ten=1, sum=0;
						for(i=size-1;i>=0;i--){
							sum+=((new->assembly[2])[i]-'0')*ten;
							ten=ten*10;
						}
						address+=(3*sum);
					}
					else if(strcmp(new->assembly[1], "RESB")==0){
						size=strlen(new->assembly[2]);
						int ten=1, sum=0;
						for(i=size-1;i>=0;i--){
							sum+=((new->assembly[2])[i]-'0')*ten;
							ten=ten*10;
						}
						address+=sum;
					}
					else if(strcmp(new->assembly[1], "BYTE")==0){
						index=0;
						if((new->assembly[2])[0]=='X') address+=1;
						else{
							index=index+2;
							while((new->assembly[2])[index++]!='\'') address+=1;
						}
					}
					else if (strcmp(new->assembly[1],"BASE")!=0){
						assemble_error=3;
						ret=new->line;
					}
				}
			}
		}
		if(list_head==NULL) list_head=new;
		if(list_tail!=NULL) list_tail->ptr=new;
		list_tail=new;
		if(strcmp(new->assembly[1], "END")==0) break;
	}	
	return ret;
}
