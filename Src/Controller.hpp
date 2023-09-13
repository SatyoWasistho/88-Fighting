#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <chrono>

enum HatDir{
    NEUTRAL,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UPLEFT,
    UPRIGHT,
    DOWNLEFT,
    DOWNRIGHT,
    INVALID
};

struct Input {
    std::chrono::_V2::system_clock::time_point time;
    uint8_t state;
};

class Controller {
private:
    uint8_t state;
    bool start_btn;
    int player_id;
    int instance_id;
    std::vector<Input> buffer;
public:
    Controller() {
        this->start_btn = false;
        this->state = 0x00;
        this->player_id = -1;
        this->instance_id = -1;
    }
    Controller(int player_id, int instance_id) {
        this->start_btn = false;
        this->player_id = player_id;
        this->instance_id = instance_id;
        this->state = 0x00;
    }
    ~Controller() {

    }

    void disconnect() {
        if (this->isConnected()) {
            this->state = 0x0000;
            this->player_id = -1;
            this->instance_id = -1;
        }
    }

    int getInstanceID() {
        return this->instance_id;
    }

    int getPlayerID() {
        return this->player_id;
    }
    int getBufferSize() {
        return this->buffer.size();
    }
    uint8_t getButtonState(int idx = -1) {
        if (idx >= 0) {
            return this->buffer[this->buffer.size() - 1 - idx].state;
        }
        
        return this->state;
    }
    uint8_t getButtonDeltas(int idx = -1) {
        if (idx >= 0) {
            return this->buffer[this->buffer.size() - 1 - idx].state ^ this->buffer[this->buffer.size() - 2 - idx].state;
        }
        return this->state ^ this->buffer[this->buffer.size() - 1].state;
    }
    bool getStartButtonState() {
        return this->start_btn;
    }
    void setStartButtonState(bool state) {
        this->start_btn = state;
    }

    bool isConnected() {
        return this->player_id >= 0;
    }
    void pressButton(int button) {
        Input in;
        in.time = std::chrono::system_clock::now();
        in.state = this->state;
        this->buffer.push_back(in);
        bool flag = true;
        while (flag) {
            std::chrono::duration<double> elapsed_seconds = in.time - this->buffer[0].time;
            if (elapsed_seconds.count() > 0.166) {
                this->buffer.erase(buffer.begin());
            }
            else {
                flag = false;
            }
        }
        this->state |= button;
    }
    void releaseButton(int button) {
        Input in;
        in.time = std::chrono::system_clock::now();
        in.state = this->state;
        this->buffer.push_back(in);
        bool flag = true;
        while (flag) {
            std::chrono::duration<double> elapsed_seconds = in.time - this->buffer[0].time;
            if (elapsed_seconds.count() > 1.0) {
                this->buffer.erase(buffer.begin());
            }
            else {
                flag = false;
            }
        }
        this->state &= ~(button);
    }
    HatDir getDirection(int idx = -1) {
        uint8_t test_state = this->state;
        if (idx >= 0) test_state = this->buffer[this->buffer.size() - 1 - idx].state;
        switch(test_state & 0b11110000) {
            case 0:
                return HatDir::NEUTRAL;
                break;
            case 0b00010000:
                return HatDir::UP;
                break;
            case 0b00100000:
                return HatDir::DOWN;
                break;
            case 0b01000000:
                return HatDir::LEFT;
                break;
            case 0b10000000:
                return HatDir::RIGHT;
                break;
            case 0b01010000:
                return HatDir::UPLEFT;
                break;
            case 0b10010000:
                return HatDir::UPRIGHT;
                break;
            case 0b01100000:
                return HatDir::DOWNLEFT;
                break;
            case 0b10100000:
                return HatDir::DOWNRIGHT;
                break;
            default:
                return HatDir::INVALID;
                break;
        }
    }
};