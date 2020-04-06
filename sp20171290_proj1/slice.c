#include <string.h>
#include "slice.h"

//extract the arguments when the command is dump start, end
void dump_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *endstr=NULL;
   //slice arguments according to comma.
   strtok(str," ");
   startstr=strtok(NULL,",");
   endstr=strtok(NULL," ");
   //extract start address
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(startstr[i]==' ') continue;
		if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
		else if (startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
		else if (startstr[i]>='0'&&startstr[i]<='9') start=start+(startstr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
		sixteen=sixteen*16;
      }
   }
   //extract end address
   if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(endstr[i]==' ') continue;
		if(endstr[i]>='A'&&endstr[i]<='F') end=end+(endstr[i]-'A'+10)*sixteen; 
		else if(endstr[i]>='a'&&startstr[i]<='f') end=end+(endstr[i]-'a'+10)*sixteen;
		else if(endstr[i]>='0'&&endstr[i]<='9') end=end+(endstr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
		sixteen=sixteen*16;
      }
   }
}

//extract arguments when the command is edit address, value
void edit_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL;
   //slice arguments according to comma
   strtok(str," ");
   startstr=strtok(NULL,",");
   valuestr=strtok(NULL," ");
   //extract address
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(startstr[i]==' ') continue;
        if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
		else if(startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
		else if(startstr[i]>='0'&&startstr[i]<='9') start=start+(startstr[i]-'0')*sixteen;
       	else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
		sixteen=sixteen*16;
      }
   }
   //extract values
   if(valuestr!=NULL){
      value_appear=true;
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(valuestr[i]==' ') continue;
		if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
		else if('a'<=valuestr[i]&&valuestr[i]<='f') value=value+(10+valuestr[i]-'a')*sixteen;
		else if('0'<=valuestr[i]&&valuestr[i]<='9')  value+=(valuestr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
		sixteen=sixteen*16;
      }
   }

}

//extract arguments when the command is fill start, end, value
void fill_slice_str(int index){
   char str[100];
   int i, size, sixteen;
   strncpy(str, sentence, index);
   str[index]='\0';
   char *startstr=NULL, *valuestr=NULL, *endstr=NULL;
   strtok(str," ");
   //slice arguments according to comma
   startstr=strtok(NULL,",");
   endstr=strtok(NULL, ",");
   valuestr=strtok(NULL," ");
   //extract the start address
   if(startstr!=NULL){
      start_appear=true;
      size=strlen(startstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(startstr[i]==' ') continue;
        if(startstr[i]>='A'&&startstr[i]<='F') start=start+(startstr[i]-'A'+10)*sixteen;
		else if(startstr[i]>='a'&&startstr[i]<='f') start=start+(startstr[i]-'a'+10)*sixteen;
		else if(startstr[i]>='0'&&startstr[i]<='9') start=start+(startstr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
        sixteen=sixteen*16;
      }
   }
    //extract end address
    if(endstr!=NULL){
      end_appear=true;
      size=strlen(endstr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
		if(endstr[i]==' ') continue;
		if(endstr[i]>='A'&&endstr[i]<='F') end=end+(endstr[i]-'A'+10)*sixteen;
		else if(endstr[i]>='a'&&endstr[i]<='f') end=end+(endstr[i]-'a'+10)*sixteen;
		else if(endstr[i]>='0'&&endstr[i]<='9') end=end+(endstr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
        sixteen=sixteen*16;
      }
   }
   //extract value
   if(valuestr!=NULL){
      value_appear=true;
      size=strlen(valuestr);
      sixteen=1;
      for(i=size-1;i>=0;i--){
        if(valuestr[i]==' ') continue;
		if('A'<=valuestr[i]&&valuestr[i]<='F') value+=(10+valuestr[i]-'A')*sixteen;
        else if('a'<=valuestr[i]&&valuestr[i]<='f') value=value+(10+valuestr[i]-'a')*sixteen;
		else if('0'<=valuestr[i]&&valuestr[i]<='9')  value+=(valuestr[i]-'0')*sixteen;
		else{	//if it is not hexadecimal
			parameter_error=true;
			break;
		}
        sixteen=sixteen*16;
      }
   }
}

