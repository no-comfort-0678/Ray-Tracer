#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include "vec.h"
#include "phy.h"
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const float PI = 3.14159265f;

// --- Helper Functions ---
float inter(ray r, vec sun_pos, float rad){
    vec oc = r.origin.sub(sun_pos);
    float a = r.dir.dot(r.dir);
    float b = 2.0f * oc.dot(r.dir);
    float c = oc.dot(oc) -(rad * rad);
    float disc =(b * b) -(4 * a * c);
    if(disc < 0) return -1.0f;
    return(-b - pow(disc,0.5)) /(2.0f * a);
}

void draw(SDL_Renderer* renderer, int cX, int cY, int rad){
    for(int w = 0; w < rad * 2; w++){
        for(int h = 0; h < rad * 2; h++){
            int dx = rad - w;
            int dy = rad - h;
            if((dx*dx + dy*dy) <=(rad * rad)) 
                SDL_RenderDrawPoint(renderer, cX + dx, cY + dy);
        }
    }
}

float norm(float angle){
    while(angle > PI) angle -= 2 * PI;
    while(angle < -PI) angle += 2 * PI;
    return angle;
}

// --- Main ---
int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Ray Tracer ig", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    vec sun_pos(100, 100);      
    float sun_rad = 10.0f; 
    vec obj_pos(400, 300);   
    float obj_rad = 50.0f;
    bool running = true;
    // State: 0 = Nothing, 1 = Dragging Sun, 2 = Dragging Object
    int tgt = 0; 
    
    SDL_Event event;
    while(running){
        // 1. INPUT HANDLING
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = false;
            
            // Mouse Down: Check WHAT we clicked
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
                vec mousePos((float)event.button.x,(float)event.button.y);
                
                // Check distance to Sun
                vec toSun = mousePos.sub(sun_pos);
                if(sqrt(toSun.dot(toSun)) < sun_rad + 10){ // +10 buffer for easy clicking
                    tgt = 1; 
                }
                // Check distance to Object
                vec toObj = mousePos.sub(obj_pos);
                if(sqrt(toObj.dot(toObj)) < obj_rad + 10){
                    tgt = 2;
                }
            }

            // Mouse Up: Stop dragging
            if(event.type == SDL_MOUSEBUTTONUP){
                tgt = 0;
            }
            
            // Mouse Move: Update Position of selected target
            if(event.type == SDL_MOUSEMOTION && tgt != 0){
                vec newMousePos((float)event.motion.x,(float)event.motion.y);

                if(tgt == 1){ 
                    // DRAGGING SUN
                    // Proposed new position
                    vec proposedPos = newMousePos;
                    
                    // Collision Check: Am I hitting the Object?
                    vec toObj = proposedPos.sub(obj_pos);
                    float dist = sqrt(toObj.dot(toObj));
                    float limit = sun_rad + obj_rad;
                    
                    if(dist < limit){
                        // Clamp Sun to the edge of the Object
                        vec pushDir = toObj.norm();
                        sun_pos = obj_pos.add(pushDir.mult(limit)); 
                    } else{
                        sun_pos = proposedPos;
                    }
                } 
                else if(tgt == 2){ 
                    // DRAGGING OBJECT
                    vec proposedPos = newMousePos;
                    
                    // Collision Check: Am I hitting the Sun?
                    vec toSun = proposedPos.sub(sun_pos);
                    float dist = sqrt(toSun.dot(toSun));
                    float limit = sun_rad + obj_rad;
                    
                    if(dist < limit){
                        // Clamp Object to the edge of the Sun
                        vec pushDir = toSun.norm();
                        obj_pos = sun_pos.add(pushDir.mult(limit));
                    } else{
                        obj_pos = proposedPos;
                    }
                }
            }
        }

        // 2. RENDER
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); 
        SDL_RenderClear(renderer);

        // --- Ray Calculation ---
        vec to_obj = obj_pos.sub(sun_pos);
        float dist = sqrt(to_obj.dot(to_obj));
        float angle = atan2(to_obj.y, to_obj.x);
        
        float ratio = obj_rad / dist;
        if(ratio > 1.0f) ratio = 0.99f; 
        float spread = asin(ratio); 
        int rays = 180; 

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 

        for(int i = 0; i < rays; i++){
            float tmp_angle =(float)i *(2.0f * PI / rays);
            float diff = norm(tmp_angle - angle);            
            if(abs(diff) < spread){
                vec dir(cos(tmp_angle), sin(tmp_angle));
                ray r(sun_pos, dir);
                float t = inter(r, obj_pos, obj_rad);
                if(t > 0){
                   vec hitPoint = r.dir_to(t);
                   SDL_RenderDrawLine(renderer, sun_pos.x, sun_pos.y, hitPoint.x, hitPoint.y);
                }
            } 
            else{
                vec dir(cos(tmp_angle), sin(tmp_angle));
                ray r(sun_pos, dir);
                vec farPoint = r.dir_to(1000.0f);
                SDL_RenderDrawLine(renderer, sun_pos.x, sun_pos.y, farPoint.x, farPoint.y);
            }
        }

        // Draw Object(Grey)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); 
        draw(renderer,(int)obj_pos.x,(int)obj_pos.y,(int)obj_rad);
        // Draw Sun(Yellow)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); 
        draw(renderer,(int)sun_pos.x,(int)sun_pos.y,(int)sun_rad);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}