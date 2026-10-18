#pragma once
#include <imgui.h>

// Embedded UI fonts, loaded once at startup (see Fonts.cpp). Roboto is the
// default UI font; Mono (JetBrains Mono) is the monospaced font used for the
// result box.
extern ImFont *Roboto;
extern ImFont *Mono;

void LoadFonts();
