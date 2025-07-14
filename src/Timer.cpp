#include "SDL2/SDL.h"
#include "Timer.h"


Timer::Timer() : frame_time(static_cast<int>(SDL_GetTicks())), tick_time(frame_time), ticks_called(0), fps(0) {}

// double fps_cap to allow for float division without casting
int Timer::tick(double fps_cap)  {
    int current_time = static_cast<int>(SDL_GetTicks());

    // counting frames to get fps
    if (current_time - tick_time >= 1000) {
        fps = ticks_called;
        ticks_called = 1;
        tick_time = current_time;
    } else {
        ticks_called++;
    }

    int elapsed = current_time - frame_time;
    int target = 1 / fps_cap * 1000;

    if (elapsed < target) {
        SDL_Delay(target - elapsed);
    }

    current_time = static_cast<int>(SDL_GetTicks());
    elapsed = current_time - frame_time;
    frame_time = current_time;

    return elapsed;

}
