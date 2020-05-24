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
	char inst[50];
	char assembly[5][30];  //0: label 1: instruction 2: operand else: extra buffer
	boolean obj_flag;
	char objcode[10];
	struct list_node *ptr;
}list_node;

//define the linked list node of modification info
typedef struct mod_node{
	int address;
	int num;
	struct mod_node* ptr;
}mod_node;

//define the linked list node of ESTAB
typedef struct estab{
	char control[10];
	char symbol[10];
	int address;
	int length;
	struct estab* ptr;
}estab;

//define the linked list node for storing reference symbol
typedef struct refer{
	char number[3];
	char name[10];
	struct refer* ptr;
}refer;

//define the linked list node of breakpoints
typedef struct bpoint{
	int breakpoint;
	struct bpoint* ptr;
}bpoint;

str_node *history_head;  //head pointer of history linked list
str_node *history_tail;  //tail pointer of history linked list

int historynum;      //number of commands
int startaddress;    //store the address when the command is only dump.
char sentence[100];  //string that stores the current command
int mem[65536][16];

op_node* hash_table[20];
boolean opcode_file_error;   //flag to check the existence of the file.

boolean start_appear;	//flag to check the existence of start argument.	
boolean end_appear;		//flag to check the existence of end argument.
boolean value_appear;	//flag to check the existence of value argument.
boolean parameter_error;  //flag to check the correctness of hexadecimal.
int start, end, value;  //store the start, end, value as decimal number

symb_node *symbol_list;  //head pointer of symbol table list
list_node *list_head;	//head pointer of list for lst file
list_node *list_tail;	//tail pointer of list for lst file
mod_node *mod_head;		//head pointer of list for modification code
mod_node *mod_tail;		//tail pointer of list for modification code
char program_title[6];	//store the program title for obj code
int assemble_error; //1: undefined variable 2: same variable name 3: undefined instruction
int program_length;		//store the program length for obj code
int base_reg;		//store the value of base register if base register is used for addressing

int progaddr;		//store the start address of program
int before_prog_length;	//store the previous program length
int current_prog_addr;	//store the current program length
int execution_addr;		//store the execution address read from obj file

estab *estab_head;	//head pointer of ESTAB linked list
estab *estab_tail;	//tail pointer of ESTAB linked list

refer *refer_head;	//head pointer of reference linked list
refer *refer_tail;	//tail pointer of reference linked list

bpoint *bpoint_head;	//head pointer of breakpoint linked list
bpoint *bpoint_tail;	//tail pointer of breakpoint linked list
int totalLength;	//store the current program length
int A, X, L, PC, B, S, T;	//store the register value
int loc;	//store the current LOC value
int compare;	//store the result of comparison instruction
