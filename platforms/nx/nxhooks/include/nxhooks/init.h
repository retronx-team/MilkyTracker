#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void nxHooksInit();
void nxHooksExit();
char* nxHooksGetInitialDirectoryName();

#ifdef __cplusplus
}
#endif