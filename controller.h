#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <stdio.h>
#include <linux/input.h>

#define MICROSOFT_VENDOR 0x045E
#define X360_CONTROLLER 0x028E

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


extern Gamepad arr[10];
extern Virtual arr_virtual[10];
extern int len;

void default_map(int index);
void push(char* name, char* path);
char* get_path(char* name);
char* get_name(char* path);
int get_index(char* path);
int create_controller();


#endif
