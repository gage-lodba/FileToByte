#pragma once

#include <imgui.h>

// Embedded UI fonts, loaded once at startup (see Fonts.cpp). Roboto is the
// default UI font; Consolas is the monospaced font used for the result box.
extern ImFont *Roboto;
extern ImFont *Consolas;

// Loads the embedded fonts into the current ImGui context. Call after the
// ImGui context exists (i.e. after constructing the Application) and before
// run().
void LoadFonts();
