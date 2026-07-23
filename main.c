#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "get.h"

#define MICROSOFT_VENDOR 0x045E
#define X360_CONTROLLER 0x028E


void show_inputs(int fd, ssize_t n, struct input_event ev);
void emit(int fd, int type, int code, int val);

// options
void list_options();
int create_controller();
//void remap(int fd, char* path);

void clear_screen();

// TODO: create a menu with greeting and that displays the available controllers
int main (int argc, char *argv[]) 
{
	//clear_screen();
	printf("====== Welcome to CustomPad! ======\n\n");
	printf("## Here's a list of available controllers:\n");
	get_controllers();
	display_controllers();
}

void clear_screen() // ANSI escape sequences
{
	printf("\033[2J\033[1;1H");
	fflush(stdout);
}

int create_controller()
{
	struct uinput_setup usetup;

	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd == -1) 
	{
		printf("error when opening /dev/uinput\n");
		return 0;
	}

	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_KEYBIT, BTN_SOUTH);
	ioctl(fd, UI_SET_KEYBIT, BTN_EAST);
	
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = MICROSOFT_VENDOR;
	usetup.id.product = X360_CONTROLLER;
	strcpy(usetup.name, "Xbox 360 Controller");
	
	ioctl(fd, UI_DEV_SETUP, &usetup);
	ioctl(fd, UI_DEV_CREATE);

	return fd;
}

/*void remap(int fd, char* path)
{
    // 1. read raw device & apply remapping
	ssize_t n;
	struct input_event ev;
	struct input_event res;
	
	char **paths = get_controllers_path();

	int raw_fd = open(path, O_RDONLY); 
	if (raw_fd == -1) 
	{
		perror("error when opening %s\n", path);
		exit(EXIT_FAILURE);
	}

	// TODO: get real data from the user and remap it

	while (1)
	{
		res = read_input(raw_fd, n, ev); 
		// simplified mapping
		if (res.code == 0x130) 
		{
			emit(fd, EV_KEY, 0x131, 1);
			emit(fd, EV_SYN, SYN_REPORT, 0);
		}
		if (res.code == 0x130 && res.value == 0) 
		{
			emit(fd, EV_KEY, 0x131, 0);
			emit(fd, EV_SYN, SYN_REPORT, 0);
		}
	
	}
}*/


void list_options()
{
	printf("\n[OPTIONS]\n");
	printf("-h -> list options\n");
	printf("-l -> list controllers\n");
}


void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;

   write(fd, &ie, sizeof(ie));
}

void show_inputs(int fd, ssize_t n, struct input_event ev) 
{
	while (1) 
	{
		n = read(fd, &ev, sizeof(struct input_event));
		if (n == -1) 
		{
			printf("error reading event");
			break;
		}
		if (ev.type == EV_KEY) {
			printf("Key event - Code: %d, Value: %d\n", ev.code, ev.value);
		}
		if (ev.type == EV_ABS) {
			printf("Abs event - Code: %d, Value: %d\n", ev.code, ev.value);
		}
	}
}
