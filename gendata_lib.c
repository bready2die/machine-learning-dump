#include <stdlib.h>
#include <math.h>
#include <sys/random.h>
#include <string.h>
#include "normal1.h"
#include "data_vec.h"
#include "alt_data_vec.h"
#include "globals.h"
#include "gendata_lib.h"

void calc_y_classif_dfile(int i, struct data_file *dfile, double sigma)
{
	int sum = ((0.9) * ((double)dfile->data->xparams[0][i]) +
                            (0.9) * (0.9) * ((double)dfile->data->xparams[1][i]) +
                            (0.9) * (0.9) * (0.9) * ((double)dfile->data->xparams[2][i]) +
                            (0.9) * (0.9) * (0.9) * (0.9) * ((double)dfile->data->xparams[3][i]) +
                            (0.9) * (0.9) * (0.9) * (0.9) * (0.9) * ((double)dfile->data->xparams[4][i]) +
                            r8_normal_ab(0,sigma));
        if (sum < 1)
                dfile->data->y[i] = DATA_FALSE;
        else
                dfile->data->y[i] = DATA_TRUE;
}


static void generate_datapoint_dfile(struct data_file *dfile, int i, int j)
{
        dfile->data->xparams[j][i] = (drand48() < 0.5 ? DATA_TRUE : DATA_FALSE);
}

static void alloc_dfile(struct data_file *dfile, int vec_count, int varpc)
{
        dfile->xparam_count = MIN_PARAM_COUNT + varpc ;
        dfile->vecs = vec_count;
        dfile->category_count = UNIQ_DATA_VALS;
        dfile->categories = malloc(dfile->category_count * sizeof(*dfile->categories));
        dfile->categories[0] = (int)DATA_FALSE;
        dfile->categories[1] = (int)DATA_TRUE;
        dfile->data = alloc_data_vec(dfile);
}

void gen_data_dfile(struct data_file *dfile,int vecs, int varpc, double sigma,
                void (calc_y)(int, struct data_file *, double))
{
	alloc_dfile(dfile,vecs,varpc);
	for (int i = 0; i < dfile->vecs; i++) {
                for ( int j = 0; j < dfile->xparam_count; j++) {
                        generate_datapoint_dfile(dfile,i,j);
                        ///printf("i:%d j:%d\n",i,j);
                }
		calc_y(i,dfile,sigma);
	}
}


void calc_y_classif_adf(int i, struct alt_data_file *adf, double sigma)
{
	int sum = ((0.9) * ((double)adf->alt_data[i].xparams[0]) +
                            (0.9) * (0.9) * ((double)adf->alt_data[i].xparams[1]) +
                            (0.9) * (0.9) * (0.9) * ((double)adf->alt_data[i].xparams[2]) +
                            (0.9) * (0.9) * (0.9) * (0.9) * ((double)adf->alt_data[i].xparams[3]) +
                            (0.9) * (0.9) * (0.9) * (0.9) * (0.9) * ((double)adf->alt_data[i].xparams[4]) +
                            r8_normal_ab(0,sigma));
        if (sum < 1)
                adf->alt_data[i].y = DATA_FALSE;
        else
                adf->alt_data[i].y = DATA_TRUE;
}



static void generate_datapoint_adf(struct alt_data_file *adf, int i, int j)
{
        adf->alt_data[i].xparams[j] = (drand48() < 0.5 ? DATA_TRUE : DATA_FALSE);
}

void gen_data_adf(struct alt_data_file *adf,int vecs, int varpc, double sigma,
                void (calc_y)(int, struct alt_data_file *, double))
{
	new_init_alt_data_file(adf,MIN_PARAM_COUNT + varpc,vecs,UNIQ_DATA_VALS);
	adf->categories[0] = (int)DATA_FALSE;
        adf->categories[1] = (int)DATA_TRUE;

	for (int i = 0; i < adf->vecs; i++) {
		for (int j = 0; j < adf->xparam_count; j++) {
			generate_datapoint_adf(adf,i,j);
		}
		calc_y(i,adf,sigma);
	}
}
