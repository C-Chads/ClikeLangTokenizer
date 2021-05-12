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


int main(int argc, char** argv){
	char* larg; 
	long local_search_point = 0;
	larg = argv[0];
	
	for(int i = 1; i < argc; i++){
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
					if(entire_input_file[i] < 32 || entire_input_file[i] > 126)
						entire_input_file[i] = ' ';
			}
	}
	puts("\nFinished Replacing Illegals!\n");
	/*Replace all double spaces.*/
	local_search_point = strfind(entire_input_file, "  ");
	while(local_search_point > -1){
		entire_input_file = str_repl_allocf(entire_input_file, "  ", " ");
		local_search_point = strfind(entire_input_file, "  ");
	}
	printf("\n\n");
	puts(entire_input_file);
	printf("\n\n");
	tokenized = tokenize(entire_input_file, " ");
	{strll* current = &tokenized;
		for(;current != NULL; current = current->right){
			printf("TOKEN IS:%s\n", current->text);
		}
	}
	return 0;
}
