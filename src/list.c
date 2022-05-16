
#include "cutil.h"
#include "pinta.h"

void
pinta_list_init(struct pinta_list *list)
{
    list->first = 0;
    list->last = 0;
}

void
pinta_list_add(struct pinta_list *list, void *element)
{
    struct pinta_list_node *node;
    new(struct pinta_list_node, 1, node);
    node->data = element;
    node->next = 0;

    if (!list->first) {
        list->first = list->last = node;
        node->prev = 0;
    } else {
        node->prev = list->last;
        list->last->next = node;
        list->last = node;
    }
}

void
pinta_list_destroy(struct pinta_list *list)
{
    struct pinta_list_node *current, *next;
    for (current = list->first; current;) {
        next = current->next;
        free(current);
        current = next;
    }
}

