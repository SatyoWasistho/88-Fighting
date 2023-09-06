#ifndef FIGHTER_H
#define FIGHTER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include "Sprite.hpp"
#include "Controller.hpp"

#define GROUND 150

enum FighterState {
    IDLE,
    WALK,
    WALKBACK,
    CROUCH,
    DASH,
    DASHBACK
};

enum Collision {
    NONE,
    ONE_HIT_TWO,
    TWO_HIT_ONE,
    CLASH
};
class Fighter: public Sprite {
private:
    //basic information
    std::string name;
    int health;

    //physics data
    float walk_speed;
    float dash_speed;
    int dash_lockout;

    float p_x; //Position (logical)
    float p_y;

    float v_x; //Velocity
    float v_y;
    
    float a_x; //Acceleration
    float a_y;

    //character state enum
    FighterState state;
    bool is_blocking;

    //animation data
    Animation idle_anim;
    Animation walk_anim;
    Animation crouch_anim;
    Animation dash_anim;
    Animation dashback_anim;

    //collision tracking
    SDL_Point ecb_standing; //Environmental Collision Box
    SDL_Point ecb_crouching;
    std::vector<SDL_Point> hit_boxes;
    std::vector<SDL_Point> hurt_boxes;


public:
    //Constructor
    Fighter(string name, SDL_Renderer* renderer) : Sprite(0, 0, renderer, 0, 0) {
        this->p_x = 0;
        this->p_y = 100;
        this->v_x = 0;
        this->v_y = 0;
        this->a_x = 0;
        this->a_y = 0;
        this->dash_lockout = 0;
        this->name = name;
        this->state = FighterState::IDLE;
        this->is_blocking = false;

        ifstream anim_guide("Data/Game Data/Characters/" + name + "/Animation Guide");
        if (!anim_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!anim_guide.eof()) {
            std::string state_str;
            int frame_ct;
            int frame_wait;
            anim_guide >> state_str >> frame_ct >> frame_wait;
            FighterState state_to_animate;
            if (state_str.compare("Idle") == 0) {
                state_to_animate = FighterState::IDLE;
            }
            else if (state_str.compare("Walk") == 0) {
                state_to_animate = FighterState::WALK;
            }
            else if (state_str.compare("Crouch") == 0) {
                state_to_animate = FighterState::CROUCH;
            }
            else if (state_str.compare("Dash") == 0) {
                state_to_animate = FighterState::DASH;
            }
            else if (state_str.compare("DashBack") == 0) {
                state_to_animate = FighterState::DASHBACK;
            }
            else {
                break;
            }
            string filename = (
                "Assets/Sprites/Characters/" + 
                name + "/" + state_str
            );
            
                
            switch (state_to_animate) {
            default:
                break;
            case FighterState::IDLE:
                this->idle_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case FighterState::WALK:
                this->walk_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case FighterState::CROUCH:
                this->crouch_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case FighterState::DASH:
                this->dash_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            case FighterState::DASHBACK:
                this->dashback_anim = Animation(frame_ct, frame_wait, filename, renderer);
                break;
            }
        }

        
        ifstream phys_guide("Data/Game Data/Characters/" + name + "/Physics Guide");
        if (!phys_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!phys_guide.eof()) {
            string attrib_str;
            float val;
            phys_guide >> attrib_str >> val;
            cout << attrib_str << " " << val << endl;
            if (attrib_str.compare("Walkspeed") == 0) {
                this->walk_speed = val;
            }
            if (attrib_str.compare("Dashspeed") == 0) {
                this->dash_speed = val;
            }
        }
        
        this->texture = idle_anim.current_frame();
        Uint32 format;
        int access;
        int w;
        int h;
        if (SDL_QueryTexture(texture, &format, &access, &w, &h) < 0) {
            cout << SDL_GetError() << endl;
        }

        cout <<"Character Loaded:" << endl;
        cout <<"\tName: " << this->name << endl;
        cout <<"\tWalkspeed: " << this->walk_speed << endl;
        cout <<"\tDashspeed: " << this->dash_speed << endl;
    }
    //Destructor
    ~Fighter() {}

    //Setters
    void set_state(FighterState state) {
        this->state = state;
    }

    //Modifiers
    void hurt(int d_health) {
        this->health -= d_health;
    }
    void heal(int d_health) {
        this->health += d_health;
    }

    //Collision Checker
    Collision get_collision(Fighter other) {
        //TODO: Actually calculate collision
        return Collision::NONE;
    }

    void drive(Controller controller) {
        switch (controller.getButtonState() & 0b00001111) {
        case 0b00000110:
            switch (this->state) {
            case FighterState::IDLE:
            case FighterState::WALK:
            case FighterState::WALKBACK:
                for (int i=0; i<controller.getBufferSize(); i++) {
                    if ((controller.getButtonState(i) & 0b00000110) == 0 && dash_lockout == 0) {
                        this->state = (
                            (controller.getDirection() == HatDir::LEFT)? 
                            FighterState::DASHBACK : 
                            FighterState::DASH);
                        this->dash_anim.reset();
                        this->dashback_anim.reset();
                        this->v_x = (
                            (controller.getDirection() == HatDir::LEFT)? 
                            -dash_speed :
                            dash_speed);
                        return;
                    }
                }
            }
        default:
            break;
        }
        FighterState prev_state = this->state;
        switch (controller.getDirection()) {
        case HatDir::NEUTRAL:
            if (
                this->state == FighterState::WALK ||
                this->state == FighterState::WALKBACK ||
                this->state == FighterState::CROUCH
                ) {
                this->set_state(FighterState::IDLE);
                this->idle_anim.reset();
                this->v_x = 0;
            }
            break;
        case HatDir::RIGHT:
            if (
                this->state == FighterState::IDLE ||
                this->state == FighterState::CROUCH
                ) {
                int dash_flag = 0;
                for (int i=0; i<controller.getBufferSize(); i++) {
                    if (controller.getDirection(i) == HatDir::NEUTRAL) {
                        if (dash_flag == 0) dash_flag = 1;
                        if (dash_flag == 2) dash_flag = 3;
                    }
                    else if (dash_flag == 1 && controller.getDirection(i) == HatDir::RIGHT) {
                        dash_flag = 2;
                    }

                }
                if (dash_flag == 3 && dash_lockout == 0) {
                    this->set_state(FighterState::DASH);
                    this->dash_anim.reset();
                    this->v_x = dash_speed;
                }
                else {
                    this->set_state(FighterState::WALK);
                    this->walk_anim.reset();
                    this->v_x = walk_speed;
                }
            }
            break;
        case HatDir::LEFT:
            if (
                this->state == FighterState::IDLE ||
                this->state == FighterState::CROUCH
                ) {
                int dash_flag = 0;
                for (int i=0; i<controller.getBufferSize(); i++) {
                    if (controller.getDirection(i) == HatDir::NEUTRAL) {
                        if (dash_flag == 0) dash_flag = 1;
                        if (dash_flag == 2) dash_flag = 3;
                    }
                    else if (dash_flag == 1 && controller.getDirection(i) == HatDir::LEFT) {
                        dash_flag = 2;
                    }

                }
                if (dash_flag == 3 && dash_lockout == 0) {
                    this->set_state(FighterState::DASHBACK);
                    this->dashback_anim.reset();
                    this->v_x = -dash_speed;
                }
                else {
                    this->set_state(FighterState::WALKBACK);
                    this->walk_anim.reset();
                    this->v_x = -walk_speed;
                }
            }
            break;
        case HatDir::DOWN:
        case HatDir::DOWNLEFT:
        case HatDir::DOWNRIGHT:
            if (this->state != FighterState::CROUCH) {
                if (
                    this->state == FighterState::WALK ||
                    this->state == FighterState::WALKBACK
                )
                    this->v_x = 0;
                this->set_state(FighterState::CROUCH);
                this->crouch_anim.reset();
            }
            break;
        default:
            break;
        }
        if (
            (
                prev_state == FighterState::DASH ||
                prev_state == FighterState::DASHBACK
            ) &&
            this->state != FighterState::CROUCH
            )
            dash_lockout = 15;
    }

    //Frame update fn
    void update() {
        this->p_x += this->v_x;
        this->p_y += this->v_y;
        this->v_x += this->a_x;
        this->v_y += this->a_y;

        if (this->dash_lockout > 0) this-> dash_lockout--;

        this->x = int(this->p_x);
        this->y = int(this->p_y);

        switch (this->state) {
        case FighterState::IDLE:
            this->texture = idle_anim.current_frame();
            idle_anim.update();
            break;
        case FighterState::WALK:
            this->texture = walk_anim.current_frame();
            walk_anim.update();
            break;
        case FighterState::WALKBACK:
            this->texture = walk_anim.current_frame();
            walk_anim.reverse_update();
            break;
        case FighterState::CROUCH:
            this->texture = crouch_anim.current_frame();
            crouch_anim.update();
            this->v_x *= 0.9;
            if (abs(this->v_x) < 0.1) this->v_x = 0;
            break;
        case FighterState::DASH:
            this->texture = dash_anim.current_frame();
            dash_anim.update();
            if (dash_anim.is_finished()) {
                this->state = FighterState::IDLE;
                this->v_x = 0;
            }
            break;
        case FighterState::DASHBACK:
            this->texture = dashback_anim.current_frame();
            dashback_anim.update();
            if (dashback_anim.is_finished()) {
                this->state = FighterState::IDLE;
                this->v_x = 0;
            }
            break;
        default:
            break;
        }
    }
};
#endif