#ifndef SPRITE_H
#define SPRITE_H
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/********** Sprite Class **********/
/*
Holds all visual information needed 
for a sprite to be rendered to the
screen. All game object classes will
inherit this one.
*/
class Sprite {
protected:
    //3D position (logical)
    int x;
    int y;

    //Horizontal and vertical flip states
    bool h_flip;
    bool v_flip;

    //Texture
    SDL_Texture* texture;
public:
    //Constructor
    Sprite() {
        this->x = 0;
        this->y = 0;

        this->h_flip = false;
        this->v_flip = false;
    }
    Sprite(int x, int y, SDL_Renderer* renderer, int w, int h) {
        this->x = x;
        this->y = y;

        this->h_flip = false;
        this->v_flip = false;

        this->texture = SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STATIC,
            w, h
        );
    }
    //Destructor
    ~Sprite() {}

    //setters
    void set_position(int x, int y) {
        this->x = x;
        this->y = y;
    }
    void set_flip(bool h_flip, bool v_flip) {
        this->h_flip = h_flip;
        this->v_flip = v_flip;
    }
    void set_texture(SDL_Texture* texture) {
        this->texture = texture;
    }
    void get_dims(int* w, int* h) {
        Uint32 format;
        int access;
        if (SDL_QueryTexture(this->texture, &format, &access, w, h) < 0) {
            std::cout << SDL_GetError() << endl;
        }
    }
    void render_to(SDL_Renderer* renderer) {
        SDL_Rect dst_rect;
        SDL_QueryTexture(texture, NULL, NULL, &dst_rect.w, &dst_rect.h);
        dst_rect.x = this->x;
        dst_rect.y = this->y;
        
        if (this->h_flip && this->v_flip) {
            SDL_RenderCopyEx(
                renderer, 
                this->texture, 
                NULL, 
                &dst_rect, 
                180, 
                NULL, 
                SDL_FLIP_NONE
            );
        } else if (this->h_flip) {
            SDL_RenderCopyEx(
                renderer, 
                this->texture, 
                NULL, 
                &dst_rect, 
                0, 
                NULL, 
                SDL_FLIP_HORIZONTAL
            );
        } else if (this->v_flip) {
            SDL_RenderCopyEx(
                renderer, 
                this->texture, 
                NULL, 
                &dst_rect, 
                0, 
                NULL, 
                SDL_FLIP_VERTICAL
            );
        } else {
            SDL_RenderCopyEx(
                renderer, 
                this->texture, 
                NULL, 
                &dst_rect, 
                0, 
                NULL, 
                SDL_FLIP_NONE
            );
        }
    }
};
/**********************************/

struct Animation {
    int frame_idx;
    int anim_frame_idx;

    std::vector<int> frame_wait;
    std::vector<SDL_Texture*> frames;

    Animation() {}

    Animation(int frame_ct, std::vector<int> frame_wait, std::string frame_dir_name, SDL_Renderer* renderer) {
        this->frame_wait = frame_wait;
        this->frame_idx = 0;
        this->anim_frame_idx = 0;
        for (int i=1; i <= frame_ct; i++) {
            std::string filename = (
                frame_dir_name + 
                "/Frame (" + 
                std::to_string(i) + 
                ").png"
            );
            SDL_Surface* surface = IMG_Load(filename.c_str());

            if (surface == NULL) {
                std::cout << SDL_GetError() << std::endl;
            }
            SDL_Texture* frame_tex = SDL_CreateTextureFromSurface(renderer, surface);
            if (frame_tex == NULL) {
                std::cout << SDL_GetError() << std::endl;
            }
            this->frames.push_back(frame_tex);
        }
    }
    void reset() {
        this->frame_idx = 0;
        this->anim_frame_idx = 0;
    }
    bool is_finished() {
        return this->anim_frame_idx == this->frames.size() - 1 && this->frame_idx == this->frame_wait[anim_frame_idx] - 1;
    }
    SDL_Texture* current_frame() {
        return this->frames[anim_frame_idx];
    }
    void reverse() {
        std::reverse(this->frames.begin(), this->frames.end());
    }
    void update() {
        this->frame_idx++;
        this->frame_idx %= this->frame_wait[anim_frame_idx];
        this->anim_frame_idx += (this->frame_idx == 0);
        if (this->anim_frame_idx >= this->frames.size()) {
            this->anim_frame_idx = 0;
        }
    }
    void reverse_update() {
        this->frame_idx++;
        this->frame_idx %= this->frame_wait[anim_frame_idx];
        this->anim_frame_idx -= (this->frame_idx == 0);
        if (this->anim_frame_idx < 0) {
            this->anim_frame_idx = this->frames.size() - 1;
        }
    }

};
#endif