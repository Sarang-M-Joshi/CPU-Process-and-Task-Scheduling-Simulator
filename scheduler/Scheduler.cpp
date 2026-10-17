#include "Scheduler.h"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace scheduler {

namespace {
std::vector<ProcessSnapshot> captureProcesses(const std::vector<Process>& processes,
                                               const std::vector<int>& remaining,
                                               const std::vector<bool>& completed,
                                               const std::vector<int>& readyPids,
                                               int runningPid) {
    std::vector<ProcessSnapshot> snapshot;
    snapshot.reserve(processes.size());
    for (std::size_t i = 0; i < processes.size(); ++i) {
        ProcessState state = ProcessState::New;
        if (completed[i]) state = ProcessState::Completed;
        else if (processes[i].pid == runningPid) state = ProcessState::Running;
        else if (std::find(readyPids.begin(), readyPids.end(), processes[i].pid) != readyPids.end()) state = ProcessState::Ready;
        snapshot.push_back({processes[i].pid, state, remaining[i]});
    }
    return snapshot;
}
}  // namespace

bool Scheduler::validate(const std::vector<Process>& processes, std::string& error) {
    std::unordered_set<int> pids;
    long long totalBurst = 0;
    int latestArrival = 0;
    for (const auto& p : processes) {
        if (p.arrivalTime < 0) { error = "Arrival times must be non-negative."; return false; }
        if (p.burstTime <= 0) { error = "Burst times must be positive."; return false; }
        if (!pids.insert(p.pid).second) { error = "PIDs must be unique."; return false; }
        totalBurst += p.burstTime;
        latestArrival = std::max(latestArrival, p.arrivalTime);
    }
    if (static_cast<long long>(latestArrival) + totalBurst > std::numeric_limits<int>::max()) {
        error = "Workload completion time exceeds the supported integer time range."; return false;
    }
    return true;
}

void Scheduler::finish(SchedulingResult& result, const std::vector<Process>& processes,
                       const std::vector<int>& completion) {
    result.metrics.reserve(processes.size());
    double totalTurnaround = 0.0, totalWaiting = 0.0;
    for (std::size_t i = 0; i < processes.size(); ++i) {
        const int turnaround = completion[i] - processes[i].arrivalTime;
        const int waiting = turnaround - processes[i].burstTime;
        result.metrics.push_back({processes[i], completion[i], turnaround, waiting});
        totalTurnaround += turnaround; totalWaiting += waiting;
    }
    if (!processes.empty()) {
        result.averageTurnaround = totalTurnaround / processes.size();
        result.averageWaiting = totalWaiting / processes.size();
    }
    if (result.elapsedTime > 0) {
        result.cpuUtilization = 100.0 * result.busyTime / result.elapsedTime;
        result.throughput = static_cast<double>(processes.size()) / result.elapsedTime;
    }
}

void Scheduler::recordEvents(SchedulingResult& result, const std::vector<Process>& processes) {
    result.events.clear();
    for (const auto& p : processes) result.events.push_back({p.arrivalTime, EventType::Arrival, p.pid});
    for (const auto& segment : result.timeline) {
        if (segment.idle) continue;
        result.events.push_back({segment.start, EventType::Dispatch, segment.pid});
        const auto metric = std::find_if(result.metrics.begin(), result.metrics.end(), [&](const ProcessMetrics& m) {
            return m.process.pid == segment.pid;
        });
        if (metric != result.metrics.end() && metric->completionTime == segment.end)
            result.events.push_back({segment.end, EventType::Completion, segment.pid});
        else if (result.policy == Policy::RoundRobin)
            result.events.push_back({segment.end, EventType::Preemption, segment.pid});
    }
    auto rank = [](EventType type) {
        switch (type) {
            case EventType::Arrival: return 0;
            case EventType::Preemption: return 1;
            case EventType::Completion: return 2;
            case EventType::Dispatch: return 3;
        }
        return 4;
    };
    std::stable_sort(result.events.begin(), result.events.end(), [&](const SimulationEvent& a, const SimulationEvent& b) {
        if (a.timestamp != b.timestamp) return a.timestamp < b.timestamp;
        if (rank(a.type) != rank(b.type)) return rank(a.type) < rank(b.type);
        return a.pid < b.pid;
    });
}

SchedulingResult Scheduler::fcfs(const std::vector<Process>& processes) {
    SchedulingResult r; r.policy = Policy::FCFS;
    if (!validate(processes, r.error)) { r.valid = false; return r; }
    std::vector<std::size_t> order(processes.size());
    for (std::size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
        if (processes[a].arrivalTime != processes[b].arrivalTime) return processes[a].arrivalTime < processes[b].arrivalTime;
        return processes[a].pid < processes[b].pid;
    });
    std::vector<int> completion(processes.size(), 0);
    std::vector<int> remaining(processes.size());
    std::vector<bool> done(processes.size(), false);
    for (std::size_t i = 0; i < processes.size(); ++i) remaining[i] = processes[i].burstTime;
    int time = 0;
    for (std::size_t i : order) {
        const Process& p = processes[i];
        if (time < p.arrivalTime) {
            r.timeline.push_back({time, p.arrivalTime, true, 0});
            r.timeline.back().processes = captureProcesses(processes, remaining, done, {}, 0);
            time = p.arrivalTime;
        }
        const int end = time + p.burstTime;
        std::vector<int> ready;
        for (std::size_t j : order) if (j != i && !done[j] && processes[j].arrivalTime <= time) ready.push_back(processes[j].pid);
        r.timeline.push_back({time, end, false, p.pid});
        auto& segment = r.timeline.back();
        segment.remainingBurst = remaining[i];
        segment.readyQueue = ready;
        segment.processes = captureProcesses(processes, remaining, done, ready, p.pid);
        r.busyTime += p.burstTime; time = end; completion[i] = time; remaining[i] = 0; done[i] = true;
    }
    r.elapsedTime = time; finish(r, processes, completion); recordEvents(r, processes); return r;
}

SchedulingResult Scheduler::roundRobin(const std::vector<Process>& processes, int quantum) {
    SchedulingResult r; r.policy = Policy::RoundRobin;
    if (!validate(processes, r.error)) { r.valid = false; return r; }
    if (quantum <= 0) { r.valid = false; r.error = "Round Robin quantum must be positive."; return r; }
    r.quantum = quantum;
    const std::size_t n = processes.size();
    if (n == 0) return r;
    if (n > static_cast<std::size_t>(std::numeric_limits<int>::max())) { r.valid = false; r.error = "Too many processes for the circular queue."; return r; }
    std::vector<std::size_t> arrivalOrder(n);
    for (std::size_t i = 0; i < n; ++i) arrivalOrder[i] = i;
    std::sort(arrivalOrder.begin(), arrivalOrder.end(), [&](std::size_t a, std::size_t b) {
        if (processes[a].arrivalTime != processes[b].arrivalTime) return processes[a].arrivalTime < processes[b].arrivalTime;
        return processes[a].pid < processes[b].pid;
    });
    dsa::QueueArray ready(n);
    std::vector<int> remaining(n), completion(n, 0);
    std::vector<bool> done(n, false);
    for (std::size_t i = 0; i < n; ++i) remaining[i] = processes[i].burstTime;
    std::size_t next = 0, completed = 0;
    int time = 0;
    while (completed < n) {
        while (next < n && processes[arrivalOrder[next]].arrivalTime <= time) {
            ready.enqueue(static_cast<int>(arrivalOrder[next++]));
        }
        if (ready.isEmpty()) {
            const int nextTime = processes[arrivalOrder[next]].arrivalTime;
            if (time < nextTime) {
                r.timeline.push_back({time, nextTime, true, 0});
                r.timeline.back().processes = captureProcesses(processes, remaining, done, {}, 0);
                time = nextTime;
            }
            continue;
        }
        const std::size_t i = static_cast<std::size_t>(*ready.dequeue());
        const int duration = std::min(quantum, remaining[i]);
        const int end = time + duration;
        r.timeline.push_back({time, end, false, processes[i].pid});
        auto& segment = r.timeline.back();
        const auto queued = ready.values();
        for (int index : queued) segment.readyQueue.push_back(processes[static_cast<std::size_t>(index)].pid);
        segment.remainingBurst = remaining[i];
        segment.processes = captureProcesses(processes, remaining, done, segment.readyQueue, processes[i].pid);
        r.busyTime += duration; remaining[i] -= duration; time = end;
        // Arrivals at the slice boundary enter the queue before the interrupted process.
        while (next < n && processes[arrivalOrder[next]].arrivalTime <= time) {
            ready.enqueue(static_cast<int>(arrivalOrder[next++]));
        }
        if (remaining[i] > 0) ready.enqueue(static_cast<int>(i));
        else { completion[i] = time; done[i] = true; ++completed; }
    }
    r.elapsedTime = time; finish(r, processes, completion); recordEvents(r, processes); return r;
}

SchedulingResult Scheduler::priority(const std::vector<Process>& processes) {
    SchedulingResult r; r.policy = Policy::NonPreemptivePriority;
    if (!validate(processes, r.error)) { r.valid = false; return r; }
    const std::size_t n = processes.size();
    std::vector<bool> done(n, false);
    std::vector<int> completion(n, 0);
    std::size_t completed = 0;
    int time = 0;
    while (completed < n) {
        std::size_t chosen = n;
        for (std::size_t i = 0; i < n; ++i) {
            if (done[i] || processes[i].arrivalTime > time) continue;
            if (chosen == n || processes[i].priority < processes[chosen].priority ||
                (processes[i].priority == processes[chosen].priority &&
                 (processes[i].arrivalTime < processes[chosen].arrivalTime ||
                  (processes[i].arrivalTime == processes[chosen].arrivalTime && processes[i].pid < processes[chosen].pid)))) chosen = i;
        }
        if (chosen == n) {
            int nextArrival = std::numeric_limits<int>::max();
            for (std::size_t i = 0; i < n; ++i) if (!done[i]) nextArrival = std::min(nextArrival, processes[i].arrivalTime);
            if (time < nextArrival) {
                r.timeline.push_back({time, nextArrival, true, 0});
                std::vector<int> remaining(n);
                for (std::size_t j = 0; j < n; ++j) remaining[j] = done[j] ? 0 : processes[j].burstTime;
                r.timeline.back().processes = captureProcesses(processes, remaining, done, {}, 0);
                time = nextArrival;
            }
            continue;
        }
        const Process& p = processes[chosen];
        const int end = time + p.burstTime;
        std::vector<std::size_t> ranked;
        for (std::size_t j = 0; j < n; ++j) if (j != chosen && !done[j] && processes[j].arrivalTime <= time) ranked.push_back(j);
        std::sort(ranked.begin(), ranked.end(), [&](std::size_t a, std::size_t b) {
            if (processes[a].priority != processes[b].priority) return processes[a].priority < processes[b].priority;
            if (processes[a].arrivalTime != processes[b].arrivalTime) return processes[a].arrivalTime < processes[b].arrivalTime;
            return processes[a].pid < processes[b].pid;
        });
        std::vector<int> ready;
        for (std::size_t j : ranked) ready.push_back(processes[j].pid);
        std::vector<int> remaining(n);
        for (std::size_t j = 0; j < n; ++j) remaining[j] = done[j] ? 0 : processes[j].burstTime;
        r.timeline.push_back({time, end, false, p.pid});
        auto& segment = r.timeline.back();
        segment.remainingBurst = p.burstTime;
        segment.readyQueue = ready;
        segment.processes = captureProcesses(processes, remaining, done, ready, p.pid);
        r.busyTime += p.burstTime; time = end;
        completion[chosen] = time; done[chosen] = true; ++completed;
    }
    r.elapsedTime = time; finish(r, processes, completion); recordEvents(r, processes); return r;
}

}  // namespace scheduler
