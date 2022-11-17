#include <stdlib.h>

#ifdef __MORPHOS__

#    define ABDAY_1	8	/* Sun */
#    define ABDAY_2	9	/* Mon */
#    define ABDAY_3	10	/* Tue */
#    define ABDAY_4	11	/* Wed */
#    define ABDAY_5	12	/* Thu */
#    define ABDAY_6	13	/* Fri */
#    define ABDAY_7	14	/* Sat */

#    define MON_1		15	/* January   */
#    define MON_2		16	/* February  */
#    define MON_3		17	/* March     */
#    define MON_4		18	/* April     */
#    define MON_5		19	/* May	     */
#    define MON_6		20	/* June      */
#    define MON_7		21	/* July      */
#    define MON_8		22	/* August    */
#    define MON_9		23	/* September */
#    define MON_10		24	/* October   */
#    define MON_11		25	/* November  */
#    define MON_12		26	/* December  */

#    define ABMON_1	27	/* Jan */
#    define ABMON_2	28	/* Feb */
#    define ABMON_3	29	/* Mar */
#    define ABMON_4	30	/* Apr */
#    define ABMON_5	31	/* May */
#    define ABMON_6	32	/* Jun */
#    define ABMON_7	33	/* Jul */
#    define ABMON_8	34	/* Aug */
#    define ABMON_9	35	/* Sep */
#    define ABMON_10	36	/* Oct */
#    define ABMON_11	37	/* Nov */
#    define ABMON_12	38	/* Dec */

#    define AM_STR		41	/* AM */
#    define PM_STR		42	/* PM */

#    include "langinfo.c"

#else
#    include <langinfo.h>
#endif

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>

char *strptime(const char *restrict s, const char *restrict f, struct tm *restrict tm)
{
	int i, w, neg, adj, min, range, *dest, dummy;
	const char *ex;
	size_t len;
	int want_century = 0, century = 0, relyear = 0;
	while (*f) {
		if (*f != '%') {
			if (isspace(*f)) for (; *s && isspace(*s); s++);
			else if (*s != *f) return 0;
			else s++;
			f++;
			continue;
		}
		f++;
		if (*f == '+') f++;
		if (isdigit(*f)) {
			char *new_f;
			w=strtoul(f, &new_f, 10);
			f = new_f;
		} else {
			w=-1;
		}
		adj=0;
		switch (*f++) {
		case 'a': case 'A':
			dest = &tm->tm_wday;
			min = ABDAY_1;
			range = 7;
			goto symbolic_range;
		case 'b': case 'B': case 'h':
			dest = &tm->tm_mon;
			min = ABMON_1;
			range = 12;
			goto symbolic_range;
		case 'c':
			s = strptime(s, nl_langinfo(D_T_FMT), tm);
			if (!s) return 0;
			break;
		case 'C':
			dest = &century;
			if (w<0) w=2;
			want_century |= 2;
			goto numeric_digits;
		case 'd': case 'e':
			dest = &tm->tm_mday;
			min = 1;
			range = 31;
			goto numeric_range;
		case 'D':
			s = strptime(s, "%m/%d/%y", tm);
			if (!s) return 0;
			break;
		case 'H':
			dest = &tm->tm_hour;
			min = 0;
			range = 24;
			goto numeric_range;
		case 'I':
			dest = &tm->tm_hour;
			min = 1;
			range = 12;
			goto numeric_range;
		case 'j':
			dest = &tm->tm_yday;
			min = 1;
			range = 366;
			adj = 1;
			goto numeric_range;
		case 'm':
			dest = &tm->tm_mon;
			min = 1;
			range = 12;
			adj = 1;
			goto numeric_range;
		case 'M':
			dest = &tm->tm_min;
			min = 0;
			range = 60;
			goto numeric_range;
		case 'n': case 't':
			for (; *s && isspace(*s); s++);
			break;
		case 'p':
			ex = nl_langinfo(AM_STR);
			len = strlen(ex);
			if (!strncasecmp(s, ex, len)) {
				tm->tm_hour %= 12;
				s += len;
				break;
			}
			ex = nl_langinfo(PM_STR);
			len = strlen(ex);
			if (!strncasecmp(s, ex, len)) {
				tm->tm_hour %= 12;
				tm->tm_hour += 12;
				s += len;
				break;
			}
			return 0;
		case 'r':
			s = strptime(s, nl_langinfo(T_FMT_AMPM), tm);
			if (!s) return 0;
			break;
		case 'R':
			s = strptime(s, "%H:%M", tm);
			if (!s) return 0;
			break;
		case 'S':
			dest = &tm->tm_sec;
			min = 0;
			range = 61;
			goto numeric_range;
		case 'T':
			s = strptime(s, "%H:%M:%S", tm);
			if (!s) return 0;
			break;
		case 'U':
		case 'W':
			/* Throw away result, for now. (FIXME?) */
			dest = &dummy;
			min = 0;
			range = 54;
			goto numeric_range;
		case 'w':
			dest = &tm->tm_wday;
			min = 0;
			range = 7;
			goto numeric_range;
		case 'x':
			s = strptime(s, nl_langinfo(D_FMT), tm);
			if (!s) return 0;
			break;
		case 'X':
			s = strptime(s, nl_langinfo(T_FMT), tm);
			if (!s) return 0;
			break;
		case 'y':
			dest = &relyear;
			w = 2;
			want_century |= 1;
			goto numeric_digits;
		case 'Y':
			dest = &tm->tm_year;
			if (w<0) w=4;
			adj = 1900;
			want_century = 0;
			goto numeric_digits;
		case '%':
			if (*s++ != '%') return 0;
			break;
		default:
			return 0;
		numeric_range:
			if (!isdigit(*s)) return 0;
			*dest = 0;
			for (i=1; i<=min+range && isdigit(*s); i*=10)
				*dest = *dest * 10 + *s++ - '0';
			if (*dest - min >= (unsigned)range) return 0;
			*dest -= adj;
			switch((char *)dest - (char *)tm) {
			case offsetof(struct tm, tm_yday):
				;
			}
			goto update;
		numeric_digits:
			neg = 0;
			if (*s == '+') s++;
			else if (*s == '-') neg=1, s++;
			if (!isdigit(*s)) return 0;
			for (*dest=i=0; i<w && isdigit(*s); i++)
				*dest = *dest * 10 + *s++ - '0';
			if (neg) *dest = -*dest;
			*dest -= adj;
			goto update;
		symbolic_range:
			for (i=2*range-1; i>=0; i--) {
				ex = nl_langinfo(min+i);
				len = strlen(ex);
				if (strncasecmp(s, ex, len)) continue;
				s += len;
				*dest = i % range;
				break;
			}
			if (i<0) return 0;
			goto update;
		update:
			//FIXME
			;
		}
	}
	if (want_century) {
		tm->tm_year = relyear;
		if (want_century & 2) tm->tm_year += century * 100 - 1900;
		else if (tm->tm_year <= 68) tm->tm_year += 100;
	}
	return (char *)s;
}
