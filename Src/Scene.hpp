#ifndef SCENE_H
#define SCENE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include "Button.hpp"
#include "Fighter.hpp"

#define SCREENWIDTH 384
#define SCREENHEIGHT 224

class Scene {
private:
    UIElem background;
    std::vector<UIElem> elems;
    std::vector<Button> buttons;
public:
    Scene() {}
    Scene(UIElem background) {
        this->background = background;
    }
    void addElement(UIElem elem) {
        this->elems.push_back(elem);
    }
    void addButton(Button button) {
        this->buttons.push_back(button);
    }
    void set_active_state(int idx, bool state) {
        if (idx == 0) {
            this->background.set_active_state(state);
        }
        else if (idx > 0) {
            this->elems[idx-1].set_active_state(state);
        }
        else {
            this->buttons[-1-idx].set_active_state(state);
        }
    }
    void set_button_state(int idx, ButtonState state) {
        if (this->buttons[idx].get_active_state() && this->buttons[idx].get_state() != state) {
            this->buttons[idx].set_state(state);
        }
    }
    ButtonState get_button_state(int idx) {
        return this->buttons[idx].get_state();
    }
    void render_to(SDL_Renderer* renderer) {
        if (this->background.get_active_state()) 
            this->background.render_to(renderer);
        for (UIElem elem : this->elems) {
            if (elem.get_active_state()) {
                elem.render_to(renderer);
            }
        }
        for (Button button : this->buttons) {
            if (button.get_active_state()) {
                button.render_to(renderer);
            }
        }
    }
    void update() {
        this->background.update();
        for (int i=0; i<this->elems.size(); i++) {
            elems[i].update();
        }
        for (int i=0; i<this->buttons.size(); i++) {
            buttons[i].update();
        }
    }
};
#endif