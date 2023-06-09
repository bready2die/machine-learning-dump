#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "alt_data_vec.h"
#include "ig_stats.h"
#include "classif_file.h"
#include "classify_lib.h"
#include "globals.h"


#define CATEGORY(_context,_cat)  (_context)->adf->categories[(_cat)]

#define CAT_COUNT(_context) (_context)->adf->category_count

#define Y_VAL(_context,_i) (_context)->adf->alt_data[(_i)].y

#define X_VAL(_context,_xparam,_i) (_context)->adf->alt_data[(_i)].xparams[(_xparam)]

static int find_best_var_split(struct classif_context *context,
			int off, int n)
{
        return get_best_ig(context->stats,off,n);
}


static int get_best_split(struct classif_context *context,int xparam, int val_index, int off, int n)
{
        int ret = 0;
        //printf("splitting on value %d\n",adf.categories[val_index]);
        for (int i = 0; i < n; i++) {
                if (X_VAL(context,xparam,off + i) != CATEGORY(context,val_index)) {
                        ret = i;
                        break;
                }
        }
	/*
	if (context->stats->best_ig == 0)
		ret = n / 2;
	*/
        return ret;
}

static double get_best_yval(struct classif_context *context,int off, int len)
{
        int ret_index = -1;
        int highscore = 0;
        int counter[UNIQ_DATA_VALS];

        memset(counter,0,UNIQ_DATA_VALS * sizeof(int));

        for (int i = 0; i < len; i++) {
                for (int j = 0; j < UNIQ_DATA_VALS; j++) {
                        if (Y_VAL(context,off + i) == CATEGORY(context,j)) {
                                counter[j]++;
                                break;
                        }
                }
        }
        for (int i = 0; i < UNIQ_DATA_VALS; i++) {
                if (counter[i] > highscore) {
                        ret_index = i;
                        highscore = counter[i];
                }
        }
        return CATEGORY(context,ret_index);
}


static double get_best_yval_avg(struct classif_context *context,int off, int len)
{
	double ret = 0;
        for (int i = 0; i < len; i++) {
                for (int j = 0; j < UNIQ_DATA_VALS; j++) {
                        if (Y_VAL(context,off + i) == CATEGORY(context,j)) {
                                ret += CATEGORY(context,j);
                                break;
                        }
                }
        }
        return ret / len;
}

static void gen_decision_tree_r_split(struct classif_context *context,
				int min, int max, int depth, int child_no)
{
        //printf("oy\n");
        int xparam;
        int mid;
        //printf("before:\n");
        //alt_data_print_xarr_split(&adf,xparam,min, max - min);
        //printf("min:%d\tmid:%d\tmax:%d\n",min,mid,max);
        if (depth == 0 || (max - min) == 0) {
                add_classif_entry_leaf(context->cla_obj,
				get_best_yval(context,min, (max - min) + 1),
                                       child_no);
                go_up(context->cla_obj);
                return;
        }
        xparam = find_best_var_split(context,min, max - min);
        //printf("before:\n");
        //alt_data_print_xarr_split(&adf,xparam,min, max - min);
        sort_alt_data_vec_split(context->adf, xparam, min, max - min);
        //printf("after:\n");
        //alt_data_print_xarr_split(&adf,xparam,min, max - min);
        mid = min + get_best_split(context,xparam,0,min, max - min);

        add_classif_entry(context->cla_obj,xparam,child_no);
        gen_decision_tree_r_split(context,min,mid,depth - 1,LEFT_CHILD);
        gen_decision_tree_r_split(context,mid + 1, max, depth - 1, RIGHT_CHILD);
        go_up(context->cla_obj);
}

void gen_decision_tree_split(struct classif_context *context,int depth)
{
        gen_decision_tree_r_split(context,0,context->adf->vecs - 1, depth, 0);
}


static void gen_decision_tree_samp_r_split(struct classif_context *context,
					int min, int max,int samp_size,int child_no)
{
        int xparam;
        int mid;
	double cur_y_inf_cont;

	if (samp_size >= (max - min))
		goto term;

        cur_y_inf_cont = get_y_inf_cont(context->stats,min,(max - min) + 1);
	if (cur_y_inf_cont == 0)
		goto term;

        xparam = find_best_var_split(context,min, (max - min) + 1);
	if (xparam < 0)
		goto term;

	sort_alt_data_vec_split(context->adf, xparam, min, (max - min) + 1);
        mid = min + get_best_split(context,xparam,0,min, (max - min) + 1);
        //printf("min:%d mid:%d max:%d\n",min,mid,max);
        add_classif_entry(context->cla_obj,xparam,child_no);

        gen_decision_tree_samp_r_split(context,min,mid - 1,samp_size,LEFT_CHILD);
        gen_decision_tree_samp_r_split(context,mid,max,samp_size,RIGHT_CHILD);
        go_up(context->cla_obj);
        return;

term:
	add_classif_entry_leaf(context->cla_obj,
				get_best_yval_avg(context,min, (max - min) + 1),
                                       child_no);
	go_up(context->cla_obj);
	return;
}

void gen_decision_tree_samp_split(struct classif_context *context,int samp_size)
{
	//printf("head before:%p\n",context->cla_obj->classif_tree);
        gen_decision_tree_samp_r_split(context,0,context->adf->vecs-1,samp_size,0);
	//printf("head after:%p\n",context->cla_obj->classif_tree);
	//print_classif_obj(context->cla_obj);
	reset_pos(context->cla_obj);
}


void classif_context_init(struct classif_context *context,
			struct alt_data_file *adf,
			struct ig_stats *stats,
			struct classif_obj *cla_obj)
{
	context->adf = adf;
	context->stats = stats;
	context->cla_obj = cla_obj;
        init_classif_obj(context->cla_obj);
}

void reset_classif_context(struct classif_context *context)
{
	free_classif_obj(context->cla_obj);
	init_classif_obj(context->cla_obj);
}

void free_classif_context(struct classif_context *context)
{
	free_classif_obj(context->cla_obj);
}
