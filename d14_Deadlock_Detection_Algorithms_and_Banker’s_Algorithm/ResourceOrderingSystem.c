#include <stdio.h>
#include <stdlib.h>

#define MAX_RESOURCES 10


typedef struct {
    int resource_id;
    int priority;
    int available;
} Resource;


typedef struct {
    Resource resources[MAX_RESOURCES];
    int num_resources;
} ResourceOrderingSystem;


// initialize resource ordering system
void initializeResourceOrdering(ResourceOrderingSystem* ros, int num_res) {
    ros->num_resources = num_res;

    for (int i = 0; i < num_res; ++i) {
        ros->resources[i].resource_id = i;
        ros->resources[i].priority = i;     // higher num -> higher priority
        ros->resources[i].available = 1;
        
    }
}


// check if resource request follows ordering
int isValidResourceRequest(ResourceOrderingSystem* ros, int curr_resource, int req_resource) {
    if (curr_resource == -1) return 1;  // First resource request

    return ros->resources[curr_resource].priority < ros->resources[req_resource].priority;
}


// request resource
int requestResource(ResourceOrderingSystem* ros, int process_id, int curr_resource, int req_resource) {
    if (!isValidResourceRequest(ros, curr_resource, req_resource)) {
        printf("Invalid resource request order! Must request resources in increasing priority.\n");
        return 0;
    }

    if (!ros->resources[req_resource].available) {
        printf("Resource %d not available\n", req_resource);
        return 0;
    }

    ros->resources[req_resource].available = 0;
    printf("Resource %d allocated to process %d\n", req_resource, process_id);
    return 1;
}


int main() {
    ResourceOrderingSystem ros;
    initializeResourceOrdering(&ros, 5);

    // Simulate resource requests
    int process_id = 1;
    int current_resource = -1;
    
    // Valid resource ordering
    if(requestResource(&ros, process_id, current_resource, 0)) {
        current_resource = 0;
    }
    
    if(requestResource(&ros, process_id, current_resource, 2)) {
        current_resource = 2;
    }
    
    // Invalid resource ordering (will fail)
    requestResource(&ros, process_id, current_resource, 1);
    
    return 0;
}
