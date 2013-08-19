#include <stdlib.h>
#include <utsname.h>

uname(struct utsname *un)
{
  static char version[16];
  char *host = getenv("HOSTNAME");
  sprintf(version, "%d.%02d", _osmajor / 10, _osminor);
  strcpy(un->sysname,
         (_osmode == DOS_MODE) && (_osmajor < 10) ? "MS-DOS" : "OS/2");
  strcpy(un->nodename, host ? host : "standalone");
  strcpy(un->release, (_osmajor < 20) ? "1" : "2");
  strcpy(un->version, version);
  strcpy(un->machine, (_osmajor < 20) ? "i286" : "i386");
}
