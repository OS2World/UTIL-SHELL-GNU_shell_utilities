/* basename.c -- return the last element in a path
   Copyright (C) 1990 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#include <string.h>
#ifndef rindex
#define rindex strrchr
#endif
#else
#include <strings.h>
#endif

/* Return NAME with any leading path stripped off.  */

char *
basename (name)
     char *name;
{
  char *base;

  base = rindex (name, '/');
#ifdef OS2
  if (!base)
    base = rindex (name, ':');
#endif
  return base ? base + 1 : name;
}
