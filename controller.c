#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>

#include "controller.h"

Gamepad *arr = NULL;
size_t arr_capacity = 1;

    { ABS_X,      0, 255, 0, 0 },  // left stick X
    { ABS_Y,      0, 255, 0, 0 },  // left stick Y
    { ABS_RX,     0, 255, 0, 0 },  // right stick X
    { ABS_RY,     0, 255, 0, 0 },  // right stick Y
    { ABS_Z,      0, 255, 0, 0 },  // L2 analog trigger
    { ABS_RZ,     0, 255, 0, 0 },  // R2 analog trigger
    { ABS_HAT0X, -1,   1, 0, 0 },  // D-pad X
    { ABS_HAT0Y, -1,   1, 0, 0 },  // D-pad Y
};

struct { int code, min, max, fuzz, flat; } xb_axes[] = {
    { ABS_X,     -32768, 32767, 16, 128 },
    { ABS_Y,     -32768, 32767, 16, 128 },
    { ABS_RX,    -32768, 32767, 16, 128 },
    { ABS_RY,    -32768, 32767, 16, 128 },
    { ABS_Z,          0,  255,  0,   0 },
    { ABS_RZ,         0,  255,  0,   0 },
    { ABS_HAT0X,     -1,     1,  0,   0 },
    { ABS_HAT0Y,     -1,     1,  0,   0 },
};

struct { int code, min, max, fuzz, flat; } procon_axes[] = {
    { ABS_X,  -32767, 32767, 250, 500 },  // left stick X
    { ABS_Y,  -32767, 32767, 250, 500 },  // left stick Y
    { ABS_RX, -32767, 32767, 250, 500 },  // right stick X
    { ABS_RY, -32767, 32767, 250, 500 },  // right stick Y
    { ABS_HAT0X, -1,      1,   0,   0 },  // D-pad X
    { ABS_HAT0Y, -1,      1,   0,   0 },  // D-pad Y
};


int create_controller(char* raw_path)
{
	struct input_id raw_id;
	struct uinput_setup usetup;
	struct uinput_abs_setup abs;

	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd == -1) 
	{
		printf("error when opening /dev/uinput\n");
		return 1;
	}

	int raw = open(raw_path, O_RDONLY);
	if (raw == -1) 
	{
		printf("error when opening %s\n", raw_path);
		return 1;
	}

	if (ioctl(raw, EVIOCGID, &raw_id) < 0)
	{
		printf("failed to get device id via ioctl\n");
		close(raw);
		close(fd);
		return 1;
	}
	unsigned short raw_vendor = raw_id.vendor;


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

	if (raw_vendor == MICROSOFT_VENDOR)
	{

		for (int i = 0; i < sizeof(xb_axes) / sizeof(xb_axes[0]); i++) {
			abs.code = xb_axes[i].code;
			abs.absinfo.minimum = xb_axes[i].min;
			abs.absinfo.maximum = xb_axes[i].max;
			abs.absinfo.fuzz    = xb_axes[i].fuzz;
			abs.absinfo.flat    = xb_axes[i].flat;
			ioctl(fd, UI_ABS_SETUP, &abs);
		}
	}

	else if (raw_vendor == SONY_VENDOR)
	{
		for (int i = 0; i < sizeof(ps_axes) / sizeof(ps_axes[0]); i++) {
			abs.code = ps_axes[i].code;
			abs.absinfo.minimum = ps_axes[i].min;
			abs.absinfo.maximum = ps_axes[i].max;
			abs.absinfo.fuzz    = ps_axes[i].fuzz;
			abs.absinfo.flat    = ps_axes[i].flat;
			ioctl(fd, UI_ABS_SETUP, &abs);
		}	
	}

	else if (raw_vendor == NINTENDO_VENDOR)
	{
		for (int i = 0; i < sizeof(procon_axes) / sizeof(procon_axes[0]); i++) {
			abs.code = procon_axes[i].code;
			abs.absinfo.minimum = procon_axes[i].min;
			abs.absinfo.maximum = procon_axes[i].max;
			abs.absinfo.fuzz    = procon_axes[i].fuzz;
			abs.absinfo.flat    = procon_axes[i].flat;
			ioctl(fd, UI_ABS_SETUP, &abs);
		}
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

void push(char* name, char* path)
{
    for (int i = 0; i < arr_capacity; i++)
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
    for (int i = 0; i < arr_capacity; i++)
    {
        if (strcmp(name, arr[i].name) == 0)
        {
            return arr[i].path;
        }
    }
}

char* get_name(char* path)
{
    for (int i = 0; i < arr_capacity; i++)
    {
        if (strcmp(path, arr[i].path) == 0)
        {
            return arr[i].name;
        }
    }
}
