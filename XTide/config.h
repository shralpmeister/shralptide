/********************************************************/
/************ STUFF YOU MIGHT WANT TO CHANGE ************/
/********************************************************/

/* Default location and name of harmonics file */
/* This is overridden by the HFILE environment variable or by the -hfile
   switch. */
#ifdef OS2
#define hfile "harmonic"
#else
#define hfile "harmonics"
#endif

/* Default location to show tides for */
/* This is overridden by the LOCATION environment variable or by the
   -location switch. */
#define deflocation "Baltimore (Fort McHenry)"

/* System default config file, read only if neither ~/.[x]tiderc nor
   -config is present.  sysconfig is for tide, xsysconfig is for xtide. */
#define sysconfig "/usr/lib/tiderc"
#define xsysconfig "/usr/lib/xtiderc"

/* This gives the location of the compiled java classes tide.class and
   xtide.class.  You should change it to point to your local copies if
   you use Java. */
#define CODEBASE "http://www.universe.digex.net/~dave/xtide/"

/* Define this to enable Dean's GIF code.  Requires Tom Boutell's
   GD gif-manipulating library, which is available from
   http://www.boutell.com/gd/
*/
/* #undef DEANGIF */

/*****************************************************************/
/************ STUFF YOU PROBABLY SHOULDN'T MESS WITH *************/
/*****************************************************************/

#define VERSION "1.6"
#define PATCHLEVEL 2

/* Define NO_LUDICROUS_SPEED to disable Jeff Dairiki's iterative
   approximation code.  With NO_LUDICROUS_SPEED, you get the old tried
   and true brute force code.

   If using Jeff's code, you can also define DO_BISECTION to back
   down to his merely warp-speed bisection code instead of the fancy
   modified Newton-Raphson root finder.
*/
/* #define NO_LUDICROUS_SPEED */
/* #undef DO_BISECTION */

/* Silly option to use /dev/urandom for -location random.  AFAIK, only
   Linux supports /dev/random and /dev/urandom.  Enabling this option
   will degrade the security of TCP/IP and will run down the supply of
   random numbers that may be needed by more important applications. */
/* #define URANDOM */
