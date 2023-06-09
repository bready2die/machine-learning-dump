#include <stdlib.h>
#include <math.h>
#include "ig_stats.h"

//#define IG_STATS_DEBUG

#ifdef IG_STATS_DEBUG

//#define PRINT_Y_PROBS
//#define PRINT_X_PROBS
//#define PRINT_COND_PROBS
#define PRINT_Y_INF_CONT
#define PRINT_X_INF_CONT
#define PRINT_BEST_IG

#endif


/* YCD: COLUMNS: Y ROWS: X */
#define _YCD_INDEX(__stats,__xval,__yval) ((__xval) * (__stats)->da_data->category_count) + (__yval)

#define CATEGORY(_stats,_cat)  (_stats)->da_data->categories[(_cat)]

#define CAT_COUNT(_stats) (_stats)->da_data->category_count

#define Y_VAL(_stats,_i) (_stats)->da_data->alt_data[(_i)].y

#define X_VAL(_stats,_xparam,_i) (_stats)->da_data->alt_data[(_i)].xparams[(_xparam)]

#define Y_PROB_VAL(_stats,_cat) (_stats)->y_prob_vals[(_cat)]

#define X_PROB_VAL(_stats,_xparam,_cat) (_stats)->xp_stats[(_xparam)].x_prob_vals[(_cat)]

#define Y_INF_CONT(_stats) (_stats)->y_inf_content

#define X_INF_CONT(_stats,_xparam) (_stats)->xp_stats[(_xparam)].x_inf_content

#define YCD_VAL(_stats,_xparam,_xval,_yval) (_stats)->xp_stats[(_xparam)].y_cond_prob_vals[_YCD_INDEX((_stats),(_xval),(_yval))]

static double y_prob(struct ig_stats *stats,
		int y_start, int length,
		int y_val)
{
	double out = 0;

	for (int i = y_start; i < y_start + length; i++) {
		if (Y_VAL(stats,i) == y_val)
			out++;
	}

	return out / length;
}

static double x_prob(struct ig_stats *stats,
		int y_start, int length,
		int xparam, int x_val)
{
	double out = 0;

	for (int i = y_start; i < y_start + length; i++) {

		if (X_VAL(stats,xparam,i) == x_val)
			out++;
	}

	return out / length;
}

static double cond_prob(struct ig_stats *stats,
			int y_start,int length,
			int y_val,
			int xparam,int x_val)
{
	int ycount = 0;
	int xcount = 0;
	double ret = 0;

	for (int i = y_start; i < y_start + length; i++) {

		if (X_VAL(stats,xparam,i) == x_val) {
			xcount++;

			if (Y_VAL(stats,i) == y_val)
				ycount++;
		}
	}

	if (xcount)
		ret = (double)ycount / (double)xcount;

	return ret;
}


static void calc_inf_content(struct ig_stats *stats)
{
	double res = 0;
	double temp = 0;

	for (int i = 0; i < CAT_COUNT(stats); i++) {

		temp = Y_PROB_VAL(stats,i);

		if (temp != 0)
			res += temp * log2(temp);
	}

	stats->y_inf_content = -res;
}

static void calc_xparam_inf_content(struct ig_stats *stats, int xparam)
{
	double res = 0;
	double temp = 0;
	double temp2 = 0;

	for (int xcat = 0; xcat < CAT_COUNT(stats); xcat++) {

		temp = 0;

		for (int ycat = 0; ycat < CAT_COUNT(stats); ycat++) {

			temp2 = YCD_VAL(stats,xparam,xcat,ycat);

			if (temp2 != 0)
				temp += temp2 * log2(temp2);
		}

		res += -temp * X_PROB_VAL(stats,xparam,xcat);
	}

	X_INF_CONT(stats,xparam) = res;
}

static void calc_y_probs(struct ig_stats *stats,
			int y_start, int length)
{

	for (int i = 0; i < CAT_COUNT(stats); i++) {

		Y_PROB_VAL(stats,i) = y_prob(stats,
					y_start,length,
					CATEGORY(stats,i));

#ifdef PRINT_Y_PROBS
		printf("prob that y is %d:%f\n",
			CATEGORY(stats,i),
			Y_PROB_VAL(stats,i));
#endif
	}
}

static void calc_cond_probs(struct ig_stats *stats,
			int y_start, int length,
			int xparam, int xcat)
{
	for (int ycat = 0; ycat < CAT_COUNT(stats); ycat++) {

		YCD_VAL(stats,xparam,xcat,ycat) =
			cond_prob(stats,
				y_start,length,
				CATEGORY(stats,ycat),
				xparam,CATEGORY(stats,xcat));

#ifdef PRINT_COND_PROBS
		printf("prob( y = %d | x%d = %d ) = %f\n",
			CATEGORY(stats,ycat),
			xparam,
			CATEGORY(stats,xcat),
			YCD_VAL(stats, xparam, xcat, ycat));
#endif
	}
}

static void calc_x_stats(struct ig_stats *stats,
			int y_start, int length)
{
	for (int cur_xparam = 0; cur_xparam < stats->da_data->xparam_count; cur_xparam++) {

		for (int cur_xcat = 0; cur_xcat < CAT_COUNT(stats); cur_xcat++) {

			X_PROB_VAL(stats,cur_xparam,cur_xcat) =
				x_prob(stats,
					y_start,length,
					cur_xparam,CATEGORY(stats,cur_xcat));

#ifdef PRINT_X_PROBS
			printf("prob( x%d = %d) = %f\n",
				cur_xparam,
				CATEGORY(stats,cur_xcat),
				X_PROB_VAL(stats,cur_xparam,cur_xcat));
#endif

			calc_cond_probs(stats,
					y_start,length,
					cur_xparam,cur_xcat);
		}
	}
}

double get_y_inf_cont(struct ig_stats *stats, int y_start, int length)
{
	stats->y_inf_calcd = true;
	calc_y_probs(stats,y_start,length);
	calc_x_stats(stats,y_start,length);
	calc_inf_content(stats);
#ifdef PRINT_Y_INF_CONT
	printf("information content of y:%f\n",Y_INF_CONT(stats));
#endif

	return Y_INF_CONT(stats);
}

static int was_run(struct ig_stats *stats,int y_start, int length)
{
	if (!stats->y_inf_calcd)
		return false;
	if (stats->last_start != y_start)
		return false;
	if (stats->last_length != length)
		return false;
	return true;
}

int get_best_ig(struct ig_stats *stats,int y_start, int length)
{
	int best_param = 0;
	double best_ig = 0;
	double cur_ig = 0;

	if (!was_run(stats,y_start,length) ) {
		calc_y_probs(stats,y_start,length);
		calc_x_stats(stats,y_start,length);
		calc_inf_content(stats);

#ifdef PRINT_Y_INF_CONT
	printf("information content of y:%f\n",Y_INF_CONT(stats));
#endif
	}
	stats->y_inf_calcd = false;

	for (int i = 0; i < stats->da_data->xparam_count; i++) {

		calc_xparam_inf_content(stats,i);

#ifdef PRINT_X_INF_CONT
		printf("information content of y given x%d:%f\n",
			i,
			X_INF_CONT(stats,i));
#endif

		cur_ig = Y_INF_CONT(stats) - X_INF_CONT(stats,i);

		if (cur_ig > best_ig) {

			best_param = i;
			best_ig = cur_ig;
		}
	}
	if (best_ig <= 0)
		best_param = -1;
	stats->best_ig = best_ig;
#ifdef PRINT_BEST_IG
	printf("best ig:%f\n",best_ig);
#endif
	return best_param;
}

void init_ig_stats(struct ig_stats *stats,struct alt_data_file *adf)
{
	stats->y_inf_calcd = false;

	stats->last_start = -1;
	stats->last_length = -1;

	stats->da_data = adf;

	stats->y_prob_vals = malloc(sizeof(*stats->y_prob_vals) *
				CAT_COUNT(stats));

	stats->xp_stats = malloc(sizeof(*stats->xp_stats) *
				stats->da_data->xparam_count);

	for (int i = 0; i < stats->da_data->xparam_count; i++) {

		stats->xp_stats[i].x_prob_vals =
			malloc(sizeof(*stats->xp_stats[i].x_prob_vals) *
				CAT_COUNT(stats));

		stats->xp_stats[i].y_cond_prob_vals =
			malloc(sizeof(*stats->xp_stats[i].y_cond_prob_vals) *
				CAT_COUNT(stats) * CAT_COUNT(stats));
	}
}

void free_ig_stats(struct ig_stats *stats)
{
	free(stats->y_prob_vals);

	for (int i = 0; i < stats->da_data->xparam_count; i++) {

		free(stats->xp_stats[i].x_prob_vals);
		free(stats->xp_stats[i].y_cond_prob_vals);

	}

	free(stats->xp_stats);
}
