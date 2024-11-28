#include "ExtendibleHashing.h"
#include <iostream>

int main() {
    ExtendibleHashing hashTable(1);

    // Insert keys to grow depth to 3 bits
    hashTable.insert("key1", "value1");
    hashTable.insert("key2", "value2");
    hashTable.insert("key3", "value3");
    hashTable.insert("key4", "value4");
    hashTable.insert("key5", "value5");
    hashTable.insert("key6", "value6");
    hashTable.insert("key7", "value7");

    // Print after inserts
    std::cout << "After growing depth to 3 bits:\n";
    hashTable.print();

    // Remove keys to shrink depth back to 2 bits
    hashTable.remove("key4");
    hashTable.remove("key5");
    hashTable.remove("key6");
    hashTable.remove("key1");


    // Print after removals
    std::cout << "After shrinking depth back to 2 bits:\n";
    hashTable.print();

    return 0;
}
