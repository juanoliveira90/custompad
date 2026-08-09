#include "get.h"
#include "controller.h"

int get_controllers_and_store_them_in_array()
{
	
	int fd;
	int count = 0;
	char path[64];
	struct dirent *entry;
	DIR *dir;
	
	arr_capacity = 1;
	arr = malloc(arr_capacity * sizeof(Gamepad));
	if (arr == NULL)
	{
		printf("error when trying to allocate memory\n");
		return 1;
	}
	
	dir = opendir("/dev/input/");
	if (dir == NULL) {
		perror("Error when opening /dev/input/*");
		exit(EXIT_FAILURE);
	}
	
	// get everything that starts with event
	while ((entry = readdir(dir)) != NULL)
	{
		if (strncmp(entry->d_name, "event", 5) == 0) 
		{
			char name[64] = "Unknown";
			unsigned long mask[ARR_SIZE(BTN_SOUTH)] = {0};
			snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
			
			fd = open(path, O_RDONLY);
			ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(mask)), mask);
			int64_t n = 1;

			// check if BTN_SOUTH is set
			if (mask[BTN_SOUTH / BITS_PER_LONG] & (n << (BTN_SOUTH % BITS_PER_LONG))) 
			{
				count++;
				if (count > arr_capacity)
				{
					arr_capacity = count;
					Gamepad* tmp = realloc(arr, arr_capacity * sizeof(Gamepad));
					if (tmp == NULL)
					{
						printf("error when trying to reallocate memory");
						return 1;
					}
					else arr = tmp;
				}
 				
				ioctl(fd, EVIOCGNAME(sizeof(name)), name);

				char* tmp_name = strdup(name);
				char* tmp_path = strdup(path);

				arr[count-1].name = tmp_name;
				arr[count-1].path = tmp_path;
			}
			close(fd);
		}
	}
	closedir(dir);

	if (count == 0)
	{
		printf("No controllers found.\n");
	}
	return count;
}

void display_controllers()
{
	for (int i = 0; i < arr_capacity; i++)
	{
		if (arr[i].name != NULL)
		{
			printf("%d. %s: %s\n", i+1, arr[i].name, arr[i].path);
		}
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
	if (ev.type == EV_KEY || ev.type == EV_ABS) 
	{
		printf("Key event - Code: %d, Value: %d\n", ev.code, ev.value);
		return ev;
	}
	return ev;
}
