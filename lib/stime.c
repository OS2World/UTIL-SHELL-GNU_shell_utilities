/* stime -- set the system clock
   Copyright (C) 1989, 1991 Free Software Foundation, Inc.

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

/* David MacKenzie <djm@ai.mit.edu> */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>

/* Set the system time to *WHEN seconds past the start of 1970 GMT. */

#ifndef OS2

int
stime (when)
     time_t *when;
{
  struct timeval tv;

  tv.tv_sec = *when;
  tv.tv_usec = 0;
  return settimeofday (&tv, (struct timezone *) 0);
}

#else

#include <os2.h>

int stime(time_t *newtime)
{
  struct tm *newtm = localtime(newtime);
  DATETIME dt;

  dt.hours   = newtm -> tm_hour;
  dt.minutes = newtm -> tm_min;
  dt.seconds = newtm -> tm_sec;
  dt.hundredths = 0;

  dt.day     = newtm -> tm_mday;
  dt.month   = newtm -> tm_mon + 1;
  dt.year    = newtm -> tm_year + 1900;
  dt.weekday = newtm -> tm_wday;

  dt.timezone = -1;

  return DosSetDateTime(&dt) ? -1 : 0;
}

#endif
