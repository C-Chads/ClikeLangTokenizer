#include "stringutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
right- representing following tokens.
child- representing this node.
left- representing the node preceding in the AST.
identification- what kind of node is this?
*/


char* infilename = NULL;
char* outfilename = "default_out.bin";
FILE* ifile = NULL;
FILE* ofile = NULL;
char* entire_input_file;
unsigned long entire_input_file_len = 0;
strll tokenized = {0};

/*DESCRIPTION-
	parses a single matched pair from allocated text.
	the tree looks like this-
	result
		text: text immediately preceding the brackets.
		left: Null
		child:
			text:
			text between tl and tr.
		right: all text following.
*/
static long strll_len(strll* head){
	long len = 1;
	if(!head) return 0;
	while(head->right) {head = head->right; len++;}
	return len;
}


static void strll_show(strll* current, long lvl){
	{long i; /*strll* current = &tokenized;*/
		for(;current != NULL; current = current->right){
			if(current->text){
				for(i = 0; i < lvl; i++) printf("\t");
				printf("TOKEN IS:'%s'\n", current->text);
			}
			if(current->child)
			{	for(i = 0; i < lvl; i++) printf("\t");
				printf("CHILDREN:\n");
				strll_show(current->child, lvl + 1);
			}
		}
	}
}

int main(int argc, char** argv){
	char* larg;  long i;
	long local_search_point = 0;
	larg = argv[0];
	
	for(i = 1; i < argc; i++){
		if(streq(larg,"-i"))
			infilename = argv[i];
		if(streq(larg,"-o"))
			outfilename = argv[i];
		larg = argv[i];
	}
	if(!infilename){
		puts("<COMPILER ERROR> No input file");
		exit(1);
	}
	if(!outfilename){
		puts("<COMPILER ERROR> No output file");
		exit(1);
	}
	ifile = fopen(infilename, "rb");
	ofile = fopen(outfilename, "wb");
	if(!ifile || !ofile){
		puts("<COMPILER ERROR> Cannot open either the input or output file");
		exit(1);
	}
	entire_input_file = read_file_into_alloced_buffer(ifile, &entire_input_file_len);
	fclose(ifile); ifile = NULL;
	/*Find all instances of illegal characters and replace them with spaces.*/
	{long i = 0; long len = strlen(entire_input_file);
		for(i = 0; i < len; i++)
			{
				if(entire_input_file[i])
					if(entire_input_file[i] < 32 || entire_input_file[i] > 126){
						if(entire_input_file[i] != '\n')
							entire_input_file[i] = ' ';
						else if(!(i > 0 && entire_input_file[i-1] != '\\')){
							entire_input_file[i] = ';';\
						}
					}
			}
	}
	/*Replace all double spaces.*/
	/*
	local_search_point = strfind(entire_input_file, "  ");
	while(local_search_point > -1){
		entire_input_file = str_repl_allocf(entire_input_file, "  ", " ");
		local_search_point = strfind(entire_input_file, "  ");
	}
	*/
	/*Force widening of double parentheses groups.*/
	
	printf("\n\n");
	puts(entire_input_file);
	printf("\n\n");
	/*tokenized = parse_matched(entire_input_file, "{", "}");*/
	
	tokenized = tokenize(entire_input_file, ";");
	/*
	{strll* current = &tokenized;
		for(;current != NULL; current = current->right){
			if(current->text)
				printf("TOKEN IS:%s\n", current->text);
			if(current->child)
				printf("CHILD IS:%s\n", current->child->text);
		}
	}*/
	/**/

	{strll* current_meta = &tokenized;
		for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right)
		{
			strll* current = current_meta;
			strll* retval = current_meta;
			do{ 
				current = retval;
				current_meta = retval;
				retval = consume_until(current, "|", 0);
				if(retval != current)
					parent_right_node(current);
			}while(retval != current);
		}
	}

	{strll* current_meta = &tokenized;
		for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right)
		{
				parse_matched(current_meta, "{", "}");
		}
	}
	strll_show(&tokenized, 0);

	return 0;
}
