#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Brainfuck {
	char *name;
	char *input;
	char *code;
	int memory[30000];
	int current_jump;
	int pc;
	int eof;
	int mc;
};

int init(char *argv[], struct Brainfuck *brainfuck) {
	if(strcmp(argv[1], "-i") == 0) {
		brainfuck->name = "";
		brainfuck->input = argv[2];
	} else if(strcmp(argv[1], "-f") == 0) {
		brainfuck->name = argv[2];
		FILE *f = fopen(brainfuck->name, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *string = malloc(fsize + 1);
		fread(string, fsize, 1, f);
		fclose(f);

		string[fsize] = 0;
		brainfuck->input = string;
	} else {
		return 0;
	}
	return 1;
}

void prepare(struct Brainfuck *brainfuck) {
	size_t len = strlen(brainfuck->input);
	char *code;
	int code_counter = 0;
	for(int i = 0; i < len; i++) {
		const char instruction = brainfuck->input[i];
		switch(instruction) {
			case '<': break;
			case '>': break;
			case '[': break;
			case ']': break;
			case ',': break;
			case '.': break;
			case '-': break;
			case '+': break;
			default:
				continue;
		}
		code[code_counter] = instruction;
		code_counter++;
	}

	brainfuck->code = code;
}

void execute(struct Brainfuck *brainfuck) {
	size_t len = strlen(brainfuck->code);
	const char instruction = brainfuck->code[brainfuck->pc];
	switch(instruction) {
		case '<': brainfuck->mc--; break;
		case '>': brainfuck->mc++; break;
		case '[': brainfuck->current_jump = brainfuck->pc; break;
		case ']': {
			if(brainfuck->memory[brainfuck->mc] > 0) {
				brainfuck->pc = brainfuck->current_jump;
			} else {
				brainfuck->current_jump = -1;
			}
		} break;
		case ',': {
			char c;
			scanf(" %c", &c);
			brainfuck->memory[brainfuck->mc] = (int)c;
		} break;
		case '.': printf("%c", (char)brainfuck->memory[brainfuck->mc]); break;
		case '-': brainfuck->memory[brainfuck->mc]--; break;
		case '+': brainfuck->memory[brainfuck->mc]++; break;
	}
	if(len == brainfuck->pc - 1) {
		brainfuck->eof = 1;
	} else {
		brainfuck->pc++;
	}
}

int main(int argc, char *argv[]) {
	if(argc <= 2) {
		printf("brainfuck -f <file>\nbrainfuck -i <code>");
		exit(EXIT_FAILURE);
	}
	struct Brainfuck brainfuck;
	brainfuck.eof = 0;
	brainfuck.pc = 0;
	brainfuck.mc = 0;
	brainfuck.current_jump = -1;
	if(!init(argv, &brainfuck)) {
		printf("brainfuck -f <file>\nbrainfuck -i <code>");
		exit(EXIT_FAILURE);
	}
	prepare(&brainfuck);
	while(!brainfuck.eof) {
		execute(&brainfuck);
	}
	return 0;
}