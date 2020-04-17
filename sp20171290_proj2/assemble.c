#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "20171290.h"
#include "assemble.h"
#include "subfunc.h"
#include "command.h"

symb_node* find_symbol(char* symbol){
	symb_node* walk=symbol_list;
	while(walk!=NULL){
		if(strcmp(walk->symbol, symbol)==0) break;
		walk=walk->ptr;
	}
	return walk;
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

void push_modification(list_node* list){
	mod_node* new=(mod_node*)malloc(sizeof(mod_node));
	new->address=list->loc+1;
	new->num=5;
	new->ptr=NULL;
	if(mod_head==NULL) mod_head=new;
	if(mod_tail!=NULL) mod_tail->ptr=new;
	mod_tail=new;
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
		new->obj_flag=true;
		index=0; number=0;
		if(temp[0]=='.'){	
			strcpy(new->assembly[0], ".");
			new->argnum=1;
		}
		else{
			strcpy(new->inst, temp);
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
			if(index!=0) new->assembly[number++][index]='\0';
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
					if(find_symbol(new->assembly[0])!=NULL){
						assemble_error=2;
						ret=new->line;
						break;
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
				else{
					if(strcmp(new->assembly[1], "END")==0){
						program_length=address-(list_head->loc);
					}
					else if(strcmp(new->assembly[1], "WORD")==0) address+=3;
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
						index=2;
						int sum=0;
						while((new->assembly[2])[index++]!='\'') sum+=1;
					 	if((new->assembly[2])[0]=='X') sum/=2;
						address+=sum;
					}
					else if (strcmp(new->assembly[1],"BASE")!=0){
						assemble_error=3;
						ret=new->line;
						break;
					}
				}
			}
		}
		if(list_head==NULL) list_head=new;
		if(list_tail!=NULL) list_tail->ptr=new;
		list_tail=new;
		if(strcmp(new->assembly[1], "END")==0) break;
	}
	fclose(fp);	
	return ret;
}

int pass_two(){
	if(list_head==NULL){
		printf("No Code\n");
		return -1;
	}
	int ret=0, i, size;
	list_node* walk=list_head;
	while(strcmp(walk->assembly[1], "END")!=0){
		if((walk->assembly[0])[0]=='.'||strcmp(walk->assembly[1], "START")==0||
			strcmp(walk->assembly[1], "RESW")==0||strcmp(walk->assembly[1], "RESB")==0){
			walk->obj_flag=false;
			walk=walk->ptr;
			continue;
		}
		else if(strcmp(walk->assembly[1], "BASE")==0){
			boolean ch=false;
			for(i=0;i<strlen(walk->assembly[2]);i++){
				if(!('0'<=(walk->assembly[2])[i]&&(walk->assembly[2])[i]<='9')) ch=true;
			}
			if(ch==true){
				symb_node* f=find_symbol(walk->assembly[2]);
				if(f==NULL){
					ret=walk->line;
					assemble_error=1;
					break;
				}
				base_reg=f->addr;
			}
			else{
				int ten=1;
				for(i=strlen(walk->assembly[2])-1;i>=0;i--){
					base_reg+=((walk->assembly[2])[i]-'0')*ten;
					ten=ten*10;
				}
			}
			walk->obj_flag=false;
			walk=walk->ptr;
			continue;
		}
		//search OPTAB for OPCODE
		op_node* found;
		if(walk->assembly[1][0]=='+'){			
			found=opcode_func(walk->assembly[1]+1);
		}
		else found=opcode_func(walk->assembly[1]);
		if(found!=NULL){
			//if format 1
			if(strcmp(found->format,"1")==0){
				strcpy(walk->objcode, found->hexa);
			}
			//if format 2
			else if(strcmp(found->format, "2")==0){
				strcpy(walk->objcode, found->hexa);
				int ind=2;
				for(i=2;i<walk->argnum;i++){
					if((walk->assembly[i])[0]=='A') (walk->objcode)[ind++]='0';
					else if((walk->assembly[i])[0]=='X')	(walk->objcode)[ind++]='1';
					else if((walk->assembly[i])[0]=='L') (walk->objcode)[ind++]='2';
					else if((walk->assembly[i])[0]=='P'&&(walk->assembly[i])[0]=='C') (walk->objcode)[ind++]='8';
					else if((walk->assembly[i])[0]=='S'&&(walk->assembly[i])[0]=='W') (walk->objcode)[ind++]='9';
					else if((walk->assembly[i])[0]=='B')	(walk->objcode)[ind++]='3';
					else if((walk->assembly[i])[0]=='S')	(walk->objcode)[ind++]='4';
					else if((walk->assembly[i])[0]=='T')	(walk->objcode)[ind++]='5';
					else if((walk->assembly[i])[0]=='F') (walk->objcode)[ind++]='6';
				}
				if(ind==3) (walk->objcode)[ind++]='0';
			}
			//if format 3 or 4
			else{
				int n=1, i=1, x=0, b=0, p=0, e=0, j;
				int sindex=(walk->assembly[1])[0]=='@'||(walk->assembly[1])[0]=='#'? 1:0;
				boolean character=false;
				if((walk->assembly[1])[0]=='+') e=1;
				
				//find appropriate n, i;
				if((walk->assembly[2])[0]=='#'){ n=0; sindex++;}
				else if((walk->assembly[2])[0]=='@') {i=0; sindex++;}
				
				//check if the instruction use x register
				for(j=2;j<walk->argnum;j++){
					if(strcmp((walk->assembly)[j],"X")==0||strcmp((walk->assembly)[j],"X,")==0) x=1;
				}	
				
				//find whether operand is constant
				for(j=sindex;j<strlen(walk->assembly[2]);j++){
					if(!('0'<=(walk->assembly[2])[j] && (walk->assembly[2])[j]<='9')) character=true;
				}

				//if there is symbol in the operand field
				//search that symbol. if found, store the symbol value
				//if not error (undefined symbol)
				symb_node* sfound=NULL;
				if(character){
					//cut out the comma
					if((walk->assembly[2])[strlen(walk->assembly[2])-1]==',') (walk->assembly[2])[strlen(walk->assembly[2])-1]='\0';
					
					if((walk->assembly[2])[0]=='#'||(walk->assembly[2])[0]=='@') sfound=find_symbol(walk->assembly[2]+1);
					else sfound=find_symbol(walk->assembly[2]);
					if(sfound==NULL) {
						assemble_error=1;
						ret=walk->line;
						break;
					}
				}
				
				//find appropriate b, p
				if(character==true&&e!=1){
					symb_node* f=NULL;
					if(n==1&&i==1) f=find_symbol(walk->assembly[2]);
					else f=find_symbol(walk->assembly[2]+1);
					if(f==NULL){
						ret=walk->line;
						assemble_error=1;
						break;
					}
					int pc=(walk->ptr)->loc;
					if(-2048<=(f->addr-pc)&&(f->addr-pc)<=2047) p=1;
					else b=1;
				}
				//if format 4
				 walk->objcode[0]=found->hexa[0];

                 int opni;
                 if('0'<=found->hexa[1]&&found->hexa[1]<='9') opni=found->hexa[1]-'0';
                 else opni=found->hexa[1]-'A'+10;
                 opni+=n*2+i*1;
                 if(0<=opni&&opni<=9) walk->objcode[1]=opni+'0';
                 else walk->objcode[1]=opni-10+'A';

                 int xbpe=x*8+b*4+p*2+e*1;
                 if(0<=xbpe&&xbpe<=9) walk->objcode[2]=xbpe+'0';
                 else walk->objcode[2]=xbpe-10+'A';

				 if(e==1){
					if(character){
						walk->objcode[3]='0';
						int value=sfound->addr;
						i=7;
						while(value!=0){
							int rest=value%16;
							value=value/16;
							if(0<=rest&&rest<=9) walk->objcode[i--]=rest+'0';
							else walk->objcode[i--]=rest-10+'A';
						}
						push_modification(walk);
					}
					else{
						for(i=3;i<8;i++) walk->objcode[i]='0';
						int end=0, value=0, ten=1, j;
						if((walk->assembly[2])[0]=='#') end=1;
						for(j=strlen(walk->assembly[2])-1;j>=end;j--){
							value=value+((walk->assembly[2])[j]-'0')*ten;
							ten=ten*10;
						}
						i=7;
						 while(value!=0){
                             int rest=value%16;
                             value=value/16;
                             if(0<=rest&&rest<=9) walk->objcode[i--]=rest+'0';
                             else walk->objcode[i--]=rest-10+'A';
                         }
					}
					walk->objcode[8]='\0';
				}
				//if format 3
				else if(b==0&&p==0){
					 for(i=3;i<6;i++) walk->objcode[i]='0';
                     int end=0, value=0, ten=1, j;
                     if((walk->assembly[2])[0]=='#') end=1;
                     for(j=strlen(walk->assembly[2])-1;j>=end;j--){
                         value=value+((walk->assembly[2])[j]-'0')*ten;
                         ten=ten*10;
                     }
                     i=5;
                     while(value!=0){
                         int rest=value%16;
                         value=value/16;
                         if(0<=rest&&rest<=9) walk->objcode[i--]=rest+'0';
                         else walk->objcode[i--]=rest-10+'A';
                     }
					walk->objcode[6]='\0';
				}
				else if(b==1){
					for(i=3;i<6;i++) walk->objcode[i]='0';
					int diff=sfound->addr-base_reg;
					i=5;
					while(diff!=0){
						int rest=diff%16;
						diff=diff/16;
						if(0<=rest&&rest<=9) walk->objcode[i--]=rest+'0';
						else walk->objcode[i--]=rest-10+'A';
					}
					walk->objcode[6]='\0';
				}
				else if(p==1){
					for(i=3;i<6;i++) walk->objcode[i]='0';
					int diff=(sfound->addr)-(walk->ptr)->loc;
					if(diff<0) diff=diff+4096;
					i=5;
					while(diff!=0){
						int rest=diff%16;
						diff=diff/16;
						if(0<=rest&&rest<=9) walk->objcode[i--]=rest+'0';
						else walk->objcode[i--]=rest-10+'A';
					}
					walk->objcode[6]='\0';
				}		
			}
		}
		else {
			//if opcode is 'WORD', convert constant to object code
			if(strcmp(walk->assembly[1], "WORD")==0){
				int value=0, ten=1;
				size=strlen(walk->assembly[2]);
				for(i=size-1;i>=0;i--){
					value+=((walk->assembly[2])[i]-'0')*ten;
					ten=ten*10;
				}
				int num=0, temp=value;
				while(temp!=0){
					num++;
					temp=temp/16;
				}
				num--;
				while(value!=0){
					int rest=value%16;
					value=value/16;
					if(rest<=9) (walk->objcode)[num]=rest+'0';
					else (walk->objcode)[num]=(rest-10+'A');
					num--;
				}
			}	
			//if opcode is 'BYTE', convert constant to object code
			else if(strcmp(walk->assembly[1], "BYTE")==0){
				int ind=0;
				boolean flag=(walk->assembly[2])[0]=='X' ? true : false;
				i=0;
				while((walk->assembly[2])[i]!='\'') i++;
				i++;
				if (flag){		//BYTE constant represendted in hexadecimal
					while((walk->assembly[2])[i]!='\'') (walk->objcode)[ind++]=(walk->assembly[2])[i++];
				}
				else{
					while((walk->assembly[2])[i]!='\'') {
						int ch=(walk->assembly[2])[i++];
						int ti;
						for(ti=1;ti>=0;ti--){
							int rest=ch%16;
							ch=ch/16;
							if(rest<=9) (walk->objcode)[ind+ti]=rest+'0';
							else (walk->objcode)[ind+ti]=(rest-10+'A');
						}
						ind=ind+2;
					}
				}
				walk->objcode[ind]='\0';
			}
			//else there is no matched opcode.
			else {
				ret=walk->line;
				printf("here?\n");
				assemble_error=3;
				break;
			}
		}
		walk=walk->ptr;
	}
	return ret;
}
