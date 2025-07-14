#ifndef CORE_H
#define CORE_H

#include <memory>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Piece.h"
#include "SerialHandler.h"

using std::string;


namespace config {
    constexpr double max_zoom = 2;
    constexpr double min_zoom = 0.4;
    constexpr double zoom_sens = 0.1;
    constexpr int boardw = 10;
    constexpr int boardh = 10;
    constexpr short win_condition = 5;
    constexpr int tile_size = 40;
    constexpr int line_spacing = 5;
    constexpr string title = "gomoku";
    constexpr int winw = (tile_size + line_spacing) * boardw + line_spacing;
    constexpr int winh = (tile_size + line_spacing) * boardh + line_spacing;
    constexpr double fps = 120;
}


class Core {
    std::vector<std::shared_ptr<Piece>> objects;

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> win;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;

    short board[config::boardh][config::boardw];
    short turn;

    // used for communication

    SerialHandler serial;
    bool await_coords;

    bool game_over;

public:
    static void init_sdl();
    static void quit_sdl();
    static bool collidepoint(const SDL_Rect *rect, int x, int y);
    static std::shared_ptr<SDL_Texture> load_texture(SDL_Renderer *renderer, const char *path);
    Core();

    void init();
    void update();
    void run();

    void set_value(short (&arr)[config::boardh][config::boardw], int x, int y, short value, Piece &obj);

};


#endif
