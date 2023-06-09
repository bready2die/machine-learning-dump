#ifndef ALT_DATA_VEC_LIB
#define ALT_DATA_VEC_LIB 1
#include "data_vec.h"
#define MAX_XPARAM 15


struct alt_data_vec {
        int xparams[MAX_XPARAM];
	int y;
};


struct alt_data_file {
        int *categories;
        struct alt_data_vec *alt_data;
	unsigned int vecs;
        unsigned int xparam_count;
        unsigned int category_count;
};

void new_init_alt_data_file(struct alt_data_file *altfile, int xp_count, int vecs,int cat_count);

void init_alt_data_file(struct alt_data_file *altfile, struct data_file *dfile);

void sort_alt_data_vec_split(struct alt_data_file *altfile, int xparam, int off, int n);

int cmp_alt_data_vec(const void *arg1, const void *arg2, void *arg3);

void sort_alt_data_vec(struct alt_data_file *altfile, int xparam);

void empty_alt_data_file(struct alt_data_file *altfile);

void alt_data_to_data_vec(struct alt_data_file *altfile, struct data_file *dfile);

void alt_data_print_xarr_split(struct alt_data_file *altfile, int xparam,int off, int len);

void alt_print_table(struct alt_data_file *altfile);

void alt_print_table_split(struct alt_data_file *altfile, int off, int len);
#endif
