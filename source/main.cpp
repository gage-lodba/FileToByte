#include <imgui.h>

#include <iostream>

#include "Application.h"
#include "Fonts.h"
#include "UserInterface.h"

namespace {

// FileToByte's dark theme. Replaces VulkanWindow's built-in default via
// Application::setStyleCallback.
void applyStyle() {
  const ImColor Dim = ImColor(32, 32, 32);
  const ImColor Faint = ImColor(40, 40, 40);
  const ImColor Pale = ImColor(48, 48, 48);
  const ImColor Soft = ImColor(56, 56, 56);

  ImGui::StyleColorsDark();

  ImGuiStyle *style = &ImGui::GetStyle();
  style->Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24);
  style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
  style->Colors[ImGuiCol_TextSelectedBg] = ImColor(0, 127, 255, 127);
  style->Colors[ImGuiCol_TextDisabled] = ImColor(127, 127, 127);
  style->Colors[ImGuiCol_FrameBg] = Dim;
  style->Colors[ImGuiCol_FrameBgHovered] = Faint;
  style->Colors[ImGuiCol_FrameBgActive] = Pale;
  style->Colors[ImGuiCol_ScrollbarBg] = Dim;
  style->Colors[ImGuiCol_ScrollbarGrab] = Faint;
  style->Colors[ImGuiCol_ScrollbarGrabHovered] = Pale;
  style->Colors[ImGuiCol_ScrollbarGrabActive] = Soft;
  style->Colors[ImGuiCol_Button] = Dim;
  style->Colors[ImGuiCol_ButtonHovered] = Faint;
  style->Colors[ImGuiCol_ButtonActive] = Pale;

  style->WindowRounding = 0.0f;
  style->FrameRounding = 0.0f;
  style->WindowBorderSize = 0.0f;
  style->WindowPadding = ImVec2(10, 10);
  style->FramePadding = ImVec2(5, 5);
  style->ItemSpacing = ImVec2(5, 10);
  // Colours are authored as plain sRGB values; VulkanWindow's default Unorm
  // swap-chain presents them verbatim, so no gamma conversion is needed.
}

}  // namespace

int main() {
  try {
    Application app(800, 600, true, "File To Byte");

    app.setFontCallback(LoadFonts);
    app.setStyleCallback(applyStyle);

    UserInterface ui;
    app.setUICallback([&ui] { ui.render(); });

    app.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
