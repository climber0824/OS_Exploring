#include <stdio.h>

typedef enum {
    REQUIREMENT_ISOLATION,
    REQUIREMENT_PERFORMANCE,
    REQUIREMENT_COMMUNICATION,
    REQUIREMENT_SECURITY
} requirement_t;

typedef struct {
    int isolation_weight;
    int performance_weight;
    int communication_weight;
    int security_weight;
} requirements_t;

const char* suggest_implementation(requirements_t reqs) {
    int process_score = 0;
    int thread_score = 0;
    
    // Calculate scores based on requirements
    process_score += reqs.isolation_weight * 9;    // Processes excel at isolation
    process_score += reqs.performance_weight * 5;  // Moderate performance
    process_score += reqs.communication_weight * 3; // Poor for communication
    process_score += reqs.security_weight * 9;     // Excellent security
    
    thread_score += reqs.isolation_weight * 3;     // Poor isolation
    thread_score += reqs.performance_weight * 8;   // Excellent performance
    thread_score += reqs.communication_weight * 9; // Excellent communication
    thread_score += reqs.security_weight * 4;      // Moderate security
    
    return (process_score > thread_score) ? 
           "Use Processes" : "Use Threads";
}

// Example usage:
void demonstrate_decision_making() {
    requirements_t reqs = {
        .isolation_weight = 8,    // High importance
        .performance_weight = 5,  // Medium importance
        .communication_weight = 3, // Low importance
        .security_weight = 9      // Critical importance
    };
    
    printf("Suggestion: %s\n", suggest_implementation(reqs));
}

int main() {
    demonstrate_decision_making();

    return 0;
}
