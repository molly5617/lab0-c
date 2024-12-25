#include <stdint.h>
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
    struct list_head *p = malloc(sizeof(struct list_head));
    if (!p)
        return NULL;
    INIT_LIST_HEAD(p);
    return p;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *temp, *safe;
    list_for_each_entry_safe (temp, safe, l, list) {
        list_del(&temp->list);
        free(temp->value);
        free(temp);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    if (!new_node)
        return false;

    new_node->value = (char *) malloc(strlen(s) + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }

    snprintf(new_node->value, strlen(s) + 1, "%s", s);
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    if (!new_node)
        return false;

    new_node->value = (char *) malloc(strlen(s) + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }

    snprintf(new_node->value, strlen(s) + 1, "%s", s);
    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;

    element_t *p = list_entry(head->next, element_t, list);
    strncpy(sp, p->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&p->list);
    return p;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;

    element_t *p = list_entry(head->prev, element_t, list);
    strncpy(sp, p->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&p->list);
    return p;
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
    if (!head || list_empty(head))
        return false;

    struct list_head *fast, *slow;
    fast = head->next->next;
    slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    element_t *del = list_entry(slow, element_t, list);
    free(del->value);
    free(del);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *current = head->next;
    element_t *cur_node, *next_node;
    bool dup = false;
    while (current->next != head) {
        cur_node = list_entry(current, element_t, list);
        next_node = list_entry(current->next, element_t, list);
        if (strcmp(cur_node->value, next_node->value) == 0) {
            dup = true;
            list_del(&next_node->list);
            q_release_element(next_node);
        } else {
            current = current->next;
            if (dup == true) {
                list_del(&cur_node->list);
                q_release_element(cur_node);
                dup = false;
            }
        }
    }
    if (dup) {
        list_del(&cur_node->list);
        q_release_element(cur_node);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;

    struct list_head *p;
    for (p = head->next; p->next != head && p != head; p = p->next)
        list_move(p, p->next);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *node, *safe, *temp = head;
    int count = 0;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            struct list_head *cur_node = temp->next,
                             *next_node = temp->next->next;
            while (count != 0) {
                list_move(cur_node, temp);
                cur_node = next_node;
                next_node = next_node->next;
                count--;
            }
            temp = safe->prev;
        }
    }
}

/* Merge two sorted lists */
struct list_head *merge_two_lists(struct list_head *l1,
                                  struct list_head *l2,
                                  bool descend)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head;
    for (; l1 && l2; ptr = &(*ptr)->next) {
        element_t *a = list_entry(l1, element_t, list);
        element_t *b = list_entry(l2, element_t, list);
        if ((strcmp(a->value, b->value) <= 0 && !descend) ||
            (strcmp(a->value, b->value) > 0 && descend)) {
            *ptr = l1;
            l1 = l1->next;
        } else {
            *ptr = l2;
            l2 = l2->next;
        }
    }
    *ptr = (struct list_head *) ((uintptr_t) l1 | (uintptr_t) l2);
    return head;
}

/* Merge sort recursively */
struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast, *slow;
    fast = slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    slow->prev->next = NULL;
    struct list_head *l1 = merge_sort(head, descend);
    struct list_head *l2 = merge_sort(slow, descend);
    return merge_two_lists(l1, l2, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;
    head->next = merge_sort(head->next, descend);
    head->next->prev = head;
    struct list_head *p = head;
    while (p->next) {
        p->next->prev = p;
        p = p->next;
    }
    head->prev = p;
    p->next = head;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    struct list_head *current = head->prev;
    element_t *cur_node = NULL, *prev_node = NULL;
    int len = 1;
    while (current->prev != head) {
        cur_node = list_entry(current, element_t, list);
        prev_node = list_entry(current->prev, element_t, list);
        if (strcmp(cur_node->value, prev_node->value) < 0) {
            list_del(&prev_node->list);
            q_release_element(prev_node);
        } else {
            len++;
            current = current->prev;
        }
    }
    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    struct list_head *current = head->prev;
    element_t *cur_node = NULL, *prev_node = NULL;
    int len = 1;
    while (current->prev != head) {
        cur_node = list_entry(current, element_t, list);
        prev_node = list_entry(current->prev, element_t, list);
        if (strcmp(cur_node->value, prev_node->value) > 0) {
            list_del(&prev_node->list);
            q_release_element(prev_node);
        } else {
            len++;
            current = current->prev;
        }
    }
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *p = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return p->size;

    struct list_head *node;
    for (node = head->next->next; node != head; node = node->next) {
        queue_contex_t *tmp = list_entry(node, queue_contex_t, chain);
        list_splice_init(tmp->q, p->q);
    }
    q_sort(p->q, descend);
    return p->size;
}