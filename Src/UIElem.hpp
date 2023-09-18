#ifndef UIELEM_H
#define UIELEM_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Sprite.hpp"

enum UIState {
    STARTUP,
    NORMAL
};

class UIElem: public Sprite {
protected:
    std::string name;

    bool is_active;

    UIState state;
    Animation startup_anim;
    Animation normal_anim;
public:
    UIElem() {}
    UIElem(string name, SDL_Renderer* renderer, bool is_active = true) : Sprite(0, 0, renderer, 0, 0) {
        this->name = name;
        this->state = UIState::STARTUP;
        this->is_active = is_active;

        ifstream anim_guide("Data/Game Data/" + name + "/Animation Guide");
        if (!anim_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!anim_guide.eof()) {
            std::string state_str;
            int frame_ct;
            
            std::vector<int> frame_wait;
            while (frame_wait.size() < frame_ct) {
                int frame_ct2;
                int frame_wait2;
                anim_guide >> frame_ct2 >> frame_wait2;
                for (int i=0; i<frame_ct2; i++) {
                    frame_wait.push_back(frame_wait2);
                }
            }
            UIState state_to_animate;
            if (state_str.compare("Startup") == 0) {
                state_to_animate = UIState::STARTUP;
            }
            else if (state_str.compare("Normal") == 0) {
                state_to_animate = UIState::NORMAL;
            }
            else {
                break;
            }
            string filename = (
                "Assets/Sprites/" + 
                name + "/" + state_str
            );
            
                
            switch (state_to_animate) {
            default:
                break;
            case UIState::STARTUP:
                this->startup_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case UIState::NORMAL:
                this->normal_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            }
        }
        this->texture = this->startup_anim.current_frame();
    }
    bool get_active_state() {
        return this->is_active;
    }
    void set_active_state(bool is_active) {
        this->is_active = is_active;
    }
    void update() {
        if (this->is_active) {
            switch (this->state) {
            case UIState::NORMAL:
                this->texture = normal_anim.current_frame();
                normal_anim.update();
                break;
            case UIState::STARTUP:
                this->texture = startup_anim.current_frame();
                startup_anim.update();
                if (startup_anim.is_finished()) this->state = UIState::NORMAL;
                break;
            default:
                break;
            }
        }
    }
};
#endif