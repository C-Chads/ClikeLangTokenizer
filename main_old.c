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
			if(current->left)
			{	for(i = 0; i < lvl; i++) printf("\t");
				printf("LCHILDREN:\n");
				strll_show(current->left, lvl + 1);
			}
			if(current->child)
			{	for(i = 0; i < lvl; i++) printf("\t");
				printf("CHILDREN:\n");
				strll_show(current->child, lvl + 1);
			}
		}
	}
}
static void strll_recursive_parse_matched(strll* current_meta, char* l, char* r){
	{
		for(;current_meta != NULL && current_meta->text != NULL && current_meta->text[0] != '\"';
			current_meta = current_meta->right)
		{
				parse_matched(current_meta, l, r);
				if(current_meta->left)
					strll_recursive_parse_matched(current_meta->left, l, r);
				if(current_meta->child)
					strll_recursive_parse_matched(current_meta->child, l, r);
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
	/*if(entire_input_file_len > 0) entire_input_file[entire_input_file_len-1] = '\0';*/
	fclose(ifile); ifile = NULL;
	/*Find all instances of illegal characters and replace them with spaces.*/
	/*
	{long i = 0; long len = strlen(entire_input_file);
		for(i = 0; i < len; i++)
			{
				if(entire_input_file[i])
					if(entire_input_file[i] < 32 || entire_input_file[i] > 126){
						if(entire_input_file[i] != '\n')
							entire_input_file[i] = ' ';
						else if((i > 0 && entire_input_file[i-1] != '\\')){
							entire_input_file[i] = ';';\
						}
					}
			}
	}
	*/
	/*Force widening of double parentheses groups.*/
	
	printf("\n\n");
	puts(entire_input_file);
	printf("\n\n");
	/*tokenized = parse_matched(entire_input_file, "{", "}");*/
	
	tokenized = tokenize(entire_input_file, "\n");
	puts("~~~~~AFTER TOKENIZATION~~~~~");
	strll_show(&tokenized, 0);
	puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	{strll* current_meta = &tokenized;
			for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right){
					while(
					current_meta->text && 
							(
								isspace(current_meta->text[0])
							)
					)
					{
						char* text_old = current_meta->text;
						char* text_new = strcatalloc(text_old + 1, "");
						free(text_old);
						current_meta->text = text_new;
					}
			}
	}
	{strll* current_meta = &tokenized;
		for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right)
		{
	
			strll* current = current_meta;
			strll* retval = current_meta;
			do{
				current = retval;
				current_meta = retval;
				if(current->text){
					 long loc_next_semicolon;
					long loc_firstquote = strfind(current->text, "\"");
					loc_next_semicolon = strfind(current->text, ";");
					/*FOUND SOME QUOTES*/
					if(
						loc_firstquote > -1 &&
						!(loc_next_semicolon > -1 && loc_next_semicolon < loc_firstquote)
					)
					{ long ii;
						/*There is indeed a set of quotes!*/
						if(loc_firstquote > 0){
							retval = consume_bytes(current, loc_firstquote);
							current = retval;
							current_meta = retval;
						} else {
							/*printf("\nCurrent seems to have a quote at the beginning...%s\n", current->text);*/
						}
						for(ii = 1; current->text[ii]!= '\0'; ii++){
							if(current->text[ii] == '\\' && current->text[ii] != '\0'){
								ii++;continue; /*Skip the escaped character.*/
							} else if(current->text[ii] == '\\' && current->text[ii] == '\0'){
								puts("ERROR: unmatched quote group.");
								exit(1);
							}
							if(current->text[ii] == '\"'){
								break;
							}
						}
						if(current->text[ii] != '\"'){
							puts("ERROR: unmatched quote group.");
							exit(1);
						}
						retval = consume_bytes(current, ii+1); 
						/*Consume the second quote too.*/
						current = retval;
						current_meta = retval;
					} else {
						retval = consume_until(current, ";", 0);
					}
				} else {
					puts("Internal Error.");
					exit(1);
				}
			}while(retval != current);
		}
	}
	{strll* current_meta = &tokenized;
			for(;current_meta != NULL && current_meta->text != NULL; current_meta = current_meta->right){
					while(
					current_meta->text && 
							(
								isspace(current_meta->text[0])
							)
					)
					{
						char* text_old = current_meta->text;
						char* text_new = strcatalloc(text_old + 1, "");
						free(text_old);
						current_meta->text = text_new;
					}
					while(
					current_meta->text && 
							(	strlen(current_meta->text) > 0 &&
								isspace(current_meta->text[strlen(current_meta->text)-1])
							)
					)
					{
						char* text_old = current_meta->text;
						char* text_new = str_null_terminated_alloc(text_old, strlen(text_old) - 1);
						free(text_old);
						current_meta->text = text_new;
					}
			}
	}
	strll_recursive_parse_matched(&tokenized, "{", "}");
	strll_show(&tokenized, 0);
	return 0;
}
