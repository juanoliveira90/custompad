#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

Gamepad arr[10];
int len = 10;

void push(char* name, char* path)
{
    for (int i = 0; i < len; i++)
    {
        if (arr[i].path == NULL)
        {
            arr[i].name = name;
            arr[i].path = path;
            return;
        }
    }
    printf("You hit the number of controllers limit\n");
    return;
    
}

void get(char* name)
{
    for (int i = 0; i < len; i++)
    {
        if (strcmp(name, arr[i].name) == 0)
        {
            printf("path -> %s\n", arr[i].path);
            return;
        }
    }
    return;
}
