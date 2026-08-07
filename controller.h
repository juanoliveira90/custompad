#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <stdio.h>
#include <linux/input.h>

#define SONY_VENDOR      0x054c
#define NINTENDO_VENDOR  0x57e
#define MICROSOFT_VENDOR 0x045E

#define X360_CONTROLLER  0x028E

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

typedef struct
{
    char* name;
    int value;
} InputMap;

typedef struct
{
    int radial_LS;
    int radial_RS;
    int axial_LS;
    int axial_RS;
} Deadzone;

typedef struct
{
    int radial_LS;
    int radial_RS;
    int axial_LS;
    int axial_RS;
} AntiDeadzone;

extern Gamepad *arr;
extern size_t arr_capacity;

void push(char* name, char* path);
char* get_path(char* name);
char* get_name(char* path);
int get_index(char* path);
int create_controller(char* raw_path);


#endif
