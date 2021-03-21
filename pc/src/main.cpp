#include "microPC.h"
#include <string>

int main(int argc, char *argv[])
{
  bool debug = false;
  if (argc > 1)
  {
    debug = argv[1] == std::string("-d") || argv[1] == std::string("-D");
  }

  microPC::PowerOn(debug);
  return 0;
}