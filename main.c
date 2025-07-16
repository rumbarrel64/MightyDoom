// General file to hold all libraries
#include "libs.h"
#include "menu.h"
#include "music.h"
#include "tutorial.h"
#include "gameState.h"

GameState state = STATE_MENU;

int main(void) {
    // Initialize systems
    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    rdpq_init();
    rdpq_debug_start(); // Only use for debugging purposes.
    joypad_init();
    t3d_init((T3DInitParams){});

    // Initialize Music
    music_init();

    // Load Font Data
    rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));
    
    // Game Font
    rdpq_font_t *DOOM = rdpq_font_load("rom:/DooM.font64");
    rdpq_text_register_font(2, DOOM);

    rdpq_font_style(DOOM, 0, &(rdpq_fontstyle_t){
	//                R   G    B  
    .color = RGBA32(255, 0,  0, 255),
	});

    while (1) {

        switch (state) {
            case STATE_MENU:
                menu_loop();
                break;
            case STATE_TUTORIAL:
                tutorial_loop();
                break;
           // case STATE_PLAY:
             //   play_loop();
             //   break;
            // Add other states here
            default:
                break;
        }
        
    }

    return 0;
}