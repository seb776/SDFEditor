// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "GUIDocument.h"

#include <imgui/imgui.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui_internal.h>
#include <imgui/ImGuiFileDialog.h>

#include "GUIIconGlyphs.h"
#include <SDFEditor/Tool/ToolApp.h>

namespace GUI
{
    bool gWantsCloseDocumentWithChanges = false;
    bool gWantsOpenFileWithChanges = false;
    bool gWantsNewFileWithChanges = false;

    void ModalYesNoDialog(const char* aName, const char* aText, TYesNoDialogCallback aCallback)
    {
        ImVec2 lCenter = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(lCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        ImGuiWindowFlags lNewDocFlags = ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_Modal            |
                                        ImGuiWindowFlags_NoMove           |
                                        ImGuiWindowFlags_NoTitleBar;

        if (ImGui::BeginPopupModal(aName, NULL, lNewDocFlags))
        {
            ImGui::Text(aText);
            if (ImGui::Button("No"))
            {
                if (aCallback)
                {
                    aCallback(false);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Yes"))
            {
                if (aCallback)
                {
                    aCallback(true);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void ConfigureFileDialogsIcons()
    {
        // define style for all directories
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".strks", ImVec4(0.8f, 1.0f, 0.3f, 0.9f), ICON_DOC_TEXT);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(0.8f, 0.8f, 0.8f, 0.9f), ICON_FOLDER);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1.0f, 1.0f, 1.0f, 0.3f), ICON_DOC);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeLink, "", ImVec4(1.0f, 1.0f, 1.0f, 0.3f), ICON_DOC);
    }

    void RequestOpenFile(CToolApp& aToolApp)
    {
        if (aToolApp.GetScene().mDocument->HasPendingChanges())
        {
            gWantsOpenFileWithChanges = true;
        }
        else
        {
            LaunchOpenFileDialog(aToolApp);
        }
    }

    void RequestSaveFile(CToolApp& aToolApp)
    {
        CScene& lScene = aToolApp.GetScene();
        if (lScene.mDocument->HasFilePath())
        {
            aToolApp.SaveScene();
        }
        else
        {
            LaunchSaveFileDialog(aToolApp);
        }
    }
    void RequestSaveShaderFile(CToolApp& aToolApp)
    {
        LaunchSaveShaderFileDialog(aToolApp);
    }

    void RequestNewFile(CToolApp& aToolApp)
    {
        if (aToolApp.GetScene().mDocument->HasPendingChanges())
        {
            gWantsNewFileWithChanges = true;
        }
        else
        {
            aToolApp.ResetScene();
        }
    }

    void LaunchOpenFileDialog(CToolApp& aToolApp)
    {
        uint32_t lFlags = ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_ReadOnlyFileNameField;
        ImGuiFileDialog::Instance()->OpenModal("OpenStrokesFile", "Open Strokes File", ".strks", ".", 1, nullptr, lFlags);
    }

    void LaunchSaveFileDialog(CToolApp& aToolApp)
    {
        uint32_t lFlags = ImGuiFileDialogFlags_ConfirmOverwrite;
        ImGuiFileDialog::Instance()->OpenModal("SaveStrokesFile", "Save Strokes File", ".strks", ".", 1, nullptr, lFlags);
    }
    void LaunchSaveShaderFileDialog(CToolApp& aToolApp)
    {
        uint32_t lFlags = ImGuiFileDialogFlags_ConfirmOverwrite;
        ImGuiFileDialog::Instance()->OpenModal("SaveShaderFile", "Save Shader File", ".glsl", ".", 1, nullptr, lFlags);
    }

    void WantCloseDocument(CToolApp& aToolApp)
    {
        if (aToolApp.GetScene().mDocument->HasPendingChanges())
        {
            gWantsCloseDocumentWithChanges = true;
        }
        else
        {
            aToolApp.Terminate();
        }
    }

    static bool TopBarButton(const char* aIconStr)
    {
        const static ImVec2 kGuizmoButtonSize = ImVec2(40.0f, 40.0f);
        return ImGui::Button(aIconStr, kGuizmoButtonSize);
    }

    void DrawDocOptionsBar(CToolApp& aToolApp)
    {
        const ImVec2 lViewPos = ImGui::GetMainViewport()->Pos;
        const ImVec2 lViewSize = ImGui::GetMainViewport()->Size;

        ImGuiWindowFlags lWindowFlags = 0;
        lWindowFlags |= ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavInputs
            | ImGuiWindowFlags_NoNavFocus
            | ImGuiWindowFlags_NoDocking;

        const ImVec2 kGuizmoPanelSize = ImVec2(470, 50);
        const ImVec2 kGuizmoPanelPos = ImVec2(lViewPos.x + 20, lViewPos.y + 20);
        ImGui::SetNextWindowSize(kGuizmoPanelSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(kGuizmoPanelPos, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x00000000);
        if (ImGui::Begin("Top Bar", NULL, lWindowFlags))
        {
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(1);
            ImGui::SetWindowFontScale(1.25f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.10f, 0.12f, 0.70f));
            if (TopBarButton(ICON_OPEN_FOLDER_INV))
            {
                RequestOpenFile(aToolApp);
            }
            ImGui::SameLine(0.0f, 10.0f);
            if (TopBarButton(ICON_SAVE_INV))
            {
                RequestSaveFile(aToolApp);
            }
            // Save to glsl
            ImGui::SameLine(0.0f, 10.0f);
            if (TopBarButton(ICON_ARROW_DOWN_BIG))
            {
                RequestSaveShaderFile(aToolApp);
            }
            // end Save to glsl
            ImGui::SameLine(0.0f, 10.0f);
            if (TopBarButton(ICON_DOC_INV))
            {
                RequestNewFile(aToolApp);
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);

            if (gWantsCloseDocumentWithChanges)
            {
                ImGui::OpenPopup("DiscardChangesCloseApp");
                gWantsCloseDocumentWithChanges = false;
            }

            if (gWantsOpenFileWithChanges)
            {
                ImGui::OpenPopup("DiscardChangesOpenFile");
                gWantsOpenFileWithChanges = false;
            }

            if (gWantsNewFileWithChanges)
            {
                ImGui::OpenPopup("DiscardChangesNewDoc");
                gWantsNewFileWithChanges = false;
            }

            // Draw modal yes/no popups
            {
                ModalYesNoDialog("DiscardChangesNewDoc", "Discard changes?", [&](bool aYes){
                    if (aYes){ aToolApp.ResetScene(); }
                });

                ModalYesNoDialog("DiscardChangesOpenFile", "Discard changes?", [&](bool aYes){
                    if (aYes){ LaunchOpenFileDialog(aToolApp); }
                });

                // Non top bar modals
                ModalYesNoDialog("DiscardChangesCloseApp", "Discard changes?", [&](bool aYes) {
                    if (aYes) { aToolApp.Terminate(); }
                });
            }
        }
        ImGui::End();
    }

    void DrawFileDialogs(CToolApp& aToolApp)
    {
        // File Dialogs
        const ImGuiWindowFlags lDialogsFlags = ImGuiWindowFlags_NoCollapse | 
                                               ImGuiWindowFlags_NoSavedSettings |
                                               ImGuiWindowFlags_NoDocking;

        if (ImGuiFileDialog::Instance()->Display("OpenStrokesFile", lDialogsFlags, ImVec2(800, 600)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
 
                // Load scene data
                aToolApp.LoadScene(filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGuiFileDialog::Instance()->Display("SaveStrokesFile", lDialogsFlags, ImVec2(800, 600)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                // Save scene data
                aToolApp.SaveScene(filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }
        if (ImGuiFileDialog::Instance()->Display("SaveShaderFile", lDialogsFlags, ImVec2(800, 600)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                // Save scene data
                aToolApp.SaveShaderScene(filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

    
}