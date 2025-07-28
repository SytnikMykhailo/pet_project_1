#include "../headers/hash_table.hpp"

#include <limits>
#include <cstdint>

template <typename T>
HashTable<T>::HashTable() {
    capacity = 10;
    numberOfElements = 0;
    arr = new Node*[capacity];
    for (int i = 0; i < capacity; i++) {
        arr[i] = new Node{nullptr, nullptr, nullptr};
    }
}

template <typename T>
HashTable<T>::HashTable(int numberOfElements, int capacity) {
    this->numberOfElements = numberOfElements;
    this->capacity = capacity;
    arr = new Node*[capacity];
    for (int i = 0; i < capacity; i++) {
        arr[i] = new Node{nullptr, nullptr, nullptr};
    }
}

template <typename T>
int HashTable<T>::hashFunction(const char* key) {
    int sum = 0, factor = 31;
    for (int i = 0; i < (int)strlen(key); i++) {
        sum = ((sum % capacity) + ((int)key[i] * factor) % capacity) % capacity;
        factor = (factor * 31) % std::numeric_limits<int16_t>::max();
    }
    return sum;
}

template <typename T>
void HashTable<T>::setNode(Node* node, const char* key, const T* value) {
    node->key = new char[strlen(key) + 1];
    strcpy(node->key, key);
    node->value = new T(*value);
    node->next = nullptr;
}

template <typename T>
void HashTable<T>::insert(const char* _key, const T* _value) {
    char* key = new char[strlen(_key) + 1];
    strcpy(key, _key);

    int index = hashFunction(key);
    Node* node = arr[index];

    if (node == nullptr || node->key == nullptr) {
        setNode(node, key, _value);
        numberOfElements++;
        return;
    }

    while (node->next != nullptr) {
        node = node->next;
    }

    Node* newNode = new Node;
    setNode(newNode, key, _value);
    node->next = newNode;
    numberOfElements++;
}

template <typename T>
void HashTable<T>::deleteKey(const char* key) {
    int index = hashFunction(key);
    Node* node = arr[index];

    if (node == nullptr || node->key == nullptr) return;

    if (strcmp(node->key, key) == 0) {
        Node* temp = node;
        arr[index] = node->next;
        delete temp;
        numberOfElements--;
        return;
    }

    while (node->next != nullptr) {
        if (strcmp(node->next->key, key) == 0) {
            Node* temp = node->next;
            node->next = node->next->next;
            delete temp;
            numberOfElements--;
            return;
        }
        node = node->next;
    }
}

template <typename T>
T* HashTable<T>::search(const char* key) {
    int index = hashFunction(key);
    Node* node = arr[index];
    while (node != nullptr) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return nullptr;
}

template <typename T>
HashTable<T>::~HashTable() {
    for (int i = 0; i < capacity; i++) {
        Node* node = arr[i];
        while (node != nullptr) {
            Node* temp = node;
            node = node->next;
            delete temp;
        }
    }
    delete[] arr;
}


template class HashTable<char>;