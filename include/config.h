#ifndef CONFIG_H
#define CONFIG_H

#include "logger.h"
#include "screencast_common.h"

struct config_screencast {
	char *output_name;
	double max_fps;
	char *exec_before;
	char *exec_after;
	char *chooser_cmd;
	enum xdpw_chooser_types chooser_type;
	bool force_mod_linear;
};

struct config_filechooser {
    char *cmd;
    char *default_dir;
};

struct xdpw_config {
	struct config_screencast screencast_conf;
	struct config_filechooser filechooser_conf;
};

void print_config(enum LOGLEVEL loglevel, struct xdpw_config *config);
void finish_config(struct xdpw_config *config);
void init_config(char ** const configfile, struct xdpw_config *config);

#endif
