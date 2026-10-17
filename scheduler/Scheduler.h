#pragma once

#include "../models/Process.h"
#include "../dsa/QueueArray.h"
#include <cstddef>
#include <string>
#include <vector>

namespace scheduler {

enum class Policy { FCFS, RoundRobin, NonPreemptivePriority };

enum class ProcessState { New, Ready, Running, Completed };

struct ProcessSnapshot {
    int pid{0};
    ProcessState state{ProcessState::New};
    int remainingBurst{0};
};

struct RunSegment {
    int start{0};
    int end{0};
    bool idle{false};
    int pid{0};
    int remainingBurst{0};
    std::vector<int> readyQueue;
    std::vector<ProcessSnapshot> processes;

    RunSegment() = default;
    RunSegment(int segmentStart, int segmentEnd, bool isIdle, int processId)
        : start(segmentStart), end(segmentEnd), idle(isIdle), pid(processId) {}
};

struct ProcessMetrics {
    Process process;
    int completionTime{0};
    int turnaroundTime{0};
    int waitingTime{0};
};

enum class EventType { Arrival, Dispatch, Preemption, Completion };

struct SimulationEvent {
    int timestamp{0};
    EventType type{EventType::Arrival};
    int pid{0};
};

struct SchedulingResult {
    Policy policy{Policy::FCFS};
    bool valid{true};
    std::string error;
    int elapsedTime{0};
    int busyTime{0};
    int quantum{0};
    double averageTurnaround{0.0};
    double averageWaiting{0.0};
    double cpuUtilization{0.0};
    double throughput{0.0};
    std::vector<RunSegment> timeline;
    std::vector<ProcessMetrics> metrics;
    std::vector<SimulationEvent> events;
};

class Scheduler {
    static bool validate(const std::vector<Process>& processes, std::string& error);
    static void finish(SchedulingResult& result, const std::vector<Process>& processes,
                       const std::vector<int>& completion);
    static void recordEvents(SchedulingResult& result, const std::vector<Process>& processes);
public:
    static SchedulingResult fcfs(const std::vector<Process>& processes);
    static SchedulingResult roundRobin(const std::vector<Process>& processes, int quantum);
    static SchedulingResult priority(const std::vector<Process>& processes);
};

}  // namespace scheduler
