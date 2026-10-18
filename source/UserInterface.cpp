#include "UserInterface.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include "Fonts.h"

namespace {

// Height of the full-width action buttons (Convert / Copy to clipboard /
// Save to file) so the result box's reserved footer is exact.
constexpr float kButtonHeight = 25.0f;

// Horizontal gap between the encoding radio buttons.
constexpr float kRadioSpacing = 25.0f;

// Status line colours: red for conversion/save errors, green for successes.
constexpr ImVec4 kErrorColor = ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
constexpr ImVec4 kSuccessColor = ImVec4(0.45f, 0.85f, 0.55f, 1.0f);

}  // namespace

void UserInterface::render() { renderMainWindow(); }

void UserInterface::renderMainWindow() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  constexpr ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

  // No p_open: the window has no title bar and cannot be closed by the user.
  if (!ImGui::Begin("File To Byte", nullptr, window_flags)) {
    ImGui::End();
    return;
  }

  ImGui::Columns(2, "locations", false);

  ImGui::Text("Array Name");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##Array Name", &array_name_);
  ImGui::PopItemWidth();

  ImGui::NextColumn();

  ImGui::Text("File Path");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##File Path", &file_path_);
  ImGui::PopItemWidth();

  ImGui::Columns();

  // Encoding is selected before the Convert button so the output can never be
  // stale relative to the selected encoding.
  ImGui::Text("Encoding");
  bool encoding_changed = false;
  if (ImGui::RadioButton("U8", encoding_ == Encoding::U8)) {
    encoding_ = Encoding::U8;
    encoding_changed = true;
  }
  ImGui::SameLine(0.0f, kRadioSpacing);
  if (ImGui::RadioButton("U32", encoding_ == Encoding::U32)) {
    encoding_ = Encoding::U32;
    encoding_changed = true;
  }
  ImGui::SameLine(0.0f, kRadioSpacing);
  if (ImGui::RadioButton("Base85", encoding_ == Encoding::Base85)) {
    encoding_ = Encoding::Base85;
    encoding_changed = true;
  }

  // Convert spans the full window width. Require both a symbol and a path: an
  // empty Array Name is not a valid C identifier, and an empty path cannot be
  // opened.
  const bool can_convert = !array_name_.empty() && !file_path_.empty();
  ImGui::BeginDisabled(!can_convert);
  const bool convert_clicked =
      ImGui::Button("Convert", ImVec2(-1, kButtonHeight));
  ImGui::EndDisabled();

  // Re-run with the same inputs when the encoding changes after a successful
  // conversion, so the output always matches the selected encoding.
  if (convert_clicked || (encoding_changed && has_output_)) convert();

  // Errors and confirmations live here, never in the read-only output box.
  if (!status_.empty()) {
    ImGui::TextColored(status_is_error_ ? kErrorColor : kSuccessColor, "%s",
                       status_.c_str());
  }

  ImGui::Text("Output Path");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##Output Path", &output_path_);
  ImGui::PopItemWidth();

  // The result box comes last, so a negative height means "the rest of the
  // window minus this footer" (ImGui::CalcItemSize). Reserve the Copy and Save
  // rows plus the spacing between them so neither is pushed off-screen.
  const float footer_height =
      2.0f * (kButtonHeight + ImGui::GetStyle().ItemSpacing.y);

  // Read-only: the box is a display for generated output, so a stray keystroke
  // cannot edit what Copy/Save then use.
  ImGui::PushFont(Mono, Mono ? Mono->LegacySize : 0.0f);
  ImGui::InputTextMultiline("##Result", &output_, ImVec2(-1, -footer_height),
                            ImGuiInputTextFlags_ReadOnly);
  ImGui::PopFont();

  ImGui::BeginDisabled(!has_output_);
  if (ImGui::Button("Copy to clipboard", ImVec2(-1, kButtonHeight)))
    ImGui::SetClipboardText(output_.c_str());
  ImGui::EndDisabled();

  // Saving also needs a destination path; empty paths are rejected here rather
  // than surfacing an ofstream error.
  ImGui::BeginDisabled(!has_output_ || output_path_.empty());
  if (ImGui::Button("Save to file", ImVec2(-1, kButtonHeight))) saveToFile();
  ImGui::EndDisabled();

  ImGui::End();
}

void UserInterface::convert() {
  ConvertResult result = Convert(file_path_, array_name_, encoding_);
  if (!result.ok()) {
    // Clear any previous output so a failure can never leave stale source that
    // Copy/Save would happily use.
    output_.clear();
    has_output_ = false;
    status_ = result.message();
    status_is_error_ = true;
    return;
  }

  output_ = std::move(result.source);
  has_output_ = true;
  status_ = "Converted " + std::to_string(output_.size()) + " bytes of source";
  status_is_error_ = false;
}

void UserInterface::saveToFile() {
  if (!has_output_ || output_path_.empty()) return;

  // Build the path from the UTF-8 bytes so the platform filesystem layer opens
  // it through the native wide (Windows) / UTF-8 (POSIX) entry point instead of
  // the ANSI code page.
  const std::filesystem::path path(
      std::u8string(output_path_.begin(), output_path_.end()));

  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file.is_open()) {
    status_ = "Failed to open the output file";
    status_is_error_ = true;
    return;
  }

  file.write(output_.data(), static_cast<std::streamsize>(output_.size()));
  file.close();
  if (!file) {
    status_ = "Failed to write the output file";
    status_is_error_ = true;
    return;
  }

  status_ =
      "Saved " + std::to_string(output_.size()) + " bytes to " + output_path_;
  status_is_error_ = false;
}
