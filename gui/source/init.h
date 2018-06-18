#ifndef INITIALIZE_H
#define INITIALIZE_H

extern char init_textOnScreen[256];

extern int initdbox_setWaitTime;

extern bool initDone;

/**
 * Play DS boot splash.
 */
void initStuff(void);

#endif /* INITIALIZE_H */
