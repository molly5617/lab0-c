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
    struct list_head *q = malloc(sizeof(*q));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;


    element_t *entry, *safe;


    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);


    free(l);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;


    element_t *new_elem = malloc(sizeof(element_t));  // 創建新的元素
    if (!new_elem)
        return false;


    new_elem->value = strdup(s);  // 分配內存並複製字符串
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }


    // 插入新的元素到鏈表的頭部
    INIT_LIST_HEAD(&new_elem->list);  // 初始化新元素的鏈表指標
    list_add(&new_elem->list, head);  // 將新元素加入到頭部


    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;
    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    INIT_LIST_HEAD(&new_elem->list);
    list_add_tail(&new_elem->list, head);
    return true;
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *first = list_first_entry(head, element_t, list);
    strncpy(sp, first->value, bufsize - 1);  // 最後一個字元是'\0'
    sp[bufsize - 1] = '\0';                  // 確保最後一個字元是'\0'
    list_del(&first->list);
    return first;
}


/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *last = list_last_entry(head, element_t, list);
    strncpy(sp, last->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&last->list);
    return last;
}


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL)
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
    struct list_head *tail = head->prev, *fast = head->next, *slow = fast;
    while (fast != tail && fast->next != tail) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_del(slow);
    q_release_element(container_of(slow, element_t, list));
    return true;
}


/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}


/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}


/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}


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
