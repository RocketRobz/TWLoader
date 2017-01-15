#ifndef PTMU_X_H
#define PTMU_X_H

// ptm:u functions not available in ctrulib.

#ifdef __cplusplus
extern "C" {
#endif

Result ptmuxInit(void);

void ptmuxExit(void);

Result PTMUX_GetAdapterState(u8 *out);

#ifdef __cplusplus
}
#endif

#endif /* PTMU_X_H */
