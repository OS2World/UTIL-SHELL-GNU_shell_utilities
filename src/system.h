/* system-dependent definitions for shellutils programs.
   Copyright (C) 89, 91, 92, 93, 1994 Free Software Foundation, Inc.

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

#include <sys/types.h>
#include <sys/stat.h>

#ifdef STAT_MACROS_BROKEN
#undef S_ISBLK
#undef S_ISCHR
#undef S_ISDIR
#undef S_ISFIFO
#undef S_ISLNK
#undef S_ISMPB
#undef S_ISMPC
#undef S_ISNWK
#undef S_ISREG
#undef S_ISSOCK
#endif /* STAT_MACROS_BROKEN.  */

#ifndef S_IFMT
#define S_IFMT 0170000
#endif
#if !defined(S_ISBLK) && defined(S_IFBLK)
#define	S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif
#if !defined(S_ISCHR) && defined(S_IFCHR)
#define	S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#if !defined(S_ISDIR) && defined(S_IFDIR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
#define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISFIFO) && defined(S_IFIFO)
#define	S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#endif
#if !defined(S_ISLNK) && defined(S_IFLNK)
#define	S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define	S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#if !defined(S_ISMPB) && defined(S_IFMPB) /* V7 */
#define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
#define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
#endif
#if !defined(S_ISNWK) && defined(S_IFNWK) /* HP/UX */
#define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef OS2

#include <process.h>
#include <stdlib.h>
#include <pwd.h>

#ifdef NICE
#define execvp(n, a)        {int rc = spawnvp(P_WAIT, n, a); if (rc != -1) exit(rc);}
#else
#define execvp(n, a)        spawnvp(P_OVERLAY, n, a)
#endif

#define PATH_MAX            260
#define NAME_MAX            256

#define getgid()            0
#define getuid()            0
#define getegid()           0
#define geteuid()           0
#define getpwnam(x)         (dummy_pw.pw_name = getlogin(), &dummy_pw)
#define getpwuid(x)         (dummy_pw.pw_name = getlogin(), &dummy_pw)
#define getgrgid(x)         (&dummy_gr)
#define getgroups(n,p)      (*p = 0, 1)
#define getugroups(n,p,u)   (*p = 0, 1)
#define getlogin()          (getenv("LOGNAME") ? getenv("LOGNAME") : "root")

struct group {
  char *gr_name;
  gid_t gr_uid;
};

static struct group dummy_gr = {"root", 0};
static struct passwd dummy_pw = {"root", 0, 0};

#define ttyname(arg)        (isatty(0) ? "con" : NULL)

#define main os2main

#else /* not OS2 */

#ifndef _POSIX_VERSION
char *getlogin ();
char *ttyname ();
off_t lseek ();
uid_t geteuid ();
#endif /* _POSIX_VERSION */

#endif /* OS2 */

#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#include <string.h>
#ifndef index
#define index strchr
#endif
#ifndef rindex
#define rindex strrchr
#endif
#ifndef bcopy
#define bcopy(from, to, len) memcpy ((to), (from), (len))
#endif
#ifndef bzero
#define bzero(s, n) memset ((s), 0, (n))
#endif
#else
#include <strings.h>
#endif

#include <errno.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#else
char *getenv ();
extern int errno;
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif

#include <ctype.h>

#include "version.h"
#define print_version(x) \
  fprintf(stderr, "\nGNU %s, %s\n\n", x, version_string + 4)

#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
#define ISASCII(c) 1
#else
#define ISASCII(c) isascii(c)
#endif

#ifdef isblank
#define ISBLANK(c) (ISASCII (c) && isblank (c))
#else
#define ISBLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifdef isgraph
#define ISGRAPH(c) (ISASCII (c) && isgraph (c))
#else
#define ISGRAPH(c) (ISASCII (c) && isprint (c) && !isspace (c))
#endif

#define ISPRINT(c) (ISASCII (c) && isprint (c))
#define ISDIGIT(c) (ISASCII (c) && isdigit (c))
#define ISALNUM(c) (ISASCII (c) && isalnum (c))
#define ISALPHA(c) (ISASCII (c) && isalpha (c))
#define ISCNTRL(c) (ISASCII (c) && iscntrl (c))
#define ISLOWER(c) (ISASCII (c) && islower (c))
#define ISPUNCT(c) (ISASCII (c) && ispunct (c))
#define ISSPACE(c) (ISASCII (c) && isspace (c))
#define ISUPPER(c) (ISASCII (c) && isupper (c))
#define ISXDIGIT(c) (ISASCII (c) && isxdigit (c))
