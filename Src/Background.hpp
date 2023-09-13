#include "UIElem.hpp"
#include <iostream>
#include <fstream>

class Background : public UIElem {
private:
    int scroll_x;
    int scroll_x_wait;
    int scroll_y;
    int scroll_y_wait;
public:
    Background(string name, SDL_Renderer* renderer, bool is_active = true): UIElem(name, renderer, is_active) {
        ifstream anim_guide("Data/Game Data/" + name + "/Animation Guide");
        if (!anim_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!anim_guide.eof()) {
            std::string state_str;
            int frame_ct;
            int frame_wait;
            anim_guide >> state_str >> frame_ct >> frame_wait;
            if (state_str.compare("ScrollX") == 0) {
                scroll_x = frame_ct;
                scroll_x_wait = frame_wait;
            }
            else if (state_str.compare("ScrollY") == 0) {
                scroll_y = frame_ct;
                scroll_y_wait = frame_wait;
            }
        }
    }
};