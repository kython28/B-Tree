#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct _btree_node {
	void *parent;
	void *childs[4];
	uint64_t keys[3];
	void *values[3];
	uint8_t use;
} *btree;

btree allocBtree();

void insert_key_to_btree(btree tree, uint64_t key, void *value);
void *get_value_from_btree(btree tree, uint64_t key);
void remove_key_from_btree(btree tree, uint64_t key, void **value);

void freeBtree(btree tree);

#endif