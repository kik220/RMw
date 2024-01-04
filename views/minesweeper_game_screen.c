#include "minesweeper_game_screen.h"
#include "minesweeper_icons.h"

#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#include <furi.h>
#include <furi_hal.h>

static const Icon* tile_icons[13] = {
    &I_tile_empty_8x8,
    &I_tile_0_8x8,
    &I_tile_1_8x8,
    &I_tile_2_8x8,
    &I_tile_3_8x8,
    &I_tile_4_8x8,
    &I_tile_5_8x8,
    &I_tile_6_8x8,
    &I_tile_7_8x8,
    &I_tile_8_8x8,
    &I_tile_mine_8x8,
    &I_tile_flag_8x8,
    &I_tile_uncleared_8x8,
};

// They way this enum is set up allows us to index the Icon* array above for some mine types
typedef enum {
    MineSweeperGameScreenTileNone = 0,
    MineSweeperGameScreenTileZero,
    MineSweeperGameScreenTileOne,
    MineSweeperGameScreenTileTwo,
    MineSweeperGameScreenTileThree,
    MineSweeperGameScreenTileFour,
    MineSweeperGameScreenTileFive,
    MineSweeperGameScreenTileSix,
    MineSweeperGameScreenTileSeven,
    MineSweeperGameScreenTileEight,
    MineSweeperGameScreenTileMine,
    MineSweeperGameScreenTileTypeCount,
} MineSweeperGameScreenTileType;

typedef enum {
    MineSweeperGameScreenTileStateFlagged,
    MineSweeperGameScreenTileStateUncleared,
    MineSweeperGameScreenTileStateCleared,
} MineSweeperGameScreenTileState;

struct MineSweeperGameScreen {
    View* view;
    void* context;
    GameScreenInputCallback input_callback;
};

typedef struct {
    int16_t x_abs, y_abs;
} CurrentPosition;

typedef struct {
    uint16_t x_abs, y_abs;
    const Icon* icon;
} IconElement;

typedef struct {
    IconElement icon_element;
    MineSweeperGameScreenTileState tile_state;
    MineSweeperGameScreenTileType tile_type;
} MineSweeperTile;

typedef struct {
    MineSweeperTile board[ MINESWEEPER_BOARD_MAX_TILES ];
    uint16_t mines_left;
    uint16_t flags_left;
    CurrentPosition curr_pos;
    uint8_t right_boundary, bottom_boundary,
            board_width, board_height, board_difficulty;
    uint32_t start_tick;
    FuriString* info_str;
} MineSweeperGameScreenModel;

void mine_sweeper_game_screen_view_enter(void* context) {
    furi_assert(context);
    UNUSED(context);

    //MineSweeperGameScreen* mine_sweeper_game_screen = context;
    
    //with_view_model(
    //    mine_sweeper_game_screen->view,
    //    MineSweeperGameScreenModel * model,
    //    {
    //    },
    //    true);
}

void mine_sweeper_game_screen_view_exit(void* context) {
    furi_assert(context);
    UNUSED(context);

    //MineSweeperGameScreen* mine_sweeper_game_screen = context;

    //with_view_model(
    //    mine_sweeper_game_screen->view,
    //    MineSweeperGameScreenModel * model,
    //    {
    //    },
    //    true);
}

void mine_sweeper_game_screen_view_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    MineSweeperGameScreenModel* model = _model;

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);
    
    /** We can use the boundary uint8_t in the model to transform the relative x/y coordinates
     *  to the absolute x/y positions on the board grid as well as the position in the board buffer.
     *
     *  The relative coordinates start at zero and go to MINESWEEPER_SCREEN_TILE_HEIGHT-1 and
     *  MINESWEEPER_SCREEN_TILE_HEIGHT-1 for x and y repsectively.
     *
     *  Once we have the absolute x/y coord we can use that to access the correct position for the
     *  tile in the board buffer within the model.
     *
     *  We draw the tile located at the absolute position in the grid onto the screen at the position of the
     *  relative x and y.
     *  We also invert the color if it is the current position selected by the user
     */

    for (uint8_t x_rel = 0; x_rel < MINESWEEPER_SCREEN_TILE_HEIGHT; x_rel++) {
        uint16_t x_abs = (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) + x_rel;
        
        for (uint8_t y_rel = 0; y_rel < MINESWEEPER_SCREEN_TILE_WIDTH; y_rel++) {
            uint16_t y_abs = (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) + y_rel;

            uint16_t board_buffer_pos_abs = x_abs * model->board_width + y_abs;
            MineSweeperTile tile = model->board[board_buffer_pos_abs];

            if (model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs == board_buffer_pos_abs) {
                inverted_canvas_white_to_black(
                    canvas,
                    {
                        canvas_draw_icon(
                            canvas,
                            y_rel * icon_get_width(tile.icon_element.icon),
                            x_rel * icon_get_height(tile.icon_element.icon),
                            tile.icon_element.icon);
                    });

            } else {
                canvas_draw_icon(
                    canvas,
                    y_rel * icon_get_width(tile.icon_element.icon),
                    x_rel * icon_get_height(tile.icon_element.icon),
                    tile.icon_element.icon);
            }

        }
    }

    // If any borders are at the limits of the game board we draw a border line
    
    // Right border 
    if (model->right_boundary == model->board_width) {
        canvas_draw_line(canvas, 127,0,127,63-8);
    }

    // Left border
    if ((model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) == 0) {
        canvas_draw_line(canvas, 0,0,0,63-8);
    }

    // Bottom border
    if (model->bottom_boundary == model->board_height) {
        canvas_draw_line(canvas, 0,63-8,127,63-8);
    }

    // Top border
    if ((model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) == 0) {
        canvas_draw_line(canvas, 0,0,127,0);
    }

    // Draw X Position Text 
    furi_string_printf(
            model->info_str,
            "X:%03hhd",
            model->curr_pos.x_abs);

    canvas_draw_str_aligned(
            canvas,
            0,
            64-7,
            AlignLeft,
            AlignTop,
            furi_string_get_cstr(model->info_str));

    // Draw Y Position Text 
    furi_string_printf(
            model->info_str,
            "Y:%03hhd",
            model->curr_pos.y_abs);

    canvas_draw_str_aligned(
            canvas,
            33,
            64-7,
            AlignLeft,
            AlignTop,
            furi_string_get_cstr(model->info_str));

    // Draw flag text
    furi_string_printf(
            model->info_str,
            "F:%03hd",
            model->flags_left);

    canvas_draw_str_aligned(
            canvas,
            66,
            64 - 7,
            AlignLeft,
            AlignTop,
            furi_string_get_cstr(model->info_str));

    // Draw time text
    uint32_t ticks_elapsed = furi_get_tick() - model->start_tick;
    uint32_t sec = ticks_elapsed / furi_kernel_get_tick_frequency();
    uint32_t minutes = sec / 60;
    sec = sec % 60;

    furi_string_printf(
             model->info_str,
             "%02ld:%02ld",
             minutes,
             sec);

    canvas_draw_str_aligned(
            canvas,
            126 - canvas_string_width(canvas, furi_string_get_cstr(model->info_str)),
            64 - 7,
            AlignLeft,
            AlignTop,
            furi_string_get_cstr(model->info_str));
}

bool mine_sweeper_game_screen_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    MineSweeperGameScreen* instance = context;
    bool consumed = false;
    
    // In this input callback we can change the view model according to the event
    // then pass it to the custom input callback defined in the scene manager
    if ((event->type == InputTypePress) && (event->key == InputKeyOk)) {

        // register that something good or bad happened
        // Do we still pass this to the input callback in the scene manager?

        consumed = true;

    } else if ((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {

        with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                bool is_outside_boundary;
                switch (event->key) {

                    case InputKeyUp :
                        model->curr_pos.x_abs = (model->curr_pos.x_abs-1 < 0) ? 0 : model->curr_pos.x_abs-1;

                        is_outside_boundary = model->curr_pos.x_abs <
                            (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT);
                        
                        if (is_outside_boundary) {
                            model->bottom_boundary--;
                        }
                        consumed= true;
                        break;

                    case InputKeyDown :
                        model->curr_pos.x_abs = (model->curr_pos.x_abs+1 >= model->board_height) ?
                            model->board_height-1 : model->curr_pos.x_abs+1;

                        is_outside_boundary = model->curr_pos.x_abs >= model->bottom_boundary;

                        if (is_outside_boundary) {
                            model->bottom_boundary++;
                        }
                        consumed = true;
                        break;

                    case InputKeyLeft :
                        model->curr_pos.y_abs = (model->curr_pos.y_abs-1 < 0) ? 0 : model->curr_pos.y_abs-1;

                        is_outside_boundary = model->curr_pos.y_abs <
                            (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH);
                        
                        if (is_outside_boundary) {
                            model->right_boundary--;
                        }
                        consumed = true;
                        break;

                    case InputKeyRight :
                        model->curr_pos.y_abs = (model->curr_pos.y_abs+1 >= model->board_width) ?
                            model->board_width-1 : model->curr_pos.y_abs+1;

                        is_outside_boundary = model->curr_pos.y_abs >= model->right_boundary;

                        if (is_outside_boundary) {
                            model->right_boundary++;
                        }
                        consumed = true;
                        break;

                    default:
                        break;
                }
            },
            true);

    }

    if (!consumed && instance->input_callback != NULL) {
        consumed = instance->input_callback(event, instance->context);
    }

    return consumed;
}

static const float difficulty_multiplier[5] = {
    0.20f,
    0.25f,
    0.29f,
    0.33f,
    0.40f,
};

static void setup_board(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    uint16_t board_tile_count = 0;
    uint8_t board_width = 0, board_height = 0, board_difficulty = 0;

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            board_width = model->board_width;
            board_height = model->board_height;
            board_tile_count =  (model->board_width*model->board_height);
            board_difficulty = model->board_difficulty;
        },
        false);

    uint16_t num_mines = board_tile_count * difficulty_multiplier[ board_difficulty ];
    FURI_LOG_D(MS_DEBUG_TAG, "Placing %hd mines", num_mines);

    /** We can use a temporary buffer to set the tile types initially
     * and manipulate then save to actual model
     */
    MineSweeperGameScreenTileType tiles[MINESWEEPER_BOARD_MAX_TILES];
    memset(&tiles, MineSweeperGameScreenTileNone, sizeof(tiles));

    // Place tiles everywhere randomly except the corners to help the solver
    for (uint16_t i = 0; i < num_mines; i++) {

        uint16_t rand_pos;
        uint16_t x;
        uint16_t y;

        do {

            rand_pos = furi_hal_random_get() % board_tile_count;
            x = rand_pos / board_width;
            y = rand_pos % board_width;

        } while (tiles[rand_pos] == MineSweeperGameScreenTileMine ||
                            (rand_pos == 0)                       ||
                            (x==0 && y==board_width-1)            ||
                            (x==board_height-1 && y==0)           || 
                            (rand_pos == board_tile_count-1));

        tiles[rand_pos] = MineSweeperGameScreenTileMine;
    }

    /** All mines are set so we look at each tile for surrounding mines */
    for (uint16_t i = 0; i < board_tile_count; i++) {
        MineSweeperGameScreenTileType tile_type = tiles[i];

        if (tile_type == MineSweeperGameScreenTileMine) {
            continue;
        }

        int8_t offsets[8][2] = {
            {-1,1},
            {0,1},
            {1,1},
            {1,0},
            {1,-1},
            {0,-1},
            {-1,-1},
            {-1,0},
        };

        uint16_t mine_count = 0;

        uint16_t x = i / board_width;
        uint16_t y = i % board_width;

        for (uint8_t j = 0; j < 8; j++) {
            int16_t dx = x + (int16_t)offsets[j][0];
            int16_t dy = y + (int16_t)offsets[j][1];

            if (dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                continue;
            }

            uint16_t pos = dx * board_width + dy;
            if (tiles[pos] == MineSweeperGameScreenTileMine) {
                mine_count++;
            }

        }

        tiles[i] = (MineSweeperGameScreenTileType) mine_count+1;

    }

    // Save tiles to view model
    // Because of way tile enum and Icon* array is set up we can
    // index tile_icons with the enum type to get the correct Icon*
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            for (uint16_t i = 0; i < board_tile_count; i++) {
                model->board[i].tile_type = tiles[i];
                model->board[i].tile_state = MineSweeperGameScreenTileStateUncleared;
                model->board[i].icon_element.icon = tile_icons[ tiles[i] ];
                model->board[i].icon_element.x_abs = (i/model->board_width);
                model->board[i].icon_element.y_abs = (i%model->board_width);
            }

            model->mines_left = num_mines;
            model->flags_left = num_mines;
            model->curr_pos.x_abs = 0;
            model->curr_pos.y_abs = 0;
            model->right_boundary = MINESWEEPER_SCREEN_TILE_WIDTH;
            model->bottom_boundary = MINESWEEPER_SCREEN_TILE_HEIGHT;
         
        },
        true);
}

static void mine_sweeper_game_screen_set_board_information(
        MineSweeperGameScreen* instance,
        uint8_t width,
        uint8_t height,
        uint8_t difficulty) {

    furi_assert(instance);
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            model->board_width = width;
            model->board_height = height;
            model->board_difficulty = difficulty;
        },
        true);
}

MineSweeperGameScreen* mine_sweeper_game_screen_alloc(uint8_t width, uint8_t height, uint8_t difficulty) {
    MineSweeperGameScreen* mine_sweeper_game_screen = (MineSweeperGameScreen*)malloc(sizeof(MineSweeperGameScreen));
    
    mine_sweeper_game_screen->view = view_alloc();

    view_set_context(mine_sweeper_game_screen->view, mine_sweeper_game_screen);
    view_allocate_model(mine_sweeper_game_screen->view, ViewModelTypeLocking, sizeof(MineSweeperGameScreenModel));

    view_set_draw_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_draw_callback);
    view_set_input_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_input_callback);
    
    // This are currently be unused
    view_set_enter_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_enter);
    view_set_exit_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_exit);

    // Secondary Input callback can be called by scene
    mine_sweeper_game_screen->input_callback = NULL;

    // Allocate strings in model
    with_view_model(
        mine_sweeper_game_screen->view,
        MineSweeperGameScreenModel * model,
        {
            model->info_str = furi_string_alloc();
        },
        true);

    // Reset the clock - This will set the start time at the allocation of the game screen
    // but this is a public api as well and can be called in a scene for more accurate start times
    mine_sweeper_game_screen_reset_clock(mine_sweeper_game_screen);

    // We need to initize board width and height before setup
    mine_sweeper_game_screen_set_board_information(mine_sweeper_game_screen, width, height, difficulty);

    FURI_LOG_D(MS_DEBUG_TAG, "Setting up board with w:%03hhd h:%03hhd d:%02hhd", width, height, difficulty);
    setup_board(mine_sweeper_game_screen);
    
    return mine_sweeper_game_screen;
}

void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    // Dealloc strings in model
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            furi_string_free(model->info_str);
        },
        false);

    // Free view and any dynamically allocated members in main struct
    view_free(instance->view);
    free(instance);
}

// This function should be called whenever you want to reset the game state
// This should NOT be called in the on_exit in the game scene
void mine_sweeper_game_screen_reset(MineSweeperGameScreen* instance, uint8_t width, uint8_t height, uint8_t difficulty) {
    furi_assert(instance);
    
    instance->input_callback = NULL;
    
    // We need to initize board width and height before setup
    mine_sweeper_game_screen_set_board_information(instance, width, height, difficulty);

    mine_sweeper_game_screen_reset_clock(instance);

    FURI_LOG_D(MS_DEBUG_TAG, "Setting up board with w:%03hhd h:%03hhd d:%02hhd", width, height, difficulty);
    setup_board(instance);

}

void mine_sweeper_game_screen_reset_clock(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                model->start_tick = furi_get_tick();
            },
            true);
}

View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    return instance->view;
}

void mine_sweeper_game_screen_set_input_callback(MineSweeperGameScreen* instance, GameScreenInputCallback callback) {
    furi_assert(instance);
    instance->input_callback = callback;
}

void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}

bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint16_t x, uint16_t y) {
    furi_assert(instance);
    bool is_mine = false;
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            uint16_t pos = x * model->board_width + y;
            if (model->board[pos].tile_type == MineSweeperGameScreenTileMine) {
                is_mine = true;
            }
        },
        false);

    return is_mine;
}
