#include "config.h"
#include "xdpw.h"
#include "logger.h"
#include "screencast_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ini.h>

#define FILECHOOSER_DEFAULT_CMD "/usr/share/xdg-desktop-portal-wlr/lf-wrapper.sh"
#define FILECHOOSER_DEFAULT_DIR "/tmp"

void print_config(enum LOGLEVEL loglevel, struct xdpw_config *config) {
	logprint(loglevel, "config: outputname:  %s", config->screencast_conf.output_name);
	logprint(loglevel, "config: max_fps:  %f", config->screencast_conf.max_fps);
	logprint(loglevel, "config: exec_before:  %s", config->screencast_conf.exec_before);
	logprint(loglevel, "config: exec_after:  %s", config->screencast_conf.exec_after);
	logprint(loglevel, "config: chooser_cmd: %s", config->screencast_conf.chooser_cmd);
	logprint(loglevel, "config: chooser_type: %s", chooser_type_str(config->screencast_conf.chooser_type));
	logprint(loglevel, "config: force_mod_linear: %d", config->screencast_conf.force_mod_linear);
}

// NOTE: calling finish_config won't prepare the config to be read again from config file
// with init_config since to pointers and other values won't be reset to NULL, or 0
void finish_config(struct xdpw_config *config) {
	logprint(DEBUG, "config: destroying config");

	// screencast
	free(config->screencast_conf.output_name);
	free(config->screencast_conf.exec_before);
	free(config->screencast_conf.exec_after);
	free(config->screencast_conf.chooser_cmd);
    free(config->filechooser_conf.cmd);
    free(config->filechooser_conf.default_dir);
}

static void parse_string(char **dest, const char* value) {
	if (value == NULL || *value == '\0') {
		logprint(TRACE, "config: skipping empty value in config file");
		return;
	}
	free(*dest);
	*dest = strdup(value);
}

static void parse_double(double *dest, const char* value) {
	if (value == NULL || *value == '\0') {
		logprint(TRACE, "config: skipping empty value in config file");
		return;
	}
	*dest = strtod(value, (char**)NULL);
}

static void parse_bool(bool *dest, const char* value) {
	if (value == NULL || *value == '\0') {
		logprint(TRACE, "config: skipping empty value in config file");
		return;
	}
	if (strcmp(value, "1") == 0) {
		*dest = true;
	} else {
		*dest = false;
	}
}

static int handle_ini_screencast(struct config_screencast *screencast_conf, const char *key, const char *value) {
	if (strcmp(key, "output_name") == 0) {
		parse_string(&screencast_conf->output_name, value);
	} else if (strcmp(key, "max_fps") == 0) {
		parse_double(&screencast_conf->max_fps, value);
	} else if (strcmp(key, "exec_before") == 0) {
		parse_string(&screencast_conf->exec_before, value);
	} else if (strcmp(key, "exec_after") == 0) {
		parse_string(&screencast_conf->exec_after, value);
	} else if (strcmp(key, "chooser_cmd") == 0) {
		parse_string(&screencast_conf->chooser_cmd, value);
	} else if (strcmp(key, "chooser_type") == 0) {
		char *chooser_type = NULL;
		parse_string(&chooser_type, value);
		screencast_conf->chooser_type = get_chooser_type(chooser_type);
		free(chooser_type);
	} else if (strcmp(key, "force_mod_linear") == 0) {
		parse_bool(&screencast_conf->force_mod_linear, value);
	} else {
		logprint(TRACE, "config: skipping invalid key in config file");
		return 0;
	}
	return 1;
}

static int handle_ini_filechooser(struct config_filechooser *filechooser_conf, const char *key, const char *value) {
    if (strcmp(key, "cmd") == 0) {
        parse_string(&filechooser_conf->cmd, value);
    } else if (strcmp(key, "default_dir") == 0) {
        parse_string(&filechooser_conf->default_dir, value);
    } else {
        logprint(TRACE, "config: skipping invalid key in config file");
        return 0;
    }
    return 1;
}

static int handle_ini_config(void *data, const char* section, const char *key, const char *value) {
	struct xdpw_config *config = (struct xdpw_config*)data;
	logprint(TRACE, "config: parsing setction %s, key %s, value %s", section, key, value);

	if (strcmp(section, "screencast") == 0) {
		return handle_ini_screencast(&config->screencast_conf, key, value);
	}

    if (strcmp(section, "filechooser") == 0) {
        return handle_ini_filechooser(&config->filechooser_conf, key, value);
    }

	logprint(TRACE, "config: skipping invalid key in config file");
	return 0;
}

static void default_config(struct xdpw_config *config) {
	config->screencast_conf.max_fps = 0;
	config->screencast_conf.chooser_type = XDPW_CHOOSER_DEFAULT;

    size_t size = snprintf(NULL, 0, "%s", FILECHOOSER_DEFAULT_CMD) + 1;
    config->filechooser_conf.cmd = malloc(size);
    snprintf(config->filechooser_conf.cmd, size, "%s", FILECHOOSER_DEFAULT_CMD);
    size = snprintf(NULL, 0, "%s", FILECHOOSER_DEFAULT_DIR) + 1;
    config->filechooser_conf.default_dir = malloc(size);
    snprintf(config->filechooser_conf.default_dir, size, "%s", FILECHOOSER_DEFAULT_DIR);
}

static bool file_exists(const char *path) {
	return path && access(path, R_OK) != -1;
}

static char *config_path(const char *prefix, const char *filename) {
	if (!prefix || !prefix[0] || !filename || !filename[0]) {
		return NULL;
	}

	char *config_folder = "xdg-desktop-portal-wlr";

	size_t size = 3 + strlen(prefix) + strlen(config_folder) + strlen(filename);
	char *path = calloc(size, sizeof(char));
	snprintf(path, size, "%s/%s/%s", prefix, config_folder, filename);
	return path;
}

static char *get_config_path(void) {
	const char *home = getenv("HOME");
	char *config_home_fallback = NULL;
	if (home != NULL && home[0] != '\0') {
		size_t size_fallback = 1 + strlen(home) + strlen("/.config");
		config_home_fallback = calloc(size_fallback, sizeof(char));
		snprintf(config_home_fallback, size_fallback, "%s/.config", home);
	}

	const char *config_home = getenv("XDG_CONFIG_HOME");
	if (config_home == NULL || config_home[0] == '\0') {
		config_home = config_home_fallback;
	}

	const char *prefix[2];
	prefix[0] = config_home;
	prefix[1] = SYSCONFDIR "/xdg";

	const char *xdg_current_desktop = getenv("XDG_CURRENT_DESKTOP");
	const char *config_fallback = "config";

	char *config_list = NULL;
	for (size_t i = 0; i < 2; i++) {
		if (xdg_current_desktop) {
			config_list = strdup(xdg_current_desktop);
			char *config = strtok(config_list, ":");
			while (config) {
				char *path = config_path(prefix[i], config);
				if (!path) {
					config = strtok(NULL, ":");
					continue;
				}
				logprint(TRACE, "config: trying config file %s", path);
				if (file_exists(path)) {
					free(config_list);
					free(config_home_fallback);
					return path;
				}
				free(path);
				config = strtok(NULL, ":");
			}
			free(config_list);
		}
		char *path = config_path(prefix[i], config_fallback);
		if (!path) {
			continue;
		}
		logprint(TRACE, "config: trying config file %s", path);
		if (file_exists(path)) {
			free(config_home_fallback);
			return path;
		}
		free(path);
	}

	free(config_home_fallback);
	return NULL;
}

void init_config(char ** const configfile, struct xdpw_config *config) {
	if (*configfile == NULL) {
		*configfile = get_config_path();
	}

	default_config(config);
	if (*configfile == NULL) {
		logprint(INFO, "config: no config file found, using the default config");
		return;
	}
	if (ini_parse(*configfile, handle_ini_config, config) < 0) {
		logprint(ERROR, "config: unable to load config file %s", *configfile);
	}
}
