/* nice -- run a program with modified scheduling priority
   Copyright (C) 90, 91, 92, 93, 1994 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* David MacKenzie <djm@gnu.ai.mit.edu> */

#include <config.h>

#ifdef OS2

#define NICE

#define INCL_NOPM
#define INCL_DOSPROCESS
#include <os2.h>
#include <errno.h>

#define PRIO_PROCESS 0

int class = PRTYC_NOCHANGE;
int adjustment;
char *current_class;
char *name[] = {"", "idle-time", "regular", "time-critical", "foreground-server"};

int getpriority(int x, int y)
{
  ULONG prio, class;
  PTIB ptib;
  PPIB ppib;

  DosGetInfoBlocks(&ptib, &ppib);
  prio = ptib -> tib_ptib2 -> tib2_ulpri;

  class = prio >> 8;
  current_class = name[class];
  return -(prio & 255);
}

int setpriority(int x, int y, int v)
{
  return DosSetPriority(PRTYS_PROCESSTREE, class, -adjustment, 0) ? -1 : 0;
}

#endif

#include <stdio.h>

#define NDEBUG
#include <assert.h>

#include <getopt.h>
#include <sys/types.h>
#ifndef NICE_PRIORITY
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "version.h"
#include "system.h"
#include "long-options.h"

#ifdef NICE_PRIORITY
#define GET_PRIORITY() nice (0)
#else
#define GET_PRIORITY() getpriority (PRIO_PROCESS, 0)
#endif

void error ();

static int isinteger ();
static void usage ();

/* The name this program was run with. */
char *program_name;

static struct option const longopts[] =
{
  {"adjustment", required_argument, NULL, 'n'},
#ifdef OS2
  {"idle-time", no_argument, NULL, 'i'},
  {"regular", no_argument, NULL, 'r'},
  {"time-critical", no_argument, NULL, 't'},
  {"foreground-server", no_argument, NULL, 'f'},
#endif
  {NULL, 0, NULL, 0}
};

void
main (argc, argv)
     int argc;
     char **argv;
{
  int current_priority;
#ifndef OS2
  int adjustment = 0;
#endif
  int minusflag = 0;
  int adjustment_given = 0;
  int long_option_priority = 0;
  int last_optind = 0;

  program_name = argv[0];
  parse_long_options (argc, argv, "nice", version_string, usage);

  for (optind = 1; optind < argc; /* empty */)
    {
      char *s;

      s = argv[optind];

      if (s[0] == '-' && s[1] == '-' && ISDIGIT (s[2]))
	{
	  if (!isinteger (&s[2]))
	    error (1, 0, "invalid option `%s'", s);

	  minusflag = 1;
	  adjustment = atoi (&s[2]);
	  adjustment_given = 1;
	  long_option_priority = 1;
	  ++optind;
	}
      else
	{
	  int optc;
#ifdef OS2
	  while ((optc = getopt_long (argc, argv, "+0123456789n:irtf", longopts,
#else
	  while ((optc = getopt_long (argc, argv, "+0123456789n:", longopts,
#endif
				      (int *) 0)) != EOF)
	    {
	      switch (optc)
		{
		case '?':
		  usage (1);

#ifdef OS2
		case 'i':
		  class = PRTYC_IDLETIME;
		  break;
		case 'r':
		  class = PRTYC_REGULAR;
		  break;
		case 't':
		  class = PRTYC_TIMECRITICAL;
		  break;
		case 'f':
		  class = PRTYC_FOREGROUNDSERVER;
		  break;
#endif

		case 'n':
		  if (!isinteger (optarg))
		    error (1, 0, "invalid priority `%s'", optarg);
		  adjustment = atoi (optarg);
		  adjustment_given = 1;
		  break;

		default:
		  assert (ISDIGIT (optc));
		  /* Reset ADJUSTMENT if the last priority-specifying option
		     was not of the same type or if it was, but a separate
		     option.  */
		  if (long_option_priority ||
		      (adjustment_given && optind != last_optind))
		    {
		      long_option_priority = 0;
		      adjustment = 0;
		    }
		  adjustment = adjustment * 10 + optc - '0';
		  adjustment_given = 1;
		  last_optind = optind;
		}
	    }
	  if (optc == EOF)
	    break;
	}
    }

  if (minusflag)
    adjustment = -adjustment;
  if (!adjustment_given)
    adjustment = 10;

  if (optind == argc)
    {
      if (adjustment_given)
	{
	  error (0, 0, "a command must be given with an adjustment");
	  usage (1);
	}
      /* No command given; print the priority. */
      errno = 0;
      current_priority = GET_PRIORITY ();
      if (current_priority == -1 && errno != 0)
	error (1, errno, "cannot get priority");
#ifdef OS2
      printf ("\ncurrent class: %s\ncurrent priority: %d\n",
              current_class, current_priority);
      usage (0);
#else
      printf ("%d\n", current_priority);
#endif
      exit (0);
    }

#ifndef NICE_PRIORITY
  errno = 0;
  current_priority = GET_PRIORITY ();
  if (current_priority == -1 && errno != 0)
    error (1, errno, "cannot get priority");
  if (setpriority (PRIO_PROCESS, 0, current_priority + adjustment))
#else
  if (nice (adjustment) == -1)
#endif
    error (1, errno, "cannot set priority");

  execvp (argv[optind], &argv[optind]);
  error (errno == ENOENT ? 127 : 126, errno, "%s", argv[optind]);
}

/* Return nonzero if S represents a (possibly signed) decimal integer,
   zero if not. */

static int
isinteger (s)
     char *s;
{
  if (*s == '-' || *s == '+')
    ++s;
  if (*s == 0)
    return 0;
  while (*s)
    {
      if (!ISDIGIT (*s))
	return 0;
      ++s;
    }
  return 1;
}

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      print_version("nice");
      printf ("Usage: %s [OPTION]... [COMMAND [ARG]...]\n", program_name);
#ifdef OS2
      printf ("\
\n\
  -ADJUST                   increment priority by ADJUST\n\
  -n, --adjustment=ADJUST   same as -ADJUST\n\
\n\
  -i, --idle-time	    idle-time class\n\
  -r, --regular		    regular class\n\
  -t, --time-critical	    time-critical class\n\
  -f, --foreground-server   foreground-server class\n\
\n\
Note that this program follows the Unix style which is that\n\
negative adjustments increase the priority while positive\n\
adjustments decrease the priority. Normally, all OS/2 processes\n\
run at the lowest priority in their class already, so only\n\
negative values make sense unless priority has been increased\n\
previously, to allow a decrease later. Range of ADJUST goes from\n\
-31 to 31. With no COMMAND, print the current scheduling priority.\n\
");
#else
      printf ("\
\n\
  -ADJUST                   increment priority by ADJUST first\n\
  -n, --adjustment=ADJUST   same as -ADJUST\n\
      --help                display this help and exit\n\
      --version             output version information and exit\n\
\n\
With no COMMAND, print the current scheduling priority.  ADJUST is 10\n\
by default.  Range goes from -20 (highest priority) to 19 (lowest).\n\
");
#endif
    }
  exit (status);
}
