#ifndef CLASS_VERIFY_LIB_H
#define CLASS_VERIFY_LIB_H
#include "alt_data_vec.h"
#include "classif_file.h"

double calc_err(struct classif_obj *cla_obj, struct alt_data_file *adf);
double calc_err_logistic(struct classif_obj *cla_obj, struct alt_data_file *adf);

#endif
