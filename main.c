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
	while(head->right) {head = head->right; len++;}
	return len;
}

static strll* parse_matched(strll* current_node, const char* tl, const char* tr){
	strll* current_child; strll* child_old; strll* right_old;
	long current_tl_location; long counter = 1;
	long current_tr_location; long start_tl_location;
	long len_tl;
	long len_tr;
	len_tl = strlen(tl);
	len_tr = strlen(tr);
	current_tl_location = strfind(current_node->text, tl);
	start_tl_location = current_tl_location;
	current_tr_location = strfind(current_node->text, tr);
	if(current_tr_location > -1 && 
		current_tr_location < current_tl_location){
		printf("\n<SYNTAX ERROR> %s before %s\n", tr, tl);
		exit(1);
	}
	if(current_tr_location == -1 &&
		current_tl_location != -1){
		printf("\n<SYNTAX ERROR> You have a %s, but no matching %s ?", tl, tr);
		exit(1);
	}
	/*We could not find a bracketed pair!*/
	if(current_tl_location == -1){
		return current_node;
	}
	current_child = current_node->child;
	child_old = current_node->child;
	current_node->child = STRUTIL_CALLOC(1, sizeof(strll));
	current_node->child->right = child_old;
	right_old = current_node->right;
	current_node->right = STRUTIL_CALLOC(1, sizeof(strll));
	current_node->right->right = right_old;
	/*Inch along, incrementing on*/
	{ char* begin;long off = 0;
		char* metaproc = current_node->text + current_tl_location + len_tl;
		begin = metaproc;
		current_tl_location = strfind(metaproc, tl);
		current_tr_location = strfind(metaproc, tr);
		while(counter > 0){
			current_tl_location = strfind(metaproc, tl);
			current_tr_location = strfind(metaproc, tr);
			/*Handle the erroneous case- we cannot find tr.*/
			if(current_tr_location == -1){
				printf("\n<SYNTAX ERROR> Unmatched %s,%s group.\n", tl, tr);
				exit(1);
			}
			if(current_tl_location != -1 &&
			current_tl_location < current_tr_location){
				printf("<DBG> incrementing counter, it is now %ld. Text: %s\n", counter, metaproc);
				off += current_tl_location + len_tl;
				metaproc += current_tl_location + len_tl; counter++;
			} else {
				printf("<DBG> decrementing counter, it is now %ld. Text: %s\n", counter, metaproc);
				metaproc += current_tr_location + len_tr; counter--;
				off += current_tr_location + len_tr;
			}
		}
		if(counter < 0){
			printf("\n<INTERNAL ERROR> Counter for %s,%s group somehow went negative.\n", tl, tr);
			exit(1);
		}
		current_node->child->text = str_null_terminated_alloc(begin, off - len_tr);
		printf("\nencapsulated text is %s\n",current_node->child->text );
		{
			char* text_old = current_node->text;
			current_node->text = str_null_terminated_alloc(text_old, start_tl_location);
			
			printf("\npre text is %s\n",current_node->text);

			/*Grab the post-text.*/
			current_node->right->text = strcatalloc(begin + off, "");
			free(text_old);
		}
	}
	return current_node->child;
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
						else if(!(i > 0 && entire_input_file[i-1] == '\\')){
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
			}while(retval != current);
		}
	}

	{strll* current_meta = &tokenized;
		for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right)
		{
				parse_matched(current_meta, "{", "}");
		}
	}

	{strll* current = &tokenized;
		for(;current != NULL; current = current->right){
			if(current->text)
				printf("TOKEN IS:%s\n", current->text);
			if(current->child)
				printf("\tCHILD IS:%s\n", current->child->text);
		}
	}
	return 0;
}
