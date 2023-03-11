#pragma once

#include <cli/cli.h>
#include <dialogs/dialogs.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdio.h>
#include <storage/storage.h>
#include <toolbox/stream/file_stream.h>

#include <gui/view_dispatcher.h>

#define APPSDATA_FOLDER "/ext/apps_data"
#define VIDEO_PLAYER_FOLDER "/ext/apps_data/video_player"
#define FILE_NAME_LEN 64

typedef enum {
    EventTypeInput,
    EventType1stHalf,
    EventType2ndHalf,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} VideoPlayerEvent;

typedef enum {
    VIEW_PLAYER,
} VideoPlayerViews;

typedef struct {
    View* view;
    void* context;
} PlayerView;

typedef struct {
    NotificationApp* notification;
    FuriMessageQueue* event_queue;
    Gui* gui;
    PlayerView* player_view;
    ViewDispatcher* view_dispatcher;
    Storage* storage;
    Stream* stream;
    FuriString* filepath;
    DialogsApp* dialogs;

    uint8_t* audio_buffer;
    uint8_t* image_buffer;

    uint32_t num_frames;
    uint16_t audio_chunk_size;
    uint16_t sample_rate;
    uint8_t height;
    uint8_t width;

    uint8_t x_offset;
    uint16_t image_buffer_length;

    uint32_t frames_played;

    bool playing;
    
    bool quit;
} VideoPlayerApp;

typedef struct {
    VideoPlayerApp* player;
} PlayerViewModel;

void draw_callback(Canvas* canvas, void* ctx);
bool input_callback(InputEvent* input_event, void* ctx);