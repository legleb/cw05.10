#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>
#include <cstdio>

DWORD send(DWORD& err, HANDLE wr, const char* b, DWORD k) {
  DWORD r = 0;
  bool st = true;
  DWORD h = 0;
  while (r < k) {
      st = WriteFile(wr, b + r, k - r, &h, NULL);
      if (!st) {
          err = GetLastError();
          break;
      }
      r += h;
  }
  return r;
}

int main(int argc, char ** argv)
{
  std::string s;
  std::getline(std::cin, s);
  size_t size = s.length();
  HANDLE read, write;
  SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};
  if (!CreatePipe(&read, &write, &sa, 256)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }
  SetHandleInformation(read, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
  PROCESS_INFORMATION pi = {};
  STARTUPINFO si = {
    .cb = sizeof(si),
    .dwFlags = STARTF_USESTDHANDLES,
    .hStdInput = read,
    .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
    .hStdError = GetStdHandle(STD_ERROR_HANDLE)
  };
  std::string cmd = std::string(argv[1]) + ' ' + std::to_string(static_cast< DWORD_PTR >(read));
  if (!CreateProcessA(argv[1], cmd.data(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
    std::cerr << GetLastError() << "\n";
    return 1;
  }
  CloseHandle(read);
  char len_buf[256] = {};
  sprintf(len_buf, "%zu\n", size);
  DWORD err = 0, k = static_cast< DWORD >(strlen(len_buf));
  if (send(err, write, len_buf, k) != k) {
    std::cerr << err << "\n";
    CloseHandle(write);
    return 1;
  }
  std::string new_s = s + '\n';
  if (send(err, write, new_s.data(), static_cast< DWORD >(size + 1)) != size + 1) {
    std::cerr << err << "\n";
    CloseHandle(write);
    return 1;
  }
  CloseHandle(write);
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return 0;
}
