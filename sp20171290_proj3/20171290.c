#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "20171290.h"
#include "assemble.h"
#include "command.h"
#include "slice.h"
#include "subfunc.h"
#include "write.h"
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
		assemble_error=0;
		printf("sicsim> ");
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
		else if((strncmp(sentence, "opcode ", 7)==0)&&index>7){
	    	char *command=NULL, copy[100];
			strncpy(copy,sentence,strlen(sentence));	//copy the command for history
 	    	//extract mnemonic
			strtok(sentence," ");
	    	command=strtok(NULL, " ");
	   	 	op_node* ret=opcode_func(command);
            if(ret==NULL) printf("There is no opcode for the command\n");
			else{
				printf("opcode is %s\n", ret->hexa);
				push(copy, index);
			}
		}
		else if((strncmp(sentence, "type ", 5)==0)&&index>5){
			char *command=NULL, copy[100];
			strncpy(copy, sentence, strlen(sentence));  //copy the command for history
			//extract filename
			strtok(sentence, " ");
			command=strtok(NULL, " ");
			int ret=type_filename_func(command);
			if(ret==-1) printf("Wrong file. Check the file\n"); // no file in current dir
			else push(copy, index);  //if the command exeucted successfully, push in history linked list.
		}
		else if((strncmp(sentence, "assemble ",9)==0)&&index>9){
			//if memory of list for object code and list for modification code is not free them
			while(list_head!=NULL){
				list_node* temp=list_head;
				list_head=list_head->ptr;
				free(temp);
			}
			while(mod_head!=NULL){
				mod_node* temp=mod_head;
				mod_head=mod_head->ptr;
				free(temp);
			}
			char *command=NULL, copy[100], filename[20];
			strncpy(copy, sentence, strlen(sentence));	//copy the command for history
			//extract filename
			strtok(sentence, " ");
			command=strtok(NULL, " ");
			int ret=pass_one(command);	//create lst list for lst file
			if(assemble_error!=0){	//if there is error while creating lst file
				if(assemble_error==1) printf("line %d: undefined variable\n",ret);
				if(assemble_error==2) printf("line %d: same variable names\n",ret);
				if(assemble_error==3) printf("line %d: undefined instruction\n",ret);
				continue;
			}
			ret=pass_two();	//create obj list for obj file
			if(assemble_error!=0){	//if there is error while creating obj file
				if(assemble_error==1) printf("line %d: undefined variable\n",ret);
				if(assemble_error==2) printf("line %d: same variable names\n",ret);
				if(assemble_error==3) printf("line %d: undefined instruction\n",ret);
				continue;
			}
			if(ret==-1) {		//if the file name is wrong
				printf("Check the file\n");
				continue;
			}
			strcpy(filename, command);		//copy the command for standard output command.
			ret=write_lst(filename);		//write lst file using lst list 
			ret=write_obj(filename);		//write obj file using obj list
			if(ret==-1) {printf("No assemblecode. Check the file\n");continue;}	//if there are no assembled code
			printf("Successfully assemble %s.\n", command);
			push(copy, index);
			
		}
		else if(strcmp(sentence, "symbol")==0){
			symbol_func();		//show the symbol of the assemble file.
			push(sentence, index);
		}
		else if(strncmp(sentence, "progaddr",8)==0){
			int res = progaddr_func(index);
			int before_progaddr = progaddr;
			if(res==-1){
				progaddr = before_progaddr;
				printf("check the address\n");
				continue;
			}
			if(parameter_error == true){
				printf("Wrong hexadecimal\n");
				progaddr = before_progaddr;
				continue;
			}
			push(sentence, index);
		}
		else if(strncmp(sentence, "loader", 6)==0){
			free_estab();
			totalLength=0;
			char copysentence[100];
			strncpy(copysentence, sentence, strlen(sentence));
			int res = loader_func(index);
			if(res==-1){
				printf("Please check the filename.\n");
			}
			else if(res==1){
				printf("duplicate external symbol.\n");
			}
			else if(res==2){
				printf("undefined external symbol.\n");
			}
			else if(res==3){
				printf("Out of Bound.\n");
			}
			else{
				push(copysentence, index);
			}
		}
		else if(strncmp(sentence, "bp", 2)==0){
			if(strcmp(sentence, "bp clear")==0){
				free_bp();
				printf("\t[ok] clear all breakpoints\n");
			}
			else{	
				if(index>3){
					bp_func();
					if(parameter_error==true){
						printf("Wrong hexadecimal\n");
						continue;
					}
				}
				else{
					print_bp();
				}	
			}	
			push(sentence, index);		
		}
		else if(strcmp(sentence, "run")==0){
			int res = run_func();
			if(res==-1){
				printf("Wrong instruction or, set break points on the variable or constants\n");
				continue;
			}
		}
		else printf("Command not found\n"); //wrong command	
    }
}
