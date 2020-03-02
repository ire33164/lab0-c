#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"
#include "strnatcmp.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* Return NULL if malloc failed */
    if (!q) {
        return NULL;
    }
    /* Initialize queue */
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* No effect if q is NULL */
    if (!q) {
        return;
    }
    /* Free all elements space */
    for (list_ele_t *cur = q->head, *prev = NULL; cur;) {
        prev = cur;
        cur = cur->next;
        /* Free value space */
        free(prev->value);
        /* Free list_ele structure */
        free(prev);
    }
    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* Return false if q is NULL */
    if (!q) {
        return false;
    }
    newh = malloc(sizeof(list_ele_t));
    /* Return false if malloc failed */
    if (!newh) {
        return false;
    }
    /* Return false if malloc failed */
    newh->value = malloc(strlen(s) + 1);
    if (!newh->value) {
        /* Free allocated space */
        free(newh);
        return false;
    }
    newh->next = q->head;
    strncpy(newh->value, s, strlen(s) + 1);
    if (q->size == 0) {
        q->head = q->tail = newh;
    } else {
        q->head = newh;
    }
    (q->size)++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* Return false if q is NULL */
    if (!q) {
        return false;
    }
    list_ele_t *newt;
    newt = malloc(sizeof(list_ele_t));
    /* Return false if malloc failed */
    if (!newt) {
        return false;
    }
    newt->value = malloc(strlen(s) + 1);
    /* Return false if malloc failed */
    if (!newt->value) {
        /* Free allocated space */
        free(newt);
        return false;
    }
    newt->next = NULL;
    strncpy(newt->value, s, strlen(s) + 1);
    if (q->size == 0) {
        q->tail = q->head = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    (q->size)++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* Return false if q is NULL or empty */
    if (!q || q->size == 0) {
        return false;
    }
    /* Copy string to sp if sp is non-NULL */
    if (sp) {
        /* Copy the string that its length is smaller than bufsize */
        strncpy(sp, q->head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_ele_t *tmp = q->head;
    q->head = q->head->next;
    (q->size)--;
    /* Set tail to NULL if queue is empty */
    q->tail = q->size == 0 ? q->head : q->tail;
    /* Free value space */
    free(tmp->value);
    /* Free list_ele structure */
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* Return 0 if q is NULL */
    if (!q) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* No effect if q is NULL or empty */
    if (!q || q->size == 0) {
        return;
    }
    /* Reverse queue */
    for (list_ele_t *prev = NULL, *cur = q->head; cur;) {
        list_ele_t *tmp = cur;
        cur = cur->next;
        tmp->next = prev;
        prev = tmp;
    }
    list_ele_t *tmp;
    tmp = q->head;
    q->head = q->tail;
    q->tail = tmp;
}

list_ele_t *divide_and_conquer(list_ele_t *head)
{
    if (!head || !head->next) {
        return head;
    }
    list_ele_t *left = head;
    list_ele_t *right = head->next;
    list_ele_t *merge = NULL;

    /* left : 1, right : 2 */
    while (right && right->next) {
        left = left->next;
        right = right->next->next;
    }
    right = left->next;
    left->next = NULL;

    left = divide_and_conquer(head);
    right = divide_and_conquer(right);

    while (left && right) {
        if (strnatcmp(left->value, right->value) < 0) {
            if (!merge) {
                merge = head = left;
            } else {
                merge->next = left;
                merge = merge->next;
            }
            left = left->next;
        } else {
            if (!merge) {
                merge = head = right;
            } else {
                merge->next = right;
                merge = merge->next;
            }
            right = right->next;
        }
    }

    merge->next = left ? left : right ? right : NULL;
    return head;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* No effect if q is NULL or empty */
    if (!q || q->size == 0) {
        return;
    }
    /* Do nothing if q has only one */
    if (q->size == 1) {
        return;
    }
    q->head = divide_and_conquer(q->head);
    list_ele_t *tmp = q->head;
    while (tmp->next) {
        tmp = tmp->next;
    }
    q->tail = tmp;
}
