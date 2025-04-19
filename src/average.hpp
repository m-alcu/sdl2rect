#pragma once

class FrameTimeAverager {
    double alpha;
    bool initialized;
    double avg;

public:
    FrameTimeAverager(double smoothing = 0.005)
        : alpha(smoothing), initialized(false), avg(0.0) {}

    double update(double value) {
        if (!initialized) {
            avg = value;
            initialized = true;
        } else {
            avg = alpha * value + (1.0 - alpha) * avg;
        }
        return avg;
    }

    double getAverage() const {
        return avg;
    }
};