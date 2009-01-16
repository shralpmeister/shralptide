/*  Arglib.c  Manage command line switches and configuration files.
    Last modified 9/18/95

    Copyright (C) 1996  David Flater.

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
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "arglib.h"

struct arg {
  char arg[MAXARGLEN+1];
  struct arg *next;
};

static struct arg *args = NULL;

/* Are there more args? */
int
moreargs ()
{
  return (int)args;
}

/* Is there another arg, which is not a switch?  (Doesn't start with -) */
int
dataarg ()
{
  if (args)
    if (args->arg[0] != '-')
      return 1;
  return 0;
}

/* Allocate an arg with error checking, and set its value. */
static struct arg *
newarg (char *s)
{
  struct arg *a;
  assert (s);
  assert ((int)(strlen (s)) <= MAXARGLEN);
  assert (a = (struct arg *) malloc (sizeof (struct arg)));
  strcpy (a->arg, s);
  a->next = NULL;
  return a;
}

/* Push an arg onto the arg stack. */
static void
pusharg (struct arg *a)
{
  a->next = args;
  args = a;
}

/* Pop an arg, returning a pointer to static memory or NULL if no more
   args. */
char *
poparg ()
{
  static char savearg[MAXARGLEN+1];
  struct arg *delme;
  if (!args)
    return NULL;
  strcpy (savearg, args->arg);
  delme = args;
  args = args->next;
  free (delme);
  return savearg;
}

/* Stack the command line */
void
push_command_line (int argc, char **argv)
{
  assert (argc > 0);
  for (argc--;argc;argc--)
    pusharg (newarg (argv[argc]));
}

/* Kill a comment in a config file. */
void
killcomment (FILE *fp)
{
  int t;
  do {
    t = fgetc (fp);
    if (t == '\n' || t == '\r')
      break;
  } while (t != EOF);
}

/* Scan an arg from a file, returning a pointer to static memory or NULL
   if end of file. */
static char *
farg (FILE *fp)
{
  static char savearg[MAXARGLEN+1];
  int len = 0, t, isdelim = 0;
  while (1) {
    /* Skip leading whitespace */
    do {
      if ((t = fgetc (fp)) == EOF)
        return NULL;
    } while (isspace (t));
    /* Kill those comments */
    if (t == '#')
      killcomment (fp);
    else
      break;
  }
  /* Is this a delimited arg? */
  if (t == '"')
    isdelim = 1;
  else
    savearg[len++] = (char)t;
  /* Read it in */
  while ((t = fgetc (fp)) != EOF) {
    if ((t == '"' && isdelim) || (isspace (t) && !isdelim) || t == '#')
      break;
    savearg[len++] = (char)t;
    assert (len <= MAXARGLEN);
  }
  assert (len <= MAXARGLEN);
  savearg[len] = '\0';
  return savearg;
}

/* Stack the contents of a file.  The -config option is specifically
   rejected to avoid recursive definitions and attacks launched through
   CGI scripts.

   Returns 0 if the reading of the file is unsuccessful.
*/
int
push_config_file (char *fname)
{
  FILE *fp;
  struct arg *revstack = NULL, *temparg;
  char *filearg;
  assert (fname);
  if (!strcmp (fname, "-"))
    fp = stdin;
  else {
    if (!(fp = fopen (fname, "r")))
      return 0;
  }
  /* Store the args temporarily in reverse order */
  while ((filearg = farg (fp))) {
    if (!strcmp (filearg, "-config")) {
      fprintf (stderr, "arglib:  -config only allowed on command line\n");
      exit (-1);
    }
    temparg = newarg (filearg);
    temparg->next = revstack;
    revstack = temparg;
  }
  fclose (fp);
  /* Push them on the stack in the correct order */
  while (revstack) {
    temparg = revstack;
    revstack = revstack->next;
    pusharg (temparg);
  }
  return 1;
}
