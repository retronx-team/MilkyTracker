#include <switch.h>
#include <stdio.h>
#include <unistd.h>

static FILE* g_logfile = NULL;
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
		printf("nxhooks: nxlink failed with res: %d\n", stdioRes);
	}

	if(R_FAILED(rc)) {
		printf("nxhooks: socketInitialize failed with rc: %x\n", rc);
	}

	rc = appletInitializeGamePlayRecording();
	if(R_FAILED(rc)) {
		printf("nxhooks: appletInitializeGamePlayRecording failed with rc: %x\n", rc);
	}
	appletSetGamePlayRecordingState(1);
}

void nxHooksExit() {
	socketExit();
	if(g_logfile != NULL) {
		fclose(g_logfile);
	}
}