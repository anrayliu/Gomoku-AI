#ifndef TIMER_H
#define TIMER_H

class Timer {
    int frame_time;
    int tick_time;
    int ticks_called;

public:
    int fps;

    Timer();

    int tick(double fps_cap);
};

#endif
