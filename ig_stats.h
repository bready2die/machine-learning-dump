#ifndef INFORMATION_GAIN_STATS_LIB
#define INFORMATION_GAIN_STATS_LIB 1
#include <stdbool.h>
#include "alt_data_vec.h"

struct xparam_stats {
        double x_inf_content;
        double *x_prob_vals;
        double *y_cond_prob_vals;
};


struct ig_stats {
        struct alt_data_file *da_data;
        double y_inf_content;
        double *y_prob_vals;
        struct xparam_stats *xp_stats;

	double best_ig;
	int last_start;
	int last_length;
	bool y_inf_calcd:1;
};

double get_y_inf_cont(struct ig_stats *stats, int y_start, int length);
//int get_best_ig(int y_start, int length);
int get_best_ig(struct ig_stats *stats,int y_start, int length);
//void init_ig_stats(struct alt_data_file *adf);
void init_ig_stats(struct ig_stats *stats,struct alt_data_file *adf);
//void free_ig_stats();
void free_ig_stats(struct ig_stats *stats);

#endif
