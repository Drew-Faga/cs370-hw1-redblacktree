#ifndef RBTREE_H
#define RBTREE_H

typedef enum { RED, BLACK } rb_color;

typedef struct rb_node {
    int data;
    rb_color color;
    struct rb_node *left, *right, *parent;
} rb_node;

typedef struct rb_tree {
    rb_node *root, *nil;
} rb_tree;

rb_tree *rb_create_tree(void);      /* makes a new empty tree */
void rb_free_tree(rb_tree *tree);   /* frees the tree */
void rb_insert(rb_tree *tree, int data);  /* adds a value */
void rb_delete(rb_tree *tree, int data);  /* removes a value */
rb_node *rb_search(rb_tree *tree, int data); /* finds a value */

void rb_print_inorder(rb_tree *tree); /* prints values in order */
bool rb_is_valid(rb_tree *tree);      /* checks the tree rules */

#endif
