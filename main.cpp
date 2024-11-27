#include "ExtendibleHashing.h"

int main() {
    ExtendibleHashing hashTable(1);

    // Insert keys
    hashTable.insert(1);
    hashTable.insert(5);
    hashTable.insert(2);

    // Search for keys
    hashTable.search(1);
    hashTable.search(10);

    // Mark a key for deletion
    hashTable.remove(5);

    // Print the hash table
    hashTable.print();

    return 0;
}
