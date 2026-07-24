#include "get.h"
#include "hashmap.h"

void get_controllers()
{
	DIR *dir;
	struct dirent *entry;
	int fd;

	dir = opendir("/dev/input/");
	if (dir == NULL) {
		perror("Error when opening /dev/input/js*");
		exit(EXIT_FAILURE);
	}
	// get everything that starts with event
	char name[256] = "Unknown";
	unsigned long mask[ARR_SIZE(BTN_SOUTH)] = {0};
	char path[64];
	int count = 0;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strncmp(entry->d_name, "event", 5) == 0) 
		{
			snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
			fd = open(path, O_RDONLY);
			ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(mask)), mask);
			int64_t n = 1;

			if (mask[BTN_SOUTH / BITS_PER_LONG] & (n << (BTN_SOUTH % BITS_PER_LONG))) 
			{
				count++;
				ioctl(fd, EVIOCGNAME(sizeof(name)), name);
				arr[count-1].index = count;
				arr[count-1].name = strdup(name);
				arr[count-1].path = strdup(path);
				//printf("[debug]%s\n", arr[count].name);
			}
			close(fd);
		}
	}
	if (count == 0)
	{
		printf("No controllers found.\n");
		return;
	}
}

void display_controllers()
{
	for (int i = 0; i < len; i++)
	{
		if (arr[i].name != NULL)
		{
			printf("%d. %s: %s\n", i+1, arr[i].name, arr[i].path);
		}
		return;
	}
	return;
}


struct input_event read_input(int fd, ssize_t n, struct input_event ev)
{
	n = read(fd, &ev, sizeof(struct input_event));
	if (n == -1) 
	{
		perror("error reading event\n");
		exit(EXIT_FAILURE);
	}
	if (ev.type == EV_KEY) 
	{
		printf("Key event - Code: %d, Value: %d\n", ev.code, ev.value);
		return ev;
	}
	return ev;
}
