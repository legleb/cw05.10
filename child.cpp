#include <iostream>
#include <string>

int main(int argc, char ** argv)
{
  size_t size = 0;
  if (!(std::cin >> size)) {
    std::cerr << "Failed\n";
    return 1;
  }
  std::cin.ignore();
  std::string msg;
  std::getline(std::cin, msg);
  std::cout << msg << "\n";
  return 0;
}