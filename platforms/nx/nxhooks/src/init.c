#include "nxhooks/init.h"
#include "nxhooks/sdl/input.h"
#include <switch.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

static FILE* g_logfile = NULL;
static char g_initial_directory_name[PATH_MAX+1];

void nxHooksInit() {
	Result rc = socketInitializeDefault();
	int stdioRes = -1;

	if(R_SUCCEEDED(rc)) {
		stdioRes = nxlinkStdio();
	}

	if(stdioRes != 0) {
		g_logfile = fopen("milkytracker.log", "w");
		if(g_logfile != NULL) {
			fflush(stdout);
			dup2(fileno(g_logfile), STDOUT_FILENO);
			fflush(stderr);
			dup2(fileno(g_logfile), STDERR_FILENO);
		}
		printf("[nxhooks] nxlink failed with res: %d\n", stdioRes);
	}

	if(R_FAILED(rc)) {
		printf("[nxhooks] socketInitialize failed with rc: %x\n", rc);
	}

	rc = appletInitializeGamePlayRecording();
	if(R_FAILED(rc)) {
		printf("[nxhooks] appletInitializeGamePlayRecording failed with rc: %x\n", rc);
	}
	appletSetGamePlayRecordingState(1);

	memset(g_initial_directory_name, 0, sizeof(g_initial_directory_name));
	getcwd(g_initial_directory_name, sizeof(g_initial_directory_name));
	printf("[nxhooks] initial_directory_name: %s\n", g_initial_directory_name);

	nxHooksSDLInputInit();
}

void nxHooksExit() {
	nxHooksSDLInputExit();

	socketExit();
	if(g_logfile != NULL) {
		fclose(g_logfile);
	}
}

char* nxHooksGetInitialDirectoryName() {
	return g_initial_directory_name;
}