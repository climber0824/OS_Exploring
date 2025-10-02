#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGICAL_MEMORY_SIZE 1024
#define PHYSICAL_MEMORY_SIZE 2048
#define PAGE_SIZE 256

// have some bugs!

typedef struct {
    unsigned int logical_address;
    unsigned int physical_address;
    int valid;
    int protection;     // 1: read, 2: write, 3: execute
} AddressMapping;


typedef struct {
    char* physical_memory;
    AddressMapping* address_map;
    int map_size;
    int used_physical_memory;
} AddressTranslator;


AddressTranslator* initializeAddressTranslator() {
    AddressTranslator* translator = (AddressTranslator*)malloc(sizeof(AddressTranslator));

    translator->physical_memory = (char*)malloc(sizeof(PHYSICAL_MEMORY_SIZE));
    memset(translator->physical_memory, 0, PHYSICAL_MEMORY_SIZE);

    translator->map_size = LOGICAL_MEMORY_SIZE / PAGE_SIZE;
    translator->address_map = (AddressMapping*)malloc(
        translator->map_size * sizeof(AddressMapping)
    );

    for (int i = 0; i < translator->map_size; ++i) {
        translator->address_map[i].logical_address = i * PAGE_SIZE;
        translator->address_map[i].physical_address = 0;
        translator->address_map[i].valid = 0;
        translator->address_map[i].protection = 0;
    }

    translator->used_physical_memory = 0;

    return translator;
}


// allocate physical memory and create mapping
int createAddressMapping(AddressTranslator* translator, 
                        unsigned int logical_address,
                        int protection) {
    int page_idx = logical_address / PAGE_SIZE;

    if (page_idx >= translator->map_size) {
        printf("Error: Logical address out of bounds");
        return -1;
    }

    if (translator->used_physical_memory + PAGE_SIZE > PHYSICAL_MEMORY_SIZE) {
        printf("Error: Physical memory full\n");
        return -1;
    }

    translator->address_map[page_idx].logical_address = logical_address;
    translator->address_map[page_idx].physical_address = translator->used_physical_memory;
    translator->address_map[page_idx].valid = 1;
    translator->address_map[page_idx].protection = protection;

    translator->used_physical_memory += PAGE_SIZE;

    return 0;
}


// translate logical to physical address
unsigned int translateAddress(AddressTranslator* translator,
                              unsigned int logical_address,
                              int access_type) {
    int page_idx = logical_address / PAGE_SIZE;
    int offset = logical_address % PAGE_SIZE;

    if (page_idx >= translator->map_size) {
        printf("Error: Logcial address out of bounds\n");
        return -1;
    }

    AddressMapping* mapping = &translator->address_map[page_idx];

    if (!mapping->valid) {
        printf("Error: Invalid mapping for logical address 0x%x\n", logical_address);
        return -1;
    }

    if ((access_type & mapping->protection) != access_type) {
        printf("Error: Access violation for logical address 0x%x\n", logical_address);
        return -1;
    }

    return mapping->physical_address + offset;    
}


void printAddressSpace(AddressTranslator* translator) {
    printf("\nAddress Space Information:\n");
    printf("Logical Memory Size: %d bytes\n", LOGICAL_MEMORY_SIZE);
    printf("Physical Memory Size: %d bytes\n", PHYSICAL_MEMORY_SIZE);
    printf("Used Physical Memory: %d bytes\n", translator->used_physical_memory);
    printf("Page Size: %d bytes\n", PAGE_SIZE);
    printf("\nValid Mappings:\n");
    
    for (int i = 0; i < translator->map_size; ++i) {
        if (translator->address_map[i].valid) {
            printf("Logical: -x%x -> Physical: -x%x (Protection:%d)\n",
                    translator->address_map[i].logical_address,
                    translator->address_map[i].physical_address,
                    translator->address_map[i].protection
                    );
        }
    }
}


int main() {
    AddressTranslator* translator = initializeAddressTranslator();

    createAddressMapping(translator, 0x0000, 3);    // RWX
    createAddressMapping(translator, 0x0100, 1);    // R
    createAddressMapping(translator, 0x0200, 2);    // W

    unsigned int logical_address[] = {0x0050, 0x0150, 0x250};
    int access_type[] = {1, 2, 3};  // R, W, RWX

    for (int i = 0; i < 3; ++i) {
        unsigned int physical_address = translateAddress(
            translator,
            logical_address[i],
            access_type[i]
        );

        if (physical_address != -1) {
            printf("Translated 0x%x -> 0x%x\n",
                    logical_address[i],
                    physical_address);
        }
    }

    printAddressSpace(translator);

    free(translator->physical_memory);
    free(translator->address_map);
    free(translator);

    return 0;
}

