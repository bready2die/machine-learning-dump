#ifndef GENDATA_LIB_H
#define GENDATA_LIB_H
#include "data_vec.h"
#include "alt_data_vec.h"

void calc_y_classif_dfile(int i, struct data_file *dfile, double sigma);
void calc_y_classif_adf(int i, struct alt_data_file *adf, double sigma);

void gen_data_adf(struct alt_data_file *adf,int vecs, int varpc, double sigma,
                void (calc_y)(int, struct alt_data_file *, double));

void gen_data_dfile(struct data_file *dfile,int vecs, int varpc, double sigma,
                void (calc_y)(int, struct data_file *, double));

#endif
