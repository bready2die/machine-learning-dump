#ifndef DATA_VEC_LIB
#define DATA_VEC_LIB 1
#include <stdio.h>

#define MIN_PARAM_COUNT 15


struct data_vec {
        int **xparams;
	int *y;
}__attribute__((packed));


struct data_file {
        unsigned int xparam_count;
        unsigned int vecs;
        unsigned int category_count;
        int *categories;
	struct data_vec *data;
}__attribute__((packed));



//void init_data_file(struct data_file *dfile, int xp_count, int vecs, int cat_count, int *cats);

struct data_vec *alloc_data_vec(struct data_file *dfile);

void load_data_vec(FILE *file, struct data_file *dfile);

int load_data_file(FILE *file, struct data_file *dfile);

void free_data_vec(struct data_file *dfile);

void write_data_file(FILE *file, struct data_file *dfile);

void sort_data_vec(struct data_file *dfile, int xparam);

void sort_data_vec_split(struct data_file *dfile, int xparam,int off, int n);

void print_xarr(struct data_file *dfile, int xparam);

void print_table(struct data_file *dfile);

void print_enums(struct data_file *dfile);
#endif
