#pragma once
#include <iostream>
#include <cstring>

template <typename T>
class HashTable {
public:
    HashTable();
    HashTable(int numberOfElements, int capacity);
    ~HashTable();

    void insert(const char* key, const T* value);
    void deleteKey(const char* key);
    T* search(const char* key);

private:
    struct Node {
        char* key;
        T* value;
        Node* next;
    };

    Node** arr;
    int capacity;
    int numberOfElements;

    int hashFunction(const char* key);
    void setNode(Node* node, const char* key, const T* value);
};
