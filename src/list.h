#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include <stdbool.h>

typedef int (*Compare)(const void *a, const void *b);

typedef struct ptr_list {
	size_t size;
	size_t capacity;
	void** data;
} List;

void list_init(List *list);

void list_init_len(List *list, size_t init_capacity);

void list_add(List *list, void* value);

void list_add_all(List *list, List *other);

void* list_get(const List *list, size_t index);

#define list_at(list, index, type) ((type)list_get(list, index)) 

void list_remove(List *list, size_t index);

void list_swap_remove(List *list, size_t index);

void list_pop_back(List *list);

void list_sort(List *list, Compare cmp);

void list_print(const List *list, void(*elem_print)(const void*));

bool list_fit(List *list);

void list_free(List *list);

#endif