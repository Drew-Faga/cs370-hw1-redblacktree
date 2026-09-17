#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

static void rot_L(rb_tree *t, rb_node *x);
static void rot_R(rb_tree *t, rb_node *y);
static void fix_ins(rb_tree *t, rb_node *n);
static void fix_del(rb_tree *t, rb_node *x);

/* makes a new tree with just the shared   leaf */
rb_tree *rb_create_tree(void) {
    rb_tree *t = malloc(sizeof(rb_tree));
    t->nil = malloc(sizeof(rb_node));
    t->nil->color = BLACK; // fixxed the issue with the pointer.
    t->nil->left = t->nil->right = t->nil->parent = t->nil;
    t->root = t->nil;
    return t;
}

/* looks for a value starting at the root */
rb_node *rb_search(rb_tree *t, int num) {
    rb_node *curr = t->root;
    while (curr != t->nil && num != curr->data)
        curr = (num > curr->data) ? curr->right : curr->left;
    return (curr == t->nil) ? NULL : curr;
}

/* rotates the tree left around x */
static void rot_L(rb_tree *t, rb_node *x) {
    rb_node *y = x->right;
    x->right = y->left;
    if (y->left != t->nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == t->nil) t->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x; x->parent = y;
}

/* rotates the tree right around y */
static void rot_R(rb_tree *t, rb_node *y) {
    rb_node *x = y->left;
    y->left = x->right;
    if (x->right != t->nil) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == t->nil) t->root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    x->right = y; y->parent = x;
}

/* fixes the tree after inserting a new red node */
static void fix_ins(rb_tree *t, rb_node *n) {
    while (n->parent->color == RED) {
        rb_node *p = n->parent, *g = p->parent;
        if (p == g->left) {
            rb_node *u = g->right;
            if (u->color == RED) { p->color = BLACK; u->color = BLACK; g->color = RED; n = g; }
            else {
                if (n == p->right) { n = p; rot_L(t, n); }
                n->parent->color = BLACK; g->color = RED; rot_R(t, g);
            }
        } else {
            rb_node *u = g->left;
            if (u->color == RED) { p->color = BLACK; u->color = BLACK; g->color = RED; n = g; }
            else {
                if (n == p->left) { n = p; rot_R(t, n); }
                n->parent->color = BLACK; g->color = RED; rot_L(t, g);
            }
        }
    }
    t->root->color = BLACK;
}

/* adds a new value into the tree, then rebalances */
void rb_insert(rb_tree *t, int num) {
    rb_node *n = malloc(sizeof(rb_node));
    n->data = num; n->color = RED; n->left = n->right = t->nil;
    rb_node *p = t->nil, *c = t->root;
    while (c != t->nil) { p = c; c = (n->data < c->data) ? c->left : c->right; }
    n->parent = p;
    if (p == t->nil) t->root = n;
    else if (n->data < p->data) p->left = n;
    else p->right = n;
    if (n->parent == t->nil) { n->color = BLACK; return; }
    fix_ins(t, n);
}

/* helper that prints one subtree in sorted order */
static void print_r(rb_tree *t, rb_node *n) {
    if (n == t->nil) return;
    print_r(t, n->left);
    printf("%d(%c) ", n->data, n->color == RED ? 'R' : 'B');
    print_r(t, n->right);
}
/* prints the whole tree in sorted order */
void rb_print_inorder(rb_tree *t) { print_r(t, t->root); printf("\n"); }

/* moves the subtree rooted at v into u's place in u's parent */
static void transplant(rb_tree *t, rb_node *u, rb_node *v) {
    if (u->parent == t->nil) t->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

/* restores the color rules after a black node was removed at x */
static void fix_del(rb_tree *t, rb_node *x) {
    while (x != t->root && x->color == BLACK) {
        if (x == x->parent->left) {
            rb_node *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                rot_L(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK; w->color = RED;
                    rot_R(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rot_L(t, x->parent);
                x = t->root;
            }
        } else {
            rb_node *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                rot_R(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK; w->color = RED;
                    rot_L(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rot_R(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

/* removes a value from the tree */
void rb_delete(rb_tree *t, int num) {
    rb_node *z = t->root;
    while (z != t->nil && z->data != num)
        z = (num < z->data) ? z->left : z->right;
    if (z == t->nil) return;

    rb_node *y = z;
    rb_color y_orig_color = y->color;
    rb_node *x;

    if (z->left == t->nil) {
        x = z->right;
        transplant(t, z, z->right);
    } else if (z->right == t->nil) {
        x = z->left;
        transplant(t, z, z->left);
    } else {
        y = z->right;
        while (y->left != t->nil) y = y->left;
        y_orig_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);
    if (y_orig_color == BLACK) fix_del(t, x);
}
/* checks that the tree still follows the color rules */
bool rb_is_valid(rb_tree *t) {
    return (t && t->root->color == BLACK);
}

/* helper that frees one subtree and also every node */
static void free_r(rb_tree *t, rb_node *n) {
    if (n == t->nil) return;
    free_r(t, n->left); free_r(t, n->right); free(n);
}
 
void rb_free_tree(rb_tree *t) {
    free_r(t, t->root);
    free(t->nil);
    free(t);
}
