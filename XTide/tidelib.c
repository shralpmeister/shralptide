/*  Tidelib  Collection of things needed by both tide and xtide,
      including the central tide prediction functions.
    Last modified 1997-08-28

    This program uses the harmonic method to compute tide levels.
    All of the data and constants are read in from the harmonics file.
    Please refer to README for more information.


    Copyright (C) 1997  David Flater.
    Also starring:  Dale DePriest; Dean Pentcheff; Jeff Dairiki.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


    The tide prediction algorithm used in this program was developed
    with United States Government funding, so no proprietary rights
    can be attached to it.  For more information, refer to the
    following publications:

    Manual of Harmonic Analysis and Prediction of Tides.  Special
    Publication No. 98, Revised (1940) Edition.  United States
    Government Printing Office, 1941.

    Computer Applications to Tides in the National Ocean Survey.
    Supplement to Manual of Harmonic Analysis and Prediction of Tides
    (Special Publication No. 98).  National Ocean Service, National
    Oceanic and Atmospheric Administration, U.S. Department of
    Commerce, January 1982.

*/

#include "everythi.h"

/* Exported variables */
char hfile_name[MAXARGLEN+1], location[MAXARGLEN*2], *ppm = NULL,
*gif = NULL, tzfile[MAXARGLEN+1], units[MAXARGLEN+1],
units_abbrv[MAXARGLEN+1], last_location[MAXARGLEN*2];
char next_ht_text[20], next_ht_date[20], next_lt_text[20],
next_lt_date[20], *geometry = NULL;
char *fgrise_color_arg = NULL, *fgfall_color_arg = NULL,
*bg_color_arg = NULL, *fgtext_color_arg = NULL, *fgmark_color_arg = NULL,
*fgmllw_color_arg = NULL, *fgmiddle_color_arg = NULL;
int tadjust = 0, utc = 0, list = 0, checkyear = 0, text = 0, skinny = 0,
now = 0, graphmode = 0, httimeoff = 0, lttimeoff = 0, tstep = 180,
middle = 0, mark = 0, mllw = 0, lines = 1, PPMWIDTH = 960,
PPMHEIGHT = 300, hinc = 0, tinc = 0, loctz = 0, iscurrent = 0,
curonly = 0, toplines = 0, hincmagic = 0, calendar = 0, banner = 0,
weekday = 0, hairy = 0, linegraph = 0, ps = 0, noampm = 0, uutc = 0,
java = 0, llevelmult = 0, hlevelmult = 0, javanh = 0,
have_offsets = 0;
time_t next_ht = 0, prev_ht = 0, next_ht_adj = 0,
prev_ht_adj = 0, faketime = 0, epoch = 0;
double amplitude = 0.0, htleveloff = 0.0, ltleveloff = 0.0, DATUM, marklev,
absmax = 0.0, absmin = 0.0, fakedatum = 0.0, fakeamplitude = 0.0, DATUM_LAST;

unit known_units[NUMUNITS] = {{"feet", "ft", LENGTH, 0.3048},
{"meters", "m", LENGTH, 1.0},
{"knots", "kt", VELOCITY, 1.0},
{"knots^2", "kt^2", BOGUS, 1.0}};

/* Local variables */

static int year = 0, meridian = 0;

/* Tide-prediction storage -- cst == constituent */

int num_csts = 0, num_nodes = 0, num_epochs = 0, first_year = 0;
double *cst_speeds, **cst_epochs, **cst_nodes, *loc_amp, *loc_epoch,
*work, *loc_amp_last, *loc_epoch_last;


void init() {
	tadjust = 0; utc = 0; list = 0; checkyear = 0; text = 0; skinny = 0;
	now = 0; graphmode = 0; httimeoff = 0; lttimeoff = 0; tstep = 180;
	middle = 0; mark = 0; mllw = 0; lines = 1; PPMWIDTH = 960;
	PPMHEIGHT = 300; hinc = 0; tinc = 0; loctz = 0; iscurrent = 0;
	curonly = 0; toplines = 0; hincmagic = 0; calendar = 0; banner = 0;
	weekday = 0; hairy = 0; linegraph = 0; ps = 0; noampm = 0; uutc = 0;
	java = 0; llevelmult = 0; hlevelmult = 0; javanh = 0;
	have_offsets = 0;
	next_ht = 0; prev_ht = 0; next_ht_adj = 0;
	prev_ht_adj = 0; faketime = 0; epoch = 0;
	amplitude = 0.0; htleveloff = 0.0; ltleveloff = 0.0;
	absmax = 0.0; absmin = 0.0; fakedatum = 0.0; fakeamplitude = 0.0;

	/* Local variables */

	year = 0; //meridian = 0;

	/* Tide-prediction storage -- cst == constituent */

	num_csts = 0; num_nodes = 0; num_epochs = 0; first_year = 0;
}
/* Exit without core dump on assertion failures. */
void
sigabort ()
{
  exit (-1);
}

/* Avoid portability problems with strdup. */
char *
stradoop (char *src)
{
  char *dest = NULL;
  if (src) {
    assert (dest = (char *) malloc ((int)(strlen (src)) + 1));
    strcpy (dest, src);
  }
  return dest;
}

/* User-friendly error messages */
void
barf (enum tideerr err)
{
  fprintf (stderr, "Tidelib Fatal Error:  ");
  switch (err) {
  case BADCOLORSPEC:
    fprintf (stderr, "BADCOLORSPEC\\n");
    fprintf (stderr,
#ifdef DEANGIF
"One of your specified colors is not understood.  This may be because it is\\nnot in rgb.txt, or because you used the wrong syntax.  In PPM/GIF mode, colors\\nmust be specified as rgb:hh/hh/hh, where hh is a 2-digit hexadecimal number.\\nOther formats are possible under X; type 'man X' for an explanation of them.\\n");
#else
"One of your specified colors is not understood.  This may be because it is\\nnot in rgb.txt, or because you used the wrong syntax.  In PPM mode, colors\\nmust be specified as rgb:hh/hh/hh, where hh is a 2-digit hexadecimal number.\\nOther formats are possible under X; type 'man X' for an explanation of them.\\n");
#endif
    break;
  case BADGEOMETRY:
    fprintf (stderr, "BADGEOMETRY\\n");
    fprintf (stderr,
#ifdef DEANGIF
"In PPM/GIF mode, geometry must be specified as WxH, where W and H are width\\nand height in pixels respectively.  Other formats are possible under X; type\\n'man X' for an explanation of them.  This error is also generated if the\\ngeometry that you specify is too small or too large in one or both directions.\\n");
#else
"In PPM mode, geometry must be specified as WxH, where W and H are width\\nand height in pixels respectively.  Other formats are possible under X; type\\n'man X' for an explanation of them.  This error is also generated if the\\ngeometry that you specify is too small or too large in one or both directions.\\n");
#endif
    break;
  case CANTOPENDISPLAY:
    fprintf (stderr, "CANTOPENDISPLAY\\n");
    fprintf (stderr,
"Can't open the X display.  Check the setting of the DISPLAY environment\\nvariable or the value supplied with the -display switch.  You may also need\\nto fix your X authorities with xhost or xauth.\\n");
    break;
  case WANTMOREARGS:
    fprintf (stderr, "WANTMOREARGS\\n");
    fprintf (stderr,
      "Your last switch wants an argument that you did not provide.\\n");
    break;
  case CONFIGFAIL:
    fprintf (stderr, "CONFIGFAIL\\n");
    perror ("fopen");
    fprintf (stderr,
      "Could not open the config file that you specified.\\n");
    break;
  case CANTOPENFILE:
    fprintf (stderr, "CANTOPENFILE\\n");
    perror ("fopen");
    fprintf (stderr,
      "Could not open a file.\\n");
    break;
  case BADINTEGER:
    fprintf (stderr, "BADINTEGER\\n");
    fprintf (stderr,
      "One of your switches wants an integer but didn't get one.\\n");
    break;
  case BADFLOAT:
    fprintf (stderr, "BADFLOAT\\n");
    fprintf (stderr,
  "One of your switches wants a floating point number but didn't get one.\\n");
    break;
  case BADMULT:
    fprintf (stderr, "BADMULT\\n");
    fprintf (stderr, "Multiplicative offsets must be greater than zero.\\n");
    break;
  case BADHHMM:
    fprintf (stderr, "BADHHMM\\n");
    fprintf (stderr,
"Offset must be of the form [-]HH:MM where HH is hours and MM is minutes.\\n"
    );
    break;
  case STRETCHTOOSMALL:
    fprintf (stderr, "STRETCHTOOSMALL\\n");
    fprintf (stderr, "-gstretch needs a positive value.\\n");
    break;
  case STRETCHTOOBIG:
    fprintf (stderr, "STRETCHTOOBIG\\n");
    fprintf (stderr, "-gstretch wants a smaller number.\\n");
    break;
  case BADTIMESTAMP:
    fprintf (stderr, "BADTIMESTAMP\\n");
    fprintf (stderr,
"-gstart, -raw, or -stats wants a timestamp of the form YYYY:MM:DD:HH:MM.\\nExample -- half past midnight, June 1, 1995:  1995:06:01:00:30.  This error\\nmay also be raised if you specify a non-existent time, such as February 31st\\nor the hour that is skipped because of Daylight Savings Time, or go outside\\nthe acceptable range of dates (i.e. before 1970 or after 2037 on most Unix\\nmachines).\\n");
    break;
  case MISSINGYEAR:
    fprintf (stderr, "MISSINGYEAR\\n");
    fprintf (stderr,
"In order to calculate tides for a given year, the node factors and\\nequilibrium arguments for that year must be in the harmonics file.  The\\nharmonics file that you are using only supports the following years:\\n");
    if (num_nodes < num_epochs)
      num_epochs = num_nodes;
    fprintf (stderr, "    %d through %d\\n", first_year,
      first_year+num_epochs-1);
    break;
  case OFFSETSTEXTONLY:
    fprintf (stderr, "OFFSETSTEXTONLY\\n");
    fprintf (stderr,
"XTide cannot apply different offsets to high and low tides in PostScript,\\nJava, or stats modes, or in combination with -mark.\\n");
    break;
  case NODEANGIF:
    fprintf (stderr, "NODEANGIF\\n");
    fprintf (stderr,
"This XTide binary was compiled without GIF support.  You can get a GIF by\\ndoing xtide -ppm - | ppmquant 256 | ppmtogif.\\n");
    break;
  case MAXBELOWMIN:
    fprintf (stderr, "MAXBELOWMIN\\n");
    fprintf (stderr, "Your offsets are unreasonable for this data set.\\n");
    break;
  default:
    assert (0);
  }
  exit (-1);
}

void
allocate_local_cst()
{
	assert (loc_amp = (double *) malloc (num_csts * sizeof (double)));
	assert (loc_amp_last = (double *) malloc (num_csts * sizeof (double)));
	assert (loc_epoch = (double *) malloc (num_csts * sizeof (double)));
	assert (loc_epoch_last = (double *) malloc (num_csts * sizeof (double)));
}

/* Allocate tide-prediction storage except for node factors and epochs. */
 void
allocate_cst ()
{
  assert (num_csts);
  assert (cst_speeds = (double *) malloc (num_csts * sizeof (double)));
  assert (work = (double *) malloc (num_csts * sizeof (double)));
}

/* Allocate cst_nodes. */
 void
allocate_nodes ()
{
  int a;
  assert (num_csts);
  assert (num_nodes);
  assert (cst_nodes = (double **) malloc (num_csts * sizeof (double *)));
  for (a=0;a<num_csts;a++)
    assert (cst_nodes[a] = (double *) malloc (num_nodes * sizeof (double)));
}

/* Allocate cst_epochs. */
 void
allocate_epochs ()
{
  int a;
  assert (num_csts);
  assert (num_epochs);
  assert (cst_epochs = (double **) malloc (num_csts * sizeof (double *)));
  for (a=0;a<num_csts;a++)
    assert (cst_epochs[a] = (double *) malloc (num_epochs * sizeof (double)));
}

/* Allocate cst_epochs. */
void
free_epochs ()
{
	int a;
	assert (num_csts);
	for (a=0;a<num_csts;a++)
		free (cst_epochs[a]);
	free (cst_epochs);
}

/* Allocate cst_nodes. */
void
free_nodes ()
{
	int a;
	assert (num_csts);
	for (a=0;a<num_csts;a++)
		free (cst_nodes[a]);
	free (cst_nodes);
}

/* Allocate tide-prediction storage except for node factors and epochs. */
void
free_cst ()
{
	free (cst_speeds);
	free (work);
}

void
free_local_cst()
{
	free (loc_amp);
	free (loc_amp_last);
	free (loc_epoch);
	free (loc_epoch_last);
}

/* Get rid of trailing garbage in linrec */
char *
nojunk (char *linrec)
{
  char *a;
  a = &(linrec[strlen(linrec)]);
  while (a > linrec)
    if (*(a-1) == '\\n' || *(a-1) == '\\r' || *(a-1) == ' ')
      *(--a) = '\\0';
    else
      break;
  return linrec;
}

/* Slackful strcmp; 0 = match.  It's case-insensitive and accepts a
   prefix instead of the entire string.  The second argument is the
   one that can be shorter. */
static int
slackcmp (char *a, char *b)
{
  int c, cmp, n;
  n = strlen (b);
  if ((int)(strlen (a)) < n)
    return 1;
  for (c=0;c<n;c++)
  {
    cmp = ((a[c] >= 'A' && a[c] <= 'Z') ? a[c] - 'A' + 'a' : a[c])
            -
          ((b[c] >= 'A' && b[c] <= 'Z') ? b[c] - 'A' + 'a' : b[c]);
    if (cmp)
      return cmp;
  }
  return 0;
}

/* Read a line from the harmonics file, skipping comment lines */
int
next_line (FILE *fp, char linrec[linelen], int end_ok)
{
  do {
    if (!fgets (linrec, linelen, fp)) {
      if (end_ok)
        return 0;
      else {
        fprintf (stderr, "Unexpected end of harmonics file '%s'\\n",
        hfile_name);
        exit (-1);
      }
    }
  } while (linrec[0] == '#' || linrec[0] == '\\r' || linrec[0] == '\\n');
  return 1;
}

/* Remove lingering carriage return, but do nothing else */
 void
skipnl (FILE *fp) {
  char linrec[linelen];
  fgets (linrec, linelen, fp);
}

/* Find a unit; returs -1 if not found. */
int
findunit (char *unit) {
  int a;
  for (a=0; a<NUMUNITS; a++) {
    if (!strcmp (unit, known_units[a].name) ||
        !strcmp (unit, known_units[a].abbrv))
      return a;
  }
  return -1;
}

/* Load up harmonics data once location is found (called by load_data). */
static void
load_this_data (FILE *fp, char linrec[linelen])
{
  char junk[80];
  int a;
  strcpy (location, linrec);
  strncpy(last_location, location, strlen(location) - 1);
  last_location[strlen(location) - 1] = '\0';
  /* Is it a current? */
  if (strstr (location, "Current"))
    iscurrent = 1;
  /* Get meridian */
  next_line (fp, linrec, 0);
  meridian = hhmm2seconds (linrec);
  /* Get tzfile, if present */
  if (sscanf (nojunk(linrec), "%s %s", junk, tzfile) < 2)
    strcpy (tzfile, "UTC0");
  /* Get DATUM and units */
  next_line (fp, linrec, 0);
  if (sscanf (nojunk(linrec), "%lf %s", &DATUM, units) < 2)
    strcpy (units, "unknown");
  if ((a = findunit (units)) == -1)
    strcpy (units_abbrv, units);
  else {
    if (!strcmp (units, known_units[a].name))
      strcpy (units_abbrv, known_units[a].abbrv);
    else {
      strcpy (units_abbrv, units);
      strcpy (units, known_units[a].name);
    }
  }
  /* Get constituents */
  for (a=0;a<num_csts;a++) {
    next_line (fp, linrec, 0);
    assert (sscanf (linrec, "%s %lf %lf", junk, &(loc_amp[a]),
      &(loc_epoch[a])) == 3);
    /* assert (loc_amp[a] >= 0.0); */
    if (loc_amp[a] < 0.0) {
      fprintf (stderr, "Location = %s\\n", location);
      assert (loc_amp[a] >= 0.0);
    }
    loc_epoch[a] *= M_PI / 180.0;
  }
	DATUM_LAST = DATUM;
	memcpy(loc_amp_last, loc_amp, sizeof(loc_amp_last));
	memcpy(loc_epoch_last, loc_epoch, sizeof(loc_epoch_last));
  fclose (fp);
}

/* Load harmonics data */
void
load_data ()
{
  init();
  FILE *fp;
  char linrec[linelen], junk[80];
  int a, b;
  if (!(fp = fopen (hfile_name, "r"))) {
    fprintf (stderr, "Could not open harmonics file '%s'\\n", hfile_name);
    barf (CANTOPENFILE);
  }
  next_line (fp, linrec, 0);
  assert (sscanf (linrec, "%d", &num_csts) == 1);
	allocate_cst();
	if (strcmp(last_location,location)) {
		free_local_cst();
		allocate_local_cst ();
	}
  /* Load constituent speeds */
  for (a=0;a<num_csts;a++) {
    next_line (fp, linrec, 0);
    assert (sscanf (linrec, "%s %lf", junk, &(cst_speeds[a])) == 2);
    cst_speeds[a] *= M_PI / 648000; /* Convert to radians per second */
          /*   M_PI / 180 to get radians, / 3600 to get seconds */
  }
  /* Get first year for nodes and epochs */
  next_line (fp, linrec, 0);
  assert (sscanf (linrec, "%d", &first_year) == 1);

  /* Load epoch table */
  next_line (fp, linrec, 0);
  assert (sscanf (linrec, "%d", &num_epochs) == 1);
  allocate_epochs ();
  for (a=0;a<num_csts;a++) {
    assert (fscanf (fp, "%s", linrec) == 1);
    for (b=0;b<num_epochs;b++) {
      assert (fscanf (fp, "%lf", &(cst_epochs[a][b])) == 1);
      cst_epochs[a][b] *= M_PI / 180.0;
    }
  }
  /* Sanity check */
  assert (fscanf (fp, "%s", linrec) == 1);
  assert (!strcmp (linrec, "*END*"));
  skipnl (fp);

  /* Load node factor table */
  next_line (fp, linrec, 0);
  assert (sscanf (linrec, "%d", &num_nodes) == 1);
  allocate_nodes ();
  for (a=0;a<num_csts;a++) {
    assert (fscanf (fp, "%s", linrec) == 1);
    for (b=0;b<num_nodes;b++)
      assert (fscanf (fp, "%lf", &(cst_nodes[a][b])) == 1);
  }
  /* Sanity check */
  assert (fscanf (fp, "%s", linrec) == 1);
  assert (!strcmp (linrec, "*END*"));
  skipnl (fp);

  /* List locations and exit? */
  if (list) {
    while (next_line (fp, linrec, 1)) {
      nojunk (linrec);
      if (curonly)
        if (!strstr (linrec, "Current")) {
          for (a=0;a<num_csts+2;a++)
            next_line (fp, linrec, 0);
          continue;
        }
      printf ("%s", linrec);
      next_line (fp, linrec, 0);
      if (list == 2)
        printf ("\\t%s\\n", nojunk(linrec));
      else
        printf ("\\n");
      for (a=0;a<num_csts+1;a++)
        next_line (fp, linrec, 0);
    }
    fclose (fp);
    exit (0);
  }

  /* Randomize location? */
  if (!strcmp (location, "random")) {
    long *locpos, temppos;
    unsigned long rnum;
    int numlocs = 1500, locctr = 0;
    assert (locpos = (long *) malloc (numlocs * sizeof (long)));
    temppos = ftell (fp);
    while (next_line (fp, linrec, 1)) {
      nojunk (linrec);
      if ((!curonly) || (strstr (linrec, "Current"))) {
        locpos[locctr++] = temppos;
        if (locctr >= numlocs) {
          numlocs <<= 1;
          assert (locpos = (long *) realloc (locpos, numlocs * sizeof (long)));
        }
      }
      for (a=0;a<num_csts+2;a++)
        next_line (fp, linrec, 0);
      temppos = ftell (fp);
    }
    assert (locctr);
#ifdef URANDOM
    /* Suck up those random bits */
    {
      FILE *fp;
      assert (fp = fopen ("/dev/urandom", "r"));
      assert (fread (&rnum, sizeof (rnum), 1, fp) == 1);
      fclose (fp);
    }
#else
    /* It's not worth fighting with random() and rand() */
    rnum = (unsigned long)(time(NULL));
#endif
    assert (!fseek (fp, locpos[rnum%locctr], SEEK_SET));
    free (locpos);
    next_line (fp, linrec, 1);
    nojunk (linrec);
    load_this_data (fp, linrec);
    return;
  }

	// if location hasn't changed, don't reload it
	if (strcmp(last_location,location)) {
		fprintf(stderr,"'%s' and '%s' are not equal. Loading location data.\n", last_location, location);
		/* Load harmonic constants for desired location */
		while (next_line (fp, linrec, 1)) {
			nojunk (linrec);
			if (curonly)
				if (!strstr (linrec, "Current"))
					continue;
			if (slackcmp (linrec, location))
				continue;
			load_this_data (fp, linrec);
			return;
		}
	} else {
		memcpy(loc_amp,loc_amp_last, sizeof(loc_amp));
		memcpy(loc_epoch,loc_epoch_last,sizeof(loc_epoch_last));
		DATUM = DATUM_LAST;
		fclose(fp);
		return;
	}

  if (curonly)
    fprintf (stderr, "Could not find current set '%s' in harmonics file\\n",
      location);
  else
    fprintf (stderr, "Could not find location '%s' in harmonics file\\n",
      location);
  exit (-1);
}

/* Check for errors in equilibrium arguments by inferring values from
   neighboring years and comparing with the stored values.  This
   function assumes that the values are normalized [0..2*pi). */
void
check_epoch ()
{
  double inferred, stored, error;
  int a, final_year;
  time_t prev_epoch, next_epoch;
  final_year = first_year + num_epochs - 1;
  if (checkyear < first_year || checkyear > final_year) {
    fprintf (stderr, "Tidelib:  Don't have equilibrium arguments for %d\\n",
      checkyear);
    barf (MISSINGYEAR);
  }
  if (checkyear != first_year) {
    year = checkyear - 1;
    set_epoch (year, num_epochs, first_year);
    prev_epoch = epoch;
  }
  if (checkyear != final_year) {
    year = checkyear + 1;
    set_epoch (year, num_epochs, first_year);
    next_epoch = epoch;
  }
  year = checkyear;
  set_epoch (year, num_epochs, first_year);
  puts ("#  Stored  Inferred  Error");
  for (a=0;a<num_csts;a++) {
    if (checkyear == first_year)
      inferred = fmod (cst_speeds[a] * (-(long)(next_epoch - epoch)) +
      cst_epochs[a][1], 2.0 * M_PI);
    else if (checkyear == final_year)
      inferred = fmod (cst_speeds[a] * (epoch - prev_epoch) +
      cst_epochs[a][final_year-1-first_year], 2.0 * M_PI);
    else {
      double t1, t2;
      t1 = fmod (cst_speeds[a] * (-(long)(next_epoch - epoch)) +
        cst_epochs[a][checkyear+1-first_year], 2.0 * M_PI);
      t2 = fmod (cst_speeds[a] * (epoch - prev_epoch) +
        cst_epochs[a][checkyear-1-first_year], 2.0 * M_PI);
      if (t1 < 0.0)
        t1 += 2.0 * M_PI;
      if (t2 < 0.0)
        t2 += 2.0 * M_PI;
      inferred = (t1 + t2) / 2.0;
      if (fabs (inferred - t1) > M_PI/2.0 || fabs (inferred - t2) > M_PI/2.0)
        inferred = fmod (inferred + M_PI, 2.0 * M_PI);
    }
    stored = cst_epochs[a][checkyear-first_year];
    if (inferred < 0.0)
      inferred += 2.0 * M_PI;
    error = fabs (stored - inferred);
    if (fabs (stored + 2.0 * M_PI - inferred) < error)
      error = fabs (stored + 2.0 * M_PI - inferred);
    if (fabs (stored - inferred - 2.0 * M_PI) < error)
      error = fabs (stored - inferred - 2.0 * M_PI);
    printf ("%d  %010.6f  %010.6f  %f\\n", a, stored, inferred, error);
  }
}

/* Make a reasonable choice for hinc for a given amplitude. */
void
pick_hinc ()
{
  if (fakeamplitude > 30.0)
    hinc = 10;
  else if (fakeamplitude > 15.0)
    hinc = 5;
  else if (fakeamplitude > 8.0)
    hinc = 2;
  else
    hinc = 1;
}

/* Make depth captions, including units */
void
make_depth_caption (int *firstflag, int a, char *temp) {
  if (*firstflag) {
    *firstflag = 0;
    if (strcmp (units_abbrv, "unknown")) {
      sprintf (temp, "%d %s", abs(a), units_abbrv);
      return;
    }
  }
  sprintf (temp, "%d", abs(a));
}

/* Figure out max amplitude over all the years in the node factors table. */
/* This function by Geoffrey T. Dairiki */
static void
figure_amplitude ()
{
  int		i, a;

  if (amplitude == 0.0)
    {
      for (i = 0; i < num_nodes; i++)
	{
	  double year_amp = 0.0;

	  for (a=0; a < num_csts; a++)
	      year_amp += loc_amp[a] * cst_nodes[a][i];
	  if (year_amp > amplitude)
	      amplitude = year_amp;
	}
      assert(amplitude > 0.0);

      /* Figure other, related global values (DWF) */
      absmax = DATUM + amplitude;
      absmin = DATUM - amplitude;
      if (htleveloff) {
	if (hlevelmult)
	  absmax *= htleveloff;
	else
	  absmax += htleveloff;
      }
      if (ltleveloff) {
	if (llevelmult)
	  absmin *= ltleveloff;
	else
	  absmin += ltleveloff;
      }
      if (absmax <= absmin)
	barf (MAXBELOWMIN);
      fakedatum = (absmax + absmin) / 2.0;
      fakeamplitude = (absmax - absmin) / 2.0;

      if (fabs (fakedatum) >= 100.0 || fakeamplitude >= 100.0)
        fprintf (stderr, "Tidelib warning:  tidal range out of normal bounds\\n");
    }
}

/* Figure out normalized multipliers for constituents for a particular
   year.  Save amplitude for drawing unit lines. */
static void
figure_multipliers ()
{
  int a;

  figure_amplitude();
  if (year < first_year || year >= first_year + num_nodes) {
    fprintf (stderr, "Tidelib:  Don't have node factors for %d\\n", year);
    barf (MISSINGYEAR);
  }
  for (a = 0; a < num_csts; a++)
      work[a] = loc_amp[a] * cst_nodes[a][year-first_year] / amplitude;
  if (hincmagic)
      pick_hinc ();
}

/* Re-initialize for a different year */
void
happy_new_year (int new_year)
{
  year = new_year;
  figure_multipliers ();
  set_epoch (year, num_epochs, first_year);
}

/* Interpolate between two color values (or any two integers) */
int linterp (int a, int b, double x) {
  return (int)((double)a + x * (double)(b-a) + 0.5);
}

#ifndef NO_LUDICROUS_SPEED

/*************************************************************************
 *
 * Here is a much more efficient (by a factor of 15 in CPU time on my
 * computer), but more complicated scheme for finding the times
 * of the next high/low tides, or tide transitions.
 *
 * The old method was just a linear search, advancing in one minute
 * steps until a local max/minimum or transition was found.
 *
 * The new method is somewhat more detailed --- the key improvement
 * is the use of a modified Newton-Raphson to find level-crossings
 * or extrema.
 *
 * Geoffrey T. Dairiki Fri Jul 19 15:44:21 PDT 1996
 *
 ************************************************************************/

/* TIDE_TIME_PREC
 *   Precision (in seconds) to which we will find roots
 */
#define TIDE_TIME_PREC (15)

/* TIDE_TIME_BLEND
 *   Half the number of seconds over which to blend the tides from
 *   one epoch to the next.
 */
#define TIDE_BLEND_TIME (3600)

/* TIDE_TIME_STEP
 *   We are guaranteed to find all high and low tides as long as their
 * spacing is greater than this value (in seconds).
 */
#define TIDE_TIME_STEP (TIDE_TIME_PREC)

#define TIDE_MAX_DERIV (2)      /* Maximum derivative supported by
                                 * time2dt_tide() and family. */

#define TIDE_BAD_TIME   ((time_t) -1)

/*
 * We will need a function for tidal height as a function of time
 * which is continuous (and has continuous first and second derivatives)
 * for all times.
 *
 * Since the epochs & multipliers for the tidal constituents change
 * with the year, the regular time2tide(t) function has small
 * discontinuities at new years.  These discontinuities really
 * fry the fast root-finders.
 *
 * We will eliminate the new-years discontinuities by smoothly
 * interpolating (or "blending") between the tides calculated with one
 * year's coefficients, and the tides calculated with the next year's
 * coefficients.
 *
 * i.e. for times near a new years, we will "blend" a tide
 * as follows:
 *
 * tide(t) = tide(year-1, t)
 *                  + w((t - t0) / Tblend) * (tide(year,t) - tide(year-1,t))
 *
 * Here:  t0 is the time of the nearest new-year.
 *        tide(year-1, t) is the tide calculated using the coefficients
 *           for the year just preceding t0.
 *        tide(year, t) is the tide calculated using the coefficients
 *           for the year which starts at t0.
 *        Tblend is the "blending" time scale.  This is set by
 *           the macro TIDE_BLEND_TIME, currently one hour.
 *        w(x) is the "blending function", whice varies smoothly
 *           from 0, for x < -1 to 1 for x > 1.
 *
 * Derivatives of the blended tide can be evaluated in terms of derivatives
 * of w(x), tide(year-1, t), and tide(year, t).  The blended tide is
 * guaranteed to have as many continuous derivatives as w(x).  */

/* time2dt_tide(time_t t, int n)
 *
 *   Calculate nth time derivative the normalized tide.
 *
 * Notes: This function does not check for changes in year.
 *  This is important to our algorithm, since for times near
 *  new years, we interpolate between the tides calculated
 *  using one years coefficients, and the next years coefficients.
 *
 *  Except for this detail, time2dt_tide(t,0) should return a value
 *  identical to time2tide(t).
 */
static double
_time2dt_tide (time_t t, int deriv)
{
  double dt_tide = 0.0;
  int a, b;
  double term, tempd;

  tempd = M_PI / 2.0 * deriv;
  for (a=0;a<num_csts;a++)
    {
      term = work[a] *
          cos(tempd +
              cst_speeds[a] * ((long)(t - epoch) + meridian) +
              cst_epochs[a][year-first_year] - loc_epoch[a]);
      for (b = deriv; b > 0; b--)
          term *= cst_speeds[a];
      dt_tide += term;
    }
  return dt_tide;
}


/* blend_weight (double x, int deriv)
 *
 * Returns the value nth derivative of the "blending function" w(x):
 *
 *   w(x) =  0,     for x <= -1
 *
 *   w(x) =  1/2 + (15/16) x - (5/8) x^3 + (3/16) x^5,
 *                  for  -1 < x < 1
 *
 *   w(x) =  1,     for x >= 1
 *
 * This function has the following desirable properties:
 *
 *    w(x) is exactly either 0 or 1 for |x| > 1
 *
 *    w(x), as well as its first two derivatives are continuous for all x.
 */
static double
blend_weight (double x, int deriv)
{
  double x2 = x * x;

  if (x2 >= 1.0)
      return deriv == 0 && x > 0.0 ? 1.0 : 0.0;

  switch (deriv) {
  case 0:
      return ((3.0 * x2 -10.0) * x2 + 15.0) * x / 16.0 + 0.5;
  case 1:
      return ((x2 - 2.0) * x2 + 1.0) * (15.0/16.0);
  case 2:
      return (x2 - 1.0) * x * (15.0/4.0);
  }
  assert(0);
}

/*
 * This function does the actual "blending" of the tide
 * and its derivatives.
 */
static double
blend_tide (time_t t, int deriv, int first_year, double blend)
{
  double        fl[TIDE_MAX_DERIV + 1];
  double        fr[TIDE_MAX_DERIV + 1];
  double *      fp      = fl;
  double        w[TIDE_MAX_DERIV + 1];
  double        fact = 1.0;
  double        f;
  int           n;

  assert (deriv >= 0 && deriv <= TIDE_MAX_DERIV);

  /*
   * If we are already happy_new_year()ed into one of the two years
   * of interest, compute that years tide values first.
   */
  if (year == first_year + 1)
      fp = fr;
  else if (year != first_year)
      happy_new_year(first_year);
  for (n = 0; n <= deriv; n++)
      fp[n] = _time2dt_tide(t, n);

  /*
   * Compute tide values for the other year of interest,
   *  and the needed values of w(x) and its derivatives.
   */
  if (fp == fl)
    {
      happy_new_year(first_year + 1);
      fp = fr;
    }
  else
    {
      happy_new_year(first_year);
      fp = fl;
    }
  for (n = 0; n <= deriv; n++)
    {
      fp[n] = _time2dt_tide(t, n);
      w[n] = blend_weight(blend, n);
    }

  /*
   * Do the blending.
   */
  f = fl[deriv];
  for (n = 0; n <= deriv; n++)
    {
      f += fact * w[n] * (fr[deriv-n] - fl[deriv-n]);
      fact *= (double)(deriv - n)/(n+1) * (1.0/TIDE_BLEND_TIME);
    }
  return f;
}

static double
time2dt_tide (time_t t, int deriv)
{
  static time_t next_epoch      = TIDE_BAD_TIME; /* next years newyears */
  static time_t this_epoch      = TIDE_BAD_TIME; /* this years newyears */
  static int    this_year       = -1;
  int           new_year        = yearoftimet(t);

  /* Make sure our values of next_epoch and epoch are up to date. */
  if (new_year != this_year)
    {
      if (new_year + 1 < first_year + num_epochs)
        {
          set_epoch(new_year + 1, num_epochs, first_year);
          next_epoch = epoch;
        }
      else
          next_epoch = TIDE_BAD_TIME;

      happy_new_year(this_year = new_year);
      this_epoch = epoch;
    }

  assert(t >= this_epoch);
  assert(next_epoch == TIDE_BAD_TIME || t < next_epoch);

  /*
   * If we're close to either the previous or the next
   * new years we must blend the two years tides.
   */
  if (t - this_epoch <= TIDE_BLEND_TIME && this_year > first_year)
      return blend_tide(t, deriv,
                        this_year - 1,
                        (double)(t - this_epoch)/TIDE_BLEND_TIME);
  else if (next_epoch - t <= TIDE_BLEND_TIME
           && this_year + 1 < first_year + num_epochs)
      return blend_tide(t, deriv,
                        this_year,
                        -(double)(next_epoch - t)/TIDE_BLEND_TIME);

  /*
   * Else, we're far enough from newyears to ignore the blending.
   */
  if (this_year != year)
      happy_new_year(this_year);
  return _time2dt_tide(t, deriv);
}

/* dt_tide_max (int n)
 *   Returns the maximum that the absolute value of the nth derivative
 * of the tide can ever attain.
 */
static double
dt_tide_max (int deriv)
{
  static double maxdt[TIDE_MAX_DERIV+2]; /* Initilized to zeroes by ANSI */
  double        max     = 0.0;
  int           myyear, wasyear;
  int           a;

  /* We need to be able to calculate max tide derivatives for one
   * derivative higher than we actually need to know the tides.
   */
  assert(deriv >= 0 && deriv <= TIDE_MAX_DERIV+1);

  if (maxdt[deriv] <= 0.0)
    {
      /* Actually doing a happy_new_year on 1970 is unsafe because
         the mktime in tm2gmt will, on rare occasions, fail because the
         uncorrected time_t is before the beginning of the Unix epoch.
         I've kludged this to include 1970 without crashing mktime.
         -- DWF
         tm2gmt has since been redone, but this "workaround" doesn't
         harm anything, so I'll leave it in. -- DWF */

      wasyear = year;
      if (!wasyear)
        wasyear = first_year+1;
      for (myyear = first_year; myyear < first_year + num_epochs; myyear++)
        {
          /* happy_new_year(myyear);    Crash.  Burn. */
          year = myyear;
          figure_multipliers ();

          max = 0.0;
          for (a=0;a<num_csts;a++)
              max += work[a] * pow(cst_speeds[a], (double) deriv);
          if (max > maxdt[deriv])
              maxdt[deriv] = max;
        }
      maxdt[deriv] *= 1.1;      /* Add a little safety margin... */
      happy_new_year (wasyear);   /* Clean up the mess */
    }
  return maxdt[deriv];
}

/* time2dt_atide (time_t t, int n)
 *   Calcualte the nth derivative of the dimensional tide.
 */
static double
time2dt_atide (time_t t, int deriv)
{
  double tide = time2dt_tide(t,deriv) * amplitude;
  if (deriv == 0)
      tide += DATUM;
  return tide;
}

/* find_zero (time_t t1, time_t t2, double (*f)(time_t t, int deriv))
 *   Find a zero of the function f, which is bracketed by t1 and t2.
 *   Returns a value which is either an exact zero of f, or slightly
 *   past the zero of f.
 */

#ifdef DO_BISECTION
/* A simple zero finder based upon the method of bisection. */
static time_t
find_zero (time_t t1, time_t t2,
           double (*f)(time_t t, int deriv))
{
  time_t tl = t1;
  time_t tr = t2;
  double fl = f(tl,0);
  double fr = f(tr,0);
  double scale = 1.0;
  time_t t;
  double ft;
  int dt;

  assert(fl != 0.0 && fr != 0.0);
  assert(tl < tr);
  if (fl > 0)
    {
      scale = -1.0;
      fl = -fl;
      fr = -fr;
    }
  assert(fl < 0.0 && fr > 0.0);

  while ((dt = tr - tl) > TIDE_TIME_PREC)
    {
      t = tl + dt / 2;
      if ((ft = scale * f(t, 0)) == 0.0)
          return t;
      else if (ft > 0.0)
          tr = t, fr = ft;
      else
          tl = t, fl = ft;
    }
  return tr;
}

#else /* DO_BISECTION */

/*
 * Here's a better root finder based upon a modified Newton-Raphson method.
 */
static time_t
find_zero (time_t t1, time_t t2,
           double (*f)(time_t t, int deriv))
{
  time_t tl = t1;
  time_t tr = t2;
  double fl = f(tl,0);
  double fr = f(tr,0);
  double scale = 1.0;
  int    dt;
  time_t t = 0;
  double fp = 0.0;
  double ft = 1.0;              /* Forces first step to be bisection */
  double f_thresh = 0.0;

  assert(fl != 0.0 && fr != 0.0);
  assert(tl < tr);
  if (fl > 0)
    {
      scale = -1.0;
      fl = -fl;
      fr = -fr;
    }
  assert(fl < 0.0 && fr > 0.0);

  while (tr - tl > TIDE_TIME_PREC)
    {
      if (fabs(ft) > f_thresh   /* not decreasing fast enough */
          || (ft > 0 ?          /* newton step would go outside bracket */
              (fp <= ft / (t - tl)) :
              (fp <= -ft / (tr - t))))
        {
          dt = 0;                       /* Force bisection */
        }
      else
        {
          /* Attempt a newton step */
          dt = (int)floor(-ft/fp + 0.5);

          /* Since our goal specifically is to reduce our bracket size
             as quickly as possible (rather than getting as close to
             the zero as possible) we should ensure that we don't take
             steps which are too small.  (We'd much rather step over
             the root than take a series of steps which approach the
             root rapidly but from only one side.) */
          if (abs(dt) < TIDE_TIME_PREC)
              dt = ft < 0 ? TIDE_TIME_PREC : -TIDE_TIME_PREC;

          if ((t += dt) >= tr || t <= tl)
              dt = 0;           /* Force bisection if outside bracket */
          f_thresh = fabs(ft) / 2.0;
        }

      if (dt == 0)
        {
          /* Newton step failed, do bisection */
          t = tl + (tr - tl) / 2;
          f_thresh = fr > -fl ? fr : -fl;
        }

      if ((ft = scale * f(t,0)) == 0)
          return t;             /* Exact zero */
      else if (ft > 0.0)
          tr = t, fr = ft;
      else
          tl = t, fl = ft;

      fp = scale * f(t,1);
    }

  return tr;
}

#endif /* DO_BISECTION */

/* next_zero(time_t t, double (*f)(), double max_fp, double max_fpp)
 *   Find the next zero of the function f which occurs after time t.
 *   The arguments max_fp and max_fpp give the maximum possible magnitudes
 *   that the first and second derivative of f can achieve.
 *
 *   Algorithm:  Our goal here is to bracket the next zero of f ---
 *     then we can use find_zero() to quickly refine the root.
 *     So, we will step forward in time until the sign of f changes,
 *     at which point we know we have bracketed a root.
 *     The trick is to use large steps in are search, which making
 *     sure the steps are not so large that we inadvertently
 *     step over more than one root.
 *
 *     The big trick, is that since the tides (and derivatives of
 *     the tides) are all just harmonic series', it is easy to place
 *     absolute bounds on their values.
 */

static time_t
next_zero (time_t t,
           double (*f)(time_t t, int deriv),
           int * risingflag,
           double max_fp,
           double max_fpp)
{
  time_t t_left = t;
  time_t t_right;
  int step, step1, step2;

  double f_left, df_left, f_right;
  double scale = 1.0;

  /* If we start at a zero, step forward until we're past it. */
  while ((f_left = f(t_left,0)) == 0.0)
      t_left += TIDE_TIME_PREC;

  if ( !(*risingflag = f_left < 0) )
    {
      scale = -1.0;
      f_left = -f_left;
    }

  while (1)
    {
      /* Minimum time to next zero: */
      step1 = (int)(fabs(f_left) / max_fp);

      /* Minimum time to next turning point: */
      df_left = scale * f(t_left,1);
      step2 = (int)(fabs(df_left) / max_fpp);

      if (df_left < 0.0)
        {
          /* Derivative is in the wrong direction. */
          step = step1 + step2;
        }
      else
        {
          step = step1 > step2 ? step1 : step2;
        }

      if (step < TIDE_TIME_STEP)
          step = TIDE_TIME_STEP; /* No rediculously small steps... */

      t_right = t_left + step;
      /*
       * If we hit upon an exact zero, step right until we're off
       * the zero.  If the sign has changed, we are bracketing a desired
       * root, if the sign hasn't changed, then the zero was at
       * an inflection point (i.e. a double-zero to within TIDE_TIME_PREC)
       * and we want to ignore it.
       */
      while ((f_right = scale * f(t_right, 0)) == 0.0)
          t_right += TIDE_TIME_PREC;
      if (f_right > 0.0)
          return find_zero(t_left, t_right, f); /* Found a bracket */

      t_left = t_right, f_left = f_right;
    }
}

static double
f_hiorlo (time_t t, int deriv)
{
  return time2dt_tide(t, deriv + 1);
}

static time_t
next_high_or_low_tide (time_t t, int * hiflag)
{
  int           rising;
  time_t        thilo   = next_zero(t, f_hiorlo, &rising,
                                    dt_tide_max(2), dt_tide_max(3));
  *hiflag = !rising;
  return thilo;
}

static double
f_mark (time_t t, int deriv)
{
  double fval = time2dt_atide(t, deriv);
  if (deriv == 0)
      fval -= marklev;
  return fval;
}

static time_t
find_mark_crossing (time_t t1, time_t t2, int * risingflag)
{
  double f1 = f_mark(t1,0);
  double f2 = f_mark(t2,0);

  assert(f1 != f2);

  if ( !(*risingflag = f1 < 0.0 || f2 > 0.0) )
    {
      f1 = -f1;
      f2 = -f2;
    }

  if (f1 == 0.0)
      return t1;
  else if (f2 == 0.0)
      return t2;

  return (f1 < 0.0 && f2 > 0.0) ? find_zero(t1, t2, f_mark) : TIDE_BAD_TIME;
}

/* Next high tide, low tide, transition of the mark level, or some
   combination.
       Bit      Meaning
        0       low tide
        1       high tide
        2       falling transition
        3       rising transition
*/

int
next_big_event (time_t *tm)
{
  static time_t last_tm   = TIDE_BAD_TIME;
  static time_t cache_hilo;
  static int    is_high;
  time_t t_hilo;
  time_t t_mark;
  int   is_rising;
  int   stat = 0;

  /* Find next high/low tide */

  if (*tm == last_tm)           /* If we have a cached hi/lo tide, use it */
      t_hilo = cache_hilo;
  else
    {
      /* Find time of next high or low tide */
      t_hilo = next_high_or_low_tide(*tm, &is_high);
      assert(t_hilo > *tm);

      if (mark &&
        ((text && !graphmode) || (!text && graphmode) || ppm || gif || ps))
        if ((t_mark = find_mark_crossing(*tm, t_hilo, &is_rising))
                    != TIDE_BAD_TIME)
        {
          assert(t_mark >= *tm && t_mark <= t_hilo);
          cache_hilo = t_hilo;  /* Save time of next hi/lo */
          last_tm = *tm = t_mark;
          stat = is_rising ? 0x08 : 0x04;
          if (t_mark < t_hilo)
              return stat;
        }
    }

  last_tm = TIDE_BAD_TIME;              /* tag cache as invalid */
  *tm = t_hilo;
  return stat | (is_high ? 0x02 : 0x01);
}

double
time2tide (time_t t)
{
  return time2dt_tide(t, 0);
}

double
time2atide (time_t t)
{
  return time2dt_atide(t, 0);
}

#else /* NO_LUDICROUS_SPEED */

/* Calculate the normalized tide (-1.0 .. 1.0) for a given time. */
double
time2tide (time_t t)
{
  double tide = 0.0;
  int a, new_year = yearoftimet (t);
  if (new_year != year)
    happy_new_year (new_year);
  for (a=0;a<num_csts;a++)
    tide += work[a] *
      cos (cst_speeds[a] * ((long)(t - epoch) + meridian) +
      cst_epochs[a][year-first_year] - loc_epoch[a]);
  return tide;
}

/* Calculate the denormalized tide. */
double
time2atide (time_t t)
{
  return time2tide (t) * amplitude + DATUM;
}

/* Next high tide, low tide, transition of the mark level, or some
   combination.
       Bit      Meaning
        0       low tide
        1       high tide
        2       falling transition
        3       rising transition
*/
int
next_big_event (time_t *tm)
{
  double p, q;
  int flags = 0, slope = 0;
  p = time2atide (*tm);
  *tm += 60;
  q = time2atide (*tm);
  *tm += 60;
  if (p < q)
    slope = 1;
  while (1) {
    if ((slope == 1 && q < p) || (slope == 0 && p < q)) {
      /* Tide event */
      flags |= (1 << slope);
    }
    /* Modes in which to return mark transitions: */
    /*    -text (no -graph)   */
    /*    -graph (no -text)   */
    /*    -ppm                */
    /*    -gif                */
    /*    -ps                 */
    if (mark && ((text && !graphmode) || (!text && graphmode)
    || ppm || gif || ps))
      if ((p > marklev && q <= marklev) || (p < marklev && q >= marklev)) {
        /* Transition event */
        if (p < q)
          flags |= 8;
        else
          flags |= 4;
        if (!(flags & 3)) {
          /* If we're incredibly unlucky, we could miss a tide event if we
             don't check for it here:

                             . <----   Value that would be returned
                        -----------    Mark level
                      .           .
          */
          p = q;
          q = time2atide (*tm);
          if ((slope == 1 && q < p) || (slope == 0 && p < q)) {
            /* Tide event */
            flags |= (1 << slope);
          }
        }
      }
    if (flags) {
      *tm -= 60;
      /* Don't back up over a transition event, but do back up to where the
         tide changed if possible.  If they happen at the same time, then
         we're off by a minute on the tide, but if we back it up it will
         get snagged on the transition event over and over. */
      if (flags < 4)
        *tm -= 60;
      return flags;
    }
    p = q;
    q = time2atide (*tm);
    *tm += 60;
  }
}

#endif /* NO_LUDICROUS_SPEED */

/* Estimate the normalized mean tide level around a particular time by
   summing only the long-term constituents. */
/* Does not do any blending around year's end. */
/* This is used only by time2asecondary for finding the mean tide level */
double
time2mean (time_t t)
{
  double tide = 0.0;
  int a, new_year = yearoftimet (t);
  if (new_year != year)
    happy_new_year (new_year);
  for (a=0;a<num_csts;a++) {
    if (cst_speeds[a] < 6e-6)
      tide += work[a] *
        cos (cst_speeds[a] * ((long)(t - epoch) + meridian) +
        cst_epochs[a][year-first_year] - loc_epoch[a]);
  }
  return tide;
}

/* If offsets are in effect, interpolate the 'corrected' denormalized
tide.  The normalized is derived from this, instead of the other way
around, because the application of height offsets requires the
denormalized tide. */
double
time2asecondary (time_t t) {

  /* Get rid of the normals. */
  if (!(have_offsets))
    return time2atide (t);

  {
/* Intervalwidth of 14 (was originally 13) failed on this input:
-location Dublon -hloff +0.0001 -gstart 1997:09:10:00:00 -raw 1997:09:15:00:00
*/
#define intervalwidth 15
#define stretchfactor 3

    static time_t lowtime=0, hightime=0;
    static double lowlvl, highlvl; /* Normalized tide levels for MIN, MAX */
    time_t T;  /* Adjusted t */
    double S, Z, HI, HS, magicnum;
    time_t interval = HOURSECONDS * intervalwidth;
    long difflow, diffhigh;
    int badlowflag=0, badhighflag=0;

    assert (t > interval*stretchfactor);  /* Avoid underflow */

    /* Algorithm by Jean-Pierre Lapointe (scipur@collegenotre-dame.qc.ca) */
    /* as interpreted, munged, and implemented by DWF */

    /* This is the initial guess (average of time offsets) */
    T = t - (httimeoff + lttimeoff) / 2;

    /* The usage of an estimate of mean tide level here is to correct
       for seasonal changes in tide level.  Previously I had simply used
       the zero of the tide function as the mean, but this gave bad
       results around summer and winter for locations with large seasonal
       variations. */
    Z = time2mean(T);
    S = time2tide(T) - Z;

    /* Find MAX and MIN.  I use the highest high tide and the lowest
       low tide over a 26 hour period, but I allow the interval to stretch
       a lot if necessary to avoid creating discontinuities.  The
       heuristic used is not perfect but will hopefully be good enough.

       It is an assumption in the algorithm that the tide level will
       be above the mean tide level for MAX and below it for MIN.  A
       changeover occurs at mean tide level.  It would be nice to
       always use the two tides that immediately bracket T and to put
       the changeover at mid tide instead of always at mean tide
       level, since this would eliminate much of the inaccuracy.
       Unfortunately if you change the location of the changeover it
       causes the tide function to become discontinuous.

       Now that I'm using time2mean, the changeover does move, but so
       slowly that it makes no difference.
    */

    if (lowtime < T)
      difflow = T - lowtime;
    else
      difflow = lowtime - T;
    if (hightime < T)
      diffhigh = T - hightime;
    else
      diffhigh = hightime - T;

    /* Update MIN? */
    if (difflow > interval * stretchfactor)
      badlowflag = 1;
    if (badlowflag || (difflow > interval && S > 0)) {
      time_t tt;
      double tl;
      tt = T - interval;
      next_big_event (&tt);
      lowlvl = time2tide (tt);
      lowtime = tt;
      while (tt < T + interval) {
        next_big_event (&tt);
        tl = time2tide (tt);
        if (tl < lowlvl && tt < T + interval) {
          lowlvl = tl;
          lowtime = tt;
        }
      }
    }
    /* Update MAX? */
    if (diffhigh > interval * stretchfactor)
      badhighflag = 1;
    if (badhighflag || (diffhigh > interval && S < 0)) {
      time_t tt;
      double tl;
      tt = T - interval;
      next_big_event (&tt);
      highlvl = time2tide (tt);
      hightime = tt;
      while (tt < T + interval) {
        next_big_event (&tt);
        tl = time2tide (tt);
        if (tl > highlvl && tt < T + interval) {
          highlvl = tl;
          hightime = tt;
        }
      }
    }

#if 0
    /* UNFORTUNATELY there are times when the tide level NEVER CROSSES
       THE MEAN for extended periods of time.  ARRRGH!  */
    if (lowlvl >= 0.0)
      lowlvl = -1.0;
    if (highlvl <= 0.0)
      highlvl = 1.0;
#endif
    /* Now that I'm using time2mean, I should be guaranteed to get
       an appropriate low and high. */
    assert (lowlvl < Z);
    assert (highlvl > Z);

    /* This turns out to be triggered routinely by redraws of a
    two-day tide graph, so screw it. */
#if 0
    if (badlowflag != badhighflag)
      fprintf (stderr, "Tidelib WARNING:  Discontinuity in interpolated tide\\nPlease report location, time, and offsets to dave@universe.digex.net\\n");
#endif

    /* Improve the initial guess. */
    if (S > 0)
      magicnum = 0.5 * S / fabs(highlvl - Z);
    else
      magicnum = 0.5 * S / fabs(lowlvl - Z);
    T = T - magicnum * (httimeoff - lttimeoff);
    HI = time2tide(T);

    /* Denormalize and apply the height offsets. */
    HI = HI * amplitude + DATUM;
    {
      double RH=1.0, RL=1.0, HH=0.0, HL=0.0;
      if (hlevelmult)
        RH = htleveloff;
      else
        HH = htleveloff;
      if (llevelmult)
        RL = ltleveloff;
      else
        HL = ltleveloff;

      /* I patched the usage of RH and RL to avoid big ugly
      discontinuities when they are not equal.  -- DWF */

      HS =  HI * ((RH+RL)/2 + (RH-RL)*magicnum)
                + (HH+HL)/2 + (HH-HL)*magicnum;
    }

    return HS;
  }
}

/************** COMMENT BLOCK ************************

This is Jean-Pierre's explanation of his original algorithm.

1-  Store the following data
     Z  (datum at reference station)
     t  (time of prediction at the secondary station)
     TH  (correction for high tide time at secondary station)
     TL  (correction for low tide time at secondary station)
     RH  (correction ratio for high tide height at secondary station)
     RL  (correction ratio for low tide height at secondary station)
     HH  (height correction for high tide height at secondary station)
     HL  (height correction for low tide height at secondary station)

2-  Run XTIDE for the reference station for the day of prediction
     to find the height of the higher tide and store it as MAX and
     to find the height of the lower tide and store it as MIN.

3-  Run XTIDE for the reference station at a time T defined as
     T =  t - (TH + TL) / 2
    Store the height calculated by XTIDE as HI (intermediate height)

4-  Store S defined as S =HI - Z

5-  Run XTIDE for the reference station at a time T defined as:
     if S > 0 then
     T = t - (TH + TL) / 2 - (TH-TL) / 2 x S / absolute value(MAX - Z)
     else
     T = t - (TH + TL) / 2 - (TH-TL) / 2 x S / absolute value(MIN - Z)

    Store the height calculated by XTIDE as HI (intermediate height)
    and calculate HS (height at secondary station at time t) defined as:
    if S > 0 then
    HS =  HI x RH + (HH + HL) / 2 + (HH-HL) / 2 x S/absolute value(MAX - Z)
    else
    HS =  HI x RL + (HH + HL) / 2 + (HH-HL) / 2 x S/absolute value(MIN - Z)

    You now have HS the height of the tide at the secondary station at a
    time t for this station.

********  END COMMENT BLOCK  **********************************************/

/* Normalized 'corrected' tide. */
double
time2secondary (time_t t) {
  /* Get rid of the normals. */
  if (!(have_offsets))
    return time2tide (t);

  return (time2asecondary(t) - fakedatum) / fakeamplitude;
}


/* Determine next big event and set up text string for write_high_tide.
   Returns are same as next_big_event. */
int
update_high_tide ()
{
  struct tm *t;
  char *tidedate, *tidetime;
  int event_type;
  prev_ht = next_ht;
  prev_ht_adj = next_ht_adj;
  event_type = next_big_event (&next_ht);
  if ((event_type & 2) || graphmode || text || ppm || gif || ps || banner) {
    tidedate = next_ht_date;
    tidetime = next_ht_text;
  } else {
    tidedate = next_lt_date;
    tidetime = next_lt_text;
  }
  if (event_type & 2)
    next_ht_adj = next_ht + httimeoff;
  else if (event_type & 1)
    next_ht_adj = next_ht + lttimeoff;
  else
    next_ht_adj = next_ht;
  t = tmtime (next_ht_adj);
  do_timestamp (tidetime, t);
  do_datestamp (tidedate, t);
  return event_type;
}

/* This function has to go in here because it accesses stuff that is
   local to tidelib. */
void
do_html ()
{
}

/************************************************************************/

/* This function was originally going to handle modifying the constituents
for offset stations, but I never got that to work very well.  I removed
everything except the trivial cases.

fudge_constituents is currently "safe" as it will make adjustments
that can be safely made and otherwise do nothing.  It is the
responsibility of the calling program to verify that all needed
adjustments have been made (i.e. that the offset variables have been
cleared.)
*/

/* Also does units conversions */
void
fudge_constituents (char *youwant)
{
  /* Modify location name to show offsets */
  if (httimeoff)
    sprintf (location+strlen(location), " htoff=%dmin", httimeoff/60);
  if (lttimeoff)
    sprintf (location+strlen(location), " ltoff=%dmin", lttimeoff/60);
  if (htleveloff) {
    sprintf (location+strlen(location), " hloff=");
    if (hlevelmult)
      strcat (location, "*");
    sprintf (location+strlen(location), "%f", htleveloff);
  }
  if (ltleveloff) {
    sprintf (location+strlen(location), " lloff=");
    if (llevelmult)
      strcat (location, "*");
    sprintf (location+strlen(location), "%f", ltleveloff);
  }

  /* Apply time offset */
  if (httimeoff == lttimeoff && httimeoff != 0) {
    int looper;
    for (looper=0; looper<num_csts; looper++)
      loc_epoch[looper] += cst_speeds[looper] * (double)httimeoff;
    httimeoff = lttimeoff = 0;
  }

  /* Apply units conversions */
  if (youwant) {
    int iyouhave, iyouwant;
    iyouhave = findunit (units);
    iyouwant = findunit (youwant);
    if (iyouhave != -1 && iyouwant != -1 && iyouhave != iyouwant) {
      if (known_units[iyouhave].type == known_units[iyouwant].type) {
        int looper;
        double mult = known_units[iyouhave].conv_factor /
                      known_units[iyouwant].conv_factor;
        for (looper=0; looper<num_csts; looper++)
          loc_amp[looper] *= mult;
        DATUM *= mult;
        strcpy (units, known_units[iyouwant].name);
        strcpy (units_abbrv, known_units[iyouwant].abbrv);
      }
    }
  }

  /* Apply level offset */
  if (htleveloff == ltleveloff && htleveloff != 0.0
                               && llevelmult == hlevelmult) {
    if (llevelmult) {
      int looper;
      for (looper=0; looper<num_csts; looper++)
        loc_amp[looper] *= htleveloff;
      DATUM *= htleveloff;
      htleveloff = ltleveloff = 0.0;
      llevelmult = hlevelmult = 0;
    } else {
      DATUM += htleveloff;
      htleveloff = ltleveloff = 0.0;
    }
  }

  /* Set flag to indicate whether offsets have been "handled" */
  if (httimeoff || lttimeoff || htleveloff != 0.0 || ltleveloff != 0.0)
    have_offsets = 1;
}
