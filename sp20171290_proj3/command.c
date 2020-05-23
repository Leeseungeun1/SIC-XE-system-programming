#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "20171290.h"
#include "slice.h"
#include "command.h"
#include "subfunc.h"
#include "loader.h"
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

//function called when the command is started with 'type'
//print out the text of the given filename
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

//function called when the command is started with 'symbol'
//print out the symbols of the assembly code in ascending order.
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

int progaddr_func(int index){
	char str[100];
	char *address=NULL;
	strncpy(str, sentence, index);
	str[index]='\0';
	strtok(str," ");
	address=strtok(NULL, " ");
	if(address==NULL){
		return -1;
	}
	int i, size, sixteen = 1;
	size = strlen(address);
	int before_addr = progaddr;
	progaddr=0;
	for(i=size-1;i>=0;i--){
		if(address[i]==' ') continue;
		if('A'<=address[i]&&address[i]<='F') progaddr = progaddr+(address[i]-'A'+10)*sixteen;
		else if('a'<=address[i]&&address[i]<='f') progaddr = progaddr+(address[i]-'a'+10)*sixteen;
		else if('0'<=address[i]&&address[i]<='9') progaddr = progaddr+(address[i]-'0')*sixteen;
		else{
			parameter_error=true;
			progaddr = before_addr;
			break;
		}
		sixteen = sixteen*16;
	}
	if(progaddr>addrend) {
		progaddr = before_addr;
		return -1;
	}
	return 0;
}

int loader_func(int index){
	char str[100]="\0";
	char *file[3];
	int i;
	strncpy(str, sentence, index);
	strtok(str," ");
	str[index]='\0';
	for(i=0;i<3;i++){
		file[i]=NULL;
		file[i]=strtok(NULL, " ");
	}
	if(file[0]==NULL) return -1;
	
	int res;	
	for(i=0;i<3;i++){
		if(file[i]==NULL) break;
		res = loader_pass1(file[i]);
		if(res!=0) return res;
	}	

	res = check_bound();
	if(res!=0) return res;

	for(i=0;i<3;i++){
		if(file[i]==NULL) break;
		res = loader_pass2(file[i]);
		if(res!=0) return res;
	}

	print_estab();
	
	return 0;			
}

void bp_func(){
	int i, size, sixteen;
	
	size = strlen(sentence);
	sixteen = 1;
	int bp=0;
	for(i=size-1;i>=3;i--){
		if(sentence[i]==' ') continue;
		if('A'<=sentence[i]&&sentence[i]<='F') bp = bp+(sentence[i]-'A'+10)*sixteen;
		else if ('a'<=sentence[i]&&sentence[i]<='f') bp = bp+(sentence[i]-'a'+10)*sixteen;
		else if('0'<=sentence[i]&&sentence[i]<='9') bp = bp+(sentence[i]-'0')*sixteen;
		else{
			parameter_error = true;
			break;
		}
		sixteen = sixteen*16;
	}

	if(parameter_error==true) return ;
	
	if(find_bpoint(bp)!=NULL){
		printf("The breakpoint is already set\n");
		return;
	}

	bpoint* new = (bpoint*)malloc(sizeof(bpoint));
	new->breakpoint = bp;
	new->ptr = NULL;
	
	if(bpoint_head == NULL){
		bpoint_head = new;
	}
	if(bpoint_tail!=NULL){
		bpoint_tail->ptr=new;
	}
	bpoint_tail=new;
	printf("\t [ok] create breakpoint %X\n", bp);	
}

int run_func(){
	int i, dec;
	boolean endflag=false;
	while(PC<progaddr+totalLength){
		// if PC==bp
			bpoint* findbp = find_bpoint(PC);
			if(findbp!=NULL){
				print_registers(false, PC);
				endflag=true;
			}
		//1. instruction parsing
		char inst[3];
		int x=loc/16;
		int y=loc%16;
		dec=(mem[x][y]/16);
		if(dec>=10) inst[0]=(dec-10+'A');
		else inst[0]=dec+'0';
		
		dec=(mem[x][y]&12);
		if(dec>=10) inst[1]=(dec-10+'A');
		else inst[1]=dec+'0';
		inst[2]='\0';
	
		op_node* opcode = find_opinst(inst);
		if(opcode==NULL) return -1;		
		
		// handle for format 1
		if(strcmp(opcode->format, "1")==0){
			PC=loc+1;
			loc=PC;
			if(endflag==true) break;
			else continue;
		}
		//2. handle for format 2
		if(strcmp(opcode->format,"2")==0){
			x=(loc+1)/16;
			y=(loc+1)%16;
			int reg1=mem[x][y]/16;
			int reg2=mem[x][y]%16;
			PC=loc+2;
			loc=PC;
			
			//ADDR
			if(strcmp(opcode->command, "ADDR")==0){
				int value=get_register(reg2)+get_register(reg1);
				set_register(reg2, value);
			}
			//CLEAR
			if(strcmp(opcode->command, "CLEAR")==0){
				set_register(reg1, 0);
			}
			//COMPR
			if(strcmp(opcode->command, "COMPR")==0){
			}
			//DIVR
			if(strcmp(opcode->command, "DIVR")==0){
				int value = get_register(reg2) / get_register(reg1);
				set_register(reg2, value);
			}
			//MULR
			if(strcmp(opcode->command, "MULR")==0){
				int value = get_register(reg2) * get_register(reg1);
				set_register(reg2, value);
			}
			//SUBR
			if(strcmp(opcode->command, "SUBR")==0){
				int value = get_register(reg2) - get_register(reg1);
				set_register(reg2, value);
			}
			//TIXR
			if(strcmp(opcode->command, "TIXR")==0){
				reg2=1;
				set_register(reg2, get_register(reg2)+1);
				int reg1Val = get_register(reg1);
				int reg2Val = get_register(reg2);
				if(reg2Val>reg1Val) compare=1;
				else if(reg2Val==reg1Val) compare = 0;
				else if (reg2Val<reg1Val) compare=-1;
			}
			if(endflag==true) break;
			else continue;
		}
		//find nixbpe
		int nb, ib, xb, bb, pb, eb;
		ib = mem[x][y]%2;
		nb = (mem[x][y]%4)/2;
		
		x=(loc+1)/16;
		y=(loc+1)%16;
		eb = (mem[x][y]%32)/16;
		pb = (mem[x][y]%64)/32;
		bb = (mem[x][y]%128)/64;
		xb = (mem[x][y]%256)/128;
		
		int data = 0, address = 0, mul=1;
		//3. handle for format 4
		if(eb==1){
			mul=1;
			if(nb==1&&ib==1){
				for(i=3;i>0;i--){
					x=(loc+i)/16;
					y=(loc+i)%16;
					if(i==1) address+=(mem[x][y]%16)*mul;
					else address+=mem[x][y]*mul;
					mul=mul*256;
				}
			}
			if(nb==0&&ib==1){
				for(i=3;i>0;i--){
					x=(loc+i)/16;
					y=(loc+i)%16;
					if(i==1) data+=(mem[x][y]%16)*mul;
					else data+=mem[x][y]*mul;
					mul=mul*256;
				}
			}
			PC=loc+4;
			loc=PC;
		}
		//4. handle for format 3
		else{
			PC=loc+3;
			//TD
			if(strcmp(opcode->command, "TD")==0){
				loc=PC;
				compare = -1;
				if(endflag==true) break;
				else continue;
			}
			//WD
			if(strcmp(opcode->command, "WD")==0){
				loc=PC;
				if(endflag==true) break;
				else continue;
			}
			//RD
			if(strcmp(opcode->command, "RD")==0){
				loc=PC;
				compare = 0;
				if(endflag==true) break;
				else continue;
			}
			if(nb==1&&ib==1){
				for(i=2;i>0;i--){
					x=(loc+i)/16;
					y=(loc+i)%16;
					if(i==1) address+=(mem[x][y]%16)*mul;
					else address+=mem[x][y]*mul;
					mul=mul*256;
				}
				loc=PC;
				if(bb==1) address+=B;
				else if(pb==1){
					if(address>2047) address=address-4096;
					 address+=PC;
				}
			
				if(xb==1) address+=X;
				if(strcmp(opcode->command, "LDCH")==0) address-=X;

				int end = 0;
				x=(address+end)/16;
				y=(address+end)%16;
				
				boolean isReg = false;
				if(mem[x][y]==0){
					 isReg=true;
				}
				if(isReg){
					while(reg[x][y]!=0&&end<3){
						end++;
						x=(address+end)/16;
						y=(address+end)%16;
					}
				}
				else{
					while(mem[x][y]!=0&&end<3){
						end++;
						x=(address+end)/16;
						y=(address+end)%16;
					}
				}
				
				if((opcode->command)[2]=='C'&&(opcode->command)[2]=='H'){
					x=address/16;
					y=address%16;
					if(isReg) data=reg[x][y];
					else data=mem[x][y];
				}
				else{
					mul=1;
					for(i=end-1;i>=0;i--){
						x=(address+i)/16;
						y=(address+i)%16;
						if(isReg) data+=reg[x][y]*mul;
						else data+=mem[x][y]*mul;
						mul = mul*256;				
					}
				}
			}
			else if(nb==1&&ib==0){
				int tempaddr=0;
				for(i=2;i>0;i--){
					x=(loc+i)/16;
					y=(loc+i)%16;
					if(i==1) tempaddr+=(mem[x][y]%16)*mul;
					else tempaddr+=mem[x][y]*mul;
					mul=mul*256;	
				}
				loc=PC;
				if(bb==1) tempaddr+=B;
				else if(pb==1){
					if(tempaddr>2047) address=address-4096;
					 tempaddr+=PC;
				}

				if(xb==1) tempaddr+=X;

				x=tempaddr/16;
				y=tempaddr%16;
				boolean isReg = false;
				if(mem[x][y]==0){
					 isReg=true;
				}
				if(isReg) address=reg[x][y];
				else address=mem[x][y];
			}
			else if(nb==0&&ib==1){
				for(i=2;i>0;i--){
					x=(loc+i)/16;
					y=(loc+i)%16;
					if(i==1) data+=(mem[x][y]%16)*mul;
					else data+=mem[x][y]*mul;
					mul=mul*256;
				}
				loc=PC;
				if(bb==1) data+=B;
				else if(pb==1) data+=PC;
	
				if(xb==1) data+=X;
			}
		}
		//handle the instructions
		//LDB
		if(strcmp(opcode->command, "LDB")==0){
			B = data;
		}
		//JSUB
		if(strcmp(opcode->command, "JSUB")==0){
			L = PC;
			loc=address;
			PC=address;
		}
		//COMP
		if(strcmp(opcode->command, "COMP")==0){
			if(A>data) compare=1;
			else if (A==data) compare = 0;
			else if (A<data) compare = -1;
		}
		//JEQ
		if(strcmp(opcode->command, "JEQ")==0){
			if(compare==0){
				loc=address;
				PC=address;
			}
		}
		//J
		if(strcmp(opcode->command, "J")==0){
			loc=address;
			PC=address;
		}
		//LDA
		if(strcmp(opcode->command, "LDA")==0){
			A = data;
		}
		//LDT
		if(strcmp(opcode->command, "LDT")==0){
			T = data;
		}
		//JLT
		if(strcmp(opcode->command, "JLT")==0){
			if(compare==-1){
				loc=address;
				PC=address;
			}
		}
		//LDCH
		if(strcmp(opcode->command, "LDCH")==0){
			int temp = data, index=0;
			while(temp!=0){
				index++;
				temp=temp/256;
			}
			temp=data;
			for(i=X;i<index;i++){
				A=temp%256;
				temp=temp/256;
			}
		}
		//RSUB
		if(strcmp(opcode->command, "RSUB")==0){
			loc=L;
			PC=L;
		}
		//STL
		if(strcmp(opcode->command, "STL")==0){
			reg[x][y]=L;
		}
		//STA
		if(strcmp(opcode->command, "STA")==0){
			reg[x][y]=A;
		}
		//STCH
		if(strcmp(opcode->command, "STCH")==0){
			x = address/16;
			y = address%16;
			reg[x][y]=A%256;
		}
		//STX
		if(strcmp(opcode->command, "STX")==0){
			reg[x][y]=X;
		}
		//TIX
		if(strcmp(opcode->command, "TIX")==0){
			if(X>data) compare = 1;
			else if(X==data) compare = 0;
			else if(X<data) compare = -1;
		}	
		if(endflag==true) break;
	}	
	if(PC>=totalLength&&endflag==false){
		print_registers(true, PC);
	}	
	return 0;
}
