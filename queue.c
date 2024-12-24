#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    element_t *q = malloc(sizeof(element_t));
    if (q == NULL) {
        return NULL;
    }
    INIT_LIST_HEAD(&q->list);
    return &q->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    struct list_head *node = l->next;
    while (node != l) {
        // cppcheck-suppress nullPointer
        element_t *e = container_of(node, element_t, list);
        node = node->next;
        q_release_element(e);
    }
    // cppcheck-suppress nullPointer
    element_t *head = container_of(l, element_t, list);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *q = malloc(sizeof(element_t));
    if (!q) {
        return false;
    }
    q->value = malloc(sizeof(s));
    memset(q->value, '\0', strlen(q->value));
    strncpy(q->value, s, strlen(s));
    list_add(&q->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *q = malloc(sizeof(element_t));
    if (!q) {
        return false;
    }
    q->value = malloc(sizeof(s));
    memset(q->value, '\0', strlen(q->value));
    strncpy(q->value, s, strlen(s));
    list_add_tail(&q->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head) != 0) {
        return NULL;
    }
    if (sp == NULL) {
        return NULL;
    }
    // cppcheck-suppress nullPointer
    element_t *cur_e = list_entry(head->next, element_t, list);
    list_del_init(head->next);
    strncpy(sp, cur_e->value, bufsize - 1);

    return cur_e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head) != 0) {
        return NULL;
    }
    if (sp == NULL) {
        return NULL;
    }
    // cppcheck-suppress nullPointer
    element_t *cur_e = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);
    strncpy(sp, cur_e->value, bufsize - 1);

    return cur_e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head) != 0) {
        return false;
    }

    if (list_is_singular(head)) {
        head = head->next;
        // cppcheck-suppress nullPointer
        element_t *e = container_of(head, element_t, list);
        list_del(head);
        q_release_element(e);
        return true;
    }

    int del_n_index = 0;
    struct list_head *node = head->next;
    /* even nodes  */
    if (q_size(node) % 2 == 0) {
        del_n_index = q_size(node) / 2;
    } else { /* odd nodes  */
        del_n_index = (q_size(node) + 1) / 2 - 1;
    }
    while (del_n_index > 0) {
        node = node->next;
        del_n_index--;
    }
    // cppcheck-suppress nullPointer
    element_t *e = container_of(node, element_t, list);
    list_del(node);
    q_release_element(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *node, *del;
    element_t *del_node;
    node = head->next;
    while (node != head) {
        char *cstr, *nstr;
        // cppcheck-suppress nullPointer
        cstr = list_entry(node, element_t, list)->value;
        if (node->next == head) {
            break;
        }
        // cppcheck-suppress nullPointer
        nstr = list_entry(node->next, element_t, list)->value;
        if (strcmp(cstr, nstr) == 0) {
            del = node;
            node = node->next;
            list_del_init(del);
            // cppcheck-suppress nullPointer
            del_node = list_entry(del, element_t, list);
            q_release_element(del_node);
        } else {
            node = node->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *odd, *even;
    odd = head->next;
    even = odd->next;
    while (true) {
        // swap odd and even
        odd->next = even->next;
        odd->next->prev = odd;
        even->prev = odd->prev;
        even->prev->next = even;
        odd->prev = even;
        even->next = odd;
        // end
        odd = odd->next;
        even = odd->next;
        if (odd == head || even == head) {
            break;
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head) != 0) {
        return;
    }
    struct list_head *node, *temp;
    for (node = head->next; node != head; node = node->prev) {
        temp = node->next;
        node->next = node->prev;
        node->prev = temp;
    }
    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void quickSort(struct list_head *low, struct list_head *high)
{
    if ((low != high) && (high->next != low)) {
        // partition elements.
        // using high element as pivot
        char *pivot_value = list_entry(high, element_t, list)->value;
        struct list_head *idx = low->prev;
        for (struct list_head *node = low; node != high; node = node->next) {
            element_t *current = list_entry(node, element_t, list);
            if (strcmp(current->value, pivot_value) <= 0) {
                idx = idx->next;
                swap_element_value(list_entry(idx, element_t, list), current);
            }
        }
        idx = idx->next;
        swap_element_value(list_entry(idx, element_t, list),
                           list_entry(high, element_t, list));
        quickSort(low, idx->prev);
        quickSort(idx->next, high);
    }
}

void q_sort(struct list_head *head, bool descend)
{
    if (head && !list_empty(head))
        quickSort(head->next, head->prev);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */

int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
