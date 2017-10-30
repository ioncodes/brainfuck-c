#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Brainfuck {
	char *name;
	char *input;
	char *code;
	int memory[30000];
	int pc;
	int eof;
	int mc;
	int highest_mc; // debug
	int break_point; // debug
};

int init(int argc, char *argv[], struct Brainfuck *brainfuck, int *debug) {
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
	if(argc == 4 && strcmp(argv[3], "-d") == 0) {
		*debug = 1;
	} else {
		*debug = 0;
	}
	return 1;
}

void prepare(struct Brainfuck *brainfuck) {
	size_t len = strlen(brainfuck->input);
	char *code = malloc(len); // allocate memory
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
			default: continue;
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
		// [ & ] dont work because it will go to the next which is not correct. 
		// It should go to it's corresponding, but it will enter the nested loop if there's one.
		case '[': {
			if(brainfuck->memory[brainfuck->mc] == 0) {
				int counter = brainfuck->pc;
				int skips = -1; // ignore starting point, TODO: verify that this really has to be -1 
				while(1) { // todo: make for loop
					if(counter == strlen(brainfuck->code) - 1) {
						printf("Missing ] at %d", brainfuck->pc);
						exit(EXIT_FAILURE);
					}
					if(brainfuck->code[counter] == ']' && skips == 0) {
						break;
					} else if(brainfuck->code[counter] == ']' && skips > 0) {
						skips--;
					} else if(brainfuck->code[counter] == '[') {
						skips++;
					}
					counter++;
				}
				brainfuck->pc = counter;
			}
		} break;
		case ']': {
			if(brainfuck->memory[brainfuck->mc] != 0) {
				int counter = brainfuck->pc;
				int skips = -1; // ignore starting point
				while(1) { // todo: make for loop
					if(counter == -1) {
						printf("Missing [ at %d", brainfuck->pc);
						exit(EXIT_FAILURE);
					}
					if(brainfuck->code[counter] == '[' && skips == 0) {
						break;
					} else if(brainfuck->code[counter] == '[' && skips > 0) {
						skips--;
					} else if(brainfuck->code[counter] == ']') {
						skips++;
					}
					counter--;
				}
				brainfuck->pc = counter;
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
		if(brainfuck->mc > brainfuck->highest_mc) {
			brainfuck->highest_mc = brainfuck->mc;
		}
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
	brainfuck.highest_mc = 0;
	brainfuck.break_point = -1;
	int debug;
	if(!init(argc, argv, &brainfuck, &debug)) {
		printf("brainfuck -f <file>\nbrainfuck -i <code>");
		exit(EXIT_FAILURE);
	}
	prepare(&brainfuck);
	while(!brainfuck.eof) {
		if(debug && brainfuck.break_point == -1) {
			system("cls||clear");
			printf("PC: %d, MC: %d, MEM: %d, BREAK: %d\n", brainfuck.pc, brainfuck.mc, brainfuck.memory[brainfuck.mc], brainfuck.break_point);
			for (int i = 0; i < brainfuck.highest_mc + 1; i++) {
				printf("[%d]\t", brainfuck.memory[i]);
			}
			printf("\n");
			for (int i = 0; i < brainfuck.mc; i++) {
				printf("\t");
			}
			printf("^^^\n");
			for(int i = 50; i > 0; i--) {
				if(brainfuck.pc - i < 0) {
					printf(" ");
					continue;
				}
				printf("%c", brainfuck.code[brainfuck.pc - i]);
			}
			printf("%c", brainfuck.code[brainfuck.pc]);
			for(int i = 1; i <= 50; i++) {
				printf("%c", brainfuck.code[brainfuck.pc + i]);
			}
			printf("\n");
			for(int i = 0; i < 50; i++) {
				printf(" ");
			}
			printf("^");
			printf("\n");
			char *cmd = (char*)malloc(100);
		    fgets(cmd, sizeof(cmd), stdin);
			if(strcmp(cmd, "\n") != 0) {
				int break_point = atoi(cmd);
				brainfuck.break_point = break_point;
			}
		} else if(brainfuck.break_point == brainfuck.pc) {
			brainfuck.break_point = -1;
		}
		execute(&brainfuck);
	}
	return 0;
}