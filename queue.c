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
    element_t *cur, *safe;
    list_for_each_entry_safe (cur, safe, l, list) {
        q_release_element(cur);
    }
    free(l);
}
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);
    int str_size = strlen(s);
    new->value = malloc((str_size + 1) * sizeof(char));

    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, str_size);
    *(new->value + str_size) = '\0';
    list_add(&new->list, head);
    return true;
}
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->next, element_t, list);
    list_del(&target->list);

    if (bufsize) {
        strncpy(sp, target->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return target;
}
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->prev, element_t, list);
    list_del(&target->list);

    if (bufsize) {
        strncpy(sp, target->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return target;
}
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);
    int str_size = strlen(s);
    new->value = malloc((str_size + 1) * sizeof(char));

    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, str_size);
    *(new->value + str_size) = '\0';
    list_add_tail(&new->list, head);
    return true;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
        return false;
    element_t *c, *n;  // current and next element
    bool is_dup = false;
    list_for_each_entry_safe (c, n, head,
                              list) {  // current node (iterator) is allowed to
                                       // be removed from the list.
        if (c->list.next != head &&
            strcmp(c->value, n->value) == 0)  // duplicate string detection
        {
            list_del(&c->list);  // delete node
            q_release_element(c);
            is_dup = true;
        } else if (is_dup) {
            list_del(&c->list);
            q_release_element(c);
            is_dup = false;
        }
    }



    return true;
}



/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head **cur = &head->next;
    for (; *cur != head && (*cur)->next != head; cur = &(*cur)->next->next) {
        struct list_head *first = *cur, *second = first->next;
        first->next = second->next;
        second->next = first;
        second->prev = first->prev;
        first->prev = second;
        *cur = second;
    }
}



/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}



/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    int count = 0;
    struct list_head sub_q, *node, *safe, *tmp = head;
    INIT_LIST_HEAD(&sub_q);
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&sub_q, tmp, node);
            q_reverse(&sub_q);
            list_splice_init(&sub_q, tmp);
            count = 0;
            tmp = safe->prev;
        }
    }
}



/* Sort elements of queue in ascending/descending order */



void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;


    // 使用快慢指針找到中間點，將鏈表分割為兩部分
    struct list_head **indir = &head, *fast;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next) {
        indir = &(*indir)->next;
    }


    LIST_HEAD(tmp);
    list_cut_position(&tmp, *indir, head->prev);  // 分割前半部分到 tmp


    q_sort(head, descend);  // 對前半部分排序
    q_sort(&tmp, descend);  // 對後半部分排序


    // 合併兩個有序部分
    LIST_HEAD(merged);
    while (!list_empty(head) && !list_empty(&tmp)) {
        element_t *E1 = list_first_entry(head, element_t, list);
        element_t *E2 = list_first_entry(&tmp, element_t, list);
        int cmp = strcmp(E1->value, E2->value);
        if (descend)
            cmp = -cmp;
        if (cmp > 0) {
            list_move_tail(&E2->list, &merged);  // 移動 tmp 的節點到 merged
        } else {
            list_move_tail(&E1->list, &merged);  // 移動 head 的節點到 merged
        }
    }


    // 將剩餘部分移動到 merged
    list_splice_tail_init(head, &merged);
    list_splice_tail_init(&tmp, &merged);


    // 將合併結果拼接回 head
    list_splice_tail_init(&merged, head);
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *curr = list_entry(head->prev, element_t, list);
    while (curr->list.prev != head) {
        element_t *next = list_entry(curr->list.prev, element_t, list);
        if (strcmp(next->value, curr->value) > 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            curr = next;
        }
    }
    return q_size(head);
}



/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *curr = list_entry(head->prev, element_t, list);
    while (curr->list.prev != head) {
        element_t *next = list_entry(curr->list.prev, element_t, list);
        if (strcmp(next->value, curr->value) < 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            curr = next;
        }
    }
    return q_size(head);
}



/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */



int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;


    struct list_head result;
    INIT_LIST_HEAD(&result);


    queue_contex_t *c_cont, *n_cont;
    list_for_each_entry_safe (c_cont, n_cont, head, chain) {
        struct list_head *current = c_cont->q;


        while (!list_empty(current)) {
            element_t *curr_elem = list_first_entry(current, element_t, list);
            list_del(&curr_elem->list);


            // 插入到合併結果的正確位置
            struct list_head *pos;
            list_for_each (pos, &result) {
                element_t *res_elem = list_entry(pos, element_t, list);
                if ((descend &&
                     strcmp(curr_elem->value, res_elem->value) > 0) ||
                    (!descend &&
                     strcmp(curr_elem->value, res_elem->value) < 0)) {
                    break;
                }
            }
            list_add_tail(&curr_elem->list, pos);
        }


        INIT_LIST_HEAD(current);  // 清空當前隊列
    }


    // 將結果插入到第一個隊列中
    queue_contex_t *first_context =
        list_first_entry(head, queue_contex_t, chain);
    list_splice_init(&result, first_context->q);


    return q_size(first_context->q);
}
