#ifndef STRPTIME_H
#define STRPTIME_H

#include <time.h>

char* strptime(const char *buf, const char *fmt, struct tm *tm);

#endif // STRPTIME_H
