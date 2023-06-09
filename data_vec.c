#include <stdlib.h>
#include <stdio.h>
#include "alt_data_vec.h"
#include "data_vec.h"



void init_data_file(struct data_file *dfile, int xp_count, int vecs, int cat_count)
{
	dfile->xparam_count = xp_count;
	dfile->vecs = vecs;
	dfile->category_count = cat_count;
	dfile->categories = malloc(cat_count * sizeof(*dfile->categories));
	dfile->data = alloc_data_vec(dfile);
}

struct data_vec *alloc_data_vec(struct data_file *dfile)
{
	struct data_vec *out = malloc(sizeof(*out));
	out->y = malloc(dfile->vecs * sizeof(*out->y));
	out->xparams = malloc(dfile->xparam_count * sizeof(*out->xparams));
	for (int i = 0; i < dfile->xparam_count; i++) {
		out->xparams[i] = malloc(dfile->vecs * sizeof(**out->xparams));
	}
	return out;
}

void load_data_vec(FILE *file, struct data_file *dfile)
{
	for (int i = 0; i < dfile->xparam_count; i++) {
	        fread(dfile->data->xparams[i],sizeof(*dfile->data->xparams[i]),dfile->vecs,file);
	}
	fread(dfile->data->y,sizeof(*dfile->data->y),dfile->vecs,file);
}

int load_data_file(FILE *file, struct data_file *dfile)
{
	fread(&dfile->xparam_count,sizeof(dfile->xparam_count),1,file);
	fread(&dfile->vecs,sizeof(dfile->vecs),1,file);
	fread(&dfile->category_count,sizeof(dfile->category_count),1,file);
	dfile->categories = malloc(dfile->category_count * sizeof(*dfile->categories));
	fread(dfile->categories,sizeof(*dfile->categories),dfile->category_count,file);
	dfile->data = alloc_data_vec(dfile);
	load_data_vec(file,dfile);
	return 0;
}

void free_data_vec(struct data_file *dfile)
{
	free(dfile->categories);
	for (int i = 0; i < dfile->xparam_count; i++) {
		free(dfile->data->xparams[i]);
	}
	free(dfile->data->xparams);
	free(dfile->data->y);
	free(dfile->data);
}


void write_data_file(FILE *file, struct data_file *dfile)
{
	fwrite(&dfile->xparam_count,sizeof(dfile->xparam_count),1,file);
	fwrite(&dfile->vecs,sizeof(dfile->vecs),1,file);
	fwrite(&dfile->category_count,sizeof(dfile->category_count),1,file);
	fwrite(dfile->categories,sizeof(*dfile->categories),dfile->category_count,file);
	for (int i = 0; i < dfile->xparam_count; i++) {
	        fwrite(dfile->data->xparams[i],sizeof(*dfile->data->xparams[i]),dfile->vecs,file);
	}
	fwrite(dfile->data->y,sizeof(*dfile->data->y),dfile->vecs,file);
}

void sort_data_vec(struct data_file *dfile, int xparam)
{
	struct alt_data_file altfile;
	init_alt_data_file(&altfile,dfile);
	sort_alt_data_vec(&altfile,xparam);
	alt_data_to_data_vec(&altfile,dfile);
	empty_alt_data_file(&altfile);
}

void sort_data_vec_split(struct data_file *dfile, int xparam,int off, int n)
{
	struct alt_data_file altfile;
	init_alt_data_file(&altfile,dfile);
	sort_alt_data_vec_split(&altfile,xparam,off,n);
	alt_data_to_data_vec(&altfile,dfile);
	empty_alt_data_file(&altfile);
}

void print_xarr(struct data_file *dfile, int xparam)
{
	printf("contents of x%d:\n",xparam);
	for (int i = 0; i < dfile->vecs; i++) {
		printf("\telement %i: x%d = %d,\ty = %d\n",i,xparam,dfile->data->xparams[xparam][i],dfile->data->y[i]);
	}
}

void print_table(struct data_file *dfile)
{
	printf("contents of file:\n");
	for (int i = 0; i < dfile->vecs; i++) {
		printf("vec %2i:",i);
		for (int j = 0; j < dfile->xparam_count; j++) {
			printf(" x%d=%2d,",j,dfile->data->xparams[j][i]);
		}
		printf(" y=%2d\n",dfile->data->y[i]);
	}
}

void print_point(struct data_file *dfile, int vec_no)
{
}

void print_enums(struct data_file *dfile)
{
	printf("valid parameter values:\n");
	for (int i = 0; i < dfile->category_count; i++) {
		printf("value %d is %d\n",i,dfile->categories[i]);
	}
}
