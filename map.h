#ifndef MAP_H
#define MAP_H
//
// TODO make map not a linkedlist (this is a placeholder till a hash table working)

struct Mapping {
  void* key;
  size_t keylen;
  void* value;
  size_t valuelen;

  struct Mapping* next;
};

typedef struct {
  struct Mapping* first;
} Map;

/* Initialize the map */
void initMap(Map *map);
/* Delete the map */
void freeMap(Map* map);

/* Puts a Mapping in the map */
void putMap(Map *map, void* key, void* value, size_t* keylen, size_t* valuelen);
/* Retrieves the value of the map given the key 
 * If the value is found, the */
void getMap(Map* map, void* key, void* value, size_t* keylen, size_t* valuelen);
/* Deletes the Mapping given by key */
void removeMap(Map* map, void* key, size_t keylen);

#endif 
