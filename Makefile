CC = gcc
CFLAGS = -Wall -O3

all: classify_ntimes

classify_ntimes: normal1.o data_vec.o alt_data_vec.o classif_file.o linux_tree.o classify_lib.o gendata_lib.o ig_stats.o class_verify_lib.o classify_ntimes.o
	$(CC) $(CFLAGS) ./normal1.o ./data_vec.o ./alt_data_vec.o ./classif_file.o ./linux_tree.o ./classify_lib.o ./gendata_lib.o ./ig_stats.o ./class_verify_lib.o ./classify_ntimes.o -o classify_ntimes -lm -lpthread

classify_lib.o:
	$(CC) $(CFLAGS) -c classify_lib.c
gendata_lib.o:
	$(CC) $(CFLAGS) -c gendata_lib.c
class_verify_lib.o:
	$(CC) $(CFLAGS) -c class_verify_lib.c
classify_ntimes.o:
	$(CC) $(CFLAGS) -c classify_ntimes.c
classif_file.o:
	$(CC) $(CFLAGS) -c ./classif_file.c
normal1.o:
	$(CC) $(CFLAGS) -c ./normal1.c
data_vec.o:
	$(CC) $(CFLAGS) -c ./data_vec.c
alt_data_vec.o:
	$(CC) $(CFLAGS) -c ./alt_data_vec.c
linux_tree.o:
	$(CC) $(CFLAGS) -c ./linux_tree.c
ig_stats.o:
	$(CC) $(CFLAGS) -c ./ig_stats.c
clean:
	rm *.o classify_ntimes
