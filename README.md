# B-Tree
This isn't special, this is only a code piece for my projects.
# How to build it?
To build it, you have to define the datatype to use:
| Number | Datatype |
|--|--|
| 0 | uint8_t |
| 1 | int8_t |
| 2 | uint16_t |
| 3 | int16_t |
| 4 | uint32_t |
| 5 | int32_t |
| 6 | uint64_t (default) |
| 7 | int64_t |
| 8 | float |
| 9 | double |
| 10 | void * |
Example:
```Bash
gcc -c src/btree.c -DBTREE_DTYPE=6 -o btree.o
```
If you will use `float` or `double` data type, you have to add `-lm`. Example:
```Bash
gcc -c src/btree.c -DBTREE_DTYPE=8 -lm -o btree.o
```

# How to use?
If you fancy using my code, you need copy the header to your project to use its functions. To use it, you don't need high knowledge, because the functions are so easy to use.

To get started, you need create btree object:
```C
btree tree =  allocBtree();
```
Well, now you can use all the functions. So, let's see some examples... If you want to add a key:
```C
uint64_t key;
void  *value;
// ...
insert_key_to_btree(tree, key, value);
```
Remember, you have to set a value for your key. Well, if you want to get the key value, you can do:
```C
void  *value =  get_value_from_btree(tree, key);
```
To get first key and value from a btree:
```C
uint64_t key;
void  *value =  get_first_value_from_btree(tree, &key);
```
To get last key and value from a btree:
```C
uint64_t key;
void  *value =  get_last_value_from_btree(tree, &key);
```
To remove a key:
```C
void  *value;
remove_key_from_btree(tree, key, &value);
// ...
free(value);
```
To remove first key and value from a btree:
```C
uint64_t key;
void  *value =  remove_first_value_from_btree(tree, &key);
// ...
free(value);
```
To remove last key and value from a btree:
```C
uint64_t key;
void  *value =  remove_last_value_from_btree(tree, &key);
// ...
free(value);
```
To finish, let's free the btree object:
```C
freeBtree(tree);
```