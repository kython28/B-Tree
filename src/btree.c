#include "btree.h"

#if BTREE_DTYPE >= 8
#include <math.h>
#include <float.h>

#if BTREE_DTYPE == 8
#define BTREE_EPSILON FLT_EPSILON
#else
#define BTREE_EPSILON DBL_EPSILON
#endif

#endif

btree allocBtree(){
	btree tree = calloc(1, sizeof(struct _btree_node));
	return tree;
}

void split_node(btree tree){
	btree child1, child2, parent;
	uint8_t run = 1, x;
	btree_k_t key, *node_keys, *parent_keys, k_tmp[2];
	void **node_childs, **node_values, *parent_childs[2], **parent_values;
	while (1){
		node_childs = tree->childs;
		node_values = tree->values;
		node_keys = tree->keys;
		parent = tree->parent;

		child1 = allocBtree();
		child1->use = 1;
		tree->use = 1;
		if (parent == NULL){
			child2 = allocBtree();

			child1->parent = tree;
			child2->parent = tree;

			child2->use = 1;
			x = 0;
			for (; x<2; x++){
				register btree node_c = node_childs[x];
				if (node_c != NULL) node_c->parent = child1;
			}
			for (; x<4; x++){
				register btree node_c = node_childs[x];
				if (node_c != NULL) node_c->parent = child2;
			}

			memcpy(child1->childs, node_childs, 2*sizeof(void*));
			memcpy(child2->childs, &node_childs[2], 2*sizeof(void*));

			child1->keys[0] = node_keys[0];
			child2->keys[0] = node_keys[2];

			child1->values[0] = node_values[0];
			child2->values[0] = node_values[2];

			node_keys[0] = node_keys[1];
			node_values[0] = node_values[1];

			memset(&node_keys[1], 0, sizeof(btree_k_t)*2);
			memset(&node_values[1], 0, sizeof(void*)*2);
			memset(node_childs, 0, sizeof(void*)*4);

			node_childs[0] = child1;
			node_childs[1] = child2;
			break;
		}else{
			child1->keys[0] = node_keys[2];
			child1->values[0] = node_values[2];
			child1->parent = parent;

			x = 2;
			for (; x<4; x++){
				register btree node_c = node_childs[x];
				if (node_c != NULL) node_c->parent = child1;
			}
			
			memcpy(child1->childs, &node_childs[2], 2*sizeof(void*));
			memset(&node_childs[2], 0, 2*sizeof(void*));

			node_childs = parent->childs;
			parent_keys = parent->keys;
			parent_values = parent->values;

			if (node_childs[0] == tree){
				
				memcpy(parent_childs, &node_childs[1], 2*sizeof(void*));
				memcpy(&node_childs[2], parent_childs, 2*sizeof(void*));
				memset(parent_childs, 0, 2*sizeof(void*));

				node_childs[1] = child1;

				memcpy(k_tmp, parent_keys, 2*sizeof(btree_k_t));
				memcpy(&parent_keys[1], k_tmp, 2*sizeof(btree_k_t));
				memset(parent_childs, 0, 2*sizeof(btree_k_t));

				parent_keys[0] = node_keys[1];

				memcpy(parent_childs, parent_values, 2*sizeof(void*));
				memcpy(&parent_values[1], parent_childs, 2*sizeof(void*));
				memset(parent_childs, 0, 2*sizeof(void*));

				parent_values[0] = node_values[1];
				
			}else if (node_childs[1] == tree){
				node_childs[3] = node_childs[2];
				node_childs[2] = child1;

				parent_keys[2] = parent_keys[1];
				parent_keys[1] = node_keys[1];

				parent_values[2] = parent_values[1];
				parent_values[1] = node_values[1];
				
			}else{
				node_childs[3] = child1;
				parent_keys[2] = node_keys[1];
				parent_values[2] = node_values[1];
			}
			memset(&node_keys[1], 0, 2*sizeof(btree_k_t));
			memset(&node_values[1], 0, 2*sizeof(void*));

			parent->use++;
			if (parent->use == 3) tree = parent;
			else break;
		}
	}
}

void insert_key_to_btree(btree tree, btree_k_t key, void *value){
	uint8_t num, run=1;
	btree_k_t *keys, k_tmp[2];
	void **values, **childs, *tmp_ptr[3];
	while (run){
		num = tree->use;
		keys = tree->keys;
		values = tree->values;
		childs = tree->childs;
		if (num == 0){
			keys[0] = key;
			values[0] = value;
			num++;
			break;
		}else{
			for (uint8_t x=0; ; x++){
				if (x == num){
					if (childs[x] == NULL){
						keys[x] = key;
						values[x] = value;
						run = 0;
						num++;
					}else{
						tree = childs[x];
					}
					break;
				#if BTREE_DTYPE < 8
				}else if (key < keys[x]){
				#else
				}else if ((key - keys[x]) <= -BTREE_EPSILON){
				#endif
					if (childs[x] == NULL){
						memcpy(k_tmp, &keys[x], 8*(2-x));
						memcpy(&keys[x+1], k_tmp, 8*(2-x));

						keys[x] = key;

						memcpy(tmp_ptr, &values[x], 8*(2-x));
						memcpy(&values[x+1], tmp_ptr, 8*(2-x));

						values[x] = value;

						memset(tmp_ptr, 0, 3*sizeof(void*));

						memcpy(tmp_ptr, &childs[x], 8*(3-x));
						memcpy(&childs[x+1], tmp_ptr, 8*(3-x));

						num++;
						run = 0;
					}else{
						tree = childs[x];
					}
					break;
				}
			}
		}
	}
	tree->use = num;
	if (num == 3) split_node(tree);
}

void *get_value_from_btree(btree tree, btree_k_t key){
	uint64_t *keys;
	uint8_t num;
	void **childs, **values;
	while (1){
		keys = tree->keys;
		childs = tree->childs;
		values = tree->values;
		num = tree->use;

		tree = childs[num];
		for (uint8_t x=0; x<num; x++){
			register void *ptr;
			register uint64_t k = keys[x];
			#if BTREE_DTYPE < 8
			if (key == k) return values[x];
			#elif BTREE_DTYPE == 8
			if (fabsf(key-k) <= BTREE_EPSILON) return values[x];
			#elif BTREE_DTYPE == 9
			if (fabs(key-k) <= BTREE_EPSILON) return values[x];
			#endif
			#if BTREE_DTYPE < 8
			else if (key < k){
			#else
			else if ((key - k) <= -BTREE_EPSILON){
			#endif
				ptr = childs[x];
				if (ptr == NULL) return NULL;
				else{
					tree = ptr;
					break;
				}
			}
		}

		if (tree == NULL) break;
	}
	return NULL;
}

static void *get_first(btree tree, btree_k_t *key, btree *node){
	void *child;
	while (1){
		child = tree->childs[0];
		if (child == NULL) break;
		tree = child;
	}
	key[0] = tree->keys[0];
	if (node != NULL) node[0] = tree;
	return tree->values[0];
}

static void *get_last(btree tree, btree_k_t *key, btree *node){
	void *child, *value = NULL;
	uint8_t num = tree->use;
	while (num){
		child = tree->childs[num];
		if (child == NULL) break;
		tree = child;
		num = tree->use;
	}
	if (num > 0){
		key[0] = tree->keys[num-1];
		value = tree->values[num-1];
		if (node != NULL) node[0] = tree;
	}
	return value;
}

void *get_first_value_from_btree(btree tree, btree_k_t *key){
	return get_first(tree, key, NULL);
}

void *get_last_value_from_btree(btree tree, btree_k_t *key){
	return get_last(tree, key, NULL);
}

void remove_key_from_btree(btree tree, btree_k_t key, void **value){
	btree parent;
	uint8_t run=1, num;
	btree_k_t *keys, *k, tmp_keys[2];
	void **node_childs, **node_values, *tmp_child[3];
	void **p_childs, **p_value;
	while (run){
		if (tree == NULL) return;
		register void *current = tree;
		keys = tree->keys;
		node_childs = tree->childs;
		node_values = tree->values;
		num = tree->use;

		tree = node_childs[num];
		for (uint8_t x=0; x<num; x++){
			register void *ptr;
			register btree_k_t _k = keys[x];
			#if BTREE_DTYPE < 8
			if (key == _k){
			#elif BTREE_DTYPE == 8
			if (fabsf(key - _k) <= BTREE_EPSILON){
			#else
			if (fabs(key - _k) <= BTREE_EPSILON){
			#endif
				p_childs = &node_childs[x];
				k = &keys[x];
				p_value = &node_values[x];
				if (value != NULL) value[0] = node_values[x];
				tree = current;
				run = 0;
				break;
			}else if (key < _k){
				tree = node_childs[x];
				break;
			}
		}
	}

	if (p_childs[1] != NULL){
		tree = p_childs[1];
		while (1){
			node_childs = tree->childs;
			if (node_childs[0] == NULL) break;
			tree = node_childs[0];
		}
		keys = tree->keys;
		node_values = tree->values;

		p_value[0] = node_values[0];
		k[0] = keys[0];

		memcpy(tmp_child, &node_childs[1], 3*sizeof(void*));
		memcpy(node_childs, tmp_child, 3*sizeof(void*));
		node_childs[3] = NULL;

		memset(tmp_child, 0, 3*sizeof(void*));

		memcpy(tmp_child, &node_values[1], 2*sizeof(void*));
		memcpy(node_values, tmp_child, 2*sizeof(void*));
		node_values[2] = NULL;

		memcpy(tmp_keys, &keys[1], 16);
		memcpy(keys, tmp_keys, 16);
		keys[2] = 0;

		num = tree->use-1;
		if (num == 0 && node_childs[0] != NULL){
			parent = tree->parent;
			if (parent == NULL){
				parent = node_childs[0];
				memcpy(tree, parent, sizeof(struct _btree_node));
				tree->parent = NULL;

				node_childs = tree->childs;
				for (uint8_t x=0; x<4; x++){
					if (node_childs[x] != NULL){
						((btree)node_childs[x])->parent = tree;
					}
				}
				free(parent);
			}else{
				node_values = parent->childs;
				((btree)node_childs[0])->parent = parent;
				for (uint8_t x=0; x<4; x++){
					if (node_values[x] == tree){
						node_values[x] = node_childs[0];
						break;
					}
				}
				free(tree);
			}
			goto remove_element_success;
		}
	}else if (p_childs[0] != NULL){
		tree = p_childs[0];
		while (1){
			register void *c_t;
			node_childs = tree->childs;
			c_t = node_childs[tree->use];
			if (c_t == NULL) break;
			tree = c_t;
		}

		num = tree->use-1;

		keys = tree->keys;
		node_values = tree->values;

		p_value[0] = node_values[num];
		k[0] = keys[num];

		keys[num] = 0;
		node_values[num] = NULL;

		if (num == 0 && node_childs[0] != NULL){
			parent = tree->parent;
			if (parent == NULL){
				parent = node_childs[0];
				memcpy(tree, parent, sizeof(struct _btree_node));
				tree->parent = NULL;

				for (uint8_t x=0; x<4; x++){
					if (node_childs[x] != NULL){
						((btree)node_childs[x])->parent = tree;
					}
				}
				free(parent);
			}else{
				node_values = parent->childs;
				((btree)node_childs[0])->parent = parent;
				for (uint8_t x=0; x<4; x++){
					if (node_values[x] == tree){
						node_values[x] = node_childs[0];
						break;
					}
				}
				free(tree);
			}
			goto remove_element_success;
		}
	}else{
		#if BTREE_DTYPE < 8
		if (keys[0] == key){
		#elif BTREE_DTYPE == 8
		if (fabsf(keys[0], key) <= BTREE_EPSILON){
		#else
		if (fabs(keys[0], key) <= BTREE_EPSILON){
		#endif
			keys[0] = keys[1];
			node_values[0] = node_values[1];
			node_childs[1] = node_childs[2];

			keys[1] = 0;
			node_values[1] = NULL;
			node_childs[2] = NULL;
		}else{
			keys[1] = 0;
			node_values[1] = NULL;
		}
	}

	tree->use--;
	if (tree->parent == NULL || tree->use > 0) goto remove_element_success;

	remove_child_empty:
	node_childs = ((btree)tree->parent)->childs;
	for (uint8_t x=0; x<3; x++){
		if (node_childs[x] == tree){
			node_childs[x] = NULL;
			break;
		}
	}
	free(tree);
	remove_element_success:
	return;
}

void *remove_first_key_from_btree(btree tree, btree_k_t *key){
	btree node;
	void *value = get_first(tree, key, &node);
	if (node->use > 0) remove_key_from_btree(node, key[0], NULL);
	return value;
}

void *remove_last_key_from_btree(btree tree, btree_k_t *key){
	btree node;
	void *value = get_last(tree, key, &node);
	if (node->use > 0) remove_key_from_btree(node, key[0], NULL);
	return value;
}

void freeBtree(btree tree){
	if (tree == NULL) return;
	void **childs = tree->childs;
	void **values = tree->values;
	for (uint8_t x=0; x<4; x++){
		if (childs[x] != NULL) freeBtree(childs[x]);
	}
	for (uint8_t x=0; x<3; x++){
		if (values[x] != NULL) free(values[x]);
	}

	free(tree);
}