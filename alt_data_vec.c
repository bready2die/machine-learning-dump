#define _GNU_SOURCE
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>
#include "alt_data_vec.h"


void new_init_alt_data_file(struct alt_data_file *altfile, int xp_count, int vecs,int cat_count)
{
	altfile->xparam_count = xp_count;
	altfile->vecs = vecs;
	altfile->category_count = cat_count;
	altfile->categories = malloc(cat_count * sizeof(*altfile->categories));
	altfile->alt_data = malloc(altfile->vecs * sizeof(*altfile->alt_data));
}

void init_alt_data_file(struct alt_data_file *altfile, struct data_file *dfile)
{
	altfile->vecs = dfile->vecs;
	altfile->xparam_count = MIN(dfile->xparam_count,MAX_XPARAM);
	altfile->alt_data = malloc(altfile->vecs * sizeof(*altfile->alt_data));
	for (int i = 0; i < dfile->vecs; i++) {
		//printf("i:%d\n",i);
		for (int j = 0; j < MIN(dfile->xparam_count,MAX_XPARAM); j++) {
			//printf("j:%d\n",j);
			altfile->alt_data[i].xparams[j] = dfile->data->xparams[j][i];
		}
		altfile->alt_data[i].y = dfile->data->y[i];
	}
	altfile->category_count = dfile->category_count;
	altfile->categories = malloc(altfile->category_count * sizeof(*altfile->categories));
	memcpy(altfile->categories,dfile->categories,altfile->category_count * sizeof(*altfile->categories));
}

int cmp_alt_data_vec(const void *arg1, const void *arg2, void *arg3)
{
	struct alt_data_vec *adv1 = (struct alt_data_vec*)arg1;
	struct alt_data_vec *adv2 = (struct alt_data_vec*)arg2;
	int param = *(int*)arg3;
	if (adv1->xparams[param] < adv2->xparams[param])
		return -1;
	if (adv1->xparams[param] > adv2->xparams[param])
		return 1;
	return 0;
	//return (int) (adv1->xparams[param] - adv2->xparams[param]);
}

void sort_alt_data_vec(struct alt_data_file *altfile, int xparam)
{
	qsort_r((void*)altfile->alt_data,altfile->vecs,sizeof(struct alt_data_vec),cmp_alt_data_vec,(void*)&xparam);
}

void sort_alt_data_vec_split(struct alt_data_file *altfile, int xparam, int off, int n)
{
	qsort_r((void*)&altfile->alt_data[off],n,sizeof(struct alt_data_vec),cmp_alt_data_vec,(void*)&xparam);
}

void empty_alt_data_file(struct alt_data_file *altfile)
{
	free(altfile->categories);
	free(altfile->alt_data);
}


void alt_data_to_data_vec(struct alt_data_file *altfile, struct data_file *dfile)
{
	for (int i = 0; i < dfile->vecs; i++) {
		for (int j = 0; j < MIN(dfile->xparam_count,MAX_XPARAM); j++) {
			dfile->data->xparams[j][i] = altfile->alt_data[i].xparams[j];
		}
		dfile->data->y[i] = altfile->alt_data[i].y;
	}
}


void alt_data_print_xarr_split(struct alt_data_file *altfile, int xparam,int off, int len)
{
	printf("contents of x%d:\n",xparam);
	for (int i = 0; i < len; i++) {
		printf("\telement %i: x%d = %d,\ty = %d\n",i,xparam,altfile->alt_data[i+off].xparams[xparam],altfile->alt_data[i+off].y);
	}
}
void alt_print_table_split(struct alt_data_file *altfile, int off, int len)
{
	printf("contents of file:\n");
        for (int i = 0; i < len; i++) {
                printf("vec %2i:",i+off);
                for (int j = 0; j < altfile->xparam_count; j++) {
                        printf(" x%d=%2d,",j,altfile->alt_data[i+off].xparams[j]);
                }
                printf(" y=%2d\n",altfile->alt_data[i+off].y);
        }
}

void alt_print_table(struct alt_data_file *altfile)
{
        printf("contents of file:\n");
        for (int i = 0; i < altfile->vecs; i++) {
                printf("vec %2i:",i);
                for (int j = 0; j < altfile->xparam_count; j++) {
                        printf(" x%d=%2d,",j,altfile->alt_data[i].xparams[j]);
                }
                printf(" y=%2d\n",altfile->alt_data[i].y);
        }
}
