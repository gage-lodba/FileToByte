#include "Fonts.h"

#include "fonts/JetBrainsMono.h"
#include "fonts/Roboto.h"

ImFont *Roboto = nullptr;
ImFont *Mono = nullptr;

void LoadFonts() {
  ImGuiIO &io = ImGui::GetIO();

  // ImGui never references the static compressed blobs in place:
  // AddFontFromMemoryCompressedTTF always decompresses the TTF into a fresh
  // IM_ALLOC() buffer and forces FontDataOwnedByAtlas = true, so the font atlas
  // owns the decompressed data and no ImFontConfig override is needed here.
  Roboto = io.Fonts->AddFontFromMemoryCompressedTTF(
      Roboto_compressed_data, Roboto_compressed_size, 14.f);
  Mono = io.Fonts->AddFontFromMemoryCompressedTTF(
      JetBrainsMono_compressed_data, JetBrainsMono_compressed_size, 14.f);

  io.FontDefault = Roboto;
}
