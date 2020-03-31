#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#define addrend 1048575
typedef enum boolean{false, true} boolean;

typedef struct str_node{
    int index;
    char sentence[150];
    struct str_node* ptr;
}str_node;

typedef struct op_node{
    char hexa[3];
    char command[10];
    char format[10];
    struct op_node* ptr;
}op_node;

str_node *history_head = NULL;
str_node *history_tail = NULL;

int historynum = 1;
int startaddress=0;
char sentence[100];
unsigned char mem[65536][16];

op_node* hash_table[20];
boolean opcode_file_error=false;

void build_hash_table(){
   FILE *fp=fopen("opcode.txt", "r");
   if(!fp){
      printf("There is no opcode.txt\n");
      opcode_file_error=true;
      return;	
   }
   while(1){
	op_node* node=(op_node*)malloc(sizeof(op_node));
	if(fscanf(fp, "%s %s %s\n", node->hexa, node->command, node->format)==EOF) break;
	(node->hexa)[2]='\0'; (node->command)[strlen(node->command)]=(node->format)[strlen(node->format)]='\0';
	int index;
	index=(node->command[0]-'A')%20;
	node->ptr=hash_table[index];
	hash_table[index]=node;
   }
}

boolean start_appear;
boolean end_appear;
boolean value_appear;
int start, end, value;
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

boolean subcmp(int start, int end, char* str1, char* str2){
    int i;
    boolean ret=true;
    for(i=start;i<end;i++){
        if(str1[i]!=str2[i-start]) ret=false;
    }
    return ret;
}

void init_memory(){
   int i,j;
   for(i=0;i<65536;i++){
      for(j=0;j<16;j++){
	 mem[i][j]=0;
      }
   }
}

void dump_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *endstr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   endstr=strtok(NULL," ");
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
	if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
	else if (startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
	else start=start+(startstr[i]-'0')*sixteen;
	sixteen=sixteen*16;
      }
     // printf("start: %d\n", start);
   }
   if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(endstr[i]==' ') continue;
	if(endstr[i]>='A'&&endstr[i]<='F') end=end+(endstr[i]-'A'+10)*sixteen; 
	else if(endstr[i]>='a'&&startstr[i]<='f') end=end+(endstr[i]-'a'+10)*sixteen;
	else end=end+(endstr[i]-'0')*sixteen;
	sixteen=sixteen*16;
      }
     // printf("end: %d\n", end);
   }
}

void edit_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   valuestr=strtok(NULL," ");
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
        if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
	else if(startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
	else start=start+(startstr[i]-'0')*sixteen;
        sixteen=sixteen*16;
      }
     // printf("start: %d\n", start);
   }
   if(valuestr!=NULL){
      value_appear=true;
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(valuestr[i]==' ') continue;
	if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
	else if('a'<=valuestr[i]&&valuestr[i]<='f') value=value+(10+valuestr[i]-'a')*sixteen;
	else value+=(valuestr[i]-'0')*sixteen;
	sixteen=sixteen*16;
      }
     // printf("end: %d\n", end);
   }

}

void fill_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL, *endstr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   endstr=strtok(NULL, ",");
   valuestr=strtok(NULL," ");
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
        if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
	else if(startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
	else start=start+(startstr[i]-'0')*sixteen;
        sixteen=sixteen*16;
      }
     // printf("start: %d\n", start);
   }
    if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(endstr[i]==' ') continue;
	if(endstr[i]>='A'&&endstr[i]<='F') end=end+(endstr[i]-'A'+10)*sixteen;
	else if(endstr[i]>='a'&&endstr[i]<='f') end=end+(endstr[i]-'a'+10)*sixteen;
	else end=end+(endstr[i]-'0')*sixteen;
        sixteen=sixteen*16;
      }
     // printf("end: %d\n", end);
   }
   if(valuestr!=NULL){
      value_appear=true;
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
        if(valuestr[i]==' ') continue;
	if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
        else if('a'<=valuestr[i]&&valuestr[i]<='f') value=value+(10+valuestr[i]-'a')*sixteen;
	else value+=(valuestr[i]-'0')*sixteen;
        sixteen=sixteen*16;
      }
     // printf("end: %d\n", end);
   }

}

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

void dump_func(int startidx, int endidx){
     int startx=startidx/16;
     int starty=startidx%16;
     int num=0, i, temp, upper, lower;
     char addr[5]={'0','0','0','0','0'};
     int address=startx;
     int endx=endidx/16;
     int endy=endidx%16;
	while(address<=endx){
	    temp=address;
            int index=3;
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
            if(address==startx){
                for(i=0;i<16;i++){
                   if((i<starty)||((startx==endx)&&i>endy) ){
                      printf("   "); continue;
                   }
                   upper=(mem[address][i]>>4);
		   if(upper<10) printf("%c", upper+'0');
                   else printf("%c", 'A'+(upper-10));
                   lower=(mem[address][i]&15);
                   if(lower<10) printf("%c ", lower+'0');
                   else printf("%c ", 'A'+(lower-10));
		}
            }
	    else if((startx<address)&&(address<endx)){
		 for(i=0;i<16;i++){
                   upper=(mem[address][i]>>4);
		   if(upper<10) printf("%c", upper+'0');
                   else printf("%c", 'A'+(upper-10));
                   lower=(mem[address][i]&15);
		   if(lower<10) printf("%c ", lower+'0');
                   else printf("%c ", 'A'+(lower-10));
                }
	    }
	    else{
		 for(i=0;i<16;i++){
                   if(i>endy){
                      printf("   "); continue;
                   }
                   upper=(mem[address][i]>>4);
                   if(upper<10) printf("%c", upper+'0');
                   else printf("%c", 'A'+(upper-10));
                   lower=(mem[address][i]&15);
                   if(lower<10) printf("%c ", lower+'0');
                   else printf("%c ", 'A'+(lower-10));
                }
	    }
	    printf(";");
            for(i=0;i<16;i++){
		if(mem[address][i]<20||mem[address][i]>126||(address==startx&&i<starty)||(address==endx&&i>endy)) printf(".");
                else printf("%c",mem[address][i]);
            }
            printf("\n");
            address++;
	}
}

void edit_func(){
    int x=start/16;
    int y=start%16;
    mem[x][y]=value;
}

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

int opcode_func(char* command){
   int index=(command[0]-'A')%20;
   int size, ret=-1;
   op_node* walk=hash_table[index];
   if(walk!=NULL){
   	if(strlen(walk->command)< strlen(command)) size=strlen(walk->command);
   	else size=strlen(command);
   }
   while(walk!=NULL){
	if(strncmp(walk->command,command,size)==0 && strlen(command)==strlen(walk->command)){
	  printf("opcode is %s\n", walk->hexa);
	  ret=0;
	  break;
       }
       walk=walk->ptr;
   }
   if(walk==NULL) printf("There is no opcode for the command\n");
   return ret;
}
void main(){
    init_memory();
    build_hash_table();
    if(opcode_file_error==true) return;
    while(1){
	start_appear=false; end_appear=false; value_appear=false;
    	start=0; end=0; value=0;
	printf("sicsim>");
    	char c;
	int index=0;
    	while((c=getchar())!='\n'){
       	   sentence[index++]=c;
    	}
	sentence[index]='\0';
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
	    dump_slice_str(index); 
	    if(start_appear==false && end_appear==false){
		if(start+175>addrend){
		  dump_func(startaddress, addrend);
		  startaddress=0;
		}
		else{
		  dump_func(startaddress, startaddress+159);
		  startaddress+=160;
		}
	    }
	    else if (start_appear==true && end_appear==false){
                if(start<0 || start>addrend){
		   printf("bounded error\n");
		   continue;
		}
		else if(start+159>addrend) dump_func(start, addrend);
		else dump_func(start, start+159);
	    }
	    else if (start_appear==true && end_appear==true){
		if(end<start || start<0 || start>addrend || end<0 || end>addrend){
		   printf("bounded error\n");
		   continue;
		}
		else dump_func(start, end);
	    }
	    push(sentence, index);
	}
	else if((strncmp(sentence, "edit", 4)==0)||(strncmp(sentence, "e", 1)==0)){
	    edit_slice_str(index);
	    if(start_appear*value_appear==0){
		printf("Pleas put two arguments or split arguments with comma.\n");
	 	continue;
	    }
	    if(start>addrend || value>255){
		printf("bounded error\n");
		continue;	
	    }
	    edit_func();
	    push(sentence, index);
	}
        else if((strncmp(sentence, "fill", 4)==0)||(strncmp(sentence, "f", 1)==0)){
	    fill_slice_str(index);
	    if(start_appear*value_appear*end_appear==0){
		printf("Please put three arguments or split arguments with comma.\n");
		continue;
	    }
	    if(start>addrend||end>addrend||end<start||value>255){
		printf("bounded error\n");
		continue;
	    }
	    fill_func();
	    push(sentence, index);
	}
	else if(strncmp(sentence, "reset", 5)==0){
	    init_memory();
	    push(sentence, index);
	}
	else if(strncmp(sentence, "opcodelist", 10)==0){
	    opcode_list_func();
	    push(sentence, index);
	}
	else if(strncmp(sentence, "opcode", 6)==0){// && strncmp(sentence, "opcodelist", 10)!=0){
	    char *op=NULL, *command=NULL;
 	    op=strtok(sentence," ");
	    command=strtok(NULL, " ");
	    int ret=opcode_func(command);
            if(ret==0) push(sentence, index);
	}
    }
}
