#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include "Button.hpp"
#include "Fighter.hpp"
#include "Scene.hpp"

enum GameState {
    STARTMENU,
    MAINMENU,
    CHARACTERSELECT,
    STAGESELECT,
    FIGHT
};

class Game {
private:
    GameState state;
    GameState next_state;
    Scene start_menu;
    Scene main_menu;
    
    bool fade;
    Uint8 fade_frame_ct;
public:
    Game(){
        this->state = GameState::STARTMENU;
        this->fade = false;
    }
    ~Game(){}

    void loadStartMenu(SDL_Renderer* renderer) {
        UIElem bg("Menus/Title Screen", renderer, false);
        cout << "Background Loaded" << endl;
        this->start_menu = Scene(bg);
        cout << "Background Added" << endl;
        UIElem logo("UI/Logo", renderer);
        cout << "Logo Loaded" << endl;
        int w, h;
        logo.get_dims(&w, &h);
        logo.set_position((SCREENWIDTH - w) / 2, 10);
        this->start_menu.addElement(logo);
        
        cout << "Logo Added" << endl;
        
        Button start_button("UI/Press Start", renderer, false);
        cout << "Start Button Loaded" << endl;
        start_button.get_dims(&w, &h);
        start_button.set_position((SCREENWIDTH - w) / 2, SCREENHEIGHT / 2);
        this->start_menu.addButton(start_button);
        
        cout << "Start Button Added" << endl;
    }
    void loadMainMenu(SDL_Renderer* renderer) {
        UIElem bg("Menus/Main Menu", renderer);
        this->main_menu = Scene(bg);

        
        Button online("UI/Online", renderer, false);
        online.set_state(ButtonState::HOVERED);
        int w, h;
        online.get_dims(&w, &h);
        online.set_position((SCREENWIDTH - w) / 2, 40);
        this->main_menu.addButton(online);

        Button versus("UI/Versus", renderer, false);
        versus.get_dims(&w, &h);
        versus.set_position((SCREENWIDTH - w) / 2, 70);
        this->main_menu.addButton(versus);

        Button training("UI/Training", renderer, false);
        training.get_dims(&w, &h);
        training.set_position((SCREENWIDTH - w) / 2, 100);
        this->main_menu.addButton(training);

        Button options("UI/Options", renderer, false);
        options.get_dims(&w, &h);
        options.set_position((SCREENWIDTH - w) / 2, 130);
        this->main_menu.addButton(options);
    }
    void loadCharacterSelect(SDL_Renderer* renderer) {
        
    }
    void driveStartMenu(Controller controller) {
        if (controller.getStartButtonState()) {
            this->start_menu.set_button_state(0, ButtonState::SELECTED);
        }
    }
    void driveMainMenu(Controller controller) {
        static int hover_idx = 0;
        static bool selected = false;

        if (!selected) {
            if (
                controller.getDirection() == HatDir::DOWN || 
                controller.getDirection() == HatDir::DOWNLEFT || 
                controller.getDirection() == HatDir::DOWNRIGHT
                ) 
                hover_idx++;
            if (
                controller.getDirection() == HatDir::UP ||
                controller.getDirection() == HatDir::UPLEFT ||
                controller.getDirection() == HatDir::UPRIGHT
                ) 
                hover_idx--; 

            if (hover_idx == -1) hover_idx = 3;
            if (hover_idx == 4) hover_idx = 0;
            for (int i=0; i<4; i++) {
                if (i == hover_idx) {
                    this->main_menu.set_button_state(i, ButtonState::HOVERED);
                    if (controller.getStartButtonState() == true) {
                        this->main_menu.set_button_state(i, ButtonState::SELECTED);
                        selected = true;
                    }
                }
                else this->main_menu.set_button_state(i, ButtonState::DEFAULT);
            }
        }
    }

    void updateStartMenu() {
        static int frame_ct = 0;
        static int exit_frame_ct = 0;
        this->start_menu.update();
        frame_ct++;
        if (frame_ct == 131) {
            this->start_menu.set_active_state(0, true);
        }
        if (frame_ct == 100) {
            this->start_menu.set_active_state(-1, true);
        }
        if (start_menu.get_button_state(0) == ButtonState::SELECTED) {
            exit_frame_ct++;
            this->next_state = GameState::MAINMENU;
        }
        if (exit_frame_ct == 20) {
            fade_frame_ct = 0;
            fade = true;
        }

    }
    void updateMainMenu() {
        static int frame_ct = 0;
        static int exit_frame_ct = 0;
        this->main_menu.update();
        frame_ct++;
        for (int i=1; i<=4; i++) {
            if (frame_ct == 10 * i) {
                this->main_menu.set_active_state(-i, true);
            }
        }
        if (main_menu.get_button_state(1) == ButtonState::SELECTED) {
            exit_frame_ct++;
            this->next_state = GameState::CHARACTERSELECT;
        }
        if (exit_frame_ct == 20) {
            fade_frame_ct = 0;
            fade = true;
        }
    }

    void drive(Controller controller) {
        switch (this->state) {
        case GameState::STARTMENU:
            this->driveStartMenu(controller);
            break;
        case GameState::MAINMENU:
            this->driveMainMenu(controller);
            break;
        default:
            break;
        };
    }
    void update() {
        switch (this->state) {
        case GameState::STARTMENU:
            this->updateStartMenu();
            break;
        case GameState::MAINMENU:
            this->updateMainMenu();
            break;
        default:
            break;
        };
    }
    void render_to(SDL_Renderer* renderer) {
        switch (this->state) {
        case GameState::STARTMENU:
            this->start_menu.render_to(renderer);
            break;
        case GameState::MAINMENU:
            this->main_menu.render_to(renderer);
        default:
            break;
        };
        if (this->fade) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, this->fade_frame_ct);
            SDL_RenderFillRect(renderer, NULL);
            this->fade_frame_ct+=4;
            if (this->fade_frame_ct > 250) {
                this->fade = false;
                this->fade_frame_ct = 0;
                this->state = this->next_state;
                switch(this->state) {
                case GameState::MAINMENU:
                    this->loadMainMenu(renderer);
                    break;
                case GameState::STARTMENU:
                    this->loadStartMenu(renderer);
                    break;
                default:
                    break;
                }
            }
        }
    }
};
#endif