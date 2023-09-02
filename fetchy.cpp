#include "fetchy.h"
#include "color.h"
#include "ascii.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <unistd.h>
#include <sys/utsname.h>

namespace fetchy {
Fetchy::Fetchy() { Output(); }

std::string Fetchy::GetDistro() {
  char delimiter = '"';
  std::ifstream file("/etc/os-release");
  std::string line;
  std::regex regex("^NAME=\"(.*?)\"$");
  std::smatch match;
  std::string name;

  while (std::getline(file, line)) {
    if (std::regex_search(line, match, regex)) {
      name = BetweenDelimiter(match[0].str(), delimiter);
      break;
    }
  }
  return name;
}

std::string Fetchy::GetArchitechture() {
  struct utsname os_info {};
  uname(&os_info);
  return os_info.machine;
}

std::string Fetchy::GetDeviceName() {
  std::ifstream file("/sys/devices/virtual/dmi/id/product_version");
  std::string name;
  std::getline(file, name);
  return name;
}

std::string Fetchy::GetCPU() {
  std::ifstream file("/proc/cpuinfo");
  std::string line;

  while(std::getline(file, line)) {
    if (line.find("model name") != std::string::npos) {
      size_t pos = line.find(": ");
      std::string name = line.erase(0, pos + 2);
      return name;
    }
  }
  return "No CPU";
}

std::string Fetchy::GetMemory() {
  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  int divisor = 1024;
  unsigned long long int total_memory = pages * page_size;
  int total_memory_gb = total_memory / 1024 / 1024 / 1024;
  return std::to_string(total_memory_gb + 1) + "GB";
}

std::string Fetchy::GetKernel() {
  struct utsname os_info {};
  uname(&os_info);
  return os_info.release;
}

std::string Fetchy::GetColorTag(int color) {
  std::string icon1 = "";
  std::string icon2 = "█";
  std::string icon3 = "";
  std::string tag = Color::AnsiEscape(color) + icon1 + icon2 + Color::reset;
  tag += Color::AnsiEscape(color + Color::to_bright, color + Color::to_bg) +
         icon1 + Color::reset;
  tag += Color::AnsiEscape(color + Color::to_bright) + icon2 + icon3 +
         Color::reset;
  tag += " ";
  return tag;
}

std::string Fetchy::GetGridRow(std::string item1, std::string item2, int column_width) {
  int spacing_count = column_width - item1.length() + 40;
  std::string spacing = "";

  for (int i = 0; i < spacing_count; i++) {
    spacing += " ";
  }

  return item1 + spacing + item2;
}

std::string Fetchy::BetweenDelimiter(std::string str, char delimiter) {
  size_t first = str.find_first_of(delimiter) + 1;
  size_t last = str.find_last_of(delimiter);

  return str.substr(first, last - first);
}

void Fetchy::Output() {
  std::cout << "\n"
    << ASCII::arch_ascii << "\n" 

    << GetGridRow(GetColorTag(Color::black) + GetDistro() + " " + GetArchitechture(),
      GetColorTag(Color::blue) + GetDistro() + " " + GetArchitechture(), 36)

    << "\n\n"  

    << GetGridRow(GetColorTag(Color::red) + GetKernel(),
      GetColorTag(Color::magenta) + GetCPU(), 36)    

    << "\n\n"

    << GetGridRow(GetColorTag(Color::green) + GetDeviceName(),
      GetColorTag(Color::cyan) + GetDistro() + " " + GetArchitechture(), 36)

    << "\n\n"  

    << GetGridRow(GetColorTag(Color::yellow) + GetMemory(),
      GetColorTag(Color::white) + GetDistro() + " " + GetArchitechture(), 36)    

    << "\n"      
    
    << std::endl;
}
} // namespace fetchy
