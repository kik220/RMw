#include "nfc_playlist.h"
#include "scences/main_menu.h"

typedef enum {
    NfcPlaylistEvent_ShowEmulatingPopup,
    NfcPlaylistEvent_ShowSettings
} NfcPlaylistMainMenuEvent;

typedef enum {
    NfcPlaylistMenuSelection_Start,
    NfcPlaylistMenuSelection_Settings
} NfcPlaylistMenuSelection;

static void nfc_playlist_menu_callback(void* context, uint32_t index) {
    NfcPlaylist* nfc_playlist = context;
    switch(index) {
    case NfcPlaylistMenuSelection_Start:
        scene_manager_handle_custom_event(
            nfc_playlist->scene_manager, NfcPlaylistEvent_ShowEmulatingPopup);
        break;
    case NfcPlaylistMenuSelection_Settings:
        scene_manager_handle_custom_event(
            nfc_playlist->scene_manager, NfcPlaylistEvent_ShowSettings);
        break;
    default:
        break;
    }
}

void nfc_playlist_main_menu_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;
    submenu_set_header(nfc_playlist->submenu, "NFC Playlist");

    submenu_add_item(
        nfc_playlist->submenu,
        "Start",
        NfcPlaylistMenuSelection_Start,
        nfc_playlist_menu_callback,
        nfc_playlist);

    submenu_add_item(
        nfc_playlist->submenu,
        "Settings",
        NfcPlaylistMenuSelection_Settings,
        nfc_playlist_menu_callback,
        nfc_playlist);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Menu);
}

bool nfc_playlist_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* nfc_playlist = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case NfcPlaylistEvent_ShowEmulatingPopup:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_EmulatingPopup);
            consumed = true;
            break;
        case NfcPlaylistEvent_ShowSettings:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_Settings);
            consumed = true;
            break;
        default:
            break;
        }
        break;
    default:
        consumed = false;
        break;
    }
    return consumed;
}

void nfc_playlist_main_menu_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    submenu_reset(nfc_playlist->submenu);
}