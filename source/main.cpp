#include "application.hpp"

#ifdef _WIN32
#include <Windows.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  Application app;

  app.ApplyTheme();
  app.Run();

  return 0;
}

#else

int main(int, char **) {
  Application app;

  app.ApplyTheme();
  app.Run();

  return 0;
}

#endif