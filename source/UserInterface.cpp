#include "UserInterface.h"
#include "FTB.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "ImGuiManager.h"

#include <string>

bool open = true;

std::string VarName;
std::string FilePath;
std::string Result;

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

  if (ImGui::Button("Convert", ImVec2(-1, 25.f)))
    Result = Convert(FilePath, VarName);

  ImGui::NextColumn();

  ImGui::Text("File Path");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##File Path", &FilePath);
  ImGui::PopItemWidth();

  if (ImGui::Button("Copy to clipboard", ImVec2(-1, 25.f))) {
    ImGui::LogToClipboard();
    ImGui::LogText("%s", Result.c_str());
    ImGui::LogFinish();
  }

  ImGui::Columns();

  ImGui::PushFont(Consolas);
  ImGui::InputTextMultiline("##Result", &Result, ImVec2(-1, -1));
  ImGui::PopFont();
  ImGui::End();
}
