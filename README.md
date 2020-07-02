# Collatz Conjecture Generator
This program aims to be one of the fastest to generate **BIG** files, that can hold varius infomation about the algorithm.
The program currently is only single threaded, but the `new` branch has a multi threaded buffer-less, generator for csv files. But this generator is slower than the single threaded 'raw' generator that is in this branch.

## Usage
Run the program without any arguments to get the usage. Here is the output of running the program without any arguments.
```sh
g	Generate numbers in binary format
	ccg g <upper limit> <output binary file> (<buffer size>)
c	Convert from binary to csv format
	ccg c <binary input file> <csv output file>
```

## License
MIT