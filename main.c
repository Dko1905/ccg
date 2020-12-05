#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static const char *usage = "Usage: ccg <max number> <output file>";

static const char *license = "Copyright (c) 2020, Daniel Florescu\n\
All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions are met:\n\
\n\
 * Redistributions of source code must retain the above copyright notice,\n\
   this list of conditions and the following disclaimer.\n\
 * Redistributions in binary form must reproduce the above copyright\n\
   notice, this list of conditions and the following disclaimer in the\n\
   documentation and/or other materials provided with the distribution.\n\
\n\
THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY\n\
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n\
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n\
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY\n\
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n\
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n\
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n\
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n\
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH\n\
DAMAGE.\n";

void generate_mem(size_t min, size_t max, size_t *buffer);

int main(int argc, char **argv){
	const int min = 50;
	const int max = 150;

	size_t *buffer = malloc((max - min) * sizeof(size_t));

	generate_mem(min, max, buffer);
	for(size_t n = 0; n < max - min; ++n){
		printf("%zu,%zu\n", max - n - 1 , buffer[n]);
	}

	return 0;
}

void generate_mem(size_t min, size_t max, size_t *buffer){
	size_t n = 0;
	size_t s = 0;
	size_t i = 0;

	while((n = max-- - 1) >= min){
		s = 0;
		while(n != 1){
			if((n & 1) == 0){ /* even */
				n >>= 1;
			}
			else{ /* odd */
				n = (n * 3 + 1) / 2;
				++s;
			}
			++s;
		}

		buffer[i++] = s;
	}
}
