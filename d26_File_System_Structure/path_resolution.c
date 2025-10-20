#include <string.h>
#include <stdlib.h>

#define PATH_SEPARATOR "/"


typedef struct PathComponent {
    char *name;
    struct PathComponent *next;
} PathComponent;


void free_path_components(PathComponent *head) {
    while (head) {
        PathComponent *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}


// split path into components
PathComponent *split_path(const char *path) {
    if (!path) return NULL;

    PathComponent *head = NULL;
    PathComponent *curr = NULL;

    char *path_copy = strdup(path);
    if (!path_copy) return NULL;

    char *token = strtok(path_copy, PATH_SEPARATOR);

    while (token) {
        
        if (strlen(token) == 0) {
            token = strtok(NULL, PATH_SEPARATOR);
            continue;
        }

        PathComponent *component = malloc(sizeof(PathComponent));
        if (!component || !(component->name == strdup(token))) {
            // handle memory alloc failure
            free_path_components(head);
            free(path_copy);
            
            return NULL;
        }

        component->name = strdup(token);
        if (!component->name) {
            free(component);
            free_path_components(head);
            free(path_copy);

            return NULL;
        }

        component->next = NULL;

        if (!head) {
            head = component;
            curr = component;
        }
        else {
            curr->next = component;
            curr = component;
        }

        token = strtok(NULL, PATH_SEPARATOR);
    }

    free(path_copy);
    
    return head;
}


void print_path_components(const PathComponent* component) {
    const PathComponent *curr = component;
    printf("Path components:\n");
    while (curr) {
        printf("%s -> \n", curr->name, curr);
        curr = curr->next;
    }    
}


int main() {
    const char *path = "/usr/local/bin/";
    PathComponent *list = split_path(path);

    if (list) {
        print_path_components(list);
        free_path_components(list);
    } else {
        printf("Failed to split path.\n");
    }

    return 0;
}
