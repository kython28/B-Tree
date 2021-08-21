#include <stdio.h>
#include "../src/btree.h"

int main(){
	btree tree = allocBtree();
	for (uint64_t x=100; x<1000000; x++){
		uint64_t *value = calloc(1, 8);
		value[0] = x*2;
		insert_key_to_btree(tree, x, value);
	}

	for (uint64_t x=0; x<100; x++){
		uint64_t *value = calloc(1, 8);
		value[0] = x*10;
		insert_key_to_btree(tree, x, value);
	}

	for (uint64_t x=0; x<1000000; x++){
		uint64_t *value = get_value_from_btree(tree, x);
		if (value != NULL){
			printf("%ld -> %ld\n", x, value[0]);
		}
	}

	for (uint64_t x=0; x<1000000; x++){
		void *value = NULL;
		remove_key_from_btree(tree, x, &value);
		free(value);
	}

	freeBtree(tree);
	return 0;
}