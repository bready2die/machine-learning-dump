#include <stdlib.h>
#include "classif_file.h"

struct classif_file {
	_CLA_FILE_FIELDS;
};

static int cla_tree_cmp(struct tree_head *a,struct tree_head *b)
{
	return 0;
}

static void cla_tree_serialize(struct tree_head *head,FILE *file)
{
	struct classif_node *node = tree_entry(head,struct classif_node, tree);
	fprintf(file,"%d ",node->is_leaf);
	if (node->is_leaf) {
		//printf("node is a leaf\n");
		fprintf(file,"%f ",node->out_y);
	} else {
		//printf("node is not a leaf\n");
		fprintf(file,"%d ", node->xparam);
	}
}
static void cla_tree_deserialize(int val, struct tree_head *head,FILE *file)
{
	//printf("oyoyoy\n");
	struct classif_node *node = tree_entry(head,struct classif_node, tree);
	node->is_leaf = val;
	if (node->is_leaf == 1) {
		//printf("node is a leaf\n");
		fscanf(file,"%lf ",&node->out_y);
	} else {
		//printf("node is not a leaf\n");
		fscanf(file,"%d ",&node->xparam);
	}
}

static struct tree_head *cla_tree_alloc(void)
{
	struct classif_node *newnode = MAKE_CNODE_LEAF_P(0.0);
	return &newnode->tree;
}

static void cla_tree_free(struct tree_head *head)
{
	struct classif_node *node = tree_entry(head,struct classif_node, tree);
	free(node);
}
/*
static inline void indent(int depth)
{
	for (int i = 0; i < depth; i++)
		printf("\t");
}
*/
static void cla_tree_print(struct tree_head *head, int depth,struct print_params *p_params)
{
	struct classif_node *node = tree_entry(head,struct classif_node, tree);

	//indent(depth);
	printf("%s",p_params->prefix);
	if (head->parent)
		printf("%s",p_params->pointer);
	printf("type: ");
	if (!node->is_leaf)
		printf("branch\n");
	else
		printf("leaf\n");
	if (node->is_leaf) {
		//indent(depth);
		printf("%s%s",p_params->prefix,p_params->segment);
		printf("out_y:%lf\n",node->out_y);
	} else {
		//indent(depth);
		printf("%s%s",p_params->prefix,p_params->segment);
		printf("x_param:%d\n",node->xparam);
	}
}


struct tree_params cla_tree_params = {
	.cmpfn = cla_tree_cmp,
	.serfn = cla_tree_serialize,
	.desfn = cla_tree_deserialize,
	.allocfn = cla_tree_alloc,
	.freefn = cla_tree_free,
	.frmleaf_fn = NULL,
	.toleaf_fn = NULL,
	.printfn = cla_tree_print,
	.ser_marker = -1,
};

void go_up(struct classif_obj *cla_obj)
{
	cla_obj->cur_node = tree_entry(cla_obj->cur_node->tree.parent, struct classif_node, tree);
}
void go_right(struct classif_obj *cla_obj)
{
	cla_obj->cur_node = tree_entry(cla_obj->cur_node->tree.rchild, struct classif_node, tree);
}

void go_left(struct classif_obj *cla_obj)
{
	cla_obj->cur_node = tree_entry(cla_obj->cur_node->tree.lchild, struct classif_node, tree);
}



int add_classif_entry(struct classif_obj *cla_obj,int xparam, int child_no)
{
	struct classif_node *new_leaf = MAKE_CNODE_P(xparam);
	if (child_no == LEFT_CHILD) {
		//printf("adding left node\n");
		tree_add_left(&new_leaf->tree,
			      &cla_obj->cur_node->tree,
			      &cla_obj->params);
		//go_left(reg_obj);
	} else if (child_no == RIGHT_CHILD) {
		//printf("adding right leaf node\n");
		tree_add_right(&new_leaf->tree,
			       &cla_obj->cur_node->tree,
			       &cla_obj->params);
		//go_right(reg_obj);
	}
	if (cla_obj->classif_tree == NULL)
		cla_obj->classif_tree = new_leaf;
	if (cla_obj->cur_node == NULL)
		cla_obj->cur_node = new_leaf;
	else {
		if (child_no == LEFT_CHILD) {
			//printf("moving left\n");
			go_left(cla_obj);
		}
		if (child_no == RIGHT_CHILD) {
			//printf("moving right\n");
			go_right(cla_obj);
		}
	}
	return 0;
}

int add_classif_entry_leaf(struct classif_obj *cla_obj,double y_out, int child_no)
{
	struct classif_node *new_leaf = MAKE_CNODE_LEAF_P(y_out);
	if (child_no == LEFT_CHILD) {
		//printf("adding left leaf node\n");
		tree_add_left(&new_leaf->tree,
			      &cla_obj->cur_node->tree,
			      &cla_obj->params);
		//go_left(reg_obj);
	} else if (child_no == RIGHT_CHILD) {
		//printf("adding right leaf node\n");
		tree_add_right(&new_leaf->tree,
			       &cla_obj->cur_node->tree,
			       &cla_obj->params);
		//go_right(reg_obj);
	}
	if (cla_obj->classif_tree == NULL)
		cla_obj->classif_tree = new_leaf;
	if (cla_obj->cur_node == NULL)
		cla_obj->cur_node = new_leaf;
	else {
		if (child_no == LEFT_CHILD) {
			//printf("moving left\n");
			go_left(cla_obj);
		}
		if (child_no == RIGHT_CHILD) {
			//printf("moving right\n");
			go_right(cla_obj);
		}
	}
	return 0;
}

static void init_classif_file(struct classif_file *cla_file)
{
	cla_file->classif_tree = NULL;
	cla_file->params = cla_tree_params;
}

void init_classif_obj(struct classif_obj *cla_obj)
{
	init_classif_file((struct classif_file *)cla_obj);
	cla_obj->cur_node = NULL;
}

static int load_classif_file(FILE *file,struct classif_file *cla_file)
{
	struct tree_head *head = deserialize_tree(file,&cla_file->params);
	cla_file->classif_tree = tree_entry(head,struct classif_node, tree);
	return 0;
}

int load_classif_obj(FILE *file,struct classif_obj *cla_obj)
{
	//printf("loading regress obj\n");
	load_classif_file(file,(struct classif_file *)cla_obj);
	cla_obj->cur_node = cla_obj->classif_tree;
	//printf("wefoi\n");
	if (!cla_obj->cur_node)
		printf("failed to load classif obj\n");
	return 0;
}

static void write_classif_file(FILE *file, struct classif_file *cla_file)
{
	serialize_tree(&cla_file->classif_tree->tree,file,&cla_file->params);
}

void write_classif_obj(FILE *file, struct classif_obj *cla_obj)
{
	write_classif_file(file,(struct classif_file *)cla_obj);
}

static void free_classif_file(struct classif_file *cla_file)
{
	free_entire_tree(&cla_file->classif_tree->tree,&cla_file->params);
	cla_file->classif_tree = NULL;
}

void free_classif_obj(struct classif_obj *cla_obj)
{
	cla_obj->cur_node = NULL;
	free_classif_file((struct classif_file *)cla_obj);
}
static void print_classif_file(struct classif_file *cla_file)
{
	print_tree(&cla_file->classif_tree->tree,&cla_file->params);
}
void print_classif_obj(struct classif_obj *cla_obj)
{
	print_classif_file((struct classif_file *)cla_obj);
	reset_pos(cla_obj);
}

static void check_superfluous(struct tree_head* head,void *arg)
{
	int *total = arg;
	struct classif_node *node = tree_entry(head,struct classif_node, tree);
	if (!node->is_leaf && node->xparam > 4)
		(*total)++;
}

void print_superfluous(struct classif_obj *cla_obj)
{
	int total = 0;
	for_each_node(&cla_obj->classif_tree->tree,&total,check_superfluous);
	printf("number of superfluous features:%d\n",total);
}

int get_superfluous(struct classif_obj *cla_obj)
{
	int total = 0;
	for_each_node(&cla_obj->classif_tree->tree,&total,check_superfluous);
	return total;
}

void reset_pos(struct classif_obj *cla_obj)
{
	cla_obj->cur_node = cla_obj->classif_tree;
}
