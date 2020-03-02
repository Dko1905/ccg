#include <stdio.h>

typedef long long unsigned int llu;

void generate_to_stream(FILE* stream, llu max_number);

int main(){
	char* filename = "test.csv";

	FILE* fptr = fopen(filename, "w");

	generate_to_stream(fptr, 10000);
	fclose(fptr);
}

void generate_to_stream(FILE* stream, llu max_number){
	llu steps;
	llu n_temp;
	for(llu n = 1; n < max_number; n++){
		n_temp = n; // by value
		steps = 0;
		while(n_temp != 1){
			if( (n_temp & 1) == 0){// If even
				n_temp >>= 1;
			}
			else{
				n_temp = n_temp * 3 + 1;
			}
			steps++;
		}
		fprintf(stream, "%llu,%llu\n", n, steps);
	}
}