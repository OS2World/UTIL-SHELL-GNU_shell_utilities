/* tee - read from standard input and write to standard output and files.
   Copyright (C) 85, 90, 91, 92, 93, 1994 Free Software Foundation, Inc.

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

/* Mike Parker, Richard M. Stallman, and David MacKenzie */

#include <config.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <getopt.h>

#include "system.h"
#include "version.h"

char *xmalloc ();
void error ();
int full_write ();

static int tee ();

/* If nonzero, append to output files rather than truncating them. */
static int append;

/* If nonzero, ignore interrupts. */
static int ignore_interrupts;

/* The name that this program was run with. */
char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help;

/* If non-zero, print the version on standard output and exit.  */
static int show_version;

static struct option const long_options[] =
{
  {"append", no_argument, NULL, 'a'},
  {"help", no_argument, &show_help, 1},
  {"ignore-interrupts", no_argument, NULL, 'i'},
  {"version", no_argument, &show_version, 1},
  {NULL, 0, NULL, 0}
};

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      print_version("tee");
      printf ("Usage: %s [OPTION]... [FILE]...\n", program_name);
      printf ("\
\n\
  -a, --append              append to the given FILEs, do not overwrite\n\
  -i, --ignore-interrupts   ignore interrupt signals\n\
      --help                display this help and exit\n\
      --version             output version information and exit\n\
");
    }
  exit (status);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  int errs;
  int optc;
	
  program_name = argv[0];
  append = 0;
  ignore_interrupts = 0;

  while ((optc = getopt_long (argc, argv, "ai", long_options, (int *) 0))
	 != EOF)
    {
      switch (optc)
	{
	case 0:
	  break;

	case 'a':
	  append = 1;
	  break;

	case 'i':
	  ignore_interrupts = 1;
	  break;

	default:
	  usage (1);
	}
    }

  if (show_version)
    {
      printf ("tee - %s\n", version_string);
      exit (0);
    }

  if (show_help)
    usage (0);

  if (isatty(0))
    usage(0);

  if (ignore_interrupts)
    {
#ifdef _POSIX_SOURCE
      struct sigaction sigact;

      sigact.sa_handler = SIG_IGN;
      sigemptyset (&sigact.sa_mask);
      sigact.sa_flags = 0;
      sigaction (SIGINT, &sigact, NULL);
#else				/* !_POSIX_SOURCE */
      signal (SIGINT, SIG_IGN);
#endif				/* _POSIX_SOURCE */
    }

  errs = tee (argc - optind, (const char **) &argv[optind]);
  if (close (0) != 0)
    error (1, errno, "standard input");
  if (close (1) != 0)
    error (1, errno, "standard output");
  exit (errs);
}

/* Copy the standard input into each of the NFILES files in FILES
   and into the standard output.
   Return 0 if successful, 1 if any errors occur. */

static int
tee (nfiles, files)
     int nfiles;
     const char **files;
{
  int *descriptors;
  char buffer[BUFSIZ];
  register int bytes_read, i, ret = 0, mode;

  descriptors = (int *) xmalloc ((nfiles + 1) * sizeof (int));

#ifdef OS2
  setmode(0, O_BINARY);
  setmode(1, O_BINARY);
#endif

  mode = O_WRONLY | O_CREAT | O_BINARY;
  if (append)
    mode |= O_APPEND;
  else
    mode |= O_TRUNC;

  /* Move all the names `up' one in the argv array to make room for
     the entry for standard output.  This writes into argv[argc].  */
  for (i = nfiles; i >= 1; i--)
    files[i] = files[i - 1];

  /* In the array of NFILES + 1 descriptors, make
     the first one correspond to standard output.   */
  descriptors[0] = 1;
  files[0] = "standard output";

  for (i = 1; i <= nfiles; i++)
    {
      descriptors[i] = open (files[i], mode, 0666);
      if (descriptors[i] == -1)
	{
	  error (0, errno, "%s", files[i]);
	  ret = 1;
	}
    }

  while (1)
    {
      bytes_read = read (0, buffer, sizeof buffer);
#ifdef EINTR
      if (bytes_read < 0 && errno == EINTR)
        continue;
#endif
      if (bytes_read <= 0)
	break;

      /* Write to all NFILES + 1 descriptors.
	 Standard output is the first one.  */
      for (i = 0; i <= nfiles; i++)
	{
	  if (descriptors[i] != -1
	      && full_write (descriptors[i], buffer, bytes_read) < 0)
	    {
	      error (0, errno, "%s", files[i]);
	      /* Don't close stdout.  That's done in main.  */
	      if (descriptors[i] != 1)
		close (descriptors[i]);
	      descriptors[i] = -1;
	      ret = 1;
	    }
	}
    }

  if (bytes_read == -1)
    {
      error (0, errno, "read error");
      ret = 1;
    }

  /* Close the files, but not standard output.  */
  for (i = 1; i <= nfiles; i++)
    if (descriptors[i] != -1 && close (descriptors[i]) != 0)
      {
	error (0, errno, "%s", files[i]);
	ret = 1;
      }

  free (descriptors);

  return ret;
}
