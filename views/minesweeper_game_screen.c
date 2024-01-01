#include "minesweeper_game_screen.h"
#include "minesweeper_icons.h"

#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#include <furi.h>
#include <furi_hal.h>

static const Icon* tile_icons[] = {
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
    uint8_t x, y;
} CurrentPosition;

typedef struct {
    uint8_t x, y;
    const Icon* icon;
} IconElement;

typedef struct {
    IconElement icon_element;
    MineSweeperGameScreenTileState tile_state;
    MineSweeperGameScreenTileType tile_type;
} MineSweeperTile;

typedef struct {
    MineSweeperTile board[ MINESWEEPER_BOARD_TILE_COUNT ];
    uint8_t mines_left;
    uint8_t flags_left;
    CurrentPosition curr_pos;
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

    for (uint8_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {

        MineSweeperTile tile = model->board[i];

        if (tile.icon_element.icon != NULL) {
            if ( model->curr_pos.x * MINESWEEPER_BOARD_WIDTH + model->curr_pos.y == i) {
                inverted_canvas_white_to_black(
                    canvas,
                    {
                        canvas_draw_icon(
                            canvas,
                            tile.icon_element.y,
                            tile.icon_element.x,
                            tile.icon_element.icon);
                    });

            } else {
                canvas_draw_icon(
                    canvas,
                    tile.icon_element.y,
                    tile.icon_element.x,
                    tile.icon_element.icon);
            }
        }
    }

}

bool mine_sweeper_game_screen_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    MineSweeperGameScreen* instance = context;
    bool consumed = false;
    
    // In this input callback we can change the view model according to the event
    // then pass it to the custom input callback defined in the scene manager
    if (event->type == InputTypePress) {
        switch (event->key) {

            case InputKeyUp :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.x = (model->curr_pos.x-1 < 0) ? 0 : model->curr_pos.x-1;
                    },
                    true);
                consumed = true;
                break;

            case InputKeyDown :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.x = (model->curr_pos.x+1 >= MINESWEEPER_BOARD_HEIGHT) ? MINESWEEPER_BOARD_HEIGHT-1 : model->curr_pos.x+1;
                    },
                    true);
                consumed = true;
                break;

            case InputKeyRight :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.y = (model->curr_pos.y+1 >= MINESWEEPER_BOARD_WIDTH) ? MINESWEEPER_BOARD_WIDTH-1 : model->curr_pos.y+1;
                    },
                    true);
                consumed = true;
                break;

            case InputKeyLeft :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.y = (model->curr_pos.y-1 < 0) ? 0 : model->curr_pos.y-1;
                    },
                    true);
                consumed = true;
                break;

            case InputKeyOk :
                break;

            default:
                break;
        }
    }

    if (!consumed && instance->input_callback != NULL) {
        consumed = instance->input_callback(event, instance->context);
    }

    return consumed;
}

static void setup_board(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    MineSweeperGameScreenTileType tiles[MINESWEEPER_BOARD_TILE_COUNT];
    memset(&tiles, 0, sizeof(tiles));

    for (uint8_t i = 0; i < MINESWEEPER_STARTING_MINES; i++) {
        uint8_t rand_pos;

        do {
            rand_pos = furi_hal_random_get() % MINESWEEPER_BOARD_TILE_COUNT;
        } while (tiles[rand_pos] == MineSweeperGameScreenTileMine);

        tiles[rand_pos] = MineSweeperGameScreenTileMine;
    }

    // All mines are set so we look at each tile for surrounding mines
    for (uint8_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
        MineSweeperGameScreenTileType tile_type = tiles[i];

        if (tile_type == MineSweeperGameScreenTileMine) {
            continue;
        }

        uint8_t x = i / MINESWEEPER_BOARD_WIDTH;
        uint8_t y = i % MINESWEEPER_BOARD_WIDTH;

        uint8_t offsets[8][2] = {
            {-1,1},
            {0,1},
            {1,1},
            {1,0},
            {1,-1},
            {0,-1},
            {-1,-1},
            {-1,0},
        };

        uint8_t mine_count = 0;

        for (uint8_t j = 0; j < 8; j++) {
            int8_t dx = x + offsets[j][0];
            int8_t dy = y + offsets[j][1];

            if (dx < 0 || dy < 0 || dx >= MINESWEEPER_BOARD_HEIGHT || dy >= MINESWEEPER_BOARD_WIDTH) {
                continue;
            }

            uint8_t pos = dx * MINESWEEPER_BOARD_WIDTH + dy;
            if (tiles[pos] == MineSweeperGameScreenTileMine) {
                mine_count++;
            }
        }

        tiles[i] = (MineSweeperGameScreenTileType) ((mine_count % (MineSweeperGameScreenTileTypeCount-1)) + 1);

    }


    // Save tiles to view model
    // Because of way tile enum and Icon* array is set up we can
    // index tile_icons with the enum type to get the correct Icon*
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            for (uint8_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
                model->board[i].tile_type = tiles[i];
                model->board[i].tile_state = MineSweeperGameScreenTileStateUncleared;
                model->board[i].icon_element.icon = tile_icons[ tiles[i] ];
                model->board[i].icon_element.x = (i/MINESWEEPER_BOARD_WIDTH) * icon_get_height(tile_icons[ tiles[i] ]);
                model->board[i].icon_element.y = (i%MINESWEEPER_BOARD_WIDTH) * icon_get_width(tile_icons[ tiles[i] ]);
                model->mines_left = MINESWEEPER_STARTING_MINES;
                model->flags_left = MINESWEEPER_STARTING_MINES;
                model->curr_pos.x = 0;
                model->curr_pos.y = 0;
            }
         
        },
        true);
    
}

MineSweeperGameScreen* mine_sweeper_game_screen_alloc() {
    MineSweeperGameScreen* mine_sweeper_game_screen = (MineSweeperGameScreen*)malloc(sizeof(MineSweeperGameScreen));
    
    mine_sweeper_game_screen->view = view_alloc();

    view_set_context(mine_sweeper_game_screen->view, mine_sweeper_game_screen);
    view_allocate_model(mine_sweeper_game_screen->view, ViewModelTypeLocking, sizeof(MineSweeperGameScreenModel));

    view_set_draw_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_draw_callback);
    view_set_input_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_input_callback);
    
    // This are currently be unused
    view_set_enter_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_enter);
    view_set_exit_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_exit);

    mine_sweeper_game_screen->input_callback = NULL;

    setup_board(mine_sweeper_game_screen);
    
    return mine_sweeper_game_screen;
}

void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            // Free any dynamically allocated members in the model
            UNUSED(model);
        },
        false);

    // Free view and any dynamically allocated members in main struct
    view_free(instance->view);
    free(instance);
}

// This function should be called whenever you want to reset the game state
// This should not be called in the on_exit in the game scene unless the state is saved.
void mine_sweeper_game_screen_reset(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    
    instance->input_callback = NULL;

    setup_board(instance);

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

bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint8_t x, uint8_t y) {
    furi_assert(instance);
    bool is_mine = false;
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            uint8_t pos = x * MINESWEEPER_BOARD_WIDTH + y;
            if (model->board[pos].tile_type == MineSweeperGameScreenTileMine) {
                is_mine = true;
            }
        },
        false);

    return is_mine;
}
