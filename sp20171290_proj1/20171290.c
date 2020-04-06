#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "command.h"
#include "slice.h"
#include "subfunc.h"
#define addrend 1048575   //end of the virtual memory address

//the main function
int main(){
    init_global_variable();
    init_memory();
    build_hash_table();
    if(opcode_file_error==true) return 0;
    while(1){				//keep showing the command prompt until the user quits the program.
		start_appear=false; end_appear=false; value_appear=false; parameter_error=false;
    	start=0; end=0; value=0;
		printf("sicsim>");
    	char c;
		int index=0;
    	while((c=getchar())!='\n'){
       	   sentence[index++]=c;
    	}
		sentence[index]='\0';
		if((strcmp(sentence, "help")==0)||(strcmp(sentence, "h")==0)){
	    	push(sentence, index);	//add the command to history linked list
	    	help_func();
		}
		else if((strcmp(sentence, "dir")==0)||(strcmp(sentence,"d")==0)){
	    	push(sentence, index);  //add the command to history linked list
            dir_func();
		}
		else if((strcmp(sentence, "quit")==0)||(strcmp(sentence, "q")==0)){
		    quit_func();
			break;
		}
		else if((strcmp(sentence, "history")==0)||(strcmp(sentence, "hi")==0)){
            push(sentence, index);  //add the command to history linked lsit
            history_func();
        }
		else if((strcmp(sentence, "dump")==0)||(strncmp(sentence, "dump ", 5)==0)
					||(strcmp(sentence, "du")==0)||(strncmp(sentence, "du ", 3)==0)){
	    	dump_slice_str(index);   //parsing the arguments
			if(parameter_error==true){
				printf("Wrong hexadecimal\n");
				continue;
			} 
	    	if(start_appear==false && end_appear==false){	//when the command is 'dump'
				if(startaddress+159>addrend){ 		//when the 160 memories from start address exceed the given virtual memory.
		 			dump_func(startaddress, addrend);
		  			startaddress=0;
				}
				else{
			  		dump_func(startaddress, startaddress+159);
		  			startaddress+=160;    //store the next start address
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
	    	push(sentence, index);   //add th command to history linked list
		}
		else if((strncmp(sentence, "edit ", 5)==0)||(strncmp(sentence, "e ", 2)==0)){
	    	edit_slice_str(index);  //parsing arguments
			if(parameter_error==true){
				printf("Wrong hexadecimal\n");
				continue;
			}
	    	if(start_appear*value_appear==0){		//check if the command is correct
				printf("Pleas put two arguments or split arguments with comma.\n");
	 			continue;
	    	}
	    	if(start>addrend || value>255){		//check the memory access error
				printf("bounded error\n");		
				continue;	
	    	}
	    	edit_func();
	    	push(sentence, index);  //add the command to history linked list
		}
        else if((strncmp(sentence, "fill ", 5)==0)||(strncmp(sentence, "f ", 2)==0)){
	    	fill_slice_str(index);  //parsing arguments
	  		if(parameter_error==true){
				printf("Wrong hexadecimal\n");
				continue;
			}
		  	if(start_appear*value_appear*end_appear==0){ 		//check if the command is correct
				printf("Please put three arguments or split arguments with comma.\n");
				continue;
	    	}	
	    	if(start>addrend||end>addrend||end<start||value>255){	//check the memory access error
				printf("bounded error\n");
				continue;
	    	}
	    	fill_func();
	    	push(sentence, index);  //add the command to history linked list
		}
		else if(strcmp(sentence, "reset")==0){
	    	init_memory();
	    	push(sentence, index);  //add the command to history linked list
		}
		else if(strcmp(sentence, "opcodelist")==0){
	    	opcode_list_func();
	    	push(sentence, index);  //add the command to history linked list
		}
		else if(strncmp(sentence, "opcode ", 7)==0){
	    	char *command=NULL, copy[100];
			strncpy(copy,sentence,strlen(sentence));	//copy the command for history
 	    	//extract mnemonic
			strtok(sentence," ");
	    	command=strtok(NULL, " ");
	   	 	int ret=opcode_func(command);
            if(ret==0) push(copy, index);  //add the command to history linked list
										   //if there was opcode of the given mnemonic
		}
		else printf("Command not found\n"); //wrong command	
    }
}
