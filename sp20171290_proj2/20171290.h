//define the boolean type
typedef enum boolean{false, true} boolean;

//define the history linked list node
typedef struct str_node{
    int index;
    char sentence[150];
    struct str_node* ptr;
}str_node;

//define the hashtable linked list node
typedef struct op_node{
    char hexa[3];
    char command[10];
    char format[10];
    struct op_node* ptr;
}op_node;

//define the symbol linked list node
typedef struct symb_node{
	char symbol[10];
	int addr;
	struct symb_node* ptr;
}symb_node;

//define the linked list node of lst file
typedef struct list_node{
	int argnum;
	int line;
	int loc;
	char assembly[3][30];  //0: label 1: instruction 2: operand
	struct list_node *ptr;
}list_node;

str_node *history_head;  //head pointer of history linked list
str_node *history_tail;  //tail pointer of history linked list

int historynum;      //number of commands
int startaddress;    //store the address when the command is only dump.
char sentence[100];  //string that stores the current command
unsigned char mem[65536][16];

op_node* hash_table[20];
boolean opcode_file_error;   //flag to check the existence of the file.

boolean start_appear;	//flag to check the existence of start argument.	
boolean end_appear;		//flag to check the existence of end argument.
boolean value_appear;	//flag to check the existence of value argument.
boolean parameter_error;  //flag to check the correctness of hexadecimal.
int start, end, value;  //store the start, end, value as decimal number

symb_node *symbol_list;  //head pointer of symbol table list
list_node *list_head;
list_node *list_tail;
char assemble_line[100][30];  //buffer to store the assemble instruction
char program_title[30];
int assemble_error; //1: undefined variable 2: same variable name 3: undefined instruction
