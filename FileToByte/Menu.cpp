#include "Menu.h"
#include "Globals.h"
#include "FTB.h"
#include "Roboto.hpp"
#include "Consolas.hpp"

ImFont* Consolas = nullptr;

namespace Menu {
    void DrawMenu(GLFWwindow* window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(800, 600));
        ImGui::Begin("File To Byte", &Globals::open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::PushItemWidth(392.5f);
            ImGui::BeginGroup();
            {
                ImGui::Text("Array Name");
                ImGui::SameLine();
                ImGui::Dummy(ImVec2(330, 0));
                ImGui::SameLine();
                ImGui::Text("File Path");
                ImGui::InputText("##Array Name", Globals::VarName, sizeof(Globals::VarName));
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);
                ImGui::InputText("##File Path", Globals::FilePath, sizeof(Globals::FilePath));
                ImGui::PopItemWidth();
            }
            ImGui::EndGroup();
            ImGui::PopItemWidth();

            if (ImGui::Button("Convert", ImVec2(392.5f, 25.f))) {
                FileToByte::Convert(Globals::FilePath);
            }

            ImGui::SameLine();

            if (ImGui::Button("Copy to clipboard", ImVec2(-1, 25.f))) {
                glfwSetClipboardString(window, Globals::res.c_str());
            }

            ImGui::PushFont(Consolas);
            ImGui::InputTextMultiline("##Result", &Globals::res, ImVec2(-1, -1));
            ImGui::PopFont();
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Initialize(GLFWwindow* window) {
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.FontDefault = io.Fonts->AddFontFromMemoryTTF(Roboto, sizeof(Roboto), 12.f);
        Consolas = io.Fonts->AddFontFromMemoryTTF(Consola, sizeof(Consola), 12.f);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
        ImGui::StyleColorsDark();
    }

    void Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
}