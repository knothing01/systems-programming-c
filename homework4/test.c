#include "array.h"
#include <assert.h>
#include <string.h>



int main()
{
	struct array* a = array_create();
    assert(0 != a);
	assert(array_size(a) == 0);
    assert(array_empty(a));
	array_push_back(a, 7);
    assert(! array_empty(a));
	assert(array_size(a) == 1);
	array_push_back(a, 6);
    assert(! array_empty(a));
	assert(array_size(a) == 2);
	array_push_back(a, 5);
    assert(! array_empty(a));
	assert(array_size(a) == 3);
	array_print(a);
	array_insert(a, 0, 8);
	assert(array_size(a) == 4);
	array_print(a);
	array_insert(a, 0, 9);
	assert(array_size(a) == 5);
	array_print(a);
	array_insert(a, 2, 0);
	assert(array_size(a) == 6);
	array_print(a);
	array_insert(a, array_size(a), 10);
	assert(array_size(a) == 7);
	array_print(a);
	array_push_back(a, 11);
    assert(array_size(a) == 8);
    assert(a->start[7] == 11);
    array_push_back(a, 12);
    assert(array_size(a) == 9);
    assert(a->start[8] == 12);
    array_pop_back(a);
    assert(array_size(a) == 8);
    assert(a->start[7] == 11);
    array_pop_back(a);
    assert(array_size(a) == 7);
    assert(a->start[6] == 10);
    array_insert(a, 3, 13);
    assert(array_size(a) == 8);
    assert(a->start[3] == 13);
    assert(a->start[4] == 7);
    array_insert(a, 1, 14);
    assert(array_size(a) == 9);
    assert(a->start[1] == 14);
    assert(a->start[2] == 8);
	array_print(a);
	return 0;
}
