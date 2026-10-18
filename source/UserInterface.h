#pragma once

#include <string>

#include "FTB.h"

// FileToByte's main window. Owns all UI state (no file-scope mutables) so
// render() may update it and the class is safe to instantiate more than once.
class UserInterface {
 public:
  UserInterface() noexcept = default;
  ~UserInterface() = default;

  UserInterface(const UserInterface &) = delete;
  UserInterface &operator=(const UserInterface &) = delete;

  void render();

 private:
  void renderMainWindow();
  void convert();
  void saveToFile();

  // User inputs.
  std::string array_name_;
  std::string file_path_;
  std::string output_path_;
  Encoding encoding_ = Encoding::U8;

  // Latest outcome. `output_` only ever holds generated source: failures are
  // reported through `status_` and never displayed in the read-only output box.
  std::string output_;
  std::string status_;
  bool status_is_error_ = false;
  bool has_output_ = false;
};
