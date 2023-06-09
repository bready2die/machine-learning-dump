#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "data_vec.h"
#include "alt_data_vec.h"
#include "ig_stats.h"
#include "classif_file.h"
#include "globals.h"
FILE *in_file, *out_file;

#define SAMP_SIZE 0
#define REG_DEPTH 1
static int prune_type = SAMP_SIZE;
//static int samp_size = 0;
//static int depth = 0;
static int prune_param = 0;


//struct regress_file reg_file;
struct classif_obj cla_obj;
//static int regress_pos = 0;

//struct data_file dfile;
struct alt_data_file adf;

struct ig_stats stats;

int find_best_var_split(int off, int n)
{
	return get_best_ig(&stats,off,n);
}
int get_best_split(int xparam, int val_index, int off, int n)
{
	int ret = 0;
	//printf("splitting on value %d\n",adf.categories[val_index]);
	for (int i = 0; i < n; i++) {
		if (adf.alt_data[off + i].xparams[xparam] != adf.categories[val_index]) {
			ret = i;
			break;
		}
	}
	
	if(stats.best_ig == 0)
		ret = n / 2;
	return ret;
}

int print_ab_arr(double *a, double *b, int n)
{
	for (int i = 0; i < n; i++)
		printf("element %d: x:%f y:%f\n",i,a[i],b[i]);
	return 0;
}

double get_best_yval(int off, int len)
{
	printf("off:%d length:%d\n",off,len);
	int ret_index = -1;
	int highscore = 0;
	int counter[UNIQ_DATA_VALS];

	memset(counter,0,UNIQ_DATA_VALS * sizeof(int));

	for (int i = 0; i < len; i++) {
		for (int j = 0; j < UNIQ_DATA_VALS; j++) {
			if (adf.alt_data[off + i].y == adf.categories[j]) {
				counter[j]++;
				break;
			}
		}
	}
	for (int i = 0; i < UNIQ_DATA_VALS; i++) {
		printf("counter[i]:%d\n",counter[i]);
		if (counter[i] > highscore) {
			ret_index = i;
			highscore = counter[i];
		}
	}
	printf("ret_index:%d\n",ret_index);
	return adf.categories[ret_index];
}


void gen_decision_tree_r_split(int min, int max, int depth, int child_no)
{
	//printf("oy\n");
	int xparam;
	int mid;
	//printf("before:\n");
	//alt_data_print_xarr_split(&adf,xparam,min, max - min);
	//printf("min:%d\tmid:%d\tmax:%d\n",min,mid,max);
	if (depth == 0 || (max - min) == 0) {
		add_classif_entry_leaf(&cla_obj,
				       get_best_yval(min, (max - min) + 1),
				       child_no);
		go_up(&cla_obj);
		return;
	}
	xparam = find_best_var_split(min, max - min);
	//printf("before:\n");
	//alt_data_print_xarr_split(&adf,xparam,min, max - min);
	sort_alt_data_vec_split(&adf, xparam, min, max - min);
	//printf("after:\n");
	//alt_data_print_xarr_split(&adf,xparam,min, max - min);
	mid = min + get_best_split(xparam,0,min, max - min);

	add_classif_entry(&cla_obj,xparam,child_no);
	gen_decision_tree_r_split(min,mid,depth - 1,LEFT_CHILD);
	gen_decision_tree_r_split(mid + 1, max, depth - 1, RIGHT_CHILD);
	go_up(&cla_obj);
}

void gen_decision_tree_split(int depth)
{
	gen_decision_tree_r_split(0,adf.vecs - 1, depth, 0);
}

void gen_decision_tree_samp_r_split(int min, int max,int samp_size,int child_no)
{
	int xparam;
	int mid;
	double cur_y_inf_cont = get_y_inf_cont(&stats,min,(max - min) + 1);
	//alt_print_table(&adf);
	alt_print_table_split(&adf,min,(max - min) + 1);
	if ( samp_size >= (max - min) || cur_y_inf_cont == 0) {
		//printf("oyoyooy\n");
	        add_classif_entry_leaf(&cla_obj,
				       get_best_yval(min, (max - min) + 1),
				       child_no);
		go_up(&cla_obj);
		return;
	}

	xparam = find_best_var_split(min, (max - min) + 1);
	sort_alt_data_vec_split(&adf, xparam, min, (max - min) + 1);
	mid = min + get_best_split(xparam,0,min, (max - min) + 1);
	printf("min:%d mid:%d max:%d\n",min,mid,max);
	add_classif_entry(&cla_obj,xparam,child_no);

	gen_decision_tree_samp_r_split(min,mid - 1,samp_size,LEFT_CHILD);
	gen_decision_tree_samp_r_split(mid,max,samp_size,RIGHT_CHILD);
	go_up(&cla_obj);
	return;
}

void gen_decision_tree_samp_split(int samp_size)
{
	gen_decision_tree_samp_r_split(0,adf.vecs-1,samp_size,0);
}

int fill_regression_file_split( int prune_param)
{
	if (prune_type == REG_DEPTH)
		gen_decision_tree_split(prune_param);
	else
		gen_decision_tree_samp_split(prune_param);
	return 0;
}

int prep_data_for_regress_split(int prune_param)
{
	struct data_file dfile;
	load_data_file(in_file,&dfile);
	print_enums(&dfile);
	init_alt_data_file(&adf,&dfile);
	printf("oy\n");
	free_data_vec(&dfile);
	init_ig_stats(&stats,&adf);
	printf("xparam_count %u\n",adf.xparam_count);
	printf("vecs %u\n",adf.vecs);
	init_classif_obj(&cla_obj);
	return 0;
}

int main(int argc, char **argv)
{
	if (!strncmp("d",argv[1],strlen("d")))
		prune_type = REG_DEPTH;
	in_file = fopen(argv[2],"r");
	if (!in_file) {
		fprintf(stderr,"couldn't open %s",argv[2]);
		perror("");
		return EXIT_FAILURE;
	}
	prune_param = atoi(argv[3]);
	out_file = fopen(argv[4],"w+");

	prep_data_for_regress_split(prune_param);
	fill_regression_file_split(prune_param);
	print_classif_obj(&cla_obj);
	print_superfluous(&cla_obj);
	write_classif_obj(out_file,&cla_obj);
	free_classif_obj(&cla_obj);
	empty_alt_data_file(&adf);
	free_ig_stats(&stats);
	fclose(in_file);
	fclose(out_file);
}

