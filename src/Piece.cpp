#include "Core.h"


Piece::Piece(int x, int y, int w, int h) : value(0), rect{x, y, w, h} {}

void Piece::give_textures(std::shared_ptr<SDL_Texture> w, std::shared_ptr<SDL_Texture> b) {
    white = std::move(w);
    black = std::move(b);
}

void Piece::update(SDL_Renderer *renderer) const {
    SDL_RenderFillRect(renderer, &rect);

    switch (value) {
        case 1:
            SDL_RenderCopy(renderer, white.get(), nullptr, &rect);
            break;
        case 2:
            SDL_RenderCopy(renderer, black.get(), nullptr, &rect);
            break;
        default:
            break;
    }

}
