#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <tomlc17.h>
#include <libevdev-1.0/libevdev/libevdev.h>

#include "get.h"

#define STICK_MAX 32767
#define STICK_MIN -32768

int map[KEY_CNT];

void show_inputs(int fd, ssize_t n, struct input_event ev);
void emit(int fd, int type, int code, int val);
void emit_configured(int fd, int raw_fd, Deadzone deadzone, AntiDeadzone anti_deadzone);
int open_raw_device_and_hide_it(char* path);
int clamp_stick(long v);
int normalize_axis(int raw, int center, int min, int max);

void default_map()
{
	for (int i = 0; i < KEY_CNT; i++)
	{
		map[i] = i;
	}
	return;
}

int main (int argc, char *argv[]) 
{
	default_map();
	int option = 0;
	int count = get_controllers_and_store_them_in_array();
	
	if (count == 0)
	{
		printf("No controllers found.\n");
		return 1;
	}

	if (count > 1)
	{
		printf("What controller do you want to use?\n");
		for (int i = 0; i < arr_capacity; i++)
			printf("%d | %s\n", i, arr[i].name);

		scanf("%d", &option);
	}
	
	int fd = create_controller(arr[option].path);
	if (fd < 0)
	{
		perror("open");
		return 1;
	}
	
	int raw_fd = open_raw_device_and_hide_it(arr[option].path);
	if (raw_fd < 0)
	{
		perror("open");
		return 1;
	}

	for (int i = 0; i < arr_capacity; i++)
	{
		free(arr[i].name);
		free(arr[i].path);
	}
	free(arr);

	Deadzone deadzone = {0};
	AntiDeadzone anti_deadzone = {0};

	if (argc == 1) 
	{
		// read toml default config file
		toml_result_t result = toml_parse_file_ex("../default.toml");
		if (!result.ok)
		{
			printf("error when parsing toml file");
			toml_free(result);
			return 1;
		}
		
		// deadzone & anti-deadzone
		toml_datum_t dLS = toml_seek(result.toptab, "deadzone.radial_LS");
		toml_datum_t dRS = toml_seek(result.toptab, "deadzone.radial_RS");
		toml_datum_t adLS = toml_seek(result.toptab, "anti-deadzone.radial_LS");
		toml_datum_t adRS = toml_seek(result.toptab, "anti-deadzone.radial_RS");

		int64_t port_dLS = dLS.u.int64;
		int64_t port_dRS = dRS.u.int64; 

		int64_t port_adLS = adLS.u.int64; 
		int64_t port_adRS = adRS.u.int64; 

		deadzone.radial_LS = port_dLS;
		deadzone.radial_RS = port_dRS;

		anti_deadzone.radial_LS = port_adLS;
		anti_deadzone.radial_RS = port_adRS;

		// button remap
		toml_datum_t keys = toml_get(result.toptab, "remap");
		for (int i = 0; i < keys.u.tab.size; i++)
		{
			const char* name = keys.u.tab.key[i];
			toml_datum_t value = keys.u.tab.value[i];

			int pos = libevdev_event_code_from_name(EV_KEY, name);
			int val = libevdev_event_code_from_name(EV_KEY, value.u.s);

			map[pos] = val;
		}

		toml_free(result);
		emit_configured(fd, raw_fd, deadzone, anti_deadzone);
	}

	if (argc == 2)
	{
		// read toml custom config file
		toml_result_t result = toml_parse_file_ex(argv[1]);
		if (!result.ok)
		{
			printf("error when parsing toml file");
			toml_free(result);
			return 1;
		}

		// deadzone & anti-deadzone
		toml_datum_t dLS = toml_seek(result.toptab, "deadzone.radial_LS");
		toml_datum_t dRS = toml_seek(result.toptab, "deadzone.radial_RS");
		toml_datum_t adLS = toml_seek(result.toptab, "anti-deadzone.radial_LS");
		toml_datum_t adRS = toml_seek(result.toptab, "anti-deadzone.radial_RS");

		int64_t port_dLS = dLS.u.int64;
		int64_t port_dRS = dRS.u.int64; 

		int64_t port_adLS = adLS.u.int64; 
		int64_t port_adRS = adRS.u.int64; 

		deadzone.radial_LS = port_dLS;
		deadzone.radial_RS = port_dRS;

		anti_deadzone.radial_LS = port_adLS;
		anti_deadzone.radial_RS = port_adRS;

		// button remap
		toml_datum_t keys = toml_get(result.toptab, "remap");
		for (int i = 0; i < keys.u.tab.size; i++)
		{
			const char* name = keys.u.tab.key[i];
			toml_datum_t value = keys.u.tab.value[i];

			int pos = libevdev_event_code_from_name(EV_KEY, name);
			int val = libevdev_event_code_from_name(EV_KEY, value.u.s);

			map[pos] = val;
		}


		toml_free(result);
		emit_configured(fd, raw_fd, deadzone, anti_deadzone);
	}

	return 0;
}

int open_raw_device_and_hide_it(char* path)
{
	int raw_fd = open(path, O_RDONLY); 
	ioctl(raw_fd, EVIOCGRAB, 1); 
	if (raw_fd == -1) 
	{
		perror("open");
		return 1;
	}
	return raw_fd;
}	

int clamp_stick(long v)
{
	if (v > STICK_MAX) return STICK_MAX;
	if (v < STICK_MIN) return STICK_MIN;
	return (int)v;
}

void emit_configured(int fd, int raw_fd, Deadzone deadzone, AntiDeadzone A)
{
	ssize_t n;
	struct input_id id;
	struct input_event ev;
	struct input_event res;
	
	ioctl(raw_fd, EVIOCGID, &id);

	int LS_cached_x = 0;
	int LS_cached_y = 0;

	int RS_cached_x = 0;
	int RS_cached_y = 0;

	if (deadzone.radial_LS < 0) deadzone.radial_LS = 0;
	if (deadzone.radial_LS >= STICK_MAX) deadzone.radial_LS = STICK_MAX - 1;
	
	if (deadzone.radial_RS < 0) deadzone.radial_RS = 0;
	if (deadzone.radial_RS >= STICK_MAX) deadzone.radial_RS = STICK_MAX - 1;

	if (A.radial_LS < 0) A.radial_LS = 0;
	if (A.radial_LS > STICK_MAX) A.radial_LS = STICK_MAX;

	if (A.radial_RS < 0) A.radial_RS = 0;
	if (A.radial_RS > STICK_MAX) A.radial_RS = STICK_MAX;

	while (1)
	{
		res = read_input(raw_fd, n, ev);
		
		if (res.type == EV_KEY)
		{
			emit(fd, EV_KEY, map[res.code], res.value);
			emit(fd, EV_SYN, SYN_REPORT, 0);
		}
		if (res.type == EV_ABS)
		{
			// triggers and the dpad are not part of the stick vector,
			// so they pass through untouched
			if (res.code != ABS_X && res.code != ABS_Y && res.code != ABS_RX && res.code != ABS_RY)
			{
				emit(fd, EV_ABS, res.code, res.value);
				emit(fd, EV_SYN, SYN_REPORT, 0);
				continue;
			}

			if (id.vendor == SONY_VENDOR)
				res.value = normalize_axis(res.value, 128, 0, 255);

			if (res.code == ABS_X) LS_cached_x = res.value;
			if (res.code == ABS_Y) LS_cached_y = res.value;

			if (res.code == ABS_RX) RS_cached_x = res.value;
			if (res.code == ABS_RY) RS_cached_y = res.value;

			if (res.code == ABS_X || res.code == ABS_Y)
			{
				double LS_magnitude = sqrt((double)LS_cached_x * LS_cached_x + (double)LS_cached_y * LS_cached_y);
				int LS_out_x = 0;
				int LS_out_y = 0;

				if (LS_magnitude > deadzone.radial_LS)
				{
						double LS_scale = (LS_magnitude - deadzone.radial_LS) / (STICK_MAX - deadzone.radial_LS);
						if (LS_scale > 1.0) LS_scale = 1.0;
						LS_scale = (double)A.radial_LS / STICK_MAX
							+ (1.0 - (double)A.radial_LS / STICK_MAX) * LS_scale;

						LS_out_x = clamp_stick(lround((LS_cached_x / LS_magnitude) * LS_scale * STICK_MAX));
						LS_out_y = clamp_stick(lround((LS_cached_y / LS_magnitude) * LS_scale * STICK_MAX));
				}

				emit(fd, EV_ABS, ABS_X, LS_out_x);
				emit(fd, EV_ABS, ABS_Y, LS_out_y);
				emit(fd, EV_SYN, SYN_REPORT, 0);
			}

			if (res.code == ABS_RX || res.code == ABS_RY)
			{
				double RS_magnitude = sqrt((double)RS_cached_x * RS_cached_x + (double)RS_cached_y * RS_cached_y);
				int RS_out_x = 0;
				int RS_out_y = 0;

				if (RS_magnitude > deadzone.radial_RS)
				{
						double RS_scale = (RS_magnitude - deadzone.radial_RS) / (STICK_MAX - deadzone.radial_RS);
						if (RS_scale > 1.0) RS_scale = 1.0;
						RS_scale = (double)A.radial_RS / STICK_MAX
							+ (1.0 - (double)A.radial_RS / STICK_MAX) * RS_scale;

						RS_out_x = clamp_stick(lround((RS_cached_x / RS_magnitude) * RS_scale * STICK_MAX));
						RS_out_y = clamp_stick(lround((RS_cached_y / RS_magnitude) * RS_scale * STICK_MAX));
				}

				emit(fd, EV_ABS, ABS_RX, RS_out_x);
				emit(fd, EV_ABS, ABS_RY, RS_out_y);
				emit(fd, EV_SYN, SYN_REPORT, 0);
			}
		}
	}
}

int normalize_axis(int raw, int center, int min, int max)
{
	if (raw >= center)
		return (raw - center) * 32767 / (max - center);

	return (raw - center) * 32768 / (center - min);
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
