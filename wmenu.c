#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>

#include "menu.h"
#include "wayland.h"

static void read_items(struct menu *menu) {
	char buf[sizeof menu->input];
	while (fgets(buf, sizeof buf, stdin)) {
		char *p = strchr(buf, '\n');
		if (p) {
			*p = '\0';
		}
		menu_add_item(menu, strdup(buf));
	}
}

static void print_item(struct menu *menu, char *text, bool exit) {
	puts(text);
	fflush(stdout);
	if (exit) {
		menu->exit = true;
	}
}

int main(int argc, char *argv[]) {
	struct menu *menu = menu_create(print_item);
	menu_getopts(menu, argc, argv);
	if (menu->single_instance) {
		// the following code was copied from `bemenu`, thanks.
		char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");

	        char buffer[1024];
		char *menu_lock_location = (xdg_runtime_dir == NULL ? "/tmp" : xdg_runtime_dir);
	        snprintf(buffer, sizeof(buffer), "%s/wmenu.lock", menu_lock_location);

		// Create a read-write lock file(if not already exists),
		// closes on exec. Creates in tmp due to permissions.
		int menu_lock = open(buffer, O_CREAT | O_RDWR | O_CLOEXEC, 0666);
		// If we cannot set the lock/the lock is already present.
		if (flock(menu_lock, LOCK_EX | LOCK_NB) == -1) {
		    fprintf(stderr, "\"%s\" instance is already running\n", argv[0]);
		    exit(EXIT_FAILURE);
		}
	    }
	read_items(menu);
	int status = menu_run(menu);
	menu_destroy(menu);
	return status;
}
