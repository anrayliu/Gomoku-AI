#ifndef PIECE_H
#define PIECE_H

#include <memory>
#include "SDL2/SDL_render.h"


class Piece {
    std::shared_ptr<SDL_Texture> white;
    std::shared_ptr<SDL_Texture> black;

public:
    int value;

    SDL_Rect rect;

    Piece(int x, int y, int w, int h);

    void update(SDL_Renderer *renderer) const;

    void give_textures(std::shared_ptr<SDL_Texture> white, std::shared_ptr<SDL_Texture> black);
};

#endif
