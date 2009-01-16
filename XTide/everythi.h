/* everything.h - external declarations for everything */

/* Include everything, all the time.  This saves _me_ time. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include "config.h"
#include "arglib.h"

/* Compatibility by Dale DePriest */

#ifdef OS2
#include <float.h>
#endif /* OS2 */

#define linelen 3000

#define MAX(a,b) (((a)<(b))?(b):(a))

/* Typo prevention */
#define DAYSECONDS 86400
#define HOURSECONDS 3600

/* Width of graph in ASCII graph mode */
#define TEXTWIDTH 73
/* Tstep adjustment for ASCII graph mode */
#define AGTSTEP 20
/* Tstep adjustment for banner mode */
#define BANTSTEP 10

/* Handle compilers that are anal about the type of signal handlers. */
#ifdef __SUNPRO_CC
#define SIGHANDTYPE void(*)(int)
#endif

enum tideerr {BADCOLORSPEC, WANTMOREARGS, CONFIGFAIL,
  BADINTEGER, BADFLOAT, BADHHMM, STRETCHTOOSMALL, STRETCHTOOBIG,
  BADTIMESTAMP, MISSINGYEAR, BADGEOMETRY, CANTOPENDISPLAY, CANTOPENFILE,
  OFFSETSTEXTONLY, BADMULT, NODEANGIF, MAXBELOWMIN};

/* Units stuff */

typedef enum {LENGTH, VELOCITY, BOGUS} unit_type;
typedef struct {
  char *name;
  char *abbrv;
  unit_type type;
  double conv_factor;
} unit;
#define NUMUNITS 4

extern void sigabort ();  /* Leave out parms for portability */
extern int hhmm2seconds (char *hhmm);
extern void load_data (void);
extern void check_epoch (void);
extern time_t parse_time_string (char *time_string);
extern int yearoftimet (time_t t);
extern void happy_new_year (int new_year);
extern double time2tide (time_t t);
extern double time2atide (time_t);
extern double time2secondary (time_t t);
extern double time2asecondary (time_t);
extern void do_timestamp (char buf[20], struct tm *t);
extern struct tm *tmtime (time_t t);
extern int update_high_tide (void);
extern void list_tides (void);
extern void tide2ascii (void);
extern void tide2ppm (char *);
extern time_t prev_hour (time_t);
extern time_t increment_hour (time_t);
extern time_t prev_day (time_t);
extern time_t increment_day (time_t);
extern void change_time_zone (char *);
extern void barf (enum tideerr);
extern char *stradoop (char *);
extern void set_epoch (int, int, int);
extern void do_datestamp (char *, struct tm *);
extern char *nojunk (char *);
extern char *do_long_timestamp (struct tm *);
extern time_t sunday_month (time_t);
extern void fudge_constituents (char *);
extern void tide2gif (char *);
extern int linterp (int, int, double);
extern int findunit (char *);
extern void make_depth_caption (int *, int, char *);
extern char *seconds2hhmm (int);
extern void allocate_epochs ();
extern void allocate_nodes ();
extern void skipnl (FILE *fp);
extern int next_line (FILE *fp, char linrec[linelen], int end_ok);
extern void free_nodes();
extern void free_epochs();
extern void free_cst();

extern char hfile_name[MAXARGLEN+1], location[MAXARGLEN*2], *ppm, *gif,
  tzfile[MAXARGLEN+1], *geometry, units[MAXARGLEN+1],
  units_abbrv[MAXARGLEN+1], last_location[MAXARGLEN*2];
extern char next_ht_text[20], next_ht_date[20], next_lt_text[20],
  next_lt_date[20];
extern int tadjust, utc, list, checkyear, text, skinny, now, graphmode,
  httimeoff, lttimeoff, tstep, mark, middle, mllw, lines, PPMWIDTH,
  PPMHEIGHT, hinc, tinc, loctz, iscurrent, curonly, toplines, hincmagic,
  calendar, banner, weekday, hairy, linegraph, noampm, uutc, java,
  javanh, hlevelmult, llevelmult, have_offsets;
extern time_t next_ht, prev_ht, next_ht_adj, prev_ht_adj, faketime, epoch;
extern double amplitude, htleveloff, ltleveloff, DATUM, marklev,
  absmax, absmin, fakedatum, fakeamplitude;
extern unit known_units[NUMUNITS];
extern int num_csts, num_nodes, num_epochs, first_year;
double *cst_speeds, **cst_epochs, **cst_nodes;
