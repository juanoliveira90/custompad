#ifndef HASHMAP_H
#define HASHMAP_H

#include <string.h>
#include <stdio.h>
#include <linux/input.h>

typedef struct 
{
    int index;
    char* name;  
    char* path; 
} Gamepad;    

typedef struct
{
    int* address;
    int index;
    int map[KEY_MAX + 1];
} Virtual;

extern Gamepad arr[10];
extern Virtual arr_virtual[10];
extern int len;

void push(char* name, char* path);
char* get_path(char* name);
char* get_name(char* path);
int get_index(char* path);



#endif
