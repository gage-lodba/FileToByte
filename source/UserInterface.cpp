#include "UserInterface.h"
#include "FTB.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Fonts.h"

#include <string>

// File-local UI state. Internal linkage is important: a previous external
// `bool open` collided with libc's open() symbol and corrupted unrelated
// ::open() calls (e.g. VulkanWindow's pipeline-cache save) at link time.
namespace {
bool open = true;

std::string VarName;
std::string FilePath;
std::string Result;
Encoding encoding = Encoding::U8;
} // namespace

void UserInterface::render() const { renderMainWindow(); }

void UserInterface::renderMainWindow() const {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

  ImGui::Begin("File To Byte", &open, window_flags);

  ImGui::Columns(2, "locations", false);

  ImGui::Text("Array Name");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##Array Name", &VarName);
  ImGui::PopItemWidth();

  ImGui::NextColumn();

  ImGui::Text("File Path");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##File Path", &FilePath);
  ImGui::PopItemWidth();

  ImGui::Columns();

  // Convert spans the full window width, below both input fields. Require both
  // a symbol and a path: an empty Array Name would emit a reserved
  // leading-underscore identifier, and an empty path can't be opened.
  const bool canConvert = !VarName.empty() && !FilePath.empty();
  ImGui::BeginDisabled(!canConvert);
  if (ImGui::Button("Convert", ImVec2(-1, 25.f)))
    Result = Convert(FilePath, VarName, encoding);
  ImGui::EndDisabled();

  // Output encoding — mutually exclusive, so radio buttons rather than
  // checkboxes. Drives the Encoding argument passed to Convert above.
  ImGui::Text("Encoding");
  constexpr float radioSpacing = 25.0f;
  if (ImGui::RadioButton("U8", encoding == Encoding::U8))
    encoding = Encoding::U8;
  ImGui::SameLine(0.0f, radioSpacing);
  if (ImGui::RadioButton("U32", encoding == Encoding::U32))
    encoding = Encoding::U32;
  ImGui::SameLine(0.0f, radioSpacing);
  if (ImGui::RadioButton("Base85", encoding == Encoding::Base85))
    encoding = Encoding::Base85;

  ImGui::Text("Output");
  // Reserve room at the bottom for the Copy button (its height + one spacing)
  // so the result box fills the rest without pushing the button off-screen.
  const float footerHeight = 25.0f + ImGui::GetStyle().ItemSpacing.y;
  ImGui::PushFont(Consolas);
  // Read-only: the box is a display for generated output, so a stray keystroke
  // shouldn't be able to edit what Copy then grabs.
  ImGui::InputTextMultiline("##Result", &Result, ImVec2(-1, -footerHeight),
                            ImGuiInputTextFlags_ReadOnly);
  ImGui::PopFont();

  if (ImGui::Button("Copy to clipboard", ImVec2(-1, 25.f)))
    ImGui::SetClipboardText(Result.c_str());

  ImGui::End();
}
