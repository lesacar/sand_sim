#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "arg_handler.h"
#include "setup.h"

bool update_should_stop = false;
int updates_per_second = 0;
int32_t displayed_ups;

typedef struct {
    Cell (*grid)[ROWS];
    Cell (*grid_duplicate)[ROWS];
    pthread_mutex_t* grid_mutex;
    pthread_mutex_t* grid_duplicate_mutex;
} UpdateData;

void* update_worker(void* data) {
    UpdateData* update_data = (UpdateData*)data;
    Cell (*grid)[ROWS] = update_data->grid;
    Cell (*grid_duplicate)[ROWS] = update_data->grid_duplicate;
    pthread_mutex_t* grid_mutex = update_data->grid_mutex;
    pthread_mutex_t* grid_duplicate_mutex = update_data->grid_duplicate_mutex;

    struct timespec start_time, end_time;
    long elapsed_ns;

    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 16666666; // Initial sleep duration for 60 updates per second

    while (!update_should_stop) {
        // Measure start time
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // Lock mutexes before modifying grid and grid_duplicate
        pthread_mutex_lock(grid_mutex);
        pthread_mutex_lock(grid_duplicate_mutex);

        // Execute updates
        memcpy(*grid_duplicate, *grid, sizeof(Cell) * COLS * ROWS);
        sand(grid, grid_duplicate);
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
        updateSteam(grid, grid_duplicate);
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
        updateWater(grid, grid_duplicate);

        // Increment updates per second counter
        updates_per_second++;
        // Unlock mutexes after updates
        pthread_mutex_unlock(grid_mutex);
        pthread_mutex_unlock(grid_duplicate_mutex);

        // Measure end time
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // Calculate elapsed time in nanoseconds
        elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000L +
                     (end_time.tv_nsec - start_time.tv_nsec);

        // Adjust sleep duration based on elapsed time and desired update rate
        long target_duration_ns = 16666666; // Target duration for 60 updates per second
        long adjusted_sleep_ns = target_duration_ns - elapsed_ns;
        if (adjusted_sleep_ns > 0) {
            delay.tv_nsec = adjusted_sleep_ns;
        } else {
            // If the update process takes longer than the target duration,
            // set a minimum sleep duration to avoid busy-waiting
            delay.tv_nsec = 1000000; // 1 millisecond
        }

        // Sleep for adjusted duration
        nanosleep(&delay, NULL);
    }

    // Cleanup and exit thread
    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char **argv)
{
    // Setup window and display settings
    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "sand_sim", LOG_WARNING, false);
    int32_t current_monitor = handle_arguments(argc, argv);
    if (current_monitor < 0)
    {
        exit(EXIT_FAILURE);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
	pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t grid_duplicate_mutex = PTHREAD_MUTEX_INITIALIZER;
	const char *cfg_file = "sim.cfg";
	ConfigData config = parse_config_file(cfg_file); // Parse the configuration file
	if (config.is_cfg_read) {
		parse_config_variables(config.cfg_buffer, &config);
        printf("File contents:\n%s\n", config.cfg_buffer);
		printf("CFG: fps: %d | brush_size = %d | brush_mode = %d\n",config.fps, config.brush_size, config.brush_mode);
    }

    uint32_t material = 0;
    
    Cell(*grid)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
    memset(grid, 0, sizeof(Cell) * COLS * ROWS);
    printf("TARGET FPS: %d\n", set_monitor_and_fps(current_monitor));
    Shader bloomShader = LoadShader(0, "src/shaders/bloom.fs");

    bool scroll_hint_message = false;

    float cur_dt = 0;
    float second_counter = 0.0f;
    float fps_counter = 0.0f;
    int frame_counter = 0;
    float average_fps = 0.0f;
    Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, 0, 251);

    RenderTexture2D gridTexture = LoadRenderTexture(COLS, ROWS);
    if (gridTexture.id == 0)
    {
        printf("Failed to create grid texture\n");
        exit(EXIT_FAILURE);
    }
    SetTargetFPS(config.fps);
    Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
    if (grid_duplicate == NULL)
    {
        printf("Failed to malloc grid\n");
        return -1;
    }

    int32_t tileCount = 0;

    pthread_t update_thread;
    UpdateData update_data = { grid, grid_duplicate, &grid_mutex, &grid_duplicate_mutex };
    pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
    while (!WindowShouldClose())
    {
        cur_dt = GetFrameTime(); // Get frame time
        frame_counter++;
        fps_counter += cur_dt;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginTextureMode(gridTexture);
        ClearBackground(BLACK);

        for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                if (grid[i][j].material != Empty)
                {
                    Color color = grid[i][j].color;
                    DrawRectangle(i, j, 1, 1, color);
                }
            }
        }

        EndTextureMode();
        DrawTexturePro(gridTexture.texture, (Rectangle){0, 0, (float)gridTexture.texture.width, (float)-gridTexture.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0, WHITE);

        // Update brush size with mouse wheel input
        config.brush_size += (int32_t)GetMouseWheelMove() * 3;
        if (config.brush_size < 1)
            config.brush_size = 1;
        else if (config.brush_size > 150)
            config.brush_size = 150;

        if (!update_should_stop) {
            tileCount = 0;
            for (int i = 0; i < COLS; i++)
            {
                for (int j = 0; j < ROWS; j++)
                {
                    if (grid[i][j].material != Empty)
                    {
                        tileCount++;
                    }
                }
            }
        }

        DrawRectangle(15, 15, 580, 50, WHITE);
        DrawTextEx(jetmono, TextFormat("Tiles on screen: %05d | Average FPS: %.2f | FPS: %05d ", tileCount, (double)average_fps, (int)(1.0f / cur_dt)), (Vector2){20, 20}, 20, 1, BLACK);
        DrawText(TextFormat("Brush size: %d mode = %s", config.brush_size, config.brush_mode ? "true" : "false"), 20, 44, 20, RED);
        DrawRectangle(15, 140, 200, 50, WHITE);
        DrawRectangle(20, 145, 40, 40, (Color){201, 170, 127, 255});
        DrawRectangle(65, 145, 40, 40, (Color){0, 0, 255, 255});
        DrawRectangle(65 + 45, 145, 40, 40, (Color){51, 83, 69, 255});

        int temp_draw_mouse_x = GetMouseX();
        int temp_draw_mouse_y = GetMouseY();
        if (temp_draw_mouse_x / BLOCK_SIZE > COLS) { temp_draw_mouse_x = COLS;}
        if (temp_draw_mouse_x / BLOCK_SIZE > ROWS) { temp_draw_mouse_y = ROWS;}
        DrawText(TextFormat("M:%d\nC:%u,%u,%u\n", grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].material,
           grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.r,
           grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.g,
           grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.b), SCREEN_WIDTH-100,45,16,WHITE);
		DrawTextEx(jetmono, TextFormat("UPS on #2: %d",displayed_ups), (Vector2){SCREEN_WIDTH-125,20},20,0,WHITE);
        if (IsKeyPressed(KEY_B)) {
            config.brush_mode = !config.brush_mode;
        }
        if(IsKeyPressed(KEY_SPACE)) {
			if (update_should_stop == true) {
				pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
			}
            update_should_stop = !update_should_stop;

        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int mx = GetMouseX();
            int my = GetMouseY();
            if (my > 145 && my < 185)
            {
                if (mx > 20 && mx < 60)
                {
                    material = Sand;
                }
                else if (mx > 65 && mx < 65 + 40)
                {
                    material = Water;
                }
                else if (mx > 65 + 45 && mx < 140)
                {
                    material = Stone;
                }
                else if (mx > 65 + 90 && mx < 140 + 45)
                {
                    material = Steam;
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            int mx = GetMouseX();
            int my = GetMouseY();
            if (mx > 15 && mx < 15 + 200 && my > 140 && my < 140 + 50)
            {
            }
            else
            {

                int gridX = mx / BLOCK_SIZE;
                int gridY = my / BLOCK_SIZE;

                if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
                {
                    spawnSandBrush(grid, mx, my, config.brush_size, material, config.brush_mode);
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            int mx = GetMouseX();
            int my = GetMouseY();

            int gridX = mx / BLOCK_SIZE;
            int gridY = my / BLOCK_SIZE;

            if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
            {
                spawnSandBrush(grid, mx, my, config.brush_size, 0, config.brush_mode);
            }
        }

        if (!scroll_hint_message)
        {
            DrawText("Scroll with mouse wheel to increase/reduce brush size (max 50)", 20, 66, 20, YELLOW);
            DrawText("Press R to reset simulation, Press B to toggle BRUSH / CIRCLE draw mode", 20, 88, 20, YELLOW);
            DrawText("Press K to toggle fps cap", 20, 110, 20, YELLOW);
            DrawText("Press H to hide this message", 20, 132, 20, YELLOW);
            if (IsKeyPressed(KEY_H))
            {
                scroll_hint_message = true;
            }
            
        }

        if (IsKeyPressed(KEY_R))
        {
            memset(grid, 0, sizeof(Cell) * COLS * ROWS);
        }
		if (IsKeyPressed(KEY_C)) {

			if (reload_config_file(cfg_file, &config) != NULL) {
				printf("Reloaded file contents:\n%s\n", config.cfg_buffer);
				parse_config_variables(config.cfg_buffer, &config);
				SetTargetFPS(config.fps);
			}
			printf("CFG(%d): fps: %d | brush_size = %d | brush_mode = %d\n",config.is_cfg_read, config.fps, config.brush_size, config.brush_mode);
		}
        EndDrawing();
        second_counter += cur_dt;

        // If one second has passed, calculate FPS and UPS
        if (second_counter >= 1.0f)
        {
            // Calculate average FPS
            average_fps = (float)frame_counter / fps_counter;

            // Print updates per second (UPS)
            // printf("Updates per second: %d\n", updates_per_second);
            
            // Reset counters
            second_counter -= 1.0f;
            fps_counter = 0.0f;
            frame_counter = 0;
			displayed_ups = updates_per_second;
            updates_per_second = 0;
        };
    }
    // Set the flag to stop the update thread
    update_should_stop = true;

    // Wait for the update thread to finish
    pthread_join(update_thread, NULL);

    // Cleanup
    UnloadShader(bloomShader);
    UnloadRenderTexture(gridTexture);
    free(grid_duplicate);
	free(config.cfg_buffer);
    CloseWindow();
    return 0;
}

