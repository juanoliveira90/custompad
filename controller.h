#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <stdio.h>
#include <linux/input.h>

#define SONY_VENDOR      0x054c
#define NINTENDO_VENDOR  0x057E
#define MICROSOFT_VENDOR 0x045E
#define GAMESIR_VENDOR   0x3537

#define X360_CONTROLLER  0x028E

typedef struct 
{
    char* name;  
    char* path;
} Gamepad;    

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

typedef struct
{
	int code; 
    int min; 
    int max; 
    int fuzz; 
    int flat;
} AxesConfig;

extern Gamepad *arr;
extern size_t arr_capacity;

int create_controller(char* raw_path);


#endif
