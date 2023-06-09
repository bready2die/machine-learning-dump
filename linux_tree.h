#ifndef _LINUX_TREE_H
#define _LINUX_TREE_H
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
/**
 * @name from other kernel headers
 */
/*@{*/

/**
 * Get offset of a member
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * Casts a member of a structure out to the containing structure
 * @param ptr        the pointer to the member.
 * @param type       the type of the container struct this is embedded in.
 * @param member     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
/*@}*/


struct tree_head {
	struct tree_head *rchild, *lchild, *parent;
	int child_count;
};

struct print_params {
	char *pointer;
	char *segment;
	char *prefix;
};

typedef int (*tree_cmpfn_t)(struct tree_head *a,struct tree_head *b);
typedef void (*tree_serial_t)(struct tree_head *head,FILE *file);
typedef void (*tree_deserial_t)(int val,struct tree_head *head,FILE *file);
typedef struct tree_head *(*tree_alloc_t)(void);
typedef void (*tree_free_t)(struct tree_head *head);
typedef void (*tree_frmleaf_t)(struct tree_head *head);
typedef void (*tree_toleaf_t)(struct tree_head *head);
typedef void (*tree_print_t)(struct tree_head *head, int depth,struct print_params *p_params);

struct tree_params {
	tree_cmpfn_t 		cmpfn;
	tree_serial_t		serfn;//optional (coreq with desfn and ser_marker)
	tree_deserial_t		desfn;//optional (coreq with serfn and ser_marker)
	tree_alloc_t		allocfn;
	tree_free_t		freefn;
	tree_frmleaf_t		frmleaf_fn;//optional
	tree_toleaf_t		toleaf_fn;//optional
	tree_print_t		printfn;//optional (required if calling print_tree)
	int			ser_marker;//optional (coreq with serfn and desfn)
}__attribute__((packed));


#define TREE_HEAD_INIT(name) { &(name), &(name) }

#define TREE_HEAD(name) \
	struct tree_head name = TREE_HEAD_INIT(name)

#define INIT_TREE_HEAD(ptr) do { 						\
	(ptr)->rchild = NULL; (ptr)->lchild = NULL; (ptr)->parent = NULL; 	\
	(ptr)->child_count = 0;							\
} while (0)

typedef enum {
	LCHILD = 0,
	RCHILD = 1,
} child_t;



int __tree_cmp(struct tree_head *a,
	       struct tree_head *b,
	       struct tree_params *params);

void tree_add_left(struct tree_head *new,
		   struct tree_head *parent,
		   struct tree_params *params);

void tree_add_right(struct tree_head *new,
		    struct tree_head *parent,
		    struct tree_params *params);

int is_child(struct tree_head *head);

int is_lchild(struct tree_head *head);

int is_rchild(struct tree_head *head);


struct tree_head *tree_del(struct tree_head *head, struct tree_params *params);


#define tree_entry(ptr, type, member) \
	container_of(ptr, type, member)

void serialize_tree(struct tree_head *head, FILE *file, struct tree_params *params);

struct tree_head *deserialize_tree(FILE *file, struct tree_params *params);

#define MAKE_PRINT_PARAMS(_name,_pointer,_segment,_prefix)	\
struct print_params (_name) = {					\
	.pointer = (_pointer),					\
	.segment = (_segment),					\
	.prefix = (_prefix),					\
}

#define PRINT_PARAMS(_pointer,_segment,_prefix)					\
({										\
	MAKE_PRINT_PARAMS(___temp_name,(_pointer),(_segment),(_prefix));	\
	___temp_name;								\
})



void print_tree(struct tree_head *head, struct tree_params *params);

void free_entire_tree(struct tree_head *head, struct tree_params *params);

void for_each_node(struct tree_head *head, void *ret, void (*fun)(struct tree_head*,void *arg));
#endif
