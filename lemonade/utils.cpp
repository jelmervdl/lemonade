
#include "utils.h"
#include <chrono>
#include <cstring>
#include <ctime>

std::string currentTime() {
  // https://stackoverflow.com/a/16358264/4565794
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
  return std::string(buffer);
}