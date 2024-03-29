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

void UpdateScreenImage(Cell (*grid)[ROWS], Image* screenImage) {
    // Calculate the scaled dimensions of the grid

    // Resize the screen image to match the scaled grid dimensions
    // *screenImage = GenImageColor(scaledWidth, scaledHeight, BLACK);

    // Cast data pointer to Color*
    Color* imageData = (Color*)screenImage->data;

    // Load grid data into the screen image, scaling by BLOCK_SIZE
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Color color = grid[i][j].color; // Assuming grid contains color information
            // Set color for the entire block
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    // Calculate the coordinates in the scaled image
                    int scaledX = i * BLOCK_SIZE + x;
                    int scaledY = j * BLOCK_SIZE + y;
                    // Set pixel color in the scaled image
                    imageData[scaledY * SCREEN_WIDTH + scaledX] = color;
                }
            }
        }
    }
}

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
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
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
    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "physim", LOG_WARNING, false);
    int32_t current_monitor = handle_arguments(argc, argv);
    if (current_monitor < 0)
    {
        exit(EXIT_FAILURE);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    int32_t selector_xval = 20; // x coordinate of the first tile's top left corner
    int32_t selector_yval = 75; // y coordinate of the first tile's top left corner
    int32_t selector_offset = 45; // how far away each icon is in the selector (subtract selector_tsize, ex. selector_offset=45; selector_tsize=40; REAL offset = 45-40 = 5)
    int32_t selector_tsize = 40; // size of icon in selector
	Image image = GenImageColor(SCREEN_WIDTH,SCREEN_HEIGHT,PINK);
	pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t grid_duplicate_mutex = PTHREAD_MUTEX_INITIALIZER;
	const char *cfg_file = "sim.cfg";
	ConfigData config = parse_config_file(cfg_file); // Parse the configuration file
	if (config.is_cfg_read) {
		parse_config_variables(config.cfg_buffer, &config);
        printf("File contents:\n%s\n", config.cfg_buffer);
		printf("CFG: Target fps: %d | brush_size = %d | brush_mode = %s\n", config.fps, config.brush_size, tf_str(config.brush_mode));
    }

    uint32_t material = 0;
    
    Cell(*grid)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
    memset(grid, 0, sizeof(Cell) * COLS * ROWS);
    Shader bloomShader = LoadShader(0, "src/shaders/bloom.fs");

    float cur_dt = 0;
    float second_counter = 0.0f;
    float fps_counter = 0.0f;
    int frame_counter = 0;
    float average_fps = 0.0f;
    Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, 0, 251);

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
	image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	image.width=SCREEN_WIDTH;
	image.height=SCREEN_HEIGHT;
	Texture2D screenTex = LoadTextureFromImage(image);
    while (!WindowShouldClose())
    {
        cur_dt = GetFrameTime(); // Get frame time
        frame_counter++;
        fps_counter += cur_dt;
		
		UpdateScreenImage(grid, &image);
		UpdateTexture(screenTex, image.data);
        BeginDrawing();
        ClearBackground(BLACK);
		if (config.wants_shader) {
			BeginShaderMode(bloomShader); }
		DrawTexture(screenTex,0,0,WHITE);
		if (config.wants_shader) {
			EndShaderMode();
		}

        /* for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                if (grid[i][j].material != Empty)
                {
                    Color color = grid[i][j].color;
                    DrawRectangle(i, j, 1, 1, color);
                }
            }
        } */


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

        // DrawRectangle(15, 15, 580, 50, WHITE);
        DrawTextEx(jetmono, TextFormat("Tiles on screen: %05d | Average FPS: %.2f | FPS: %05d ", tileCount, (double)average_fps, (int)(1.0f / cur_dt)), (Vector2){20, 20}, 20, 1, WHITE);
        DrawText(TextFormat("Brush size: %d mode = %s", config.brush_size, config.brush_mode ? "true" : "false"), 20, 44, 20, RED);
        DrawRectangle(selector_xval-5, selector_yval-5, selector_tsize*sizeof(MaterialTypes)+5*sizeof(MaterialTypes)+5, selector_tsize+10, WHITE);
        DrawRectangle(selector_xval+selector_offset*0, selector_yval, selector_tsize, selector_tsize, COLOR_SAND);
        DrawRectangle(selector_xval+selector_offset*1, selector_yval, selector_tsize, selector_tsize, COLOR_WATER);
        DrawRectangle(selector_xval+selector_offset*2, selector_yval, selector_tsize, selector_tsize, COLOR_STONE);
        DrawRectangle(selector_xval+selector_offset*3, selector_yval, selector_tsize, selector_tsize, COLOR_STEAM);

        int temp_draw_mouse_x = GetMouse_X_safe()/ BLOCK_SIZE;
        if (temp_draw_mouse_x < 0)
        {
            temp_draw_mouse_x = 0;
        } else if (temp_draw_mouse_x >= COLS)
        {
            temp_draw_mouse_x = COLS-1;
        }
        int temp_draw_mouse_y = GetMouse_Y_safe()/BLOCK_SIZE;
        if (temp_draw_mouse_y < 0)
        {
            temp_draw_mouse_y = 0;
        } else if (temp_draw_mouse_y >= ROWS)
        {
            temp_draw_mouse_y = ROWS-1;
        }
        
        
        
        DrawTextEx(jetmono, TextFormat("M:%s\nC:%u,%u,%u\nFR:%f\nvX:%.1f,vY:%.1f\nFALL:%s\nSF:%f\nMASS:%d\n",
			str_mat(grid[temp_draw_mouse_x][temp_draw_mouse_y].material),
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.r,
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.g,
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.b,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].friction,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].velocityX,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].velocityY,
			tf_str(grid[temp_draw_mouse_x][temp_draw_mouse_y].isFreeFalling),
			grid[temp_draw_mouse_x][temp_draw_mouse_y].spreadFactor,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].mass),
			(Vector2){SCREEN_WIDTH-150,65},20,0,WHITE);
		DrawTextEx(jetmono, TextFormat("UPS on #2: %d\nmx:%d my:%d i:%d j:%d\n",displayed_ups,GetMouse_X_safe(), GetMouse_Y_safe(), temp_draw_mouse_x,temp_draw_mouse_y), (Vector2){SCREEN_WIDTH-235,20},20,0,WHITE);
        if (IsKeyPressed(KEY_B)) {
            config.brush_mode = !config.brush_mode;
        }
		if (IsKeyPressed(KEY_S)) {
            config.wants_shader = !config.wants_shader;
        }
        if(IsKeyPressed(KEY_SPACE)) {
			if (update_should_stop == true) {
				pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
			}
            update_should_stop = !update_should_stop;

        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int mx = GetMouse_X_safe();
            int my = GetMouse_Y_safe();
            if (my > selector_yval && my < selector_yval+selector_tsize)
            {
                if (mx > selector_xval+selector_offset*0 && mx < selector_xval+selector_offset*1)
                {
                    material = Sand;
                }
                else if (mx > selector_xval+selector_offset*1 && mx < selector_xval+selector_offset*2)
                {
                    material = Water;
                }
                else if (mx > selector_xval+selector_offset*2 && mx < selector_xval+selector_offset*3)
                {
                    material = Stone;
                }
                else if (mx > selector_xval+selector_offset*3 && mx < selector_xval+selector_offset*4)
                {
                    material = Steam;
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            int32_t mx = GetMouse_X_safe();
            int32_t my = GetMouse_Y_safe();
            if (mx > (int32_t)(selector_xval-5) && mx < (int32_t)(selector_tsize*sizeof(MaterialTypes)+5*sizeof(MaterialTypes)+selector_xval) && my > selector_yval-5 && my < selector_yval+selector_tsize+5)
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
            int mx = GetMouse_X_safe();
            int my = GetMouse_Y_safe();

            int gridX = mx / BLOCK_SIZE;
            int gridY = my / BLOCK_SIZE;

            if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
            {
                spawnSandBrush(grid, mx, my, config.brush_size, 0, config.brush_mode);
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
			printf("CFG(%d): fps: %d | brush_size = %d | brush_mode = %d | M:%s\n",config.is_cfg_read, config.fps, config.brush_size, config.brush_mode, str_mat(Water));
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
	UnloadImage(image);
    // Set the flag to stop the update thread
    update_should_stop = true;

    // Wait for the update thread to finish
    pthread_join(update_thread, NULL);

    // Cleanup
    UnloadShader(bloomShader);
	UnloadTexture(screenTex);
    free(grid_duplicate);
	free(config.cfg_buffer);
    CloseWindow();
    return 0;
}

