#include <stdlib.h>
#include <string.h>
#include "linux_tree.h"


int __tree_cmp(struct tree_head *a,
	       struct tree_head *b,
	       struct tree_params *params)
{
	return params->cmpfn(a,b);
}


static void __tree_add(struct tree_head *new,
		       struct tree_head *parent,
		       child_t child,
		       struct tree_params *params)
{
	if (parent == NULL) {
		//printf("parent is null, doing nothing...\n");
		return;
	}
	parent->child_count++;
	if (parent->child_count == 1) {
		if (params->frmleaf_fn)
			params->frmleaf_fn(parent);
	}
	if (child == LCHILD) {
		//printf("setting left child of parent\n");
		parent->lchild = new;
	} else {
		//printf("setting right child of parent\n");
		parent->rchild = new;
	}
	new->parent = parent;
}

void tree_add_left(struct tree_head *new,
		   struct tree_head *parent,
		   struct tree_params *params)
{
	__tree_add(new,parent,LCHILD,params);
}

void tree_add_right(struct tree_head *new,
		    struct tree_head *parent,
		    struct tree_params *params)
{
	__tree_add(new,parent,RCHILD,params);
}

int is_child(struct tree_head *head)
{
	return head->child_count == 0;
}

int is_lchild(struct tree_head *head)
{
	if (!head->parent)
		return 0;
	if (head->parent->lchild == head)
		return 1;
	else
		return 0;
}

int is_rchild(struct tree_head *head)
{
	if (!head->parent)
		return 0;
	if (head->parent->rchild == head)
		return 1;
	else
		return 0;
}


struct tree_head *tree_del(struct tree_head *head, struct tree_params *params)
{
	struct tree_head *new_head = NULL;
	if (head == NULL)
		return NULL;
	if (is_child(head)) {
		goto out;
	}
	if (head->lchild == NULL) {
		new_head = head->rchild;
		goto out;
	}
	if (head->rchild == NULL) {
		new_head = head->lchild;
		goto out;
	}
	//this code should never be run by regression_file2
	//for now it'll just crash the program, cause i don't know what to do with it
	if (params->cmpfn(head->lchild,head->rchild) > 0) {
		printf("never should have come here\n");
		assert(0);
	} else {
		printf("never should have come here\n");
		assert(0);
	}
out:
	if (head->parent != NULL) {
		head->parent->child_count--;
		if (head->parent->child_count == 0) {
			if (params->toleaf_fn)
				params->toleaf_fn(head->parent);
		}
		if (head->parent->lchild == head)
			tree_add_left(new_head,head->parent,params);
		else if (head->parent->rchild == head)
			tree_add_right(new_head,head->parent,params);
	}
	return new_head;
}

void serialize_tree(struct tree_head *head, FILE *file, struct tree_params *params)
{
	if (head == NULL) {
		fprintf(file,"%d ",params->ser_marker);
		return;
	}
	params->serfn(head,file);
	serialize_tree(head->lchild,file,params);
	serialize_tree(head->rchild,file,params);
}

static struct tree_head *deserialize_tree_r(FILE *file, struct tree_params *params)
{
	int val;
	struct tree_head *head;
	struct tree_head *tmp;
	if (!fscanf(file,"%d ",&val) || val == params->ser_marker) {
		//printf("reached null\n");
		//printf("serial marker:%d\n",params.ser_marker);
		return NULL;
	}
	//printf("did not reach null, val:%d\n",val);
	head = params->allocfn();
	params->desfn(val,head,file);
	tmp = deserialize_tree_r(file,params);
	if (tmp)
		tree_add_left(tmp,head,params);
	tmp = deserialize_tree_r(file,params);
	if (tmp)
		tree_add_right(tmp,head,params);
	return head;
}

struct tree_head *deserialize_tree(FILE *file, struct tree_params *params)
{
	return deserialize_tree_r(file,params);
}



static void print_tree_r(int depth, struct tree_head *head,struct tree_params *params,
									char* prefix)
{
	struct print_params p_params;
	char *next_prefix;
	if (head == NULL)
		return;
	if (is_rchild(head))
		p_params = PRINT_PARAMS("└── ","    ",prefix);
	else
		p_params = PRINT_PARAMS("├── ","│   ",prefix);
	if (head->parent != NULL) {
		next_prefix = malloc(strlen(prefix) + strlen(p_params.segment) + 1);
		sprintf(next_prefix, "%s%s", prefix, p_params.segment);
	}
	else
		next_prefix = "";
	params->printfn(head,depth,&p_params);
	print_tree_r(depth + 1,head->lchild,params,next_prefix);
	print_tree_r(depth + 1,head->rchild,params,next_prefix);
	if (head->parent != NULL)
		free(next_prefix);
}

void print_tree(struct tree_head *head, struct tree_params *params)
{
	print_tree_r(0,head,params,"");
}

static void free_entire_tree_r(struct tree_head *head, struct tree_params *params)
{
	if (head == NULL)
		return;
	free_entire_tree_r(head->lchild,params);
	free_entire_tree_r(head->rchild,params);
	params->freefn(head);
}

void free_entire_tree(struct tree_head *head, struct tree_params *params)
{
	free_entire_tree_r(head,params);
}

void for_each_node(struct tree_head *head, void *ret, void (*fun)(struct tree_head*,void *arg))
{
	if (head == NULL)
		return;
	fun(head,ret);
	for_each_node(head->lchild,ret,fun);
	for_each_node(head->rchild,ret,fun);
}
