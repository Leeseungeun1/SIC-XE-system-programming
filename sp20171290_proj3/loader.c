#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "20171290.h"
#include "loader.h"
#include "subfunc.h"
#include "command.h"
#include "slice.h"
#define addrend 1048575

int loader_pass1(char* filename){
	FILE *fp = fopen(filename, "r");
	if(fp==NULL) return -1;
	char s[300];
	int i, sixteen, size;
	//make estab
	while(fgets(s, sizeof(s), fp)!=NULL){	//read record
		if(s[0]=='H'){
			estab* new = (estab*)malloc(sizeof(estab));
			new->ptr=NULL;
			(new->symbol)[0]='\0';
			for(i=0;i<6;i++){
				(new->control)[i]=s[i+1];
			}
			(new->control)[6]='\0';		//store program name
			if(find_progname(new->control)!=NULL){	//same program name
				free(new);
				fclose(fp);
				return 1;
			}
			//store length
			sixteen = 1;
			size =strlen(s)-1;	//strlen(s) includes '\n'
			int length = 0;
			for(i=size-1;i>=size-6;i--){
				if('A'<=s[i]&&s[i]<='F') length=length+(s[i]-'A'+10)*sixteen;
				else length = length+(s[i]-'0')*sixteen;
				sixteen=sixteen*16; 	
			}
			new->length = length;
			new->address = before_prog_length + progaddr;
			before_prog_length += length;
			current_prog_addr=new->address;
			if(current_prog_addr>addrend){
				free(new);
				return 3;
			}
			link_to_estab(new);
		}
		else if(s[0]=='D'){
			int index=1;
			size=strlen(s)-1;
			while(index<size){
				estab* new = (estab*)malloc(sizeof(estab));
				new->ptr=NULL;
				(new->control)[0]='\0';
				for(i=0;i<6;i++){
					(new->symbol)[i]=s[index++];
				}
				(new->symbol)[6]='\0';
				if(find_progsymb(new->symbol)!=NULL){
					free(new);
					fclose(fp);
					return 1;
				}
				//store address
				sixteen = 1;
				int curaddr = 0;
				for(i=index+5;i>=index;i--){
					if('A'<=s[i]&&s[i]<='F') curaddr = curaddr+(s[i]-'A'+10)*sixteen;
					else curaddr = curaddr+(s[i]-'0')*sixteen;
					sixteen = sixteen*16;
				}
				index=index+6;
				new->address = current_prog_addr+curaddr;
				if(new->address>addrend){
					free(new);
					 return 3;
				}
				new->length = 0;
				link_to_estab(new);
			}
		}		
	}
	fclose(fp);
	return 0;
}

int loader_pass2(char* filename){
	FILE *fp = fopen(filename, "r");
	if(fp==NULL) return -1;	
	char s[300];
	int i,size, startaddr, sixteen, memnum, taddr;
	while(fgets(s, sizeof(s), fp)!=NULL){
		if(s[0]=='H'){
			//set start address
			char name[6];
			for(i=0;i<6;i++){
				name[i]=s[i+1];
			}
			name[6]='\0';
			estab* prog=find_progname(name);
			startaddr = prog->address;
			refer* new = (refer*)malloc(sizeof(refer));
			new->ptr=NULL;
			new->number[0]='0';
			new->number[1]='1';
			new->number[2]='\0';
			strncpy(new->name, name, 7);
			link_to_refer(new);
		}
		if(s[0]=='T'){
			//find_start_address
			sixteen=1;
			taddr=startaddr;
			for(i=6;i>=1;i--){
				if('A'<=s[i]&&s[i]<='F') taddr=taddr+(s[i]-'A'+10)*sixteen;
				else if('0'<=s[i]&&s[i]<='9') taddr = taddr+(s[i]-'0')*sixteen;
				sixteen = sixteen*16;
			}
			for(i=9;i<strlen(s)-1;i=i+2){
				//make the address to hexadecimal string
				memnum=0;
				if('A'<=s[i]&&s[i]<='F') memnum+=(s[i]-'A'+10)*16;
				else memnum+=(s[i]-'0')*16;
				if('A'<=s[i+1]&&s[i+1]<='F') memnum+=(s[i+1]-'A'+10)*1;
				else memnum+=(s[i+1]-'0')*1;
				int x=taddr/16;
				int y=taddr%16;
				mem[x][y]=memnum;
				taddr++;
			}	
		}
		if(s[0]=='R'){
			int index = 1;
			size = strlen(s);
			while(index<size){
				refer* new = (refer*)malloc(sizeof(refer));
				new->ptr=NULL;
				for(i=0;i<2;i++){
					(new->number)[i]=s[i+index];
				}
				(new->number)[2]='\0';
				index=index+2;
				for(i=0;i<6;i++){
					if(i+index>=size || s[i+index]=='\n') (new->name)[i]=' ';
					else (new->name)[i]=s[i+index];
				}
				(new->name)[6]='\0';
				index=index+6;
				link_to_refer(new);
			}	
		}
		if(s[0]=='M'){
			char oper, mNumber[3];
			sixteen=1;
			int modaddr=0, bytenum=0, modmem=0, charsize=1;
			for(i=6;i>0;i--){
				if('A'<=s[i]&&s[i]<='F') modaddr+=(s[i]-'A'+10)*sixteen;
				else if('0'<=s[i]&&s[i]<='9') modaddr+=(s[i]-'0')*sixteen;
				sixteen=sixteen*16;
			}
			modaddr+=startaddr;

			sixteen=1;
			for(i=8;i>6;i--){
				if('A'<=s[i]&&s[i]<='F') bytenum+=(s[i]-'A'+10)*sixteen;
				else if('0'<=s[i]&&s[i]<='9') bytenum+=(s[i]-'0')*sixteen;
				sixteen = sixteen*16;
			}
			bytenum=(bytenum/2)+(bytenum%2);
			oper=s[9];		
	
			for(i=0;i<2;i++){
				mNumber[i]=s[i+10];
			}
			mNumber[2]='\0';

			refer* estabSYM = find_refer(mNumber);
			if(estabSYM==NULL) return 2;
			estab* est;
			if(strcmp(mNumber, "01")==0) est = find_progname(estabSYM->name);			
			else est = find_progsymb(estabSYM->name);
			
			if(est==NULL)return 2;
			for(i=bytenum+modaddr-1;i>=modaddr;i--){
				int x=i/16;
				int y=i%16;
				int memory=mem[x][y];
				modmem+=memory*charsize;
				charsize=charsize*256;	
			}

			if(oper=='+') modmem=modmem+(est->address);
			else if(oper=='-') modmem = modmem-(est->address);
	
			for(i=bytenum+modaddr-1;i>=modaddr;i--){
				int x=i/16;
				int y=i%16;
				mem[x][y]=modmem%256;
				modmem=modmem/256;
			}
		}
		if(s[0]=='E'){
			sixteen=1;
			execution_addr=0;
			for(i=6;i>0;i--){
				if('A'<=s[i]&&s[i]<='F') execution_addr+=(s[i]-'A'+10)*sixteen;
				else if('0'<=s[i]&&s[i]<='9') execution_addr+=(s[i]-'0')*sixteen;
				sixteen=sixteen*16;
			}
		}
	}
	fclose(fp);
	free_refer();
	return 0;
}

void link_to_estab(estab* new){
	if(estab_head==NULL){
		estab_head=new;
	}
	if(estab_tail!=NULL){
		estab_tail->ptr=new;
	}
	estab_tail=new;
}

void link_to_refer(refer* new){
	if(refer_head==NULL){
		refer_head=new;
	}
	if(refer_tail!=NULL){
		refer_tail->ptr=new;
	}
	refer_tail=new;
}

void free_refer(){
	while(refer_head!=NULL){
		refer* walk = refer_head;
		refer_head = refer_head->ptr;
		free(walk);
	}
	refer_head=NULL;
	refer_tail=NULL;
}

int check_bound(){
	estab *walk=estab_head;
	while(walk!=NULL){
		totalLength = totalLength + walk->length;
		walk=walk->ptr;
	}
	if(progaddr+totalLength>addrend){
		return 3;
	}
	return 0;
}

void print_estab(){
	printf("control symbol address length\n");
	printf("section name\n");
	printf("--------------------------------\n");
	estab* walk=estab_head;
	while(walk!=NULL){
		printf("%6s ",walk->control);
		printf("%-6s ", walk->symbol);
		printf("   %04X ", walk->address);
		if(walk->length!=0) {
			printf("   %04X ", walk->length);
		}	
		printf("\n");
		walk=walk->ptr;
	}
	 printf("--------------------------------\n");
	 printf("\t\ttotal length %04X\n", totalLength);
	 
	//init registers
	A=0;
	B=0;
	X=0;
	S=0;
	T=0;
	PC=progaddr;
	L = totalLength;
	init_reg();
}
