#include "array.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct array* array_create()
{
	struct array* a = (struct array*)malloc(sizeof(struct array));
        assert(0 != a);
        memset(a, 0, sizeof(struct array));
	a->size = 0;
	a->capacity = 2;
	a->start = (element_type*)malloc(a->capacity * sizeof(element_type));
	return a;
}



void array_push_back(struct array* a, element_type e)
{
    assert(0 != a);  // check that the array is not null
    
    // if the array is full, double its capacity
    if (a->size == a->capacity) {
        a->capacity *= 2;
        element_type* temp = realloc(a->start, a->capacity * sizeof(element_type)); // reallocate memory
        if (temp == NULL) {
            assert(0);  // handle memory allocation failure 
        }
        a->start = temp;
    }
    
    a->start[a->size] = e;  // add the new element at the end of the array
    a->size++;  // increment the size of the array
}

void array_pop_back(struct array* a)
{
    assert(0 != a);  // check that the array is not null
    
    // only pop if the array is not empty
    if (a->size > 0) {
        a->size--;  // decrease the size of the array
    }
    
    // if the size is less than a quarter of the capacity, halve the capacity
    if (a->size < a->capacity / 4) {
        a->capacity /= 2;
        element_type* temp = realloc(a->start, a->capacity * sizeof(element_type)); // reallocate memory
        if (temp == NULL) {
            assert(0);  // handle memory allocation failure
        }
        a->start = temp;
    }
}

void array_insert(struct array* a, size_type index, element_type e)
{
    assert(0 != a);  // check that the array is not null
    assert(index <= a->size);  // ensure the index is within the bounds of the array

    // if the array is full, double its capacity
    if (a->size == a->capacity) {
        a->capacity *= 2;
        element_type* temp = realloc(a->start, a->capacity * sizeof(element_type)); // reallocate memory
        if (temp == NULL) {
            assert(0);  // handle memory allocation failure
        }
        a->start = temp;
    }

    // Correctly shift elements to the right to make room for the new element
    if (index < a->size) {
        memmove(&a->start[index + 1], &a->start[index], (a->size - index) * sizeof(element_type));
    }

    a->start[index] = e;  // insert the new element at the specified index
    a->size++;  // increment the size of the array
    printf("Successfully inserted %d at index %d\n", e, index);
}

size_type array_size(struct array* a)
{
	assert(0 != a);
	return a->size;
}

size_type array_capacity(struct array* a)
{
	assert(0 != a);
	return a->capacity;
}

void array_print(struct array* a)
{
	assert(0 != a);
	for (size_type i = 0; i < a->size; ++i) {
		printf("%d ", a->start[i]);
	}
	printf("\nsize = %d", a->size);
	printf("\ncapacity = %d\n\n", a->capacity);
}

int array_empty(struct array* a)
{
	assert(0 != a);
	return a->size == 0;
}
