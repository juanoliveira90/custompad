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
Virtual arr_virtual[10];


void show_inputs(int fd, ssize_t n, struct input_event ev);
void emit(int fd, int type, int code, int val);
void emit_remapped(int fd, int raw_fd, int index);
int open_raw_device(char* path);
int map_index_to_virtual(int index);

// options
int create_controller();
void remap(int fd, char* path);

void clear_screen();

int main (int argc, char *argv[]) 
{
	clear_screen();
	printf("====== Welcome to CustomPad! ======\n\n");
	printf("## Here's a list of available controllers:\n");
	get_controllers();
	display_controllers();

	int choice = 0;
	printf("\n## Choose one controller to edit: ");
	scanf("%d", &choice);

	for (int i = 0; i < choice; i++)
	{
		if (choice == arr[i].index)
		{
			int fd = create_controller();
			int position = map_index_to_virtual(choice);
			default_map(position);
			
			int schoice;
			printf("\n## What do you want to change?\n");
			printf("1. Remap\n");
			printf("2. Deadzone\n");
			scanf("%d", &schoice);
		
			switch (schoice)
			{
				case 1:
					ssize_t n;
					int count = 0;
					int finput = 0;
					int sinput = 0;
					struct input_event res;
					struct input_event ev;
					
					int raw_fd = open_raw_device(arr[i].path);
					printf("\n## You'll press two buttons:\n"); 
					printf("- the first one is the original button.\n");
					printf("- the second one is the new value of the last button you pressed.\n");
					fflush(stdout);

					while (true)
					{
						
						n = read(raw_fd, &ev, sizeof(struct input_event));
						if (ev.type == EV_KEY)
						{
							if (count == 0 && ev.value == 1)
							{
								if (ev.code == 308)	finput = ev.code - 1;
								else finput = ev.code;
								 
								count++;
							}
							else if (count == 1 && ev.value == 1) 
							{
								if (ev.code == 307)	sinput = ev.code + 1;
								else sinput = ev.code;
								
								count++;
								break;
							}
						}
					}
					arr_virtual[i].map[finput] = sinput;
					printf("\nRemap complete!");
					printf("\nfinput -> %d\n", finput);
					printf("sinput -> %d\n", sinput);

					// emit now for testing
					emit_remapped(fd, raw_fd, position);
			}
			return 0;
		}
	}
	return 0;

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

int map_index_to_virtual(int index)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (arr_virtual[i].address == NULL)
		{
			arr_virtual[i].index = index;
			break;
		}
	}
	return i;
}

int open_raw_device(char* path)
{
	int raw_fd = open(path, O_RDONLY); 
	if (raw_fd == -1) 
	{
		printf("error when opening %s\n", path);
		return 0;
	}
	return raw_fd;
}	


void emit_remapped(int fd, int raw_fd, int index)
{
	ssize_t n;
	struct input_event ev;
	struct input_event res;

	while (1)
	{
		res = read_input(raw_fd, n, ev); 

		if (res.type == EV_KEY)
		{
			int mapped = arr_virtual[index].map[res.code];
			printf("%d | ", mapped);
			
			emit(fd, EV_KEY, mapped, res.value);
			emit(fd, EV_SYN, SYN_REPORT, 0);
		}
	}
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
