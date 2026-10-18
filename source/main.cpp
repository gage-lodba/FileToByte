#include <imgui.h>

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>

#include "Application.h"
#include "Fonts.h"
#include "UserInterface.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

// Converts an sRGB byte colour to ImGui's float representation. ImColor is
// marked obsolete in ImGui 1.92, so build explicit ImVec4 values instead.
[[nodiscard]] constexpr ImVec4 rgba(int r, int g, int b, int a = 255) {
  return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
          static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f};
}

// FileToByte's dark theme. Replaces VulkanWindow's built-in default via
// Application::setStyleCallback.
void applyStyle() {
  const ImVec4 Dim = rgba(32, 32, 32);
  const ImVec4 Faint = rgba(40, 40, 40);
  const ImVec4 Pale = rgba(48, 48, 48);
  const ImVec4 Soft = rgba(56, 56, 56);

  ImGui::StyleColorsDark();

  ImGuiStyle *style = &ImGui::GetStyle();
  style->Colors[ImGuiCol_WindowBg] = rgba(24, 24, 24);
  style->Colors[ImGuiCol_Text] = rgba(255, 255, 255);
  style->Colors[ImGuiCol_TextSelectedBg] = rgba(0, 127, 255, 127);
  style->Colors[ImGuiCol_TextDisabled] = rgba(127, 127, 127);
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

// Reports a fatal startup error. stderr is invisible in the GUI-subsystem build
// when the app is launched from Explorer, so on Windows also show a dialog.
void reportFatalError(const char *detail) {
  std::cerr << "Fatal error: " << detail << std::endl;
#ifdef _WIN32
  std::wstring message = L"File To Byte failed to start.";
  const int length = MultiByteToWideChar(CP_UTF8, 0, detail, -1, nullptr, 0);
  if (length > 0) {
    // The returned length includes the terminating NUL, so drop it after the
    // conversion.
    std::wstring wide(static_cast<std::size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, detail, -1, wide.data(), length);
    wide.resize(static_cast<std::size_t>(length) - 1u);
    message += L"\n\n";
    message += wide;
  }
  MessageBoxW(nullptr, message.c_str(), L"File To Byte", MB_OK | MB_ICONERROR);
#endif
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
    reportFatalError(e.what());
    return 1;
  } catch (...) {
    reportFatalError("Unknown fatal error");
    return 1;
  }

  return 0;
}
