#ifndef CLASSIFY_LIB_H
#define CLASSIFY_LIB_H

#include "ig_stats.h"
#include "classif_file.h"
#include "alt_data_vec.h"

struct classif_context {
        struct classif_obj *cla_obj;
        struct alt_data_file *adf;
        struct ig_stats *stats;
};


void gen_decision_tree_split(struct classif_context *context,int depth);

void gen_decision_tree_samp_split(struct classif_context *context,int samp_size);

void classif_context_init(struct classif_context *context,
                        struct alt_data_file *adf,
                        struct ig_stats *stats,
                        struct classif_obj *cla_obj);

void reset_classif_context(struct classif_context *context);

void free_classif_context(struct classif_context *context);
#endif
