#include <math.h>
#include "alt_data_vec.h"
#include "class_verify_lib.h"
#include "globals.h"

double calc_err_r(struct classif_obj *cla_obj, struct alt_data_file *adf, int i)
{
	double out;
	//printf("i:%d\n",i);
	if (cla_obj->cur_node->is_leaf) {
		//printf("oyoy\n");
		out = cla_obj->cur_node->out_y;
		//go_up(reg_obj);
	} else if (adf->alt_data[i].xparams[cla_obj->cur_node->xparam] == DATA_FALSE) {
		//printf("oyoy2\n");
		go_left(cla_obj);
		out = calc_err_r(cla_obj,adf,i);
	} else {
		//printf("oyoy3\n");
		go_right(cla_obj);
		out = calc_err_r(cla_obj,adf,i);
	}
	if (cla_obj->cur_node->tree.parent)
		go_up(cla_obj);
	return out;
}

double calc_err_logistic(struct classif_obj *cla_obj, struct alt_data_file *adf)
{
	double cum_err = 0;
	double exp_y;
	double tru_y;
	for (int i = 0; i < adf->vecs; i++) {
	        exp_y = calc_err_r(cla_obj,adf,i);
		tru_y = adf->alt_data[i].y;
	        //printf("y:%f\n",dfile->data->y[i]);
	        //printf("exp_y:%f\n",exp_y);
		//printf("%d: err = %f\n",i,fabs(dfile->data->y[i] - exp_y));
		//printf("%d: cum_err_sq:%f\n",i,cum_err_sq);
		if (exp_y == 0)
			cum_err += - ((1 - tru_y) * log(1 - exp_y));
		else if (exp_y == 1)
			cum_err += -(tru_y * log(exp_y));
		else
			cum_err += -(tru_y * log(exp_y)) - ((1 - tru_y) * log(1 - exp_y));
	}
	//printf("cum_err_sq:%f\n",cum_err_sq);
	return cum_err / adf->vecs;
}

double calc_err(struct classif_obj *cla_obj, struct alt_data_file *adf)
{
	double cum_err = 0;
	//double cur_err;
	double y_pred;
	for (int i = 0; i < adf->vecs; i++) {
		y_pred = calc_err_r(cla_obj,adf,i);
		//printf("%d: err = %f\n",i,dfile->data->y[i] - reg_obj->points[j].out_y);
		//printf("%d: cum_err_sq:%f\n",i,cum_err_sq);
		//cur_err = fabs(adf->alt_data[i].y - y_pred);
		if (adf->alt_data[i].y != y_pred)
			cum_err++;
		//if(cur_err != 0)
			//printf("error on vec%02d\n",i);
		//cum_err += cur_err;
	}
	//printf("cum_err_sq:%f\n",cum_err_sq);
	return cum_err;
}
