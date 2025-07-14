#include "Core.h"
#include "Timer.h"
#include <cmath>
#include <format>
#include <iostream>
#include "../SerialPort/SerialPort.hpp"
#include "SDL2/SDL_image.h"


Core::Core(): win(nullptr, SDL_DestroyWindow),
              renderer(nullptr, SDL_DestroyRenderer),
              board{}, turn(1), await_coords(false), game_over(false) {
    init_sdl();
}

void Core::init() {
    win.reset(SDL_CreateWindow(config::title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config::winw,
                               config::winh, SDL_RENDERER_ACCELERATED));
    if (!win) {
        throw std::runtime_error(std::format("Error creating window {}", SDL_GetError()));
    }

    // index -1 automatically chooses driver
    renderer.reset(SDL_CreateRenderer(win.get(), -1, 0));
    if (!win) {
        throw std::runtime_error(std::format("Error creating renderer {}", SDL_GetError()));
    }

    std::shared_ptr white = Core::load_texture(renderer.get(), "../assets/white.png");
    std::shared_ptr black = Core::load_texture(renderer.get(), "../assets/black.png");

    objects.clear();

    int spacing = config::line_spacing + config::tile_size;

    // lay down tiles
    for (int i = 0; i < config::boardh; i++) {
        for (int j = 0; j < config::boardw; j++) {
            objects.emplace_back(std::make_shared<Piece>(config::line_spacing + spacing * j,
                                                               config::line_spacing + spacing * i,
                                                               config::tile_size, config::tile_size));
            objects[i * config::boardw + j]->give_textures(white, black);
        }
    }

    turn = 1;

    serial.write("reset");
    serial.write(std::to_string(config::boardw));
    serial.write(std::to_string(config::boardh));
    serial.write(std::to_string(config::win_condition));
}

void Core::init_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        throw std::runtime_error(std::format("Error initializing SDL {}", SDL_GetError()));
    }
}

void Core::quit_sdl() {
    SDL_Quit();
}

void Core::update() {
    serial.read();
    if (serial.has_message) {
        serial.has_message = false;

        if (turn == 2 && strcmp(serial.message, "set") == 0) {
            await_coords = true;
        } else if (await_coords) {
            await_coords = false;

            int x = atoi(strtok(serial.message, " "));
            int y = atoi(strtok(nullptr, " "));

            set_value(board, x, y, 2, *objects[y * config::boardw + x]);
        } else {
            std::cout << serial.message << std::endl;
        }
    }

    // clear screen
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
    for (const std::shared_ptr<Piece> &obj: objects) {
        obj->update(renderer.get());
    }

    SDL_RenderPresent(renderer.get());
}

void Core::run() {
    init();

    SDL_Event event;
    Timer timer;

    while (true) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && turn == 1 && !game_over) {
                        for (int j = 0; j < static_cast<int>(objects.size()); j++) {
                            if (objects[j]->value == 0 && collidepoint(&(objects[j]->rect), event.button.x,
                                                                       event.button.y)) {
                                serial.write("set");
                                serial.write(std::to_string(j % config::boardw));
                                serial.write(std::to_string(j / config::boardw));

                                set_value(board, j % config::boardw, j / config::boardw, turn, *objects[j]);
                                break;
                            }
                        }
                    }

                    break;

                case SDL_QUIT:
                    return;

                default:
                    break;
            }
        }
        timer.tick(config::fps);
        update();
    }
}

bool Core::collidepoint(const SDL_Rect *rect, const int x, const int y) {
    return x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h;
}

std::shared_ptr<SDL_Texture> Core::load_texture(SDL_Renderer *renderer, const char *path) {
    struct TextureDeleter {
        void operator()(SDL_Texture *texture) const {
            SDL_DestroyTexture(texture);
        }
    };

    std::shared_ptr<SDL_Texture> texture(IMG_LoadTexture(renderer, path), TextureDeleter{});
    if (!texture.get()) {
        throw std::runtime_error(std::format("Error loading texture {}", IMG_GetError()));
    }

    return texture;
}


void Core::set_value(short (&arr)[config::boardh][config::boardw], int x, int y, short value, Piece &obj) {
    arr[y][x] = value;

    // default everything to 0 = possible
    short possibilities[8]{};
    for (short i = 1; i < config::win_condition; i++) {
        if ((y - i < 0 || board[y - i][x] != value) && (possibilities[0] == 0)) {
            possibilities[0] = i;
        }
        if ((y + i > config::boardh - 1 || board[y + i][x] != value) && (possibilities[4] == 0)) {
            possibilities[4] = i;
        }
        if ((x - i < 0 || board[y][x - i] != value) && (possibilities[1] == 0)) {
            possibilities[1] = i;
        }
        if ((x + i > config::boardw - 1 || board[y][x + i] != value) && (possibilities[5] == 0)) {
            possibilities[5] = i;
        }
        if ((y - i < 0 || x - i < 0 || board[y - i][x - i] != value) && (possibilities[2] == 0)) {
            possibilities[2] = i;
        }
        if ((y + i > config::boardh - 1 || x + i > config::boardw - 1 || board[y + i][x + i] != value) && (
                possibilities[6] == 0)) {
            possibilities[6] = i;
        }
        if ((y + i > config::boardh - 1 || x - i < 0 || board[y + i][x - i] != value) && (possibilities[3] == 0)) {
            possibilities[3] = i;
        }
        if ((y - i < 0 || x + i > config::boardw - 1 || board[y - i][x + i] != value) && (possibilities[7] == 0)) {
            possibilities[7] = i;
        }
    }

    for (const short j: possibilities) {
        if (j == 0) {
            game_over = true;
        }
    }

    for (short j = 0; j < 4; j++) {
        if (possibilities[j] - 1 + possibilities[j + 4] == config::win_condition) {
            game_over = true;
        }
    }

    obj.value = turn;
    turn = turn == 1 ? 2 : 1;
}

int main(int argc, char *argv[]) {
    int return_val = EXIT_SUCCESS;

    try {
        Core core;
        core.run();
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return_val = EXIT_FAILURE;
    }

    Core::quit_sdl();

    return return_val;
}
