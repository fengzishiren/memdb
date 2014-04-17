/*
 * util.c
 *
 *  Created on: 2013-9-12
 *      Author: lunatic
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
/*
 * 	 双向链表：
 *
 *	 分布：
 *	 head     0       1     2      3 	  4
 *	 |---|  |---|  |---|  |---|  |---|  |---|
 *
 *	 形式： _____			_____		 _____
 *	 ---->|		|----->|	 |----->|	 |----->
 *	 <----|		|<-----|	 |<-----|	 |<-----
 *		   —————		—————		 ————
 *
 *	 head位置不存储元素 索引index从head的下一个位置以0开始
 *
 *	 note 允许存储NULL元素
 */

struct list *list_new() {
	struct list *list = malloc(sizeof(struct list));
	struct list_entry * entry = (struct list_entry *) malloc(
			sizeof(struct list_entry));
	entry->data = NULL;
	entry->prev = entry->next = entry;
	list->head = entry;
	list->size = 0;
	return list;
}

static inline void entry_addbefore(void *data, struct list_entry *entry) {
	struct list_entry *new_entry = (struct list_entry *) malloc(
			sizeof(struct list_entry));
	new_entry->data = data;
	new_entry->next = entry;
	new_entry->prev = entry->prev;
	new_entry->prev->next = new_entry;
	new_entry->next->prev = new_entry;

}

static inline struct list_entry *get_entry(struct list *list, size_t index) {
	int i;
	struct list_entry *curr = list->head;
	if (index >= list->size) {
		return NULL;
	}
	if (index < (list->size >> 1)) {
		for (i = 0; i <= index; ++i) {
			curr = curr->next;
		}
	} else {
		for (i = list->size; i > index; --i) {
			curr = curr->prev;
		}
	}
	return curr;
}
/*
 * @brief  每次添加一个元素到链表的尾部，由于是双向链表，因此可以直接定位到头部的前一个位置
 *
 *
 */
void list_add(struct list *list, void *data) {
	entry_addbefore(data, list->head);
	list->size++;
}

void list_add_pos(struct list *list, void *data, size_t index) {
	struct list_entry * e =
			index == list->size ? list->head : get_entry(list, index);
	if (e == NULL)
		return;

	entry_addbefore(data, e);
	list->size++;
}

void *list_get(struct list *list, size_t index) {
	struct list_entry *e = get_entry(list, index);
	return e == NULL ? NULL : e->data;
}
/*
 *
 *
 */
void *list_del(struct list *list, size_t index) {
	void *data;
	struct list_entry *curr = get_entry(list, index);
	if (curr == NULL)
		return NULL;

	data = curr->data;
	curr->next->prev = curr->prev;
	curr->prev->next = curr->next;
	curr->next = curr->prev = NULL;

	list->size--;

	free(curr);
	return data;
}

void *list_peek(struct list *list) {
	return get_entry(list, 0)->data;
}

/*
 * 加入队尾
 *
 * note:这里将链表的尾部当做队尾
 */
void list_offer(struct list *list, void *data) {
	entry_addbefore(data, list->head);
	list->size++;
}
/*
 * 弹出队头
 *
 * note:这里将链表的头部当做队头
 */
void *list_poll(struct list *list) {
	return list_del(list, 0);
}

/* 、
 * 入栈
 *
 * note:这里将链表的尾部当做栈顶
 */
void list_push(struct list *list, void *data) {
	entry_addbefore(data, list->head);
	list->size++;
}
/*
 * 出栈
 *
 * note:这里将链表的尾部当做栈顶
 */
void *list_pop(struct list *list) {
	return list_del(list, list->size - 1);
}

size_t list_size(struct list *list) {
	return list->size;
}


void list_free(struct list *list, int del_data) {
	int len;
	struct list_entry *del_curr, *curr = list->head;
	len = list->size;
	while (len--) {
		del_curr = curr;
		curr = curr->next;
		if (del_data) {
			free(del_curr->data);
		}
		free(del_curr);
	}
	free(list);
}

struct list_iter *list_iter(struct list *list, struct list_iter *it) {
	it->next = list->head->next;/*note:head->data = NULL*/
	it->cursor = 0;
	it->size = list->size;
	return it;
}

struct list_entry *list_next(struct list_iter *it) {
	if (it->cursor == it->size)
		return NULL;
	struct list_entry *e = it->next;
	it->next = e->next;
	it->cursor++;
	return e;
}

void list_each(struct list *list, void (*handle)(void *e)) {
	struct list_entry *curr;
	int len = list->size;
	curr = list->head;
	while (len--) {
		curr = curr->next;
		handle(curr);
	}
}
