
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "task.h"
#include "tasklist.h"

struct tasklist_t* tasklist_new(){
    struct tasklist_t* this = (struct tasklist_t*)malloc(sizeof(struct tasklist_t));
    this->task = NULL;
    this->next = NULL;
    return this;
}

void tasklist_free(struct tasklist_t* list){
    if (list == NULL) return;
    task_free(list->task);
    tasklist_free(list->next); 
    free(list);
}

int tasklist_append(struct tasklist_t* this, struct task_t* t){
    if (this == NULL){
        // Make sure it's heard.
        return 1;
    }
    struct tasklist_t* index = this;
    //Probably sure we can move this to another function.
    while (index->task != NULL && index->next != NULL){
        index = index->next;
    }
    // Stopped because this task is null.
    if (index->task == NULL){
        index->task = t;
   } else { //Stopped because of the end of the list.
        struct tasklist_t* new = tasklist_new();
        new->task = t;
        index->next = new;
    }
    return 0;
}

struct task_t* tasklist_get(struct tasklist_t* list, int index){
    if (!list || !list->task && !list->next) {
        return NULL;
    }
    struct tasklist_t* slider = list;
    int i = 0;
    while (i < index && slider->next != NULL){
        slider = slider->next;
        i++;
    }
    return slider->task;
}

struct tasklist_t* tasklist_remove(struct tasklist_t* list, int index){
    if (!list || !list->task && !list->next) {
        return NULL;
    }
    struct tasklist_t* slider = list;
    int i = 0;
    while (i < index-1){
        slider = slider->next;
    }
    struct tasklist_t* removed = slider->next;
    slider->next = slider->next->next;
    return removed;
}

struct tasklist_t* tasklist_search(struct tasklist_t* list, char* filter){
    if (!list || !list->task && !list->next){
        puts("Empty tasklist.");
        return NULL;
    }

    struct tasklist_t* matches = tasklist_new();
    struct tasklist_t* current = list;
    while (current != NULL){ 
        if (task_has_keyword(current->task, filter)){
            tasklist_append(matches, current->task);
        }
        current = current->next;
    }
    return matches;
}

int tasklist_display(struct tasklist_t* list){
    struct tasklist_t* iter = list;
    int count = 0;
    while(iter != NULL && iter->task != NULL){
        // Change this to a task's print function or something.
        printf("%d: %s", count, iter->task->description);
        iter = iter->next;
        count++;
    }
    return count;
}

int tasklist_dump(struct tasklist_t *list, char* filename)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL){
        return 1;
    }
    struct tasklist_t *iter = list;
    while (iter != NULL){
        fprintf(f, "%s", task_dump(iter->task));
        iter = iter->next;
    }
    fclose(f);
    return 0;
}

int tasklist_read(struct tasklist_t *list, char* filename){
    FILE* f = fopen(filename, "r");
    if (f == NULL){
        puts("Could not open file for tasklist writing.");
        return 1;
    }
    struct tasklist_t* iter = list;
    
    // Moves the index to the end.
    while (iter->next){
        iter = iter->next;
    }    
    char buffer[256];
    while (fgets(buffer, 255, f) != NULL){
        // Remove the ending newline.
        char* start = buffer;
        char* endOfString = strchr(buffer, '\n');
        if (endOfString != NULL){
            endOfString = '\0';
        }
        struct task_t* temp = task_new();
        task_append(temp, buffer);
        tasklist_append(list, temp);
    }
    return 0;
}