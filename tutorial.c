// tutorial.c
// General file to hold all libraries
#include "libs.h"

// Custom game libraries
#include "slayer.h"
#include "bullets.h"
#include "map.h"
#include "camera.h"
#include "zombie.h"
#include "levels.h"
#include "level_update.h"
#include "gameaudio.h"
#include "banners.h"
#include "gameState.h"
#include "slayer_animation.h"

// Function to get game runtime (Since n64 turned on)
float get_time_s() {
  return (float)((double)get_ticks_us() / 1000000.0); // Converts microsecond ticks to seconds for timing
}

// This is used for zombie blood shrinking
float level_timer = 0.0f;

void tutorial_loop() {

  // At tutorial start:
  float time_offset = get_time_s();  // Capture menu time to subtract

  // Reset level progression when starting tutorial
  reset_level_index();

  T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);
  uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

  // Audio
  // Load Audio Files (prepare but don't play yet)
  music_load("rom:/Tutorial_5_5_11_5.wav64");
  sfx1_load("rom:/fight.wav64");
  sfx2_load("rom:/huntbegins.wav64");

  // Ensure fight sound only played once
  bool fight_sound_played = false;
  bool hunt_sound_played = false; 

  // Start Music Immediately
  music_play();
  // Audio

   // Level definition
  const LevelData *level = ALL_LEVELS[0];  // Start with first level directly

  // Initialize slayer entrance animation (fall from 125 units to ground level)
  SlayerAnimation slayer_anim;
  slayer_animation_init(&slayer_anim, 125.0f, 0.15f);

  // Initialize Camera
  Camera camera;
  camera_init(&camera);
  // Initialize Camera

  
  // Initialize Bullet
  Bullet bullet;
  bullet_init(&bullet);
  // Initialize Bullet
  
  // Initialize Map(s)
  Map map, mapWall, mapPortal;
  map_init(&map, "rom:/map.t3dm");
  map_init(&mapWall, "rom:/mapWall.t3dm");
  map_init(&mapPortal, "rom:/mapPortal.t3dm");
  // Initialize Map(s)

  // Initialize Player
  Player player;
  player_init(&player);
  player.position = level->player.position;
  player.rotation_y = level->player.rotation_y;
  // Initialize Player

  // Initialize Zombie(s)
  int zombie_count = level->zombie_count;
  Zombie *zombies = malloc_uncached(sizeof(Zombie) * zombie_count);
  for (int i = 0; i < zombie_count; i++) {
      zombie_init(&zombies[i], &level->zombies[i].position);
      zombies[i].rotation_y = level->zombies[i].rotation_y;
  }
  // Initialize Zombie(s)

  // Initilaize Enemy count (Sum off all enemy counts)
  // This goes down by one each time an enemy is killed
  int enemy_count = zombie_count;
  //int enemy_count = 0; // Only use for debugging
  // Initilaize Enemy count

  
  // To ensure proper clean up of zombies. Static number
  int allocated_zombie_count = zombie_count;  // Always remembers max allocation

  // Initialize Banners
  Crack crack;
  crack_init(&crack, "rom:/crack.t3dm");
  banners_init();
  // Allocate array of T3DMat4FP based on enemy count
  T3DMat4FP* spawn_matrices = malloc(sizeof(T3DMat4FP) * enemy_count);
  T3DMat4FP* blood_matrices = malloc(sizeof(T3DMat4FP) * enemy_count);
  
  // Initialize Arrow (Only needed for tutorial)
  int arrow_count = 3;
  T3DMat4FP* arrow_matrices = malloc(sizeof(T3DMat4FP) * arrow_count);
  T3DModel *arrow_model = t3d_model_load("rom:/arrow.t3dm");

  // Build each arrow's transform with increasing Y position Once
  for (int i = 0; i < arrow_count; i++) {
    float y_offset = -155.0f + i * 20.0f;
    t3d_mat4fp_from_srt_euler(
      &arrow_matrices[i],
      (float[3]){0.15f, 0.15f, 0.15f},    // Scale:
      (float[3]){0.0f, 0.0f, 0.0f},       // Rotation:
      (float[3]){0.0f, 0.15f, y_offset} // Position: X, Z, Y
    );
  };
  // Initialize Banners

  // Initialize Sprite(s)
  //sprite_t *weapon_circle = sprite_load("rom:/weapon_circle.rgba32.sprite");
  //sprite_t *arrow = sprite_load("rom:/arrow.rgba32.sprite");
  // Initialize Sprite(s)

  float lastTime = get_time_s() - (1.0f / 60.0f);
  rspq_syncpoint_t syncPoint = 0;

    while (state ==  STATE_TUTORIAL) {

      // Get Tutorial Game Time:
      float tutorial_time = get_time_s() - time_offset;  // This starts at 0

      // ======== Update ======== //

      // CRITICAL: Feed audio buffer EARLY and OFTEN
      audio_update();  // Call #1 - Start of frame

      // get memory usage (Must be inside the loop to get memory per frame)
      heap_stats_t heap_stats;
      sys_get_heap_stats(&heap_stats);

      //The joypad subsystem only polls the controllers once per VI interrupt.
      joypad_poll();

      float newTime = get_time_s();
      float deltaTime = newTime - lastTime;
      lastTime = newTime;

      joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
      joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

      // Play hunt begins sound after 0.4 seconds, only once
      if (!hunt_sound_played && tutorial_time >= 0.4f) {
          sfx2_play();
          hunt_sound_played = true;
      }
      
      // Play fight sound after 2.4 seconds, only once
      if (!fight_sound_played && tutorial_time >= 2.4f) {
          sfx1_play();
          fight_sound_played = true;
      }

      // Update slayer animation state and position
      slayer_animation_update(&slayer_anim, &player, &crack, tutorial_time);
      if (slayer_animation_has_control(&slayer_anim)) {
          player_update(&player, deltaTime, joypad, btn, zombies, zombie_count);
      } else {
          joypad_inputs_t no_input = {0};
          joypad_buttons_t no_buttons = {0};
          player_update(&player, deltaTime, no_input, no_buttons, zombies, zombie_count);
      };

      // Level Switch Logic - Let level_update.c handle everything
      if (level_update(&player, zombies, zombie_count, &enemy_count, &level)) {
          level_timer = get_time_s();
          zombie_count = level->zombie_count;  // Just sync the count, don't manage levels
          continue;
      };

      // Update Particle
      //Hard Code Map Boundary for now also used by player (140.f)
      //bullet_update(&bullet, &player.position, player.rotation_y, 140.0f, zombies, &zombie_count, &enemy_count, btn);
      bullet_update(&bullet, &player.position, player.rotation_y, zombies, &zombie_count, &enemy_count);

      // Update Zombie
      zombie_update_all(zombies, zombie_count, &player.position, deltaTime);

      // Update Camera
      camera_update(&camera, &player.position, player.rotation_y);

      // Camera toggle (only if player has control) - safer version
      if (slayer_animation_has_control(&slayer_anim)) {
          if (btn.l) {
              camera_toggle_mode(&camera);
          }
      };

      // Call #2 - Before rendering (optional)
      audio_update();

      if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame
    
      // ======== Draw (3D) ======== //
      rdpq_attach(display_get(), display_get_zbuf());

      t3d_frame_start();
      t3d_viewport_attach(camera_get_viewport(&camera));

      t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
      t3d_screen_clear_depth();

      t3d_light_set_ambient(colorAmbient);
      t3d_light_set_directional(0, colorDir, &lightDirVec);
      t3d_light_set_count(1);

      syncPoint = rspq_syncpoint_new();

      T3DViewport *vp = camera_get_viewport(&camera);

      // Draw order Matters!!!!!!
      // Level End logic
      if (enemy_count == 0) {
        // Draw (Map With Portal)
        map_draw(&map);
        map_draw(&mapPortal);

        // Draw each arrow
        // Reverse index order (2,1,0)
        int current_arrow_index = arrow_count - 1 - ((int)(get_time_s() * 3.0f) % arrow_count);
        t3d_matrix_push(&arrow_matrices[current_arrow_index]);
        rdpq_set_prim_color(RGBA32(255, 0, 0, 255));  
        t3d_model_draw(arrow_model);
        t3d_matrix_pop(1);
      
      } else {
          // Draw (Regular Map)
          map_draw(&mapWall);
          map_draw(&map);
      };
 
      // Draw (Banners)
      for (int i = 0; i < zombie_count; i++) {
        
        // 1. Draw BLOOD banner if zombie is dead
        if (!zombies[i].alive) {

        float time_since_death = get_time_s() - zombies[i].blood_time;

        // Compute scale: start from (3.0, 0.3), shrink by 0.1 per second
        zombies[i].blood_scale -= 0.01f * time_since_death;

        // Clamp: If scale reaches zero or below, skip drawing
        if (zombies[i].blood_scale <= 0.0f) {
            continue;  // Skip this blood banner
        };        

          t3d_mat4fp_from_srt_euler(&blood_matrices[i],
              (float[3]){zombies[i].blood_scale, 0.3f, zombies[i].blood_scale},  // scale: X, Z, Y
              (float[3]){0, 0, 0},
              (float[3]){zombies[i].position.v[0], 0, zombies[i].position.v[2]});

          draw_floor_banner(&blood_matrices[i], BANNER_BLOOD);
      };

        // 2. Draw SPAWN banner (only during first 4 seconds)
        if (get_time_s() - level_timer < 4.0f) {

          t3d_mat4fp_from_srt_euler(&spawn_matrices[i],
            (float[3]){0.3f, 0.3f, 0.3f},
            (float[3]){0, 0, 0},
            (float[3]){level->zombies[i].position.v[0], 0, level->zombies[i].position.v[2]});
            
            draw_floor_banner(&spawn_matrices[i], BANNER_SPAWN);
          
        };

      };

      // Draw (Bullets)
      bullet_draw(&bullet);
      
      // Draw (Player) - only after 1.15 seconds
      if (slayer_animation_should_draw_slayer(&slayer_anim)) {
          player_draw(&player);
      };

      // (Draw (Player Crack Animation)
      if (slayer_animation_should_draw_crack(&slayer_anim)) {
          crack_draw(&crack);
      }

      // Draw (Zombies)
      zombie_draw_all(zombies, zombie_count);

      // Call #3 - End of frame
      audio_update();

      //SYNC PIPE ISSUE
      // Draw Health Bar (Zombie)
      rdpq_sync_pipe();
      for (int i = 0; i < zombie_count; i++) {
        draw_zombie_health_bar(&zombies[i], vp, camera.mode);  // Now passes CameraMode
      }

      /*
      SYNY PIPE ISSUE
      // Draw Health Bar (Slayer)
      draw_player_health_bar(&player, vp);
      */

      // ======== Draw (UI) ======== //
      float posX = 16;
      float posY = 24;

      rdpq_sync_pipe();

      posY = 216;
      // MEMORY TRACKING
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 10, 15, "Mem: %d KiB", heap_stats.used/1024); // get memory usage
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "FPS: %.2f", display_get_fps()); posY += 10; // Get FPS
  
      // LEVEL
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Map (X, Y): (%.4f, %.4f)",  map.position.v[0], map.position.v[2]); posY += 10; //Displays position
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Positions (X, Y): (%d, %d)", joypad.stick_x, joypad.stick_y); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Enemy Count: (%d)", enemy_count); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Total Levels: (%d)", TOTAL_LEVELS); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Current Level: (%d)", get_current_level_index() + 1); posY += 10;

      // BULLET
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Bullet Position (X, Y): (%.4f, %.4f)", bullet.position.v[0], bullet.position.v[2]); posY += 10; //Displays position
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Bullet Rotation: %.4f", bullet.rotation_y); posY += 10;

      //TIME
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Game Time: (%.4f)", get_time_s()); posY += 10; // Since turning on n64
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Level Start Time: (%.4f)", level_timer); posY += 10;
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Level Start Time: (%.4f)", tutorial_time); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "After Death time: (%.4f)", get_time_s() - zombies[0].blood_time); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Reset Time: (%.4f)", get_time_s() - level_timer); posY += 10;
      

      //SLAYER
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zomebie Player Distance (X, Y): (%.4f)", sqrt((player.position.v[0] - zombies[0].position.v[0]) * (player.position.v[0] - zombies[0].position.v[0]) + (player.position.v[2] - zombies[0].position.v[2]) * (player.position.v[2] - zombies[0].position.v[2]))); posY += 10; //Displays position
      rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Player Pos. (X, Y, Z): (%.4f, %.4f, %.4f)", player.position.v[0], player.position.v[2], player.position.v[1]); posY += 10; //Displays position
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Slayer Rotation (Y):%.4f", player.rotation_y); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Speed: %.4f", player.speed); posY += 10; //Speed

      // ZOMBIE
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zombie blood scale: (%.4f)",zombies[1].blood_scale - 0.01f * (get_time_s() - zombies[0].blood_time)); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zombie Count: (%d)", zombie_count); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Enemy Count: (%d)", enemy_count); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zombie Death time:%.4f", zombies[0].blood_time); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zombie Life: (%d, %d)", zombies[0].health, zombies[1].health); posY += 10;
      //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Zombie Alive: (%d, %d)", zombies[0].alive, zombies[1].alive); posY += 10;


      rdpq_detach_show();

    // Manual exit - press start OR reached end level
    if (btn.start || level == &LEVEL_END) {
        state = STATE_MENU;
    };
  
    }; // End Tutorial Loop

    // Audio Cleanup (stops and unloads everything)
    audio_cleanup_all();
    
    // Bullets Cleanup
    bullet_cleanup(&bullet);
    
    // Map Cleanup
    map_destroy(&map);
    map_destroy(&mapWall);
    map_destroy(&mapPortal);
    
    // Player Cleanup
    player_cleanup(&player);
    
    // Sprite Cleanup

    // Zombie(s) Cleanup
    zombie_destroy_all(zombies, allocated_zombie_count);

    // Banner(s) Cleanup
    crack_cleanup(&crack);
    banners_destroy();
    free_uncached(spawn_matrices);
    free_uncached(blood_matrices);

    // Arrow Cleanup
    // Free model matrix
    free_uncached(arrow_matrices);
    // Free model
    t3d_model_free(arrow_model);

}