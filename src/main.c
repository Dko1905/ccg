#include <stdio.h>
#include <stdlib.h>

int generate_to_stream(FILE* stream, size_t max, size_t buffer_size);
int convert_to_csv(FILE* input, FILE* output);

void test1();
void test2();

int main(){
	test1();
	printf("Generated\n");
	test2();

	return 0;
}

void test1(){
	char* filename = "input.bin";

	FILE* file = fopen(filename, "w");
	if(file == NULL){
		perror("Failed to open file");
		return;
	}
	int r = generate_to_stream(file, 1000000000, BUFSIZ*100);
	if(r < 0){
		fprintf(stderr, "Failed to generate to stream\n");
	}

	fclose(file);
}

void test2(){
	char* input = "input.bin";
	char* output = "output.csv";

	FILE* inputFile = fopen(input, "r");
	if(inputFile == NULL){
		perror("Failed to open input file");
		return;
	}
	FILE* outputFile = fopen(output, "w");
	if(outputFile == NULL){
		perror("Failed to open input file");
		return;
	}
	convert_to_csv(inputFile, outputFile);

	fclose(inputFile);
	fclose(outputFile);
}

int convert_to_csv(FILE* input, FILE* output){
	size_t max, r;
	fread(&max, sizeof(size_t), 1, input);

	for(size_t n = max; n > 0 && fread(&r, sizeof(size_t), 1, input) > 0; --n){
		fprintf(output, "%llu,%llu\n", (unsigned long long)n, (unsigned long long)r);
	}
	return 0;
}

int generate_to_stream(FILE* stream, size_t max, size_t buffer_size){
	size_t* buffer = malloc(buffer_size*sizeof(size_t));
	size_t n, s, i, fwr;// n = the current number, s is the amount of numbers, i is the buffer index and fwr is the write result.

	fwrite(&max, sizeof(size_t), 1, stream);
	i = 0;
	while((n = max--) > 0){
		s = 0;
		while(n != 1){
			if( (n & 1) == 0){ // Even
				n >>= 1;
			}
			else{ // Odd
				n = n * 3 + 1;
			}
			++s;
		}
		buffer[i++] = s + 1;
		if(i >= buffer_size){
			i = 0;
			fwr = fwrite(buffer, sizeof(size_t), buffer_size, stream);
			if(fwr != buffer_size){
				return -1;
			}
		}
	}
	if(i > 0){
		fwr = fwrite(buffer, sizeof(size_t), i, stream);
		if(fwr != i){
			return -1;
		}
	}

	free(buffer);
	return 0;
}