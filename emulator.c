#include "feab.h"

#include <SDL2/SDL.h>
#include <signal.h>

#define ZOOM 8
#define WIDTH 64
#define HEIGHT 64
#define PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888

void keydown(SDL_KeyCode key);
void draw();
void events();

SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *texture, *sprites;

bool
emulator_init()
{
    if(SDL_Init(SDL_INIT_VIDEO))
        {
            fprintf(stderr, "[ERROR, SDL] SDL_Init failed: %s\n", SDL_GetError());
            return true;
        }

    window = SDL_CreateWindow("feab", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * ZOOM, HEIGHT * ZOOM, 0);
    if(!window)
        {
            fprintf(stderr, "[ERROR, SDL] SDL_CreateWindow failed: %s\n", SDL_GetError());
            return true;
        }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(!render)
        {
            fprintf(stderr, "[ERROR, SDL] SDL_CreateRenderer failed: %s\n", SDL_GetError());
            return true;
        }
    SDL_RenderSetLogicalSize(render, WIDTH, HEIGHT);
    SDL_RenderSetIntegerScale(render, 1);
    SDL_SetRenderDrawColor(render, 0, 0, 0, 0xff);
    SDL_RenderClear(render);
    SDL_RenderPresent(render);

    texture = SDL_CreateTexture(render, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    if(!texture)
        {
            fprintf(stderr, "[ERROR, SDL] SDL_CreateTexture for texture failed: %s\n", SDL_GetError());
            return true;
        }

    sprites = SDL_CreateTexture(render, PIXEL_FORMAT, SDL_TEXTUREACCESS_STATIC, 16, 16);
    if(!sprites)
        {
            fprintf(stderr, "[ERROR, SDL] SDL_CreateTexture for sprites failed: %s\n", SDL_GetError());
            return true;
        }
    SDL_SetTextureBlendMode(sprites, SDL_BLENDMODE_BLEND);

    return false;
}

void
emulator_deinit()
{
    SDL_DestroyTexture(sprites);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void
keydown(SDL_KeyCode key)
{
    switch(key)
        {
            case 'q':
            case SDLK_ESCAPE:
                feab.memory[MEMORY_FLAGS] |= FLAG_QUIT;
                break;

            case SDLK_LEFT:
                feab.memory[MEMORY_KEYS] |= KEY_LEFT;
                break;
            case SDLK_RIGHT:
                feab.memory[MEMORY_KEYS] |= KEY_RIGHT;
                break;
            case SDLK_UP:
                feab.memory[MEMORY_KEYS] |= KEY_UP;
                break;
            case SDLK_DOWN:
                feab.memory[MEMORY_KEYS] |= KEY_DOWN;
                break;

            case 'z':
                feab.memory[MEMORY_KEYS] |= KEY_Z;
                break;
            case 'x':
                feab.memory[MEMORY_KEYS] |= KEY_X;
                break;
            case 'c':
                feab.memory[MEMORY_KEYS] |= KEY_C;
                break;
            case 'v':
                feab.memory[MEMORY_KEYS] |= KEY_V;
                break;

            default:
                break;
        }
}

void
keyup(SDL_KeyCode key)
{
    switch(key)
        {
            case SDLK_LEFT:
                feab.memory[MEMORY_KEYS] &= ~KEY_LEFT;
                break;
            case SDLK_RIGHT:
                feab.memory[MEMORY_KEYS] &= ~KEY_RIGHT;
                break;
            case SDLK_UP:
                feab.memory[MEMORY_KEYS] &= ~KEY_UP;
                break;
            case SDLK_DOWN:
                feab.memory[MEMORY_KEYS] &= ~KEY_DOWN;
                break;

            case 'z':
                feab.memory[MEMORY_KEYS] &= ~KEY_Z;
                break;
            case 'x':
                feab.memory[MEMORY_KEYS] &= ~KEY_X;
                break;
            case 'c':
                feab.memory[MEMORY_KEYS] &= ~KEY_C;
                break;
            case 'v':
                feab.memory[MEMORY_KEYS] &= ~KEY_V;
                break;

            default:
                break;
        }
}

void
events()
{
    static SDL_Event event;

    while(SDL_PollEvent(&event))
        {
            switch(event.type)
                {
                    case SDL_QUIT:
                        feab.memory[MEMORY_FLAGS] |= FLAG_QUIT;
                        break;

                    case SDL_KEYDOWN:
                        keydown(event.key.keysym.sym);
                        break;

                    case SDL_KEYUP:
                        keyup(event.key.keysym.sym);
                        break;
                }
        }
}

void
draw()
{
    SDL_SetRenderDrawColor(render,
                           ((feab.memory[MEMORY_PALETTE_0] >> 4) & 3) * 0x55,
                           ((feab.memory[MEMORY_PALETTE_0] >> 2) & 3) * 0x55,
                           (feab.memory[MEMORY_PALETTE_0] & 3) * 0x55,
                           0xff);

    SDL_SetRenderTarget(render, texture);
    {
        static SDL_Rect src = { 0, 0, 4, 4 };
        static SDL_Rect dest = { 0, 0, 4, 4 };
        static int i;

        SDL_RenderClear(render);

        for(i = 0; i < NUM_SPRITES; ++i)
            {
                if(!(feab.memory[MEMORY_SPRITES_ROW_0_FLAGS] & (1 << i)))
                    continue;

                src.x = 4 * (i % 4), src.y = 4 * (i / 4);
                dest.x = feab.memory[MEMORY_SPRITE_0_X + 6 * i] - 4, dest.y = feab.memory[MEMORY_SPRITE_0_Y + 6 * i] - 4;

                SDL_RenderCopy(render, sprites, &src, &dest);
            }
    }
    SDL_SetRenderTarget(render, NULL);

    SDL_RenderClear(render);
    SDL_RenderCopy(render, texture, NULL, NULL);
    SDL_RenderPresent(render);
}

uint32_t
to_color(uint8_t byte)
{
    if(byte)
        {
            uint32_t color = 0xff;
            color |= (((byte >> 4) & 3) * 0x55) << 24;
            color |= (((byte >> 2) & 3) * 0x55) << 16;
            color |= ((byte & 3) * 0x55) << 8;
            return color;
        }
    else
        return 0;
}
void
load_sprites()
{
    static uint32_t pixels[256 * sizeof(uint32_t)];
    int sprite, sprite_x, sprite_y, x, y;
    uint8_t row, color;

    for(sprite = 0; sprite < 16; ++sprite)
        {
            sprite_x = sprite % 4;
            sprite_y = sprite / 4;

            for(y = 0; y < 4; ++y)
                {
                    row = feab.memory[sprite * 6 + MEMORY_SPRITE_0_ROW_0 + y];

                    for(x = 0; x < 4; ++x)
                        {
                            color = (row >> ((3 - x) * 2)) & 3;
                            pixels[sprite_x * 4 + sprite_y * 16 * 4 + x + y * 16] = to_color(feab.memory[MEMORY_PALETTE_0 + color]);
                        }
                }
        }

    SDL_UpdateTexture(sprites, NULL, pixels, 16 * sizeof(uint32_t));
}

void
sigint(int sig)
{
    feab.memory[MEMORY_FLAGS] |= FLAG_QUIT;
}

int
main(int argc, char *argv[])
{
    if(cmd(argc, argv))
        return 0;

    emulator_init();

    signal(SIGINT, sigint);

    while(!(feab.memory[MEMORY_FLAGS] & FLAG_QUIT))
        {
            events();
            draw();
            run();

            if(feab.memory[MEMORY_FLAGS] & FLAG_LOAD_SPRITE)
                {
                    load_sprites();
                    feab.memory[MEMORY_FLAGS] &= ~FLAG_LOAD_SPRITE;
                }

            fflush(stdout);
            SDL_Delay(16);
        }

    return 0;
}
