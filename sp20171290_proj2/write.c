#include <stdio.h>
#include <string.h>
#include "20171290.h"

int write_lst(char *filename){
	if(list_head==NULL) return -1;
	int i=strlen(filename)-1;
	filename[i]='t';
	filename[i-1]='s';
	filename[i-2]='l';
	FILE *fp=fopen(filename, "w");
	list_node* walk=list_head;
	while(walk!=NULL){
		fprintf(fp,"%5d\t", walk->line);
		if(strcmp(walk->assembly[0], ".")==0){
			fprintf(fp, ".\n");
			walk=walk->ptr;
			 continue;
		}
		if(strcmp(walk->assembly[1], "END")!=0) fprintf(fp, "%04X\t", walk->loc);
		else fprintf(fp, "\t");
		fprintf(fp, "%-30s", walk->inst);
		if(walk->obj_flag==true){
			fprintf(fp,"%-15s", walk->objcode);
		}
		fprintf(fp,"\n");
		walk=walk->ptr;
	}
	fclose(fp);
	return 0;
}
int write_obj(){

}
