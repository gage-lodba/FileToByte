#include "Fonts.h"

#include "fonts/Consolas.h"
#include "fonts/Roboto.h"

ImFont *Roboto = nullptr;
ImFont *Consolas = nullptr;

void LoadFonts() {
  ImGuiIO &io = ImGui::GetIO();

  // The compressed TTF blobs are static and outlive the font atlas, so let
  // ImGui reference them in place rather than copying.
  ImFontConfig fontConfig;
  fontConfig.FontDataOwnedByAtlas = false;

  Roboto = io.Fonts->AddFontFromMemoryCompressedTTF(
      Roboto_compressed_data, Roboto_compressed_size, 14.f, &fontConfig);
  Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(
      Consolas_compressed_data, Consolas_compressed_size, 14.f, &fontConfig);

  io.FontDefault = Roboto;
}
