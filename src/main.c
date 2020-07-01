#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int generate_to_stream(FILE* stream, size_t max, size_t buffer_size);
int convert_to_csv(FILE* input, FILE* output);

void printUsage();

#define CHECK_ARGUMENT_COUNT(min) \
	if(argc < min){ \
		fprintf(stderr, "Incorrect usage\n"); \
		printUsage(); \
		return 1; \
	}

#define ERRBUFFERSIZE 255
#define ERR_CHECK(exp, ...) \
	if(exp){ \
		char str[ERRBUFFERSIZE]; \
		snprintf(str, ERRBUFFERSIZE, __VA_ARGS__); \
		perror(str); \
		return errno; \
	}

int main(int argc, char* argv[]){
	CHECK_ARGUMENT_COUNT(2);
	// 1       2           3              4
	// g - <max size> <output file> (<buffer size>)
	if(strncmp(argv[1], "g", 1) == 0){
		CHECK_ARGUMENT_COUNT(3);
		char* output = argv[3];
		size_t upper = 0;
		size_t buffer_size = BUFSIZ;
		sscanf(argv[2],"%llu", (unsigned long long*)&upper);
		if(argc > 4){
			sscanf(argv[4],"%llu", (unsigned long long*)&buffer_size);
		}
		if(upper == 0){
			fprintf(stderr, "Incorrect usage\n");
			printUsage();
			return 1;
		}

		FILE* file = fopen(output, "w");
		ERR_CHECK(file == NULL, "Failed to open %s", output);
		
		if(generate_to_stream(file, upper, buffer_size) != 0){
			fclose(file);
			ERR_CHECK(1 == 1, "Failed to generate to stream");
		}
		fclose(file);
		return 0;
	}
	else if(strncmp(argv[1], "c", 1) == 0){
		CHECK_ARGUMENT_COUNT(3);
		FILE *inputFile, *outputFile;
		char* input = argv[2];
		char* output = argv[3];

		inputFile = fopen(input, "r");
		ERR_CHECK(inputFile == NULL, "Failed to open %s", input);
		outputFile = fopen(output, "w");
		if(outputFile == NULL){
			fclose(inputFile);
			ERR_CHECK(outputFile == NULL, "Failed to open %s", output);
		}

		if(convert_to_csv(inputFile, outputFile) != 0){
			fclose(inputFile);
			fclose(outputFile);
			ERR_CHECK(1 == 1, "Failed to convert to csv");
		}

		fclose(inputFile);
		fclose(outputFile);
		return 0;
	} else{
		printUsage();
	}

	return 0;
}

void printUsage(){
	printf("Usage:\n");
	printf("g\tGenerate numbers in binary format\n\tccg g <upper limit> <output binary file> (<buffer size>)\n");
	printf("c\tConvert from binary to csv format\n\tccg c <binary input file> <csv output file>\n");
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
				goto error;
			}
		}
	}
	if(i > 0){
		fwr = fwrite(buffer, sizeof(size_t), i, stream);
		if(fwr != i){
			goto error;
		}
	}

	free(buffer);
	return 0;

error:
	free(buffer);
	return -1;
}