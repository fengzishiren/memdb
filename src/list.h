/*
 * util.h
 *
 *  Created on: 2013-9-12
 *      Author: lunatic
 */

#ifndef LIST_H_
#define LIST_H_


struct list_entry {
	void *data;
	struct list_entry *prev;
	struct list_entry *next;
};

struct list {
	struct list_entry *head;
	size_t size;
};

struct list_iter {
	size_t size;
	size_t cursor;
	struct list_entry *next;
};

struct list *list_new();

void list_add(struct list *list, void *data);

void list_add_pos(struct list *list, void *data, size_t index);

void *list_get(struct list *list, size_t pos);
/*
 * @brief  删除元素 并返回被删除元素的指针
 *
 * @return 如果被删除的元素本身为NULL或出现错误均返回NULL
 *
 */
void *list_del(struct list *list, size_t pos);

void list_push(struct list *list, void *data);

void *list_pop(struct list *list);

void *list_peek(struct list *list);

void list_offer(struct list *list, void *data);

void *list_poll(struct list *list);

size_t list_size(struct list *list);

void list_show(struct list *list);

struct list *list_clear(struct list *list);

struct list_iter *list_iter(struct list *list, struct list_iter *it);

struct list_entry *list_next(struct list_iter *it);

void list_free(struct list *list);

void list_each(struct list *list, void (*handle)(void *e));

#endif /* UTIL_H_ */
