/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <gui/canvas.h>
#include <gui/modules/widget.h>
#include <wch_swio_flasher_icons.h>
#include "../wch_swio_flasher.h"
#include "wch_swio_flasher_scene.h"

void wch_swio_flasher_scene_about_on_enter(void* context) {
    WchSwioFlasherApp* app = context;

    widget_add_string_element(
        app->views.widget, 63, 2, AlignCenter, AlignTop, FontPrimary, "WCH Flasher v1.0");

    widget_add_string_element(
        app->views.widget, 63, 24, AlignCenter, AlignCenter, FontSecondary, "author");
    widget_add_string_element(
        app->views.widget, 63, 33, AlignCenter, AlignCenter, FontPrimary, "github.com/sukvojte");

    widget_add_string_element(
        app->views.widget, 63, 48, AlignCenter, AlignCenter, FontSecondary, "with support by");
    widget_add_string_element(
        app->views.widget, 63, 58, AlignCenter, AlignCenter, FontSecondary, "github.com/ah01");

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewWidget);
}

bool wch_swio_flasher_scene_about_on_event(void* context, SceneManagerEvent event) {
    WchSwioFlasherApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void wch_swio_flasher_scene_about_on_exit(void* context) {
    WchSwioFlasherApp* app = context;
    widget_reset(app->views.widget);
}
