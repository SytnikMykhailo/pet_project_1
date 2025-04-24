#include "../headers/hash_table.hpp"

HashTable::HashTable(){
    capacity = 10;
    numberOfElements = 0;
    arr = new struct node*[capacity];
    for(int i = 0; i < capacity; i++){
        arr[i] = new struct node;
        arr[i]->key = nullptr;
        arr[i]->value = nullptr;
        arr[i]->next = nullptr;
    }
}
HashTable::HashTable(int numberOfElements, int capacity){
    this->numberOfElements = numberOfElements;
    this->capacity = capacity;
    arr = new struct node*[capacity];
    for(int i = 0; i < capacity; i++){
        arr[i] = new struct node;
        arr[i]->key = nullptr;
        arr[i]->value = nullptr;
        arr[i]->next = nullptr;
    }
}
int HashTable::hashFunction(const char *key){
    int bucketIndex;
    int sum = 0, factor = 31;
    for (int i = 0; i < strlen(key); i++) {
        sum = ((sum % capacity) + (((int)key[i]) * factor) % capacity) % capacity;
        factor = ((factor % std::numeric_limits<int16_t>::max()) * (31 % std::numeric_limits<int16_t>::max())) % std::numeric_limits<int16_t>::max();
    }
    bucketIndex = sum;
    return bucketIndex;
}
void HashTable::setNode(struct node *node, const char *key, const char *value){
    node->key = new char[strlen(key) + 1];
    node->value = new char[strlen(value) + 1];
    strcpy(node->key, key);
    strcpy(node->value, value);
    node->next = nullptr;
}
void HashTable::insert(const char *_key, const char *_value){
    char *key = new char[strlen(_key) + 1];
    char *value = new char[strlen(_value) + 1];
    strcpy(key, _key);
    strcpy(value, _value);
    int bucketIndex = hashFunction(key);
    struct node *node = arr[bucketIndex];
    if(node == nullptr){
        arr[bucketIndex] = new struct node;
        setNode(arr[bucketIndex], key, value);
        numberOfElements++;
        return;
    }
    if(node->key == nullptr){
        setNode(node, key, value);
        numberOfElements++;
    }else{
        struct node *temp = node;
        while(temp->next != nullptr){
            temp = temp->next;
        }
        struct node *newNode = new struct node;
        setNode(newNode, key, value);
        temp->next = newNode;
        numberOfElements++;
    }
}
void HashTable::deleteKey(const char *key) {
    int bucketIndex = hashFunction(key);
    struct node *node = arr[bucketIndex];
    if (node == nullptr || node->key == nullptr) {
        return;
    }
    if (strcmp(node->key, key) == 0) {
        struct node *temp = node;
        arr[bucketIndex] = node->next;
        delete temp;
        numberOfElements--;
        return;
    }
    while (node->next != nullptr) {
        if (strcmp(node->next->key, key) == 0) {
            struct node *temp = node->next;
            node->next = node->next->next;
            delete temp;
            numberOfElements--;
            return;
        }
        node = node->next;
    }
}


char * HashTable::search(const char *key){
    int bucketIndex = hashFunction(key);
    struct node *node = arr[bucketIndex];
    if(node == nullptr || node->key == nullptr){
        return nullptr;
    }
    while(node != nullptr){
        if(strcmp(node->key, key) == 0){
            return node->value;
        }
        node = node->next;
    }
    return nullptr; 
}


HashTable::~HashTable(){
    for(int i = 0; i < capacity; i++){
        struct node *node = arr[i];
        while(node != nullptr){
            struct node *temp = node;
            node = node->next;
            delete temp;
        }
    }
    delete[] arr;
}
