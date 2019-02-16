// TODO make map not a linkedlist (this is a placeholder till a hash table working)

struct map_mapping {
  void* key;
  size_t keylen;
  void* value;
  size_t valuelen;

  struct map_mapping* next;
};

typedef struct {
  struct map_mapping* first;
} Map;

/* Initialize the map */
void map_init(Map *map);
/* Delete the map */
void map_free(Map* map);

/* Puts a mapping in the map */
void map_put(Map *map, void* key, void* value, size_t* keylen, size_t* valuelen);
/* Retrieves the value of the map given the key 
 * If the value is found, the */
void map_get(Map* map, void* key, void* value, size_t* keylen, size_t* valuelen);
/* Deletes the mapping given by key */
void map_del(Map* map, void* key, size_t keylen);

