// TODO: add more materials, lava, obsidian, wood (planks)
// TODO: make right click menu on tiles actually allow customization
// off that tile, like tile.w_material, etc...
// TODO: add support for multiple world files, zstd is great for saving raw memory currently
// now just add a worlds/ folder, and open a "native file dialogue" to choose
// which world to load, instead of just './world.zst'

#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "arg_handler.h"
#include "arbitrary.h"
#include "setup.h"
#include "render.h"
#include "zstd.h"
#include "common.h"


static void compress(Cell (*grid)[ROWS], const char* fname) {
	size_t fSize = sizeof(grid[0][0]) * COLS * ROWS;
	void* const fBuff = grid;
	size_t const cBuffSize = ZSTD_compressBound(fSize);
	void* const cBuff = malloc_orDie(cBuffSize);
	size_t const cSize = ZSTD_compress(cBuff, cBuffSize, fBuff, fSize, 1);
	CHECK_ZSTD(cSize);
	saveFile_orDie(fname, cBuff, cSize);
	printf("%s : %6u -> %7u - %s \n", "grid", (unsigned)fSize, (unsigned)cSize, "world.zst");
	free(cBuff);
}

static void decompress(Cell (*grid)[ROWS], const char* fname)
{
    size_t cSize;
    void* const cBuff = mallocAndLoadFile_orDie(fname, &cSize);
    /* Read the content size from the frame header. For simplicity we require
     * that it is always present. By default, zstd will write the content size
     * in the header when it is known. If you can't guarantee that the frame
     * content size is always written into the header, either use streaming
     * decompression, or ZSTD_decompressBound().
     */
    unsigned long long const rSize = ZSTD_getFrameContentSize(cBuff, cSize);
    CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
    CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);

    void* const rBuff = grid;
    /* Decompress.
     * If you are doing many decompressions, you may want to reuse the context
     * and use ZSTD_decompressDCtx(). If you want to set advanced parameters,
     * use ZSTD_DCtx_setParameter().
     */
    size_t const dSize = ZSTD_decompress(rBuff, rSize, cBuff, cSize);
    CHECK_ZSTD(dSize);
    /* When zstd knows the content size, it will error if it doesn't match. */
    CHECK(dSize == rSize, "Impossible because zstd will check this condition!");

    /* success */
    printf("%s : %6u -> %7u \n", fname, (unsigned)cSize, (unsigned)rSize);

    // free(rBuff);
    free(cBuff);
}

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
    delay.tv_nsec = 16666666; // second number is desired ups

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
        updateSpawner(grid, grid_duplicate);
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
        updateVoidTile(grid, grid_duplicate);
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
	version_info(argv, argc);
    // Setup window and display settings
    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "master", LOG_WARNING, false);
    int32_t current_monitor = handle_arguments(argc, argv);
    if (current_monitor < 0)
    {
        exit(EXIT_FAILURE);
    }

	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
	MsgBox *save_grid = malloc(sizeof(MsgBox));
	MsgBox *load_grid = malloc(sizeof(MsgBox));
	load_grid->show = false;
	load_grid->title = "Load grid from file";
	load_grid->text = malloc(sizeof(char) * 96);
	load_grid->text[0] = '\0';
	strcat(load_grid->text, basename(argv[0]));
	strcat(load_grid->text, " will attempt to load world.zst");

	load_grid->buttons = 2;
	load_grid->position.x = (int)(GetScreenWidth()/2-150);
	load_grid->position.y = (int)(GetScreenHeight()/2-60);
	load_grid->position.width = 300;
	load_grid->position.height = 120;
	load_grid->dragging = false;
	load_grid->drag_offset = (Vector2){0,0};

	save_grid->show = false;
	save_grid->position.x = 400;
	save_grid->position.y = 200;
	save_grid->position.width = 260;
	save_grid->position.height = 140;
	save_grid->title = "File saving";
	save_grid->buttons = 2;
	save_grid->drag_offset = (Vector2){0,0};
	save_grid->dragging = false;

	char cwd[10] = "";
	strcat(cwd, GetDirectoryPath("."));
	size_t str_length = strlen("Save file to `") + strlen(cwd) + strlen("` ?") + 1; // +1 for null terminator
	save_grid->text = malloc(str_length);
	if (!save_grid->text) {
		perror("Failed to allocate memory for save_grid->text");
		exit(EXIT_FAILURE);
	}
	snprintf(save_grid->text, str_length, "Save file to `%s` ?", cwd);
	save_grid->text[str_length - 1] = '\0';

	bool show_rmb_menu_tile = false;
	// maybe should malloc?
	RmbMenu rmbmenu = {0};
    Color colors[] = {
		NOCOLOR,
		COLOR_SAND,
		COLOR_WATER,
		COLOR_STONE,
		COLOR_STEAM,
		WHITE,
		PURPLE,
		NOCOLOR
	};
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

    uint32_t material = Water;
    srand(time(NULL));
    Cell(*grid)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
    memset(grid, 0, sizeof(Cell) * COLS * ROWS);
	printf("Loading world from world.bin\n");
	FILE *f_save_grid_load = fopen("world.bin","r+");
	if (f_save_grid_load != NULL) {
		while (true) {
			fread(grid, sizeof(Cell), ROWS*COLS, f_save_grid_load);
			if (feof(f_save_grid_load)) {
				break;
			}
			//fseek(f_save_grid_load, 1, SEEK_CUR);
		}
		fclose(f_save_grid_load);

	}
	else { perror("physim: "); }


	
    Shader bloomShader = LoadShader(0, "src/shaders/bloom.fs");

    float cur_dt = 0;
    float second_counter = 0.0f;
    float fps_counter = 0.0f;
    int frame_counter = 0;
    float average_fps = 0.0f;
    Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 64, NULL, 0);
	//GenTextureMipmaps(&jetmono.texture);
	SetTextureFilter(jetmono.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(config.fps);
    Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
    if (grid_duplicate == NULL)
    {
        printf("Failed to malloc grid\n");
        return -1;
    }
    memset(grid_duplicate, 0, sizeof(Cell) * COLS * ROWS);

    int32_t tileCount = 0;

    pthread_t update_thread;
	UpdateData update_data = { grid, grid_duplicate, &grid_mutex, &grid_duplicate_mutex };
	pthread_create(&update_thread, NULL, update_worker, (void*)&update_data); 
	image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	image.width=SCREEN_WIDTH;
	image.height=SCREEN_HEIGHT;
	Texture2D screenTex = LoadTextureFromImage(image);
	uint32_t w_material = Empty;
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

        // Update brush size with mouse wheel input
        config.brush_size += (int32_t)GetMouseWheelMove() * 3;
        if (config.brush_size < 1)
            config.brush_size = 1;
        else if (config.brush_size > 150)
			config.brush_size = 150;

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

		// DrawRectangle(15, 15, 580, 50, WHITE);
        DrawTextEx(jetmono, TextFormat("Tiles on screen: %05d | Average FPS: %.2f | FPS: %05d ", tileCount, (double)average_fps, (int)(1.0f / cur_dt)), (Vector2){20, 20}, 20, 1, WHITE);
        DrawText(TextFormat("Brush size: %d mode = %s", config.brush_size, config.brush_mode ? "true" : "false"), 20, 44, 20, RED);
        DrawRectangle(selector_xval-5, selector_yval-5, selector_tsize*MatCount+5*MatCount+5, selector_tsize+10, DARKGRAY);
        DrawRectangleWithBorder(selector_xval + selector_offset * Empty, selector_yval, selector_tsize, selector_tsize, (Color){0, 0, 0, 255}, BLACK);
		for (size_t i = 0; i < MatCount; i++) {
			DrawRectangleWithBorder(selector_xval + selector_offset * i, selector_yval, selector_tsize, selector_tsize, colors[i], BLACK);
		}
		for (size_t i = 0; i < MatCount; i++) {
			Draw_selector_tooltip(selector_xval + selector_offset * i, selector_yval, selector_tsize, selector_tsize, i, &jetmono);
		}
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
        
        
        
        DrawTextEx(jetmono, TextFormat("M:%s\nC:%u,%u,%u\nwM:%s\nFR:%f\nvX:%.1f,vY:%.1f\nFALL:%s\nSF:%f\nMASS:%d\n",
			str_mat(grid[temp_draw_mouse_x][temp_draw_mouse_y].material),
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.r,
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.g,
			(uint8_t)grid[temp_draw_mouse_x][temp_draw_mouse_y].color.b,
			str_mat(grid[temp_draw_mouse_x][temp_draw_mouse_y].w_material),
            grid[temp_draw_mouse_x][temp_draw_mouse_y].friction,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].velocityX,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].velocityY,
			tf_str(grid[temp_draw_mouse_x][temp_draw_mouse_y].isFreeFalling),
			grid[temp_draw_mouse_x][temp_draw_mouse_y].spreadFactor,
			grid[temp_draw_mouse_x][temp_draw_mouse_y].mass),
			(Vector2){SCREEN_WIDTH-150,65},20,0,WHITE);
		DrawTextEx(jetmono, TextFormat("UPS on #2: %d\nmx:%d my:%d i:%d j:%d\n",displayed_ups,GetMouse_X_safe(), GetMouse_Y_safe(), temp_draw_mouse_x,temp_draw_mouse_y), (Vector2){SCREEN_WIDTH-235,20},20,0,WHITE);
        if (IsKeyPressed(KEY_N)) {
            config.brush_mode = !config.brush_mode;
        }
		if (IsKeyPressed(KEY_B)) {
            config.wants_shader = !config.wants_shader;
        }
		if (IsKeyPressed(KEY_E) && tileCount == 0) {
			update_should_stop = true;
			pthread_join(update_thread, NULL);


			int randmat = Empty;
			printf("%ld\n", config.random_grid_randomness);
			for (int i = 0; i < COLS; i++) {
				for (int j = 0; j < ROWS; j++) {
					randmat = GetRandomValue(0,config.random_grid_randomness);
					if (randmat > 4) {
						randmat = 0;
					}
                    // randmat = (int)((float)((float)rand() / (float)RAND_MAX) * 4);
                    if (GetRandomValue(0, INT32_MAX) < 2100000)
                    {
                        randmat = Spawner;
						if (GetRandomValue(0, 10) > 5) {
							grid[i][j].w_material = Water;
						} else {
							grid[i][j].w_material = Sand;
						}
                    }
                    if (GetRandomValue(0, INT32_MAX) < 1500000)
                    {
                        randmat = VoidTile;
                        grid[i][j].w_material = Empty;
                    }
					grid[i][j].material = randmat;
					grid[i][j].color = rand_color_mat(randmat);
				}
			}
			update_should_stop = false;
			pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
		}
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
		{
			save_grid->show = true;
		}
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) {
			load_grid->show = true;
		}
		if (Draw_message_box(load_grid, &jetmono) == 1) {
			if (FileExists("world.zst")) {
				bool tmp = update_should_stop;
				update_should_stop = true;
				// nanosleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 30000000}, NULL);
				pthread_join(update_thread, NULL);
				update_should_stop = tmp;
				decompress(grid, "world.zst");
				pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
			}
			else {
				fprintf(stderr, "You might want to check world.zst exists before loading it\n");
			} 
		}
		if (Draw_message_box(save_grid, &jetmono) == 1) {
			if (FileExists("world.zst")) {
				fprintf(stderr, "world.zst was overwritten, was this intended?\n");
			}
			compress(grid, "world.zst");
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
				if (mx > selector_xval+selector_offset*Empty && mx < selector_xval+selector_offset*Sand)
				{
					material = Empty;
				}
				else if (mx > selector_xval+selector_offset*Sand && mx < selector_xval+selector_offset*Water)
				{
					material = Sand;
				}
				else if (mx > selector_xval+selector_offset*Water && mx < selector_xval+selector_offset*Stone)
				{
					material = Water;
				}
				else if (mx > selector_xval+selector_offset*Stone && mx < selector_xval+selector_offset*Steam)
				{
					material = Stone;
				}
				else if (mx > selector_xval+selector_offset*Steam && mx < selector_xval+selector_offset*Spawner)
				{
					material = Steam;
				}
				else if (mx > selector_xval+selector_offset*Spawner && mx < selector_xval+selector_offset*VoidTile)
				{
					material = Spawner;
				}
				else if (mx > selector_xval+selector_offset*VoidTile && mx < selector_xval+selector_offset*Obsidian)
				{
					material = VoidTile;
				}
				else if (mx > selector_xval+selector_offset*Obsidian && mx < selector_xval+selector_offset*MatCount)
				{
					material = Obsidian;
				}
			}
		}

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            int32_t mx = GetMouse_X_safe();
            int32_t my = GetMouse_Y_safe();
            if (mx > (int32_t)(selector_xval-5) && mx < (int32_t)(selector_tsize*MatCount+5*MatCount+selector_xval) && my > selector_yval-5 && my < selector_yval+selector_tsize+5)
            {
            }
            else
            {

                int gridX = mx / BLOCK_SIZE;
                int gridY = my / BLOCK_SIZE;
				if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS && !CheckCollisionPointRec((Vector2){mx,my}, rmbmenu.spos))
                {
					if (save_grid->show && !CheckCollisionPointRec(GetMousePosition(), save_grid->position)) {
						spawnSandBrush(grid, mx, my, config.brush_size, material, w_material,config.brush_mode);
					} else if (!save_grid->show){
						spawnSandBrush(grid, mx, my, config.brush_size, material, w_material,config.brush_mode);
					}
                }
            }
        }

        int mx = GetMouse_X_safe();
        int my = GetMouse_Y_safe();

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			
			rmbmenu.status = 0;
		}
			rmbmenu.mpos.x = mx;
			rmbmenu.mpos.y = my;
		draw_rmb_menu_tile(&rmbmenu, &show_rmb_menu_tile);

        if ( IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ZERO))
        {
            update_should_stop = true;
            // struct timespec tmp = { .tv_sec = 0, .tv_nsec = 20000000 };
            // nanosleep(&tmp, NULL);
			pthread_join(update_thread, NULL);
            memset(grid, 0, sizeof(Cell) * COLS * ROWS);
            update_should_stop = false;
            pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
        }
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R) ) {

			if (reload_config_file(cfg_file, &config) != NULL) {
				printf("Reloaded file contents:\n%s\n", config.cfg_buffer);
				parse_config_variables(config.cfg_buffer, &config);
				SetTargetFPS(config.fps);
			}
			printf("CFG(%s): fps: %d | brush_size = %d | brush_mode = %d | M:%s\n",tf_str(config.is_cfg_read), config.fps, config.brush_size, config.brush_mode, str_mat(Water));
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
    
	update_should_stop = true;
    pthread_join(update_thread, NULL);

    // Cleanup
    UnloadShader(bloomShader);
	UnloadTexture(screenTex);
	UnloadFont(jetmono);

	free(save_grid->text);
	free(load_grid->text);
	free(load_grid);

	free(save_grid);
	free(grid);
    free(grid_duplicate);
	free(config.cfg_buffer);
    CloseWindow();
    return 0;
}

