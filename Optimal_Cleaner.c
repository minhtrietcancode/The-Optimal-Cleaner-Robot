#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/****************************************************************************
                      DEFINE CONSTANTS AND STRUCTURES
****************************************************************************/
#define MAX_WORLD_SIZE 6
#define MAX_MOVES 100
#define MAX_QUEUE_SIZE 10000

// Direction constants
#define DIR_UP    'u'
#define DIR_DOWN  'd'
#define DIR_LEFT  'l'
#define DIR_RIGHT 'r'

// Tile constants
#define TILE_DIRT   'D'
#define TILE_EMPTY  'E'
#define TILE_WALL   'W'
#define TILE_ROBOT  'X'

// Movement offsets [row, col]
const int MOVES[4][2] = {
    {-1, 0},  // UP
    {1, 0},   // DOWN
    {0, -1},  // LEFT
    {0, 1}    // RIGHT
};

const char MOVE_CHARS[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

// Structure to represent a state in the search
typedef struct {
    char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
    char moves[MAX_MOVES];
    int move_count;
} State_t;

// Queue structure for BFS
typedef struct {
    State_t* states[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue_t;

/****************************************************************************    
                           FUNCTION PROTOTYPES
****************************************************************************/
// Queue operations
void init_queue(Queue_t* q);
bool is_queue_empty(Queue_t* q);
void enqueue(Queue_t* q, State_t* state);
State_t* dequeue(Queue_t* q);

// State management
State_t* create_state(int rows, int cols);
void copy_world(char dest[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                char src[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                int rows, int cols);

// World operations
bool is_legal_move(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                   int rows, int cols, int row, int col);
void find_robot(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                int rows, int cols, int* robot_row, int* robot_col);
bool is_world_clean(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                    int rows, int cols);
void make_move(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
               int robot_row, int robot_col, 
               int new_row, int new_col);
void serialize_world(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                    int rows, int cols, char* result);

// Main algorithm
char* optimal_vacuuminator(char** input_world, int rows, int cols);

// Test function prototypes
void print_world(char** world, int rows, int cols);
char** create_world(const char* const* strings, int rows, int cols);
void free_world(char** world, int rows);
void run_test_case(const char* const* input_world, int rows, int cols, 
                   const char* test_name);

/****************************************************************************
                    SCAFFOLDING FOR TESTING OUTPUT 
****************************************************************************/
int main() {
    // Test Case 1: Simple horizontal path
    const char* world1[] = {"XED"};
    run_test_case(world1, 1, 3, "Simple horizontal path");
    
    // Test Case 2: Path with vertical movement
    const char* world2[] = {
        "XDD",
        "DEE"
    };
    run_test_case(world2, 2, 3, "Path with vertical movement");
    
    // Test Case 3: Path with wall
    const char* world3[] = {
        "XWD",
        "EEE",
        "DWE"
    };
    run_test_case(world3, 3, 3, "Path with wall");
    
    // Additional test case: Unreachable dirt
    const char* world4[] = {
        "XWD",
        "WWW",
        "DEE"
    };
    run_test_case(world4, 3, 3, "Unreachable dirt");
    
    // Additional test case: More complex path
    const char* world5[] = {
        "EDD",
        "DEE",
        "DEX"
    };
    run_test_case(world5, 3, 3, "Complex path");
    
    return 0;
}

/****************************************************************************
                           QUEUE OPERATIONS
****************************************************************************/
// Helper function to initialize queue
void init_queue(Queue_t* q) {
    q->front = 0;
    q->rear = -1;
}

// Helper function to check if queue is empty
bool is_queue_empty(Queue_t* q) {
    return q->rear < q->front;
}

// Helper function to enqueue a state
void enqueue(Queue_t* q, State_t* state) {
    q->rear++;
    q->states[q->rear] = state;
}

// Helper function to dequeue a state
State_t* dequeue(Queue_t* q) {
    if (is_queue_empty(q)) return NULL;
    return q->states[q->front++];
}

/****************************************************************************
                           STATE MANAGEMENTS
****************************************************************************/
// Helper function to create a new state
State_t* create_state(int rows, int cols) {
    State_t* state = (State_t*)malloc(sizeof(State_t));
    state->move_count = 0;
    memset(state->moves, 0, MAX_MOVES);
    return state;
}

// Helper function to copy a world
void copy_world(char dest[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                char src[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

/****************************************************************************
                            WORLD OPERATIONS
****************************************************************************/
// Helper function to check if move is legal
bool is_legal_move(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                   int rows, int cols, int row, int col) {
    return row >= 0 && row < rows && 
           col >= 0 && col < cols && 
           world[row][col] != TILE_WALL;
}

// Helper function to find robot position
void find_robot(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                int rows, int cols, int* robot_row, int* robot_col) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (world[i][j] == TILE_ROBOT) {
                *robot_row = i;
                *robot_col = j;
                return;
            }
        }
    }
}

// Helper function to check if world is clean
bool is_world_clean(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                    int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (world[i][j] == TILE_DIRT) {
                return false;
            }
        }
    }
    return true;
}

// Helper function to make a move in the world
void make_move(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
               int robot_row, int robot_col, 
               int new_row, int new_col) {
    world[robot_row][robot_col] = TILE_EMPTY;
    world[new_row][new_col] = TILE_ROBOT;
}

// Helper function to serialize world state for comparison
void serialize_world(char world[MAX_WORLD_SIZE][MAX_WORLD_SIZE], 
                    int rows, int cols, char* result) {
    int idx = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[idx++] = world[i][j];
        }
    }
    result[idx] = '\0';
}

/****************************************************************************
            THE MAIN FUNCTIONS THAT USE BREADTH FIRST SEARCH 
****************************************************************************/
// Main function to find optimal cleaning path
char* optimal_vacuuminator(char** input_world, int rows, int cols) {
    // Initialize queue and visited states
    Queue_t queue;
    init_queue(&queue);
    
    // Create initial state
    State_t* initial_state = create_state(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            initial_state->world[i][j] = input_world[i][j];
        }
    }
    
    // Start BFS
    enqueue(&queue, initial_state);
    
    // Keep track of visited states
    char** visited = (char**)malloc(MAX_QUEUE_SIZE * sizeof(char*));
    int visited_count = 0;
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        visited[i] = (char*)malloc((rows * cols + 1) * sizeof(char));
    }
    
    // Add initial state to visited
    char initial_serial[MAX_WORLD_SIZE * MAX_WORLD_SIZE + 1];
    serialize_world(initial_state->world, rows, cols, initial_serial);
    strcpy(visited[visited_count++], initial_serial);
    
    while (!is_queue_empty(&queue)) {
        State_t* current = dequeue(&queue);
        
        // Check if current state is clean
        if (is_world_clean(current->world, rows, cols)) {
            // Found solution - prepare return string
            char* result = (char*)malloc((current->move_count + 1) * sizeof(char));
            strcpy(result, current->moves);
            
            // Clean up
            for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
                free(visited[i]);
            }
            free(visited);
            
            // Free remaining states in queue
            while (!is_queue_empty(&queue)) {
                State_t* temp = dequeue(&queue);
                if (temp != NULL) free(temp);
            }
            
            free(current);
            return result;
        }
        
        // Find robot position
        int robot_row, robot_col;
        find_robot(current->world, rows, cols, &robot_row, &robot_col);
        
        // Try all possible moves
        for (int i = 0; i < 4; i++) {
            int new_row = robot_row + MOVES[i][0];
            int new_col = robot_col + MOVES[i][1];
            
            if (is_legal_move(current->world, rows, cols, new_row, new_col)) {
                // Create new state
                State_t* next = create_state(rows, cols);
                copy_world(next->world, current->world, rows, cols);
                
                // Make move
                make_move(next->world, robot_row, robot_col, new_row, new_col);
                
                // Add move to path
                strcpy(next->moves, current->moves);
                next->moves[current->move_count] = MOVE_CHARS[i];
                next->move_count = current->move_count + 1;
                
                // Check if new state has been visited
                char next_serial[MAX_WORLD_SIZE * MAX_WORLD_SIZE + 1];
                serialize_world(next->world, rows, cols, next_serial);
                
                bool visited_before = false;
                for (int j = 0; j < visited_count; j++) {
                    if (strcmp(visited[j], next_serial) == 0) {
                        visited_before = true;
                        break;
                    }
                }
                
                if (!visited_before) {
                    strcpy(visited[visited_count++], next_serial);
                    enqueue(&queue, next);
                } else {
                    free(next);
                }
            }
        }
        
        free(current);
    }
    
    // Clean up if no solution found
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        free(visited[i]);
    }
    free(visited);
    
    return NULL;
}

/****************************************************************************
                  HELPER FUNCTIONS FOR TESTING OUTPUT
****************************************************************************/
// Function to print the world grid
void print_world(char** world, int rows, int cols) {
    printf("World:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", world[i][j]);
        }
        printf("\n");
    }
}

// Function to create a 2D char array from string array
char** create_world(const char* const* strings, int rows, int cols) {
    char** world = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        world[i] = (char*)malloc(cols * sizeof(char));
        for (int j = 0; j < cols; j++) {
            world[i][j] = strings[i][j];
        }
    }
    return world;
}

// Function to free the 2D world array
void free_world(char** world, int rows) {
    for (int i = 0; i < rows; i++) {
        free(world[i]);
    }
    free(world);
}

// Function to run a test case
void run_test_case(const char* const* input_world, int rows, int cols, 
                   const char* test_name) {
    printf("\nTest Case: %s\n", test_name);
    
    // Create the world
    char** world = create_world(input_world, rows, cols);
    
    // Print initial world
    print_world(world, rows, cols);
    
    // Get optimal path
    char* result = optimal_vacuuminator(world, rows, cols);
    
    // Print result
    if (result != NULL) {
        printf("Optimal path: %s\n", result);
        printf("Path length: %zu\n", strlen(result));
        free(result);
    } else {
        printf("No solution found\n");
    }
    
    // Clean up
    free_world(world, rows);
}
/****************************************************************************
                                THE END
****************************************************************************/
