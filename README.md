# B-Tree

This isn't special, this is only a code piece for my projects.

# How to use?

If you fancy using my code, you need copy the header to your project to use its functions. To use it, you don't need high knowledge, because the functions are very easy to use.

To get started, you need create btree object:
```C
btree tree = allocBtree();
```
Well, now you can use all the functions. So, we'll see some examples... If you want to add a key:
```C
uint64_t key;
void *value;
// ...
insert_key_to_btree(tree, key, value);
```
Remember, you must set a value for your key. Ok, if you want to get the key value, you can do:
```C
void *value = get_value_from_btree(tree, key);
```
To remove a key:
```C
void *value;
remove_key_from_btree(tree,  key,  &value);
// ...
free(value);
```
To finish, you must free the Cimporon object:
```C
freeBtree(tree);
```