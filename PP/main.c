/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 10 //defines the max number of tasks per list
#define MAX_LISTS 5 //defines the max number of lists

typedef struct {
    char description[50]; //stores the task description
    int completed; //stores 1 if completed and 0 if not
}Task; //this represents an individual task in the to-do list

typedef struct {
    char title[50]; //list title
    Task tasks[MAX_ITEMS]; //defines multiple task items
    int taskCount; //number of tracks in the  list
}ToDoList; //a list

ToDoList lists[MAX_LISTS]; //defines multiple to-do lists
int listCount = 0; //number of lits

void clearScreen() {
    system("cls");
}


void addToDoList() {
    clearScreen();
    printf("----- Add New To-Do List -----\n");
    if(listCount >= MAX_LISTS)
    {
        printf("Sorry, you cannot add more lists. \n");
        return;
    }
    printf("Enter the list title: ");
    getchar();
    fgets(lists[listCount].title, 50, stdin);
    strtok(lists[listCount].title, "\n");
    printf("Enter the number of tasks: ");
    if (scanf("%d", &lists[listCount].taskCount) != 1)
    {
        printf("Invalid input.\n");
        return;
    }
    getchar();
    //we ask the user to input the tasks
    for(int i=0; i<lists[listCount].taskCount; i++)
    {
        printf("Enter a task %d: ", i+1);
        fgets(lists[listCount].tasks[i].description, 50, stdin);
        strtok(lists[listCount].tasks[i].description,"\n");
        lists[listCount].tasks[i].completed = 0;
    }
    listCount++;
    saveListsToFile("todolists.txt");
}

//marking the task as complete
void markTaskComplete()
{
    clearScreen();
    printf("----- Mark Task as Done -----\n");
    if(listCount == 0)
    {
        printf("No lists available,\n");
        return;
    }
    printf("Choose a list:\n");
    for(int i=0; i<listCount; i++)
    {
        printf("%d. %s\n", i+1,lists[i].title);
    }
    int listIndex;
    if (scanf("%d", &listIndex) != 1)
    {
        printf("Invalid selection.\n");
        return;
    }
    getchar();
    listIndex--;
    if(listIndex<0 || listIndex>=listCount)
    {
        printf("Invalid selection.\n");
        return;
    }
    printf("Choose a task:\n");
    for(int i=0; i<lists[listIndex].taskCount; i++)
    {
        printf("%d. [%c] %s\n", i+1, lists[listIndex].tasks[i].completed ? 'x' : ' ', lists[listIndex].tasks[i].description);
    }
    int taskIndex;
    if (scanf("%d", &taskIndex) != 1)
    {
        printf("Invalid selection.\n");
        return;
    }
    getchar();
    taskIndex--;
    if(taskIndex < 0 || taskIndex >= lists[listIndex].taskCount)
    {
        printf("Invalid selection.\n");
        return;
    }
    lists[listIndex].tasks[taskIndex].completed = 1;
    saveListsToFile("todolists.txt");
    int allDone = 1;
    for (int i = 0; i<lists[listIndex].taskCount; i++) {
        if (lists[listIndex].tasks[i].completed==0) {
            allDone = 0;
            break;
        }
    }
     if (allDone)
    {
        printf("All the tasks have been completed!\n");
        for (int i = listIndex; i < listCount - 1; i++)
        {
            lists[i] = lists[i + 1];
        }
        listCount--;
        saveListsToFile("todolists.txt");
    }
}


//function to display lists
void displayLists()
{
    clearScreen();
    printf("----- To-Do Lists -----\n");
    if (listCount == 0) {
        printf("No lists available.\n");
        return;
    }

    for (int i = 0; i < listCount; i++) {
        printf("\n%s\n", lists[i].title);
        for (int j = 0; j < lists[i].taskCount; j++) {
            printf("[%c] %s\n", lists[i].tasks[j].completed ? 'x' : ' ', lists[i].tasks[j].description);
        }
    }
}

void loadListsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    listCount = 0;
    while (fgets(lists[listCount].title, sizeof(lists[listCount].title), file) != NULL && listCount < MAX_LISTS) {
        strtok(lists[listCount].title, "\n");

        char buffer[10];
        if (fgets(buffer, sizeof(buffer), file) == NULL) break;
        int numTasks = atoi(buffer);
        lists[listCount].taskCount = numTasks;

        for (int i = 0; i < numTasks && i < MAX_ITEMS; i++) {
            if (fgets(lists[listCount].tasks[i].description, sizeof(lists[listCount].tasks[i].description), file) == NULL) break;
            strtok(lists[listCount].tasks[i].description, "\n");

            if (fgets(buffer, sizeof(buffer), file) == NULL) break;
            lists[listCount].tasks[i].completed = atoi(buffer);
        }

        listCount++;
    }

    fclose(file);
}

void saveListsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error saving to file: %s\n", filename);
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


int main() {
    int choice;
    loadListsFromFile("todolists.txt");
    while (1) {
        clearScreen();
        printf("\nTo-Do List Menu:\n");
        printf("1. Add To-Do List\n");
        printf("2. Mark Task as Done\n");
        printf("3. Show Lists\n");
        printf("4. Load Lists from File\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
        printf("Invalid input. Please try again.\n");
        getchar();
        continue;
        }

        if (choice == 1) {
            addToDoList();
        } else if (choice == 2) {
            markTaskComplete();
        } else if (choice == 3) {
            displayLists();
             printf("\nPress Enter to return to the main menu.\n");
            getchar();
            getchar();
        } else if (choice == 4) {
            loadListsFromFile("todolists.txt");
            printf("\nPress Enter to return to the main menu.\n");
            getchar();
            getchar();
        }else if (choice == 5) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice, try again.\n");
            getchar();
        }
    }

    return 0;
}
*/

