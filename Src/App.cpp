#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>
#include "Controller.hpp"
#include "Fighter.hpp"
#include "Scene.hpp"
#include "Game.hpp"
#include "Background.hpp"

#define RENDERSCALE 4

using namespace std;
int main(int argv, char** args) {
    /********** SDL Boilerplate **********/
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << SDL_GetError() << endl;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        cout << IMG_GetError() << endl;
    }

    SDL_Window* main_window = SDL_CreateWindow(
        "88 Fighting",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREENWIDTH * RENDERSCALE,
        SCREENHEIGHT * RENDERSCALE,
        0
    );

    SDL_Renderer* main_renderer = SDL_CreateRenderer(
        main_window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    SDL_RenderSetScale(main_renderer, RENDERSCALE, RENDERSCALE);

    SDL_Event event;

    SDL_GameController* controller = nullptr;
    Controller controllers[4];

    auto start = chrono::system_clock::now();
    
    SDL_SetRenderDrawBlendMode(main_renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
    //SDL_SetRenderDrawColor(main_renderer, 50, 50, 50, 255);

    /*************************************/

    Game game;
    cout << "Here" << endl;
    game.loadStartMenu(main_renderer);

    //Fighter test("Rich Brian", main_renderer);
    //test.set_flip(true, false);

    /************** App Loop **************/
    bool ctr_read = false;
    bool is_running = true;
    while (is_running) {
        SDL_PollEvent(&event);

        switch(event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_CONTROLLERDEVICEADDED:
                if (!controller) {
                    controller = SDL_GameControllerOpen(event.cdevice.which);
                    controllers[event.cdevice.which] = Controller(
                        event.cdevice.which,
                        SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))
                    );
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (controller && event.cdevice.which == SDL_JoystickInstanceID(
                        SDL_GameControllerGetJoystick(controller))) {
                    SDL_GameControllerClose(controller);
                    controller = nullptr;
                }
                for (int i=0; i<4; i++) {
                    if (controllers[i].getInstanceID() == event.cdevice.which) {
                        controllers[i].disconnect();
                    }
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                ctr_read = true;
                for (int i=0; i<4; i++) {
                    if (controllers[i].getInstanceID() == event.cdevice.which) {
                        switch (event.cbutton.button) {
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
                            controllers[i].pressButton(0b00000001);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
                            controllers[i].pressButton(0b00000010);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
                            controllers[i].pressButton(0b00000100);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
                            controllers[i].pressButton(0b00001000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
                            controllers[i].pressButton(0b00010000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                            controllers[i].pressButton(0b00100000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                            controllers[i].pressButton(0b01000000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                            controllers[i].pressButton(0b10000000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
                            controllers[i].setStartButtonState(true);
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                ctr_read = true;
                for (int i=0; i<4; i++) {
                    if (controllers[i].getInstanceID() == event.cdevice.which) {
                        switch (event.cbutton.button) {
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
                            controllers[i].releaseButton(0b00000001);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
                            controllers[i].releaseButton(0b00000010);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
                            controllers[i].releaseButton(0b00000100);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
                            controllers[i].releaseButton(0b00001000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
                            controllers[i].releaseButton(0b00010000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                            controllers[i].releaseButton(0b00100000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                            controllers[i].releaseButton(0b01000000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                            controllers[i].releaseButton(0b10000000);
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
                            controllers[i].setStartButtonState(false);
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            default:
                break;
        }

        auto end = chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        if (elapsed_seconds.count() > 0.016666) {
            if (ctr_read) {
                game.drive(controllers[0]);
                ctr_read = false;
            }
            game.update();

            game.render_to(main_renderer);
            SDL_RenderPresent(main_renderer);
            SDL_RenderClear(main_renderer);
            start = chrono::system_clock::now();
        }

        /*
        updateStartMenu(start_menu);

        SDL_RenderClear(main_renderer);
        start_menu.render_to(main_renderer);
        SDL_RenderPresent(main_renderer);

        SDL_Delay(16);
        */
    }
    /**************************************/

    return 0;
}