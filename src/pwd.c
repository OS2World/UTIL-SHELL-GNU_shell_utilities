/* pwd - print current directory
   Copyright (C) 1994 Free Software Foundation, Inc.

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

/* Jim Meyering <meyering@comco.com> */

#include <config.h>
#include <stdio.h>
#include <sys/types.h>

#include "system.h"
#include "version.h"
#include "long-options.h"

void error ();
char *xgetcwd ();

/* The name this program was run with. */
char *program_name;

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      print_version("pwd");
      printf ("Usage: %s [OPTION]\n", program_name);
      printf ("\
\n\
  --help      display this help and exit\n\
  --version   output version information and exit\n\
");
    }
  exit (status);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  char *wd;

  program_name = argv[0];

  parse_long_options (argc, argv, "pwd", version_string, usage);

  if (argc != 1)
    error (0, 0, "ignoring non-option arguments");

  wd = xgetcwd ();
  if (wd == NULL)
    error (1, errno, "cannot get current directory");
  printf ("%s\n", wd);

  exit (0);
}
