#ifndef BUTTON_H
#define BUTTON_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "UIElem.hpp"
#include "Controller.hpp"

enum ButtonState {
    DEFAULT,
    HOVERED,
    SELECTED,
};

class Button: public UIElem {
private:
    ButtonState button_state;

    Animation default_anim;
    Animation hover_anim;
    Animation select_anim;

public:
    Button(string name, SDL_Renderer* renderer, bool is_active = true) : UIElem(name, renderer, is_active) {
        this->button_state = ButtonState::DEFAULT;

        ifstream anim_guide("Data/Game Data/" + name + "/Animation Guide");
        if (!anim_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!anim_guide.eof()) {
            std::string state_str;
            int frame_ct;
            int frame_wait;
            anim_guide >> state_str >> frame_ct >> frame_wait;
            ButtonState state_to_animate;
            if (state_str.compare("None") == 0) {
                state_to_animate = ButtonState::DEFAULT;
            }
            else if (state_str.compare("Hovered") == 0) {
                state_to_animate = ButtonState::HOVERED;
            }
            else if (state_str.compare("Selected") == 0) {
                state_to_animate = ButtonState::SELECTED;
            }
            else {
                continue;
            }
            string filename = (
                "Assets/Sprites/" + 
                name + "/" + state_str
            );
            
                
            switch (state_to_animate) {
            default:
                break;
            case ButtonState::DEFAULT:
                this->default_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case ButtonState::HOVERED:
                this->hover_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case ButtonState::SELECTED:
                this->select_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            }
        }
    }
    void set_state(ButtonState state) {
        this->button_state = state;
        switch (this->button_state) {
        case ButtonState::DEFAULT:
            default_anim.reset();
            break;
        case ButtonState::HOVERED:
            hover_anim.reset();
            break;
        case ButtonState::SELECTED:
            select_anim.reset();
            break;
        default:
            break;
        }
    }

    ButtonState get_state() {
        return this->button_state;
    }

    void update() {
        if (!this->is_active) return;
        switch (this->state) {
        case UIState::NORMAL:
            switch (this->button_state) {
            case ButtonState::DEFAULT:
                this->texture = default_anim.current_frame();
                default_anim.update();
                break;
            case ButtonState::HOVERED:
                this->texture = hover_anim.current_frame();
                hover_anim.update();
                break;
            case ButtonState::SELECTED:
                this->texture = select_anim.current_frame();
                select_anim.update();
                break;
            default:
                break;
            }
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
};
#endif