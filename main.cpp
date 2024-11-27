#include "ExtendibleHashing.h"

int main() {
    ExtendibleHashing hashTable(1);

    // Insert key-value pairs
    hashTable.insert(1, "Value1");
    hashTable.insert(5, "Value5");
    hashTable.insert(2, "Value2");

    // Search for keys
    std::string value;
    if (hashTable.search(1, value)) {
        std::cout << "Key 1 found with value: " << value << "\n";
    } else {
        std::cout << "Key 1 not found.\n";
    }

    if (hashTable.search(10, value)) {
        std::cout << "Key 10 found with value: " << value << "\n";
    } else {
        std::cout << "Key 10 not found.\n";
    }

    // Remove a key
    hashTable.remove(5);

    // Print the hash table
    hashTable.print();

    return 0;
}
