#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define N		(10)

int stack[N];
int x = 0;
int i = 0;
int line = 1;

bool number;

void stackNumber(){
	stack[i++] = x;
	//printf("input nmbr: %d\n", x);
	number = false;
	x = 0;
}
void newLine(){
	line++;
	i = 0;
	x = 0;
	number = false;
	
}

int main(void)
{
	int c;
	

	bool fin = false;
	bool goNewLine = false;

	while(!fin){
		
		c = getchar();
		if(c == EOF){
			fin = true;
			continue;
		}
		else if(c == '\n'){
			if(number){
				printf("line %d: error at \\n\n", line);
			}
			else if(i==0){
				printf("line %d: error at \\n\n", line);
			}
			else{
				printf("line %d: %d\n", line, stack[0]);
			}
			newLine();
			continue;
		}
		else if(isdigit(c)){
			//printf("in isdigit: i = %d\n", i);
			x = 10 * x + c - '0';
			number = true;
		} 	
		else if((c == '+' || c == '-' || c == '/' || c == '*')){
			if(number) stackNumber(); // kanske behöver förbättras om det dyker upp problem.
			
			int result;
			int divz;
			if(i<2){
				printf("line %d: error at %c\n", line, c);
				newLine();
				goNewLine = true;
			} else {
			switch(c){
				case '+':
					result = stack[i-2] + stack[i-1];
					stack[i-2] = result;
					i--;
					//printf("+\n", result);
					//printf("+\n");
					break;

				case '-':
					result = stack[i-2] - stack[i-1];
					stack[i-2] = result;
					i--;
					//printf("-\n");
					break;

				case '/':
					divz = stack[i-1];
					if(divz == 0){
						//lägg till errorhandling

						printf("line %d: error at /\n", line);
						newLine();
						goNewLine = true;
					} else {

					result = stack[i-2] / stack[i-1];
					stack[i-2] = result;
					i--;
					}
					//printf("/\n");
					break;
				
				case '*':
					result = stack[i-2] * stack[i-1];
					stack[i-2] = result;
					i--;
					//printf("res: %d \n", result);
					//printf("*\n");
					break;

				}
			}
		}
		else if(number){
			if(i > 9){

				printf("line %d: error at %d\n", line, x);
				newLine();
				goNewLine = true;
			} else{
			stackNumber();
			}
		
		}
		else if(!isspace(c)){
			printf("line %d: error at %c\n", line, c);
			goNewLine = true;
		}

		if(goNewLine){
			while((c = getchar()) != '\n'){

			}
			goNewLine = false;
		}
		
		
	}
	
	return 0;
}
