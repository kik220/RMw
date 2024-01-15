#pragma once

#include <furi.h>

#include <dialogs/dialogs.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/popup.h>
#include <gui/modules/submenu.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#include <storage/storage.h>

#include "mfc_editor_app.h"
#include "mfc_editor_icons.h"
#include "scenes/mfc_editor_scene.h"

#define TAG "MFCEditor"

#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

enum MfcEditorCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    MfcEditorCustomEventReserved = 100,

    MfcEditorCustomEventViewExit,
};

struct MfcEditorApp {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;

    Submenu* submenu;
    Popup* popup;

    NfcDevice* nfc_device;

    FuriString* file_path;
};

typedef enum {
    MfcEditorAppViewSubmenu,
    MfcEditorAppViewPopup,
} MfcEditorAppView;

typedef enum {
    // Generic
    MfcEditorPromptResponseSuccess,
    MfcEditorPromptResponseFailure,

    // Backed out of a prompt
    MfcEditorPromptResponseExitedFile,
    MfcEditorPromptResponseExitedShadow,
} MfcEditorPromptResponse;

MfcEditorPromptResponse mfc_editor_prompt_load_file(MfcEditorApp* instance);

bool mfc_editor_load_file(MfcEditorApp* instance, FuriString* file_path, bool show_dialog);
