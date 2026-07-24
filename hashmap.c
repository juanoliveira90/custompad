#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

Gamepad arr[10];
int len = 10;


void default_map(int index)
{
    for (int i = 0; i < KEY_MAX; i++)
    {
        arr_virtual[index].map[i] = i;
    }
    arr_virtual[index].map[BTN_NORTH] = BTN_WEST;
    arr_virtual[index].map[BTN_WEST] = BTN_NORTH;
}

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
