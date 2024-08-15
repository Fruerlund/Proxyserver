#ifndef POLLHANDLING_H_
#define POLLHANDLING_H_

#include "../headers/main.h"

void poll_CLIENT_TO_TARGET(listener_h *, int, char *);
void poll_TARGET_TO_CLIENT(listener_h *, int, char *);
void *pooling(listener_h *);

#endif