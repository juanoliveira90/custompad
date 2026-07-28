#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "get.h"

Virtual arr_virtual[10];


void show_inputs(int fd, ssize_t n, struct input_event ev);
void emit(int fd, int type, int code, int val);
void emit_remapped(int fd, int raw_fd, int index);
int open_raw_device_and_hide_it(char* path);
int map_index_to_virtual(int index);

// options
int create_controller();
void remap(int fd, char* path);

void clear_screen();

InputMap map[] = {
	{ "BTN_SOUTH", BTN_SOUTH },  					// Xbox A
    { "BTN_EAST", BTN_EAST },   					// Xbox B
    { "BTN_WEST", BTN_WEST },   					// Xbox Y
    { "BTN_NORTH", BTN_NORTH },						// Xbox X
	{ "BTN_TL", BTN_TL },							// Left bumper (LB)
	{ "BTN_TR", BTN_TR },  							// Right bumper (RB)
	{ "BTN_SELECT", BTN_SELECT },					// Back
	{ "BTN_START", BTN_START },						// Start
	{ "BTN_MODE", BTN_MODE },						// Xbox Guide button
	{ "BTN_THUMBL", BTN_THUMBL },					// Left stick click (LS)
	{ "BTN_THUMBR", BTN_THUMBR },					// Reft stick click (RS)
	
	// Extra buttons
	{ "BTN_TRIGGER_HAPPY", BTN_TRIGGER_HAPPY },		
	{ "BTN_TRIGGER_HAPPY1", BTN_TRIGGER_HAPPY1 },	
	
	// Rear buttons; 2 is bottom
	{ "BTN_GRIPR", BTN_GRIPR },
	{ "BTN_GRIPR", BTN_GRIPR2 },
	{ "BTN_GRIPL", BTN_GRIPL },
	{ "BTN_GRIPL", BTN_GRIPL2 },
};

int main (int argc, char *argv[]) 
{
	get_controllers_and_store_them_in_array();
	int fd = create_controller();							
	int position = map_index_to_virtual(0);					// TODO: get index value dynamically
	default_map(position);									/* apply xbox mapping */
	int raw_fd = open_raw_device_and_hide_it(arr[0].path);
	

	if (argc == 1) // temporary: get first controller available
	{
	}

	
	if (strcmp(argv[1], "-m") == 0)
	{
		int original;
		int mapped;

		if (argc != 4)
		{ 
			printf("\nmapping usage: ./prog -m [original key button] [mapped key button]");
			return 0;
		}
		for (int i = 0; i < 4; i++)
		{
			if (strcmp(argv[2], map[i].name) == 0)
				original = map[i].value;
			if (strcmp(argv[3], map[i].name) == 0)
				mapped = map[i].value;	
		}
		arr_virtual[0].map[original] = mapped;

		printf("Remap complete! %s is now %s\n", argv[2], argv[3]);
		fflush(stdout);
		emit_remapped(fd, raw_fd, position);
	}

	if (strcmp(argv[1], "-d") == 0)
	{
		// TODO: deadzone handling
	}

	return 0;
}

void clear_screen() // ANSI escape sequences
{
	printf("\033[2J\033[1;1H");
	fflush(stdout);
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

int open_raw_device_and_hide_it(char* path)
{
	int raw_fd = open(path, O_RDONLY); 
	ioctl(raw_fd, EVIOCGRAB, 1); 
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
