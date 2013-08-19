#include <config.h>
#include "version.h"
const char *version_string = "GNU sh-utils 1.12";

#ifdef __EMX__

#undef main

int main(int argc, char **argv)
{
  _wildcard(&argc, &argv);
  _response(&argc, &argv);
  set_program_name(argv[0]);
  tzset();
  return os2main(argc, argv);
}

#endif
