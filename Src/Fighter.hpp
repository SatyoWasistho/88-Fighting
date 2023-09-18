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

#define GROUND 200

//States for Fighter Class
enum FighterState {
    //Movement States
    IDLE,
    WALK,
    WALKBACK,
    PRECROUCH,
    CROUCH,
    POSTCROUCH,
    DASH,
    DASHBACK,
    JUMPSQUAT,
    JUMPUP,
    JUMPDOWN,
    JUMPDASH,
    JUMPDASHBACK,

    //Attack States (Unimplemented)
    SLPUNCH,
    SLKICK,
    SHPUNCH,
    SHKICK,
    CLPUNCH,
    CLKICK,
    CHPUNCH,
    CHKICK,
    FTHROW,
    BTHROW,
    SPEC1,
    SPEC2,
    SPEC3,
    SPEC4,
    SPEC5,
    SPEC6,
    SPEC7,
    SPEC8,
    SPEC9,
    SPEC10,
    SUPER1,
    SUPER2,
    SUPER3,

    //Getting the Shit Kicked Out of Them States (Unimplemented)
    BLOCKSTUN,
    HITSTUN,
    GRABBED,
    HARDKNOCKDOWN,
    //There will probably be more states. Depends on the combo system we decide on

    SIZE //Number of states that exist
};

//TODO: Actually implement these collision states
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
    float jump_speed_x;
    float jump_speed_y;
    float jump_accel;

    int dash_lockout;

    bool grounded;

    float p_x; //Position (logical)
    float p_y;

    float v_x; //Velocity
    float v_y;

    float next_vx;
    
    float a_x; //Acceleration
    float a_y;

    //character state enum
    FighterState state;
    FighterState next_state;
    bool is_blocking;

    //animation data
    Animation anims[FighterState::SIZE];

    //collision tracking
    SDL_Point ecb_standing; //Environmental Collision Box
    SDL_Point ecb_crouching;
    std::vector<SDL_Point> hit_boxes;
    std::vector<SDL_Point> hurt_boxes;


public:
    //Constructor
    Fighter(string name, SDL_Renderer* renderer) : Sprite(0, 0, renderer, 0, 0) {
        this->grounded = true;
        this->v_x = 0;
        this->v_y = 0;
        this->a_x = 0;
        this->a_y = 0;
        this->dash_lockout = 0;
        this->name = name;
        this->state = FighterState::IDLE;
        this->next_state = FighterState::IDLE;
        this->is_blocking = false;

        ifstream anim_guide("Data/Game Data/Characters/" + name + "/Animation Guide");
        if (!anim_guide.is_open()) {
            cout << "error reading file" << endl;
        } 
        while (!anim_guide.eof()) {
            std::string state_str;
            int frame_ct;
            std::vector<int> frame_wait;
            anim_guide >> state_str >> frame_ct;
            while (frame_wait.size() < frame_ct) {
                int frame_ct2;
                int frame_wait2;
                anim_guide >> frame_ct2 >> frame_wait2;
                for (int i=0; i<frame_ct2; i++) {
                    frame_wait.push_back(frame_wait2);
                }
            }
            FighterState state_to_animate;
            if (state_str.compare("Idle") == 0) {
                state_to_animate = FighterState::IDLE;
            }
            else if (state_str.compare("Walk") == 0) {
                state_to_animate = FighterState::WALK;
            }
            else if (state_str.compare("PreCrouch") == 0) {
                state_to_animate = FighterState::PRECROUCH;
            }
            else if (state_str.compare("Crouch") == 0) {
                state_to_animate = FighterState::CROUCH;
            }
            else if (state_str.compare("PostCrouch") == 0) {
                state_to_animate = FighterState::POSTCROUCH;
            }
            else if (state_str.compare("Dash") == 0) {
                state_to_animate = FighterState::DASH;
            }
            else if (state_str.compare("DashBack") == 0) {
                state_to_animate = FighterState::DASHBACK;
            }
            else if (state_str.compare("JumpSquat") == 0) {
                state_to_animate = FighterState::JUMPSQUAT;
            }
            else if (state_str.compare("JumpUp") == 0) {
                state_to_animate = FighterState::JUMPUP;
            }
            else if (state_str.compare("JumpDown") == 0) {
                state_to_animate = FighterState::JUMPDOWN;
            }
            else {
                break;
            }
            string filename = (
                "Assets/Sprites/Characters/" + 
                name + "/" + state_str
            );
            
                
            this->anims[state_to_animate] = Animation(frame_ct, frame_wait, filename, renderer);
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
            if (attrib_str.compare("JumpspeedX") == 0) {
                this->jump_speed_x = val;
            }
            if (attrib_str.compare("JumpspeedY") == 0) {
                this->jump_speed_y = val;
            }
            if (attrib_str.compare("Jumpaccel") == 0) {
                this->jump_accel = val;
            }
        }
        
        this->texture = this->anims[this->state].current_frame();
        Uint32 format;
        int access;
        int w;
        int h;
        if (SDL_QueryTexture(texture, &format, &access, &w, &h) < 0) {
            cout << SDL_GetError() << endl;
        }
        
        this->p_x = 100;
        this->p_y = GROUND;

        cout <<"Character Loaded:" << endl;
        cout <<"\tName: " << this->name << endl;
        cout <<"\tWalkspeed: " << this->walk_speed << endl;
        cout <<"\tDashspeed: " << this->dash_speed << endl;
        cout <<"\tJumpspeedX: " << this->jump_speed_x << endl;
        cout <<"\tJumpspeedY: " << this->jump_speed_y << endl;
        cout <<"\tJumpaccel: " << this->jump_accel << endl;
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

    bool is_crouching() {
        return (
            this->state == FighterState::PRECROUCH ||
            this->state == FighterState::CROUCH
        );
    }

    bool is_jumping() {
        return (
            this->state == FighterState::JUMPSQUAT ||
            this->state == FighterState::JUMPUP ||
            this->state == FighterState::JUMPDOWN
        );
    }

    void drive(Controller controller) {
        switch ((controller.getButtonState() | controller.getButtonDeltas()) & 0b00001111) {
        case 0b00000110:
            switch (this->state) {
            case FighterState::IDLE:
            case FighterState::WALK:
            case FighterState::WALKBACK:
            case FighterState::POSTCROUCH:
                for (int i=0; i<controller.getBufferSize(); i++) {
                    if ((controller.getButtonState(i) & 0b00000110) == 0 && dash_lockout == 0) {
                        this->state = (
                            (controller.getDirection() == HatDir::LEFT || controller.getDirection() == HatDir::DOWNLEFT)? 
                            FighterState::DASHBACK : 
                            FighterState::DASH);
                        this->v_x = (
                            (this->state == FighterState::DASHBACK)? 
                            -dash_speed :
                            dash_speed);
                        this->anims[this->state].reset();
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
                this->state == FighterState::WALKBACK
                ) {
                this->set_state(FighterState::IDLE);
                this->v_x = 0;
            }
            else if (this->is_crouching()) {
                this->state = FighterState::POSTCROUCH;
            }
            this->next_state = FighterState::IDLE;
            this->next_vx = 0;
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
                    this->v_x = dash_speed;
                }
                else {
                    this->set_state(FighterState::WALK);
                    this->v_x = walk_speed;
                }
            }
            this->next_state = FighterState::WALK;
            this->next_vx = walk_speed;
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
                    this->v_x = -dash_speed;
                }
                else {
                    this->set_state(FighterState::WALKBACK);
                    this->v_x = -walk_speed;
                }
            }
            this->next_state = FighterState::WALKBACK;
            this->next_vx = -walk_speed;
            break;
        case HatDir::DOWN:
        case HatDir::DOWNLEFT:
        case HatDir::DOWNRIGHT:
            if (
                !(this->is_crouching()) &&
                !(this->is_jumping())
                ) {
                if (
                    this->state == FighterState::WALK ||
                    this->state == FighterState::WALKBACK
                )
                    this->v_x = 0;
                this->set_state(FighterState::PRECROUCH);
            }
            this->next_state = FighterState::CROUCH;
            this->next_vx = 0;
            break;
        case HatDir::UPLEFT:
        case HatDir::UPRIGHT:
        case HatDir::UP:
            this->next_state = FighterState::JUMPSQUAT;
            this->next_vx = 0;
            if (controller.getDirection() == UPRIGHT) {
                this->next_vx = this->jump_speed_x;
                if (
                    //abs(this->v_x) < this->jump_speed_x &&
                    this->state != FighterState::JUMPDOWN &&
                    this->state != FighterState::JUMPUP
                ) {
                    this->v_x = this->jump_speed_x;
                }
            }
            else if (controller.getDirection() == UPLEFT) {
                this->next_vx = -this->jump_speed_x;
                if (
                    //abs(this->v_x) < this->jump_speed_x &&
                    this->state != FighterState::JUMPDOWN &&
                    this->state != FighterState::JUMPUP
                ) {
                    this->v_x = -this->jump_speed_x;
                }
            }
            if (
                this->state == FighterState::IDLE ||
                this->state == FighterState::WALK ||
                this->state == FighterState::WALKBACK ||
                this->state == FighterState::CROUCH ||
                this->state == FighterState::DASH ||
                this->state == FighterState::DASHBACK
            ) {
                this->set_state(FighterState::JUMPSQUAT);
            }
            break;
        default:
            break;
        }
        if (this->state != FighterState::DASH && this->state != FighterState::DASHBACK && this->state != prev_state) this->anims[this->state].reset();
    }

    //Frame update fn
    void update() {
        this->p_x += this->v_x;
        this->p_y += this->v_y;
        this->v_x += this->a_x;
        this->v_y += this->a_y;

        
        static Uint32 format = 0;
        static int access = 0;
        static int w = 0;
        static int h = 0;

        if (this->dash_lockout > 0) this-> dash_lockout--;
        
        if (this->state != FighterState::WALKBACK) {
            this->texture = this->anims[this->state].current_frame();
            this->anims[this->state].update();
        }
        switch (this->state) {
        case FighterState::WALKBACK:
            this->texture = this->anims[FighterState::WALK].current_frame();
            this->anims[FighterState::WALK].reverse_update();
            break;
        case FighterState::PRECROUCH:
            if (this->anims[this->state].is_finished()) {
                this->state = FighterState::CROUCH;
                this->anims[this->state].reset();
            }
            break;
        case FighterState::CROUCH:
            this->v_x *= 0.89;
            if (abs(this->v_x) < 0.15) this->v_x = 0;
            break;
        case FighterState::POSTCROUCH:
            if (this->anims[this->state].is_finished()) {
                this->state = FighterState::IDLE;
                this->anims[this->state].reset();
                this->v_x = 0;
            }
            break;
        case FighterState::DASH:
        case FighterState::DASHBACK:
            if (this->anims[this->state].is_finished()) {
                this->dash_lockout = 20;
                this->anims[this->state].reset();
                this->state = this->next_state;
                this->anims[this->state].reset();
                this->v_x = next_vx;
            }
            break;
        case FighterState::JUMPSQUAT:
            if (this->anims[this->state].is_finished()) {
                this->state = FighterState::JUMPUP;
                this->anims[this->state].reset();
                this->v_y = -(this->jump_speed_y);
                this->a_y = this->jump_accel;
            }
            break;
        case FighterState::JUMPUP:
            if (this->grounded) {
                this->p_y = GROUND;
                grounded = false;
            }
            if (this->v_y >= 0) {
                this->anims[this->state].reset();
                this->state = FighterState::JUMPDOWN;
                this->anims[this->state].reset();
            }
            break;
        case FighterState::JUMPDOWN:
            if (this->p_y > GROUND) {
                this->state = this->next_state;
                this->v_x = this->next_vx;
                this->v_y = 0;
                this->a_y = 0;
                this->grounded = true;
            }
            break;
        default:
            break;
        }
        if (this->grounded) {
            this->p_y = GROUND;
        }

        SDL_QueryTexture(this->texture, &format, &access, &w, &h);
        
        this->x = int(this->p_x) - w/2;
        this->y = int(this->p_y) - h;
    }
};
#endif