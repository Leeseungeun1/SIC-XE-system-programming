#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "command.h"
#include "slice.h"
#include "subfunc.h"
#define addrend 1048575

//the main function
int main(){
    init_global_variable();
    init_memory();
    build_hash_table();
    if(opcode_file_error==true) return 0;
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
	    	push(sentence, index);	//add the command to history linked list
	    	help_func();
		}
		else if((strncmp(sentence, "dir",3)==0)||((strncmp(sentence,"d",1)==0)&&index==1)){
	    	push(sentence, index);
            dir_func();
		}
		else if((strncmp(sentence, "quit", 4)==0)||(strncmp(sentence, "q", 1)==0)){
		    quit_func();
			break;
		}
		else if((strncmp(sentence, "history", 7)==0)||(strncmp(sentence, "hi", 2)==0)){
            push(sentence, index);
            history_func();
        }
		else if(((strncmp(sentence, "dump", 4)==0)&&index==4)||(strncmp(sentence, "dump ", 5)==0)
					||((strncmp(sentence, "du", 2)==0)&&index==2)||(strncmp(sentence, "du ", 3)==0)){
	    	dump_slice_str(index); 
	    	if(start_appear==false && end_appear==false){	//when the command is 'dump'
				if(startaddress+159>addrend){
		 			dump_func(startaddress, addrend);
		  			startaddress=0;
				}
				else{
			  		dump_func(startaddress, startaddress+159);
		  			startaddress+=160;
				}
	    	}
	    	else if (start_appear==true && end_appear==false){	//when the command is given with the start address
                if(start<0 || start>addrend){		//check the memory access error
		   			printf("bounded error\n");
		   			continue;
				}
				else if(start+159>addrend) dump_func(start, addrend);
				else dump_func(start, start+159);
	    	}
	    	else if (start_appear==true && end_appear==true){	//when the command is given with the start and end address
				if(end<start || start<0 || start>addrend || end<0 || end>addrend){		//check the memory acess error
		   			printf("bounded error\n");
		   			continue;
				}
				else dump_func(start, end);
	    	}		
	    	push(sentence, index);
		}
		else if((strncmp(sentence, "edit ", 5)==0)||(strncmp(sentence, "e ", 2)==0)){
	    	edit_slice_str(index);
	    	if(start_appear*value_appear==0){		//check if the command is correct
				printf("Pleas put two arguments or split arguments with comma.\n");
	 			continue;
	    	}
	    	if(start>addrend || value>255){		//check the memory access error
				printf("bounded error\n");		
				continue;	
	    	}
	    	edit_func();
	    	push(sentence, index);
		}
        else if((strncmp(sentence, "fill ", 5)==0)||(strncmp(sentence, "f ", 2)==0)){
	    	fill_slice_str(index);
	    	if(start_appear*value_appear*end_appear==0){ 		//check if the command is correct
				printf("Please put three arguments or split arguments with comma.\n");
				continue;
	    	}	
	    	if(start>addrend||end>addrend||end<start||value>255){	//check the memory access error
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
		else if(strncmp(sentence, "opcode", 6)==0){
	    	char *command=NULL, copy[100];
			strncpy(copy,sentence,strlen(sentence));	//copy the command for history
 	    	//extract mnemonic
			strtok(sentence," ");
	    	command=strtok(NULL, " ");
	   	 	int ret=opcode_func(command);
            if(ret==0) push(copy, index);
		}
		else printf("Command not found\n");	
    }
}
