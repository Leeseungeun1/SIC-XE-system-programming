#include <stdio.h>
#include <string.h>
#include "20171290.h"

//write lst file using lst linked list
int write_lst(char *filename){
	if(list_head==NULL) return -1;
	int i=strlen(filename)-1;
	filename[i]='t';
	filename[i-1]='s';
	filename[i-2]='l';
	//change it for making file name with .lst
	FILE *fp=fopen(filename, "w");
	list_node* walk=list_head;
	while(walk!=NULL){
		fprintf(fp,"%5d\t", walk->line);	//print the line number
		if(strcmp(walk->assembly[0], ".")==0){	//print . if the line was comment
			fprintf(fp, ".\n");
			walk=walk->ptr;
			 continue;
		}
		if(strcmp(walk->assembly[1], "END")!=0 && strcmp(walk->assembly[1],"BASE")!=0) fprintf(fp, "%04X\t", walk->loc);
			//print the loc when the code is not a notification code for programmer
		else fprintf(fp, "\t");
		fprintf(fp, "%-30s", walk->inst);	//print the assembly instruction code
		if(walk->obj_flag==true){		//print the object code if there is.
			fprintf(fp,"%-10s", walk->objcode);
		}
		fprintf(fp,"\n");
		walk=walk->ptr;
	}
	fclose(fp);
	return 0;
}
int write_obj(char* filename){
	if(list_head==NULL) return -1;
	int i=strlen(filename)-1;
	filename[i]='j';
	filename[i-1]='b';
	filename[i-2]='o';
	//change it for making file name with .obj
	FILE *fp=fopen(filename, "w");
	//write HEAD part of the obj code
	fprintf(fp,"H%-6s%06X%06X\n",program_title,(list_head->ptr)->loc,program_length);
	list_node *start=list_head->ptr, *walk;
	//write TEXT part of the obj code
	while(start!=NULL){
		fprintf(fp,"T");
		walk=start;
		while(walk!=NULL&&((walk->loc)-(start->loc))<28&&(strncmp(walk->assembly[1],"RES",3)!=0)){
			walk=walk->ptr;
		}
		if(walk==NULL) fprintf(fp, "%06X%02X", start->loc, ((program_length+(list_head->ptr)->loc)-start->loc));
			//last line of the TEXT part
		else fprintf(fp,"%06X%02X",start->loc,(walk->loc)-(start->loc));
		while(walk!=NULL&&(strcmp(walk->assembly[0], ".")==0||strncmp(walk->assembly[1], "RES",3)==0)) walk=walk->ptr;
			//skip the variable and constant part
		while(start!=walk){
			if(start->obj_flag==true) fprintf(fp, "%s", start->objcode);	//print out the objcode to obj file
			start=start->ptr;
		}
		fprintf(fp, "\n");
	}
	//write MODIFICATION part of the obj code
	mod_node *walk2=mod_head;
	while(walk2!=NULL){
		fprintf(fp, "M");
		fprintf(fp,"%06X%02X\n", walk2->address, walk2->num);
		walk2=walk2->ptr;
	}
	//write END part of the obj code
	fprintf(fp,"E%06X\n", (list_head->ptr)->loc);
	fclose(fp);
	return 0;
}
