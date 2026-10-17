#pragma once

#include <iostream>

namespace scheduler {

struct Process {
    int pid{0};
    int arrivalTime{0};
    int burstTime{1};
    int priority{0};

    void display(std::ostream& out = std::cout) const {
        out << "PID: " << pid << " | Arrival: " << arrivalTime
            << " | Burst: " << burstTime << " | Priority: " << priority;
    }
};

}  // namespace scheduler
