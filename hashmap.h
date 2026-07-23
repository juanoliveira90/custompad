#ifndef HASHMAP_H
#define HASHMAP_H

#include <string.h>
#include <stdio.h>

typedef struct 
{
    char* name;  
    char* path; 
} Gamepad;    

extern Gamepad arr[10];
extern int len;

void push(char* name, char* path);
void get(char* name);

#endif
