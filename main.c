#include <stdio.h>
#include "rbtree.h"

int main() {
    rb_tree *t = rb_create_tree();

    rb_insert(t, 10);
    rb_insert(t, 5);
    rb_insert(t, 20);
    rb_insert(t, 15);
    rb_insert(t, 1);

    printf("tree in order: ");
    rb_print_inorder(t);

    rb_node *found = rb_search(t, 15);
    if (found)
        printf("found 15, its color is %d\n", found->color);
    else
        printf("15 not found\n");

    printf("after deleting 5: ");
    rb_delete(t, 5);
    rb_print_inorder(t);

    if (rb_is_valid(t))
        printf("tree looks valid\n");
    else
        printf("tree is broken\n");

    rb_free_tree(t);
    return 0;
}
