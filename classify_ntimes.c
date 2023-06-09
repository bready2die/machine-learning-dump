#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <sys/random.h>
#include <pthread.h>
#include "gendata_lib.h"
#include "classify_lib.h"
#include "class_verify_lib.h"


int varpc;

int train_vecs;
int test_vecs;

double sigma;

int samp_start;
int samp_ciel;
int samp_inc;

int threads;
int runs_per_thread;
pthread_barrier_t class_barrier;

int plot_length;

double avg_best_sample_size;
pthread_mutex_t best_sample_size_lock = PTHREAD_MUTEX_INITIALIZER;

double *err_plot;
pthread_mutex_t err_plot_lock = PTHREAD_MUTEX_INITIALIZER;

double *sup_plot;
pthread_mutex_t sup_plot_lock = PTHREAD_MUTEX_INITIALIZER;

static double *alloc_plot(int count)
{
	double *out = calloc(count,sizeof(double));
	return out;
}

static void free_plot(double *plot)
{
	free(plot);
}

void *class_child(void *args)
{
	struct alt_data_file train_data;
	struct alt_data_file test_data;
	struct ig_stats stats;
	struct classif_obj cla_obj;
	struct classif_context class_cont;
	double *temp_err_plot = alloc_plot(plot_length);
	double *temp_sup_plot = alloc_plot(plot_length);
	int cum_best_index = 0;
	int plot_pos;

	for (int i = 0; i < runs_per_thread; i++) {
		int cur_best_samp_size = 0;
		double cur_best_err = DBL_MAX;

		gen_data_adf(&train_data,train_vecs,varpc,sigma,calc_y_classif_adf);
		gen_data_adf(&test_data,test_vecs,varpc,sigma,calc_y_classif_adf);

		init_ig_stats(&stats,&train_data);
		classif_context_init(&class_cont,&train_data,&stats,&cla_obj);

		for (int j = samp_start, plot_pos = 0; plot_pos < plot_length; j += samp_inc, plot_pos++) {
			double cur_err;
			gen_decision_tree_samp_split(&class_cont,j);

			//print_classif_obj(class_cont.cla_obj);
			cur_err = calc_err_logistic(&cla_obj,&test_data);
			temp_sup_plot[plot_pos] += get_superfluous(&cla_obj);
			temp_err_plot[plot_pos] += cur_err;

			if (cur_err <= cur_best_err) {
				cur_best_err = cur_err;
				cur_best_samp_size = j;
			}
			reset_classif_context(&class_cont);
		}

		cum_best_index += cur_best_samp_size;

		free_classif_context(&class_cont);
		free_ig_stats(&stats);
		empty_alt_data_file(&train_data);
		empty_alt_data_file(&test_data);
	}

	pthread_mutex_lock(&best_sample_size_lock);
	avg_best_sample_size += cum_best_index;
	pthread_mutex_unlock(&best_sample_size_lock);

	pthread_mutex_lock(&sup_plot_lock);
	for (int i = 0; i < plot_length; i++)
		sup_plot[i] += temp_sup_plot[i];
	pthread_mutex_unlock(&sup_plot_lock);

	pthread_mutex_lock(&err_plot_lock);
	for (int i = 0; i < plot_length; i++)
		err_plot[i] += temp_err_plot[i];
	pthread_mutex_unlock(&err_plot_lock);

	free_plot(temp_sup_plot);
	free_plot(temp_err_plot);
	pthread_barrier_wait(&class_barrier);
	return NULL;
}



void print_plots(int count)
{
	printf("%d ",count);
	printf("%lf ",avg_best_sample_size);
	for (int i = 0; i < count; i++)
		printf("%lf ",err_plot[i]);
	for (int i = 0; i < count; i++)
		printf("%lf ",sup_plot[i]);
	//printf("\n");
}



int main(int argc, char **argv)
{
	long seed;
	pthread_t children;
	int run_count;

	getrandom(&seed,sizeof(seed),GRND_RANDOM);
	srand48(seed);

	varpc = atoi(argv[1]);
	train_vecs = atoi(argv[2]);
	test_vecs = atoi(argv[3]);
	sigma = atof(argv[4]);
	samp_start = atoi(argv[5]);
	samp_ciel = atoi(argv[6]);
	samp_inc = atoi(argv[7]);
	threads = atoi(argv[8]);
	runs_per_thread = atoi(argv[9]);

	run_count = threads * runs_per_thread;

	plot_length = ((samp_ciel - samp_start) / samp_inc);

	err_plot = alloc_plot(plot_length);
	sup_plot = alloc_plot(plot_length);

	pthread_barrier_init(&class_barrier,NULL,threads+1);

	for (int i = 0; i < threads; i++)
		pthread_create(&children,NULL,class_child,NULL);

	pthread_barrier_wait(&class_barrier);

	avg_best_sample_size /= run_count;

	for (int i = 0; i < plot_length; i++) {
		err_plot[i] /= run_count;
		sup_plot[i] /= run_count;
	}

	print_plots(plot_length);
	free_plot(sup_plot);
	free_plot(err_plot);
}
