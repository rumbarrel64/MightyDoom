// menu.c
#include "libs.h"
#include "music.h"
#include "menu.h"
#include "gameState.h"

#define MENU_ITEM_COUNT 2
#define JOY_THRESHOLD 32  // Adjust this if needed

void menu_loop() {

    // Initialize Sprite(s)
    sprite_t *menuCursor = sprite_load("rom:/Main-Menu-Mighty-Doom.rgba32.sprite");

    int menu_index = 0;                         // 0 = Tutorial, 1 = Options
    const int menu_x = 80;
    const int menu_y_base = 80;
    const int menu_spacing = 20;
    const int cursor_x = menu_x - 20;           // align sprite left of text

    // Load in Music
    music_load("Main_Menu_n64.wav64");

    while (state == STATE_MENU) {

        // get memory usage (Must be inside the loop to get memory per frame)
        heap_stats_t heap_stats;
        sys_get_heap_stats(&heap_stats);

        display_context_t disp = display_get();
        rdpq_attach(disp, NULL);

        // Clear the screen BEFORE drawing anything
        rdpq_clear(RGBA32(0, 0, 0, 255));  // Black background

        // Draw UI

        //Sprite(s)
         // This is needed for sprites with a transparent background
        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(1);
        
        // Draw sprite next to current selection
        rdpq_sprite_blit(menuCursor, cursor_x, menu_y_base + menu_index * menu_spacing, NULL);

        //Font(s)
        rdpq_set_mode_standard();
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER}, 2, 50, 20, "Mighty Doom 64");
        rdpq_text_printf(NULL, 2, 100, 100, "Tutorial");
        rdpq_text_printf(NULL, 2, 100, 120, "Options");
        //rdpq_text_printf(NULL, 1, 20, 220, "Mem: %d KiB", heap_stats.used/1024); // get memory usage
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 10, 220, "Mem: %d KiB", heap_stats.used/1024); // get memory usage

        rdpq_detach_show();

        //Music
        music_play();

        joypad_poll();
        joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
        joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        
        //
        if (/*cursor_near_tutorial_option() &&*/ btn.start) {
            state = STATE_TUTORIAL;
            //return;
        }

        static int stick_pressed = 0;

        if (!stick_pressed) {
            if (btn.d_down || joypad.stick_y < -JOY_THRESHOLD) {
                menu_index++;
                if (menu_index >= MENU_ITEM_COUNT)
                    menu_index = 0;
                stick_pressed = 1;
                wait_ms(120);
            } else if (btn.d_up || joypad.stick_y > JOY_THRESHOLD) {
                if (menu_index == 0)
                    menu_index = MENU_ITEM_COUNT - 1;
                else
                    menu_index--;
                stick_pressed = 1;
                wait_ms(120);
            }
        } else if (joypad.stick_y > -JOY_THRESHOLD && joypad.stick_y < JOY_THRESHOLD) {
            // Reset when stick returns to center
            stick_pressed = 0;
        }

        /*
        if (btn.d_down || btn.d_up || joypad.stick_y) { // dobule check if joypad works
            menu_index = 1 - menu_index; // Toggle between 0 and 1
            wait_ms(120); // Prevents rapid scrolling
        }
        */

    }

    // Free Assests to prevent memory leaks
    sprite_free(menuCursor);
    music_stop();

}
