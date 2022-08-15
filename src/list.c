#include "list.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define INITIAL_CAPACITY 8

void list_init(List *list) {
	list->size = 0;
	list->capacity = INITIAL_CAPACITY;
	
	list->data = (void**) malloc(INITIAL_CAPACITY * sizeof(void*));
	assert(list->data != NULL);
}

void list_init_len(List *list, size_t init_capacity) {
	list->size = 0;
	list->capacity = init_capacity;
	list->data = (void**) malloc(init_capacity * sizeof(void*));
	assert(list->data != NULL);
}

void list_add(List *list, void* value ) {
	if (list->size == list->capacity) {
		void** data = (void**) malloc(2 * list->capacity * sizeof(void*));
		assert(data != NULL);
		memcpy(data, list->data, list->size * sizeof(void*));
		free(list->data);
		list->data = data;
		list->capacity = 2 * list->capacity;
	}
	list->data[list->size] = value;
	list->size++;
}

void list_add_all(List *list, List *other) {
	for (size_t i = 0; i < other->size; i++) {
		list_add(list, list_get(other, i));
	}
}

void* list_get(const List *list, size_t index) {
	return list->data[index];
}

void list_remove(List *list, size_t index) {
	memmove(list->data + index, list->data + index + 1, (list->size - index - 1) * sizeof(void*));
	list->size--;
}

void list_swap_remove(List *list, size_t index) {
	void * tmp = list->data[index];
	list->data[index] = list->data[list->size - 1];
	list->data[list->size -1] = tmp;
	list->size--;
}

void list_pop_back(List *list) {
	list->size--;
}

void list_print(const List *list, void(*elem_print)(const void*)) {
	if(list->size == 0) {
		printf("{}");
	} else {
		printf("{");
		elem_print(list->data[0]);
		for (size_t i = 1; i < list->size; ++i) {
			printf(", ");
			elem_print(list->data[i]);
		}
		printf("}");
	}
}

static void list_sort_sub(void **data, void **temp, size_t left, size_t right, Compare cmp) {
	if (left != right) {
		size_t mid = (left + right) / 2;
		list_sort_sub(data, temp, left, mid, cmp);
		list_sort_sub(data, temp, mid + 1, right, cmp);
		memcpy(temp + left, data + left, (right - left + 1) * sizeof(void*));
		size_t i1 = left;
		size_t i2 = mid + 1;
		for (int curr = left; curr <= right; curr++) {
			if (i1 == mid+1) {
				data[curr] = temp[i2++];
			} else if (i2 > right) {
				data[curr] = temp[i1++];
			} else if (cmp(temp[i1], temp[i2]) < 0) {
				data[curr] = temp[i1++];
			} else  {
				data[curr] = temp[i2++];
			}
		}
	}
}

void list_sort(List *list, Compare cmp) {
	void **temp = (void**)malloc(list->size * sizeof(void*));
	assert(temp != NULL);
	list_sort_sub(list->data, temp, 0, list->size - 1, cmp);
	free(temp);
}

bool list_fit(List *list) {
	if (list->size == 0) {
		list->capacity = 0;
		free(list->data);
		list->data = NULL;
		return true;
	}
	void** data = (void**)realloc(list->data, sizeof(void*) * list->size);
	if(data == NULL) {
		return false;
	}
	list->data = data;
	list->capacity = list->size;
	return true;
}

void list_free(List *list) {
	if (list->data != NULL) { //So that I can count mallocs and frees.
		free(list->data);
	}
}
