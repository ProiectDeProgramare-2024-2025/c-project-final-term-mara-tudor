#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void enableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void enableVirtualTerminalProcessing() {
    // No operation needed on Linux/macOS
}
#endif





#define MAX_ITEMS 10
#define MAX_LISTS 5

#define COLOR_RESET  "\x1b[0m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE   "\x1b[34m"
#define COLOR_CYAN   "\x1b[36m"
#define COLOR_WHITE  "\x1b[37m"
#define COLOR_INPUT  "\x1b[96m"
#define COLOR_TITLE  "\x1b[94m"
#define COLOR_ERROR  "\x1b[91m"
#define COLOR_DONE   "\x1b[32m"  // alias for completed tasks (same as GREEN)
#define COLOR_TODO   "\x1b[33m"  // alias for pending tasks (same as YELLOW)
#define COLOR_RESET  "\x1b[0m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE   "\x1b[34m"
#define COLOR_GREEN  "\x1b[32m"




typedef struct {
    char description[50];
    int completed;
} Task;

typedef struct {
    char title[50];
    Task tasks[MAX_ITEMS];
    int taskCount;
} ToDoList;

ToDoList lists[MAX_LISTS];
int listCount = 0;

// --- Utility Functions ---

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void trim_leading_spaces(char* str) {
    int i = 0;
    while (str[i] == ' ') i++;
    if (i > 0) memmove(str, str + i, strlen(str + i) + 1);
}


int getValidatedInt(const char* prompt, int min, int max) {
    int value;
    char line[100];
    while (1) {
        printf(COLOR_INPUT "%s" COLOR_RESET, prompt);
        if (fgets(line, sizeof(line), stdin)) {
            if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max) {
                return value;
            }
        }
        printf(COLOR_ERROR "❌ Invalid input. Please enter a number between %d and %d.\n" COLOR_RESET, min, max);
    }
}

// --- Core Functions ---

void saveListsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf(COLOR_ERROR "Error saving to file.\n" COLOR_RESET);
        return;
    }

    for (int i = 0; i < listCount; i++) {
        fprintf(file, "%s\n", lists[i].title);
        fprintf(file, "%d\n", lists[i].taskCount);
        for (int j = 0; j < lists[i].taskCount; j++) {
            fprintf(file, "%s\n", lists[i].tasks[j].description);
            fprintf(file, "%d\n", lists[i].tasks[j].completed);
        }
    }

    fclose(file);
}

void loadListsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;

    listCount = 0;
    while (fgets(lists[listCount].title, sizeof(lists[listCount].title), file) && listCount < MAX_LISTS) {
        strtok(lists[listCount].title, "\n");
        char buffer[10];
        if (!fgets(buffer, sizeof(buffer), file)) break;
        int numTasks = atoi(buffer);
        lists[listCount].taskCount = numTasks;

        for (int i = 0; i < numTasks && i < MAX_ITEMS; i++) {
            fgets(lists[listCount].tasks[i].description, sizeof(lists[listCount].tasks[i].description), file);
            strtok(lists[listCount].tasks[i].description, "\n");
            if (!fgets(buffer, sizeof(buffer), file)) break;
            lists[listCount].tasks[i].completed = atoi(buffer);
        }

        listCount++;
    }

    fclose(file);
}

void addToDoList() {
    clearScreen();
    printf("----- " COLOR_TITLE "Add New To-Do List" COLOR_RESET " -----\n");
    if (listCount >= MAX_LISTS) {
        printf(COLOR_ERROR "Maximum number of lists reached.\n" COLOR_RESET);
        return;
    }

    printf(COLOR_INPUT "Enter the list title: " COLOR_RESET);
    fgets(lists[listCount].title, sizeof(lists[listCount].title), stdin);
    strtok(lists[listCount].title, "\n");

    lists[listCount].taskCount = getValidatedInt("Number of tasks (1-10): ", 1, MAX_ITEMS);

    for (int i = 0; i < lists[listCount].taskCount; i++) {
        printf(COLOR_INPUT "Enter task %d description: " COLOR_RESET, i + 1);
        fgets(lists[listCount].tasks[i].description, sizeof(lists[listCount].tasks[i].description), stdin);
        strtok(lists[listCount].tasks[i].description, "\n");
        lists[listCount].tasks[i].completed = 0;
    }

    listCount++;
    saveListsToFile("todolists.txt");
}

void markTaskComplete() {
    clearScreen();
    if (listCount == 0) {
        printf(COLOR_ERROR "No to-do lists available.\n" COLOR_RESET);
        return;
    }

    printf("----- " COLOR_TITLE "Mark Task as Done" COLOR_RESET " -----\n");
    for (int i = 0; i < listCount; i++) {
        printf("%d. " COLOR_TITLE "%s" COLOR_RESET "\n", i + 1, lists[i].title);
    }

    int listIndex = getValidatedInt("Select list number: ", 1, listCount) - 1;

    for (int i = 0; i < lists[listIndex].taskCount; i++) {
        printf("%d. [%s] %s\n", i + 1,
               lists[listIndex].tasks[i].completed ? COLOR_DONE "x" COLOR_RESET : COLOR_TODO " " COLOR_RESET,
               lists[listIndex].tasks[i].description);
    }

    int taskIndex = getValidatedInt("Select task number to mark as done: ", 1, lists[listIndex].taskCount) - 1;
    lists[listIndex].tasks[taskIndex].completed = 1;

    int allDone = 1;
    for (int i = 0; i < lists[listIndex].taskCount; i++) {
        if (!lists[listIndex].tasks[i].completed) {
            allDone = 0;
            break;
        }
    }

    if (allDone) {
        printf(COLOR_DONE "All tasks completed. Removing list...\n" COLOR_RESET);
        for (int i = listIndex; i < listCount - 1; i++) {
            lists[i] = lists[i + 1];
        }
        listCount--;
    }

    saveListsToFile("todolists.txt");
}

void printStructuredTask(const char* input) {
    char part1[100], part2[100], part3[100];
    int matches = sscanf(input, " %[^-] - %[^-] - %[^\n]", part1, part2, part3);

    if (matches == 3) {
        // Remove leading spaces
        trim_leading_spaces(part1);
        trim_leading_spaces(part2);
        trim_leading_spaces(part3);



        printf(COLOR_YELLOW "%s" COLOR_RESET " − " COLOR_BLUE "%s" COLOR_RESET " − " COLOR_GREEN "%s" COLOR_RESET,
               part1, part2, part3);
    } else {
        printf("%s", input);  // fallback: print as-is
    }
}


void displayLists() {
    clearScreen();
    printf("----- To-Do Lists -----\n");
    if (listCount == 0) {
        printf("No lists available.\n");
        return;
    }

    for (int i = 0; i < listCount; i++) {
        printf("\n%s\n", lists[i].title);  // You can color this too if you want
        for (int j = 0; j < lists[i].taskCount; j++) {
            if (lists[i].tasks[j].completed) {
                printf("[" COLOR_GREEN "x" COLOR_RESET "] ");
            } else {
                printf("[" COLOR_RED " " COLOR_RESET "] ");
            }
            printf(COLOR_YELLOW "%s" COLOR_RESET "\n", lists[i].tasks[j].description);
        }
    }

    printf("\nPress Enter to return to the main menu.");
    getchar();
}


int main() {
    enableVirtualTerminalProcessing(); // Windows compatibility
    loadListsFromFile("todolists.txt");

    int choice;
    char buffer[10];

    while (1) {
        clearScreen();
        printf("==== " COLOR_TITLE "To-Do List Menu" COLOR_RESET " ====\n");
        printf("1. Add To-Do List\n");
        printf("2. Mark Task as Done\n");
        printf("3. Show Lists\n");
        printf("4. Exit\n");

        choice = getValidatedInt("Enter your choice (1-4): ", 1, 4);

        switch (choice) {
            case 1: addToDoList(); break;
            case 2: markTaskComplete(); break;
            case 3: displayLists(); break;
            case 4: printf("Exiting...\n"); return 0;
        }
    }

    return 0;
}
