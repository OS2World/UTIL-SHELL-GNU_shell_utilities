/* tty -- print the path of the terminal connected to standard input
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

/* Displays "not a tty" if stdin is not a terminal.
   Displays nothing if -s option is given.
   Exit status 0 if stdin is a tty, 1 if not, 2 if usage error,
   3 if write error.

   Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#include <config.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>

#include "system.h"
#include "version.h"

void error ();

static void usage ();

/* The name under which this program was run. */
char *program_name;

/* If nonzero, return an exit status but produce no output. */
static int silent;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output and exit.  */
static int show_version;

static struct option const longopts[] =
{
  {"help", no_argument, &show_help, 1},
  {"silent", no_argument, NULL, 's'},
  {"quiet", no_argument, NULL, 's'},
  {"version", no_argument, &show_version, 1},
  {NULL, 0, NULL, 0}
};

void
main (argc, argv)
     int argc;
     char **argv;
{
  char *tty;
  int optc;

  program_name = argv[0];
  silent = 0;

  while ((optc = getopt_long (argc, argv, "s?", longopts, (int *) 0)) != EOF)
    {
      switch (optc)
	{
	case 0:
	  break;

	case 's':
	  silent = 1;
	  break;

	default:
	  usage (0);
	}
    }

  if (show_version)
    {
      printf ("tty - %s\n", version_string);
      exit (0);
    }

  if (show_help)
    usage (0);

  if (optind != argc)
    usage (2);

  tty = ttyname (0);
  if (!silent)
    {
      if (tty)
	puts (tty);
      else
	puts ("not a tty");

      if (ferror (stdout) || fclose (stdout) == EOF)
	error (3, errno, "standard output");
    }

  exit (isatty (0) ? 0 : 1);
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
      print_version("tty");
      printf ("Usage: %s [OPTION]...\n", program_name);
      printf ("\
\n\
  -s, --silent, --quiet   print nothing, only return an exit status\n\
      --help              display this help and exit\n\
      --version           output version information and exit\n\
");
    }
  exit (status);
}
