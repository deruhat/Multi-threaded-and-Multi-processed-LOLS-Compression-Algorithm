all: compressR_LOLS compressT_LOLS compressR_worker_LOLS

compressR_LOLS:
	gcc -Wall -g -o compressR_LOLS compressR_LOLS.c

compressT_LOLS:
	gcc -Wall -g -pthread -o compressT_LOLS compressT_LOLS.c
	
compressR_worker_LOLS:
	gcc -Wall -g -o compressR_worker_LOLS compressR_worker_LOLS.c
	
clean:
	rm -rf compressR_LOLS compressT_LOLS compressR_worker_LOLS 