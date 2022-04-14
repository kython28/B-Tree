#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef BTREE_DTYPE
#define BTREE_DTYPE 6
#endif

#if BTREE_DTYPE == 0
#define btree_k_t uint8_t
#elif BTREE_DTYPE == 1
#define btree_k_t int8_t
#elif BTREE_DTYPE == 2
#define btree_k_t uint16_t
#elif BTREE_DTYPE == 3
#define btree_k_t int16_t
#elif BTREE_DTYPE == 4
#define btree_k_t uint32_t
#elif BTREE_DTYPE == 5
#define btree_k_t int32_t
#elif BTREE_DTYPE == 6
#define btree_k_t uint64_t
#elif BTREE_DTYPE == 7
#define btree_k_t int64_t
#elif BTREE_DTYPE == 8
#define btree_k_t float
#elif BTREE_DTYPE == 9
#define btree_k_t double
#elif BTREE_DTYPE == 10
#define btree_k_t void *
#endif

typedef struct _btree_node {
	void *parent;
	void *childs[4];
	

	#if BTREE_DTYPE == 10
	uint64_t keys_len[3];
	void *keys[3];
	#else
	btree_k_t keys[3];
	#endif

	void *values[3];
	uint8_t use;
} *btree;

btree allocBtree();

void insert_key_to_btree(
	btree tree,
	btree_k_t key,
	#if BTREE_DTYPE == 10
	uint64_t length,
	#endif
	void *value
);

void *get_value_from_btree(
	btree tree,
	btree_k_t key
	#if BTREE_DTYPE == 10
	, uint64_t length
	#endif
);

void *get_first_value_from_btree(
	btree tree, btree_k_t *key
	#if BTREE_DTYPE == 10
	, uint64_t *length
	#endif
);

void *get_last_value_from_btree(
	btree tree, btree_k_t *key
	#if BTREE_DTYPE == 10
	, uint64_t *length
	#endif
);

void remove_key_from_btree(
	btree tree, btree_k_t key,
	#if BTREE_DTYPE == 10
	uint64_t length,
	#endif
	void **value
);

void *remove_first_key_from_btree(
	btree tree, btree_k_t *key
	#if BTREE_DTYPE == 10
	, uint64_t *length
	#endif
);
void *remove_last_key_from_btree(
	btree tree, btree_k_t *key
	#if BTREE_DTYPE == 10
	, uint64_t *length
	#endif
);

void freeBtree(btree tree);

#endif