#ifndef CLASSIFICATION_FILE_LIB
#define CLASSIFICATION_FILE_LIB 1
#include "linux_tree.h"

#define LEFT_CHILD  0

#define RIGHT_CHILD 1

#define INIT_CNODE_P(_name,_xparam)		\
do {						            \
	(_name)->is_leaf = 0;		    	\
	(_name)->xparam = (_xparam);		\
	INIT_TREE_HEAD(&(_name)->tree);		\
} while(0)


#define ALLOC_CNODE_P(_name)			\
	(_name) = malloc(sizeof(*(_name)))

#define MAKE_CNODE_P(_xparam)			    \
({						                    \
	struct classif_node *___tmp_name;	    \
	ALLOC_CNODE_P(___tmp_name);		        \
	INIT_CNODE_P(___tmp_name,(_xparam));	\
	(___tmp_name);				            \
})


#define INIT_CNODE_LEAF_P(_name,_out_y)		\
do {						                \
	(_name)->is_leaf = 1;		        	\
	(_name)->out_y = (_out_y);		        \
	INIT_TREE_HEAD(&(_name)->tree);		    \
} while(0)


#define ALLOC_CNODE_LEAF_P(_name)	ALLOC_CNODE_P((_name))

#define MAKE_CNODE_LEAF_P(_out_y)			    \
({							                    \
	struct classif_node *___tmp_name;		    \
	ALLOC_CNODE_LEAF_P(___tmp_name);	    	\
	INIT_CNODE_LEAF_P(___tmp_name,(_out_y));	\
	(___tmp_name);					            \
})



#define MAKE_CNODE(_name,_xparam)	\
({					                \
	struct classif_node (_name) {	\
		.is_leaf = 0,		        \
		.xparam = (_xparam),    	\
	};				                \
	INIT_TREE_HEAD(&(_name).tree);	\
	(_name);		            	\
})



#define MAKE_LEAF_CNODE(_name,_out_y)	\
({					                    \
	struct classif_node (_name) {	    \
		.is_leaf = 1	            	\
		.out_y = (_out_y),	            \
	};				                    \
	INIT_TREE_HEAD(&(_name).tree);  	\
	(_name);			                \
})


struct classif_node {
	struct tree_head tree;
	int is_leaf;
	union {
		struct {
			int xparam;
		};
		struct {
			double out_y;
		};
	};
};


#define _CLA_FILE_FIELDS			    \
struct {					            \
	struct classif_node *classif_tree;	\
	struct tree_params params;		    \
	}__attribute__((packed))

struct classif_obj {
	_CLA_FILE_FIELDS;
	struct classif_node *cur_node;
}__attribute__((packed));

void go_up(struct classif_obj *cla_obj);

void go_right(struct classif_obj *cla_obj);

void go_left(struct classif_obj *cla_obj);

int read_classif_entry(struct classif_obj *cla_obj, double *max_x, double *out_y);

int add_classif_entry(struct classif_obj *cla_obj,int xparam, int child_no);

int add_classif_entry_leaf(struct classif_obj *cla_obj,double y_out, int child_no);

void init_classif_obj(struct classif_obj *cla_obj);

int load_classif_obj(FILE *file,struct classif_obj *cla_obj);

void write_classif_obj(FILE *file, struct classif_obj *cla_obj);

void free_classif_obj(struct classif_obj *cla_obj);

void print_classif_obj(struct classif_obj *cla_obj);

void print_superfluous(struct classif_obj *cla_obj);

int get_superfluous(struct classif_obj *cla_obj);

void reset_pos(struct classif_obj *cla_obj);
#endif
