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

str_node *history_head = NULL;
str_node *history_tail = NULL;

int historynum = 1;
int startaddress=0;
char sentence[100];
unsigned char mem[65536][16];

boolean start_appear;
boolean end_appear;
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
   int i, size, ten;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *endstr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   endstr=strtok(NULL," ");
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      ten=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
	start=start+(startstr[i]-'0')*ten;
	ten=ten*10;
      }
     // printf("start: %d\n", start);
   }
   if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      ten=1;
      for(i=size-1;i>=0;i--){
	if(endstr[i]==' ') continue;
	end=end+(endstr[i]-'0')*ten;
	ten=ten*10;
      }
     // printf("end: %d\n", end);
   }
}

void edit_slice_str(int index){
   char str[100];
   int i, size, ten, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   valuestr=strtok(NULL," ");
   if(startstr!=NULL){
      size=strlen(startstr);
      ten=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
        start=start+(startstr[i]-'0')*ten;
        ten=ten*10;
      }
     // printf("start: %d\n", start);
   }
   if(valuestr!=NULL){
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
	if(valuestr[i]==' ') continue;
	if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
	else value+=(valuestr[i]-'0')*sixteen;
	sixteen=sixteen*16;
      }
     // printf("end: %d\n", end);
   }

}

void fill_slice_str(int index){
   char str[100];
   int i, size, ten, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL, *endstr=NULL, *dummy=NULL;
   dummy=strtok(str," ");
   startstr=strtok(NULL,",");
   endstr=strtok(NULL, ",");
   valuestr=strtok(NULL," ");
   if(startstr!=NULL){
      size=strlen(startstr);
      ten=1;
      for(i=size-1;i>=0;i--){
	if(startstr[i]==' ') continue;
        start=start+(startstr[i]-'0')*ten;
        ten=ten*10;
      }
     // printf("start: %d\n", start);
   }
    if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      ten=1;
      for(i=size-1;i>=0;i--){
	if(endstr[i]==' ') continue;
	end=end+(endstr[i]-'0')*ten;
        ten=ten*10;
      }
     // printf("end: %d\n", end);
   }
   if(valuestr!=NULL){
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
        if(valuestr[i]==' ') continue;
	if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
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
		if(mem[address][i]==0||(address==startx&&i<starty)||(address==endx&&i>endy)) printf(".");
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
void main(){
    init_memory();
    while(1){
	start_appear=false; end_appear=false;
    	start=0; end=0; value=0;
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
	    if(start>addrend || value>255){
		printf("bounded error\n");
		continue;	
	    }
	    edit_func();
	    push(sentence, index);
	}
        else if((strncmp(sentence, "fill", 4)==0)||(strncmp(sentence, "f", 1)==0)){
	    fill_slice_str(index);
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
    }
}
