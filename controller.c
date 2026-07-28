#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>

#include "controller.h"

Gamepad arr[10];
int len = 10;

int create_controller()
{
	struct uinput_setup usetup;
	struct uinput_abs_setup abs;

	// TODO: get those values dynamically
	// TODO: declare, dinamically, the values for thumbsticks, triggers and dpad
	struct { int code, min, max, fuzz, flat; } axes[] = {
		{ ABS_X,     -32768, 32767, 16, 128 },
		{ ABS_Y,     -32768, 32767, 16, 128 },
		{ ABS_RX,    -32768, 32767, 16, 128 },
		{ ABS_RY,    -32768, 32767, 16, 128 },
		{ ABS_Z,          0,  255,  0,   0 },
		{ ABS_RZ,         0,  255,  0,   0 },
		{ ABS_HAT0X,     -1,     1,  0,   0 },
		{ ABS_HAT0Y,     -1,     1,  0,   0 },
	};

	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd == -1) 
	{
		printf("error when opening /dev/uinput\n");
		return 0;
	}

	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_KEYBIT, BTN_SOUTH);
	ioctl(fd, UI_SET_KEYBIT, BTN_EAST);
	ioctl(fd, UI_SET_KEYBIT, BTN_NORTH);
	ioctl(fd, UI_SET_KEYBIT, BTN_WEST);
	ioctl(fd, UI_SET_KEYBIT, BTN_TL);
	ioctl(fd, UI_SET_KEYBIT, BTN_TR);
	ioctl(fd, UI_SET_KEYBIT, BTN_TL2);
	ioctl(fd, UI_SET_KEYBIT, BTN_TR2);
	ioctl(fd, UI_SET_KEYBIT, BTN_SELECT);
	ioctl(fd, UI_SET_KEYBIT, BTN_START);
	ioctl(fd, UI_SET_KEYBIT, BTN_MODE);
	ioctl(fd, UI_SET_KEYBIT, BTN_THUMBL);
	ioctl(fd, UI_SET_KEYBIT, BTN_THUMBR);
	ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY);
	ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY1);
	ioctl(fd, UI_SET_KEYBIT, BTN_GRIPL);
	ioctl(fd, UI_SET_KEYBIT, BTN_GRIPL2);
	ioctl(fd, UI_SET_KEYBIT, BTN_GRIPR);
	ioctl(fd, UI_SET_KEYBIT, BTN_GRIPR2);

	// Digital dpad
	ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_UP);
	ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
	ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
	ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);

	ioctl(fd, UI_SET_EVBIT, EV_ABS);
	ioctl(fd, UI_SET_ABSBIT, ABS_X);
	ioctl(fd, UI_SET_ABSBIT, ABS_Y);
	ioctl(fd, UI_SET_ABSBIT, ABS_RX);
	ioctl(fd, UI_SET_ABSBIT, ABS_RY);
	
	// Triggers
	ioctl(fd, UI_SET_ABSBIT, ABS_Z);
	ioctl(fd, UI_SET_ABSBIT, ABS_RZ);

	// Analog dpad
	ioctl(fd, UI_SET_ABSBIT, ABS_HAT0X);
	ioctl(fd, UI_SET_ABSBIT, ABS_HAT0Y);

	memset(&abs, 0, sizeof(abs));
	for (int i = 0; i < sizeof(axes) / sizeof(axes[0]); i++) {
		abs.code = axes[i].code;
		abs.absinfo.minimum = axes[i].min;
		abs.absinfo.maximum = axes[i].max;
		abs.absinfo.fuzz    = axes[i].fuzz;
		abs.absinfo.flat    = axes[i].flat;
		ioctl(fd, UI_ABS_SETUP, &abs);
	}
	
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = MICROSOFT_VENDOR;
	usetup.id.product = X360_CONTROLLER;
	strcpy(usetup.name, "Xbox 360 Controller");
	
	ioctl(fd, UI_DEV_SETUP, &usetup);
	
	ioctl(fd, UI_DEV_CREATE);
	return fd;
}


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

char* get_path(char* name)
{
    for (int i = 0; i < len; i++)
    {
        if (strcmp(name, arr[i].name) == 0)
        {
            return arr[i].path;
        }
    }
}

char* get_name(char* path)
{
    for (int i = 0; i < len; i++)
    {
        if (strcmp(path, arr[i].path) == 0)
        {
            return arr[i].name;
        }
    }
}

int get_index(char* path)
{
    for (int i = 0; i < len; i++)
    {
        if (strcmp(path, arr[i].path) == 0)
        {
            return arr[i].index;
        }
    }
}
