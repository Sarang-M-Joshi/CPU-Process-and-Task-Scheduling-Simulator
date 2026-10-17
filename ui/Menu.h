#pragma once

#include "../dsa/DLL.h"
#include "../dsa/QueueArray.h"
#include "../dsa/QueueLinked.h"
#include "../dsa/SLL.h"
#include "../dsa/StackArray.h"
#include "../dsa/StackLinked.h"
#include "../dsa/SearchSort.h"
#include "../dsa/HashTable.h"
#include "../models/Process.h"
#include "../storage/SequentialProcessFile.h"
#include "../scheduler/Scheduler.h"
#include "Terminal.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

namespace ui {

class Menu {
    std::vector<scheduler::Process> processes_;
    bool running_{true};
    Terminal terminal_;

    void line(const std::string& text, Terminal::Color color = Terminal::Color::White, int revealDelay = 0) const {
        if (revealDelay > 0) terminal_.revealLine(text, color, revealDelay);
        else { terminal_.write(text, color); std::cout << '\n'; }
    }
    void bootSequence() const {
        std::cout << "\n+----------------------------------------------------------+\n";
        terminal_.typewriter("# DOOMSDAY // TEMPORAL CORE", Terminal::Color::Green);
        line("CPU Process & Task Scheduling Simulator", Terminal::Color::Cyan, Terminal::Timing::BootLine);
        line("Every process has a timeline. Every decision changes the outcome.", Terminal::Color::Grey, Terminal::Timing::BootLine);
        std::cout << "+----------------------------------------------------------+\n";
        line("TEMPORAL CORE INITIALIZING // presentation sequence", Terminal::Color::Yellow, Terminal::Timing::BootLine);
        line("PROCESS REGISTRY READY // in-memory registry initialized", Terminal::Color::Green, Terminal::Timing::BootLine);
        line("SCHEDULER MODULE READY // FCFS, Round Robin, Priority", Terminal::Color::Green, Terminal::Timing::BootLine);
        line("DSA TRAINING CHAMBER READY // structures initialized", Terminal::Color::Green, Terminal::Timing::BootLine);
        line("TIMELINE ARCHIVE INTERFACE READY // log checked when opened", Terminal::Color::Cyan, Terminal::Timing::BootLine);
        terminal_.progressBar(18, Terminal::Timing::BootProgressStep);
        line("CORE STATUS: ONLINE", Terminal::Color::Green, Terminal::Timing::BootLine);
    }
    void revealMenuLine(const std::string& key, const std::string& label, const std::string& detail = "") const {
        std::cout << terminal_.paint(key, Terminal::Color::Green) << " "
                  << terminal_.paint(label, Terminal::Color::White);
        if (!detail.empty()) std::cout << "  " << terminal_.paint("// " + detail, Terminal::Color::Grey);
        std::cout << '\n' << std::flush;
        terminal_.delay(Terminal::Timing::MenuReveal);
    }

    bool readInt(const std::string& prompt, int& value) const {
        for (;;) {
            terminal_.write(prompt, Terminal::Color::Yellow);
            if (std::cin >> value) return true;
            if (std::cin.eof()) return false;
            terminal_.write("Please enter a whole number.\n", Terminal::Color::Red);
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    bool readIndex(const std::string& prompt, std::size_t& value) const {
        int raw{};
        if (!readInt(prompt, raw)) return false;
        if (raw < 0) { terminal_.write("Index must be non-negative.\n", Terminal::Color::Red); value = std::numeric_limits<std::size_t>::max(); }
        else value = static_cast<std::size_t>(raw);
        return true;
    }
    static void pauseLine() { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
    bool readForStructure(int& x) { if (!readInt("Value: ", x)) { running_ = false; return false; } return true; }
    bool readListIndex(std::size_t& i) { if (!readIndex("Index (zero-based): ", i)) { running_ = false; return false; } return true; }
    void showResult(const dsa::Maybe<int>& result, const char* action) const {
        if (result) line(std::string(action) + " value " + std::to_string(*result) + ".", Terminal::Color::Green);
        else line("Operation unavailable: the structure is empty or the index is invalid.", Terminal::Color::Red);
    }
    static void displayList(dsa::SLL& list, bool) { list.display(); }
    static void displayList(dsa::DLL& list, bool backward) { if (backward) list.displayBackward(); else list.displayForward(); }

    bool readValues(std::vector<int>& values) const {
        int n; if (!readInt("Number of values: ", n)) return false;
        if (n < 0 || n > 100000) { terminal_.write("Count must be between 0 and 100000.\n", Terminal::Color::Red); return true; }
        values.clear(); values.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) { int v; if (!readInt("Value: ", v)) return false; values.push_back(v); }
        return true;
    }
    void algorithmsMenu() {
        while (running_) {
            line("\nSEARCH // PROCESS LOCATOR // SORT // WORKLOAD ORDERING CONSOLE", Terminal::Color::Green);
            line("1. Linear search  2. Binary search (sorted input)", Terminal::Color::Cyan);
            line("3. Bubble sort  4. Insertion sort  5. Quick sort  0. Back", Terminal::Color::White, Terminal::Timing::DsaMenuRow);
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            if (!c) return;
            std::vector<int> values; if (c < 1 || c > 5) { line("Choose a listed option.", Terminal::Color::Red); continue; }
            if (!readValues(values)) { running_ = false; return; }
            if (c == 1 || c == 2) {
                if (c == 2 && !std::is_sorted(values.begin(), values.end())) { line("Binary search requires ascending sorted input.", Terminal::Color::Red); continue; }
                int key; if (!readInt("Search value: ", key)) { running_ = false; return; }
                int at = c == 1 ? dsa::linearSearch(values, key) : dsa::binarySearch(values, key);
                if (at < 0) line("Value not found.", Terminal::Color::Yellow);
                else line("Found at zero-based index " + std::to_string(at) + ".", Terminal::Color::Green);
            } else {
                if (c == 3) dsa::bubbleSort(values); else if (c == 4) dsa::insertionSort(values); else dsa::quickSort(values);
                std::cout << "Ascending: "; for (int v : values) std::cout << v << ' '; std::cout << '\n';
            }
        }
    }
    void hashMenu(dsa::HashTable& table) {
        while (running_) {
            line("\nHASH TABLE // PID LOOKUP INDEX", Terminal::Color::Green);
            line("Open addressing // linear probing", Terminal::Color::Cyan);
            line("1. Insert  2. Search  3. Delete  4. Display  0. Back", Terminal::Color::White, Terminal::Timing::MenuRow);
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; } if (!c) return;
            if (c == 4) { table.display(std::cout); continue; }
            if (c < 1 || c > 3) { std::cout << "Choose a listed option.\n"; continue; }
            int key; if (!readInt("Key: ", key)) { running_ = false; return; }
            if (c == 1) {
                const bool inserted = table.insert(key);
                line(inserted ? "Inserted." : "Duplicate key or table full.", inserted ? Terminal::Color::Green : Terminal::Color::Yellow);
            } else if (c == 2) line(table.contains(key) ? "Key found." : "Key not found.", table.contains(key) ? Terminal::Color::Green : Terminal::Color::Yellow);
            else { const bool erased = table.erase(key); line(erased ? "Deleted." : "Key not found.", erased ? Terminal::Color::Green : Terminal::Color::Yellow); }
        }
    }
    void fileMenu(storage::SequentialProcessFile& file) {
        while (running_) {
            line("\nSEQUENTIAL FILE // PROCESS RECORD ARCHIVE", Terminal::Color::Green);
            line("" + file.path() + " // CSV: PID,arrival,burst,priority", Terminal::Color::Cyan);
            line("1. Append  2. Display  3. Search PID  4. Delete PID  0. Back", Terminal::Color::White, Terminal::Timing::MenuRow);
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; } if (!c) return;
            if (c == 1) {
                scheduler::Process p;
                if (!readInt("PID: ", p.pid) || !readInt("Arrival time (>= 0): ", p.arrivalTime) || !readInt("Burst time (> 0): ", p.burstTime) || !readInt("Priority: ", p.priority)) { running_ = false; return; }
                if (p.arrivalTime < 0 || p.burstTime <= 0) line("Invalid process values.", Terminal::Color::Red);
                else {
                    const bool saved = file.append(p);
                    line(saved ? "Record appended." : "Append failed (duplicate PID, malformed file, or I/O error).",
                         saved ? Terminal::Color::Green : Terminal::Color::Red);
                }
            } else if (c == 2) {
                std::vector<scheduler::Process> records;
                if (!file.display(records)) { line("Could not read file; it may contain a malformed record.", Terminal::Color::Red); continue; }
                if (records.empty()) std::cout << "No records.\n";
                for (const auto& p : records) { p.display(); std::cout << '\n'; }
            } else if (c == 3 || c == 4) {
                int pid; if (!readInt("PID: ", pid)) { running_ = false; return; }
                if (c == 3) { scheduler::Process p; if (file.find(pid, p)) { p.display(); std::cout << '\n'; } else line("PID not found or file unreadable.", Terminal::Color::Yellow); }
                else { bool found; if (!file.erase(pid, found)) line("Delete failed; original file was retained.", Terminal::Color::Red); else line(found ? "Record deleted." : "PID not found.", found ? Terminal::Color::Green : Terminal::Color::Yellow); }
            } else std::cout << "Choose a listed option.\n";
        }
    }

    static const char* policyName(scheduler::Policy policy) {
        switch (policy) {
            case scheduler::Policy::FCFS: return "FCFS";
            case scheduler::Policy::RoundRobin: return "Round Robin";
            case scheduler::Policy::NonPreemptivePriority: return "Non-preemptive Priority";
        }
        return "Scheduling";
    }
    static const char* eventName(scheduler::EventType type) {
        switch (type) {
            case scheduler::EventType::Arrival: return "ARRIVAL";
            case scheduler::EventType::Dispatch: return "DISPATCH";
            case scheduler::EventType::Preemption: return "PREEMPTION";
            case scheduler::EventType::Completion: return "COMPLETION";
        }
        return "EVENT";
    }
    static const char* eventMessage(scheduler::EventType type, scheduler::Policy policy) {
        switch (type) {
            case scheduler::EventType::Arrival: return "NEW PROCESS DETECTED";
            case scheduler::EventType::Dispatch: return "PROCESS DISPATCHED";
            case scheduler::EventType::Preemption:
                return policy == scheduler::Policy::RoundRobin ? "TIME QUANTUM EXHAUSTED // PROCESS ROTATED" : "PROCESS PREEMPTED";
            case scheduler::EventType::Completion: return "PROCESS COMPLETED";
        }
        return "SCHEDULER EVENT";
    }
    static const char* stateName(scheduler::ProcessState state) {
        switch (state) {
            case scheduler::ProcessState::New: return "NEW";
            case scheduler::ProcessState::Ready: return "READY";
            case scheduler::ProcessState::Running: return "RUNNING";
            case scheduler::ProcessState::Completed: return "COMPLETED";
        }
        return "UNKNOWN";
    }
    void saveRun(const scheduler::SchedulingResult& result) const {
        if (!result.valid) return;
        std::ofstream out("simulation_events.log", std::ios::app);
        if (!out) { line("Archive unavailable: could not write simulation_events.log.", Terminal::Color::Red); return; }
        static unsigned long runSequence = 0;
        const auto runTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        const std::string runId = std::to_string(runTime) + "-" + std::to_string(++runSequence);
        out << "RUN|" << policyName(result.policy) << '\n';
        out << "META|" << runId << '|' << policyName(result.policy) << '|' << result.quantum << '\n';
        for (const auto& metric : result.metrics)
            out << "PROCESS|" << metric.process.pid << '|' << metric.process.arrivalTime << '|'
                << metric.process.burstTime << '|' << metric.process.priority << '\n';
        for (const auto& event : result.events)
            out << "EVENT|" << event.timestamp << '|' << eventName(event.type) << '|' << event.pid << '\n';
        for (const auto& segment : result.timeline)
            out << "SEG|" << segment.start << '|' << segment.end << '|' << (segment.idle ? 1 : 0) << '|' << segment.pid << '\n';
        for (const auto& metric : result.metrics)
            out << "METRIC|" << metric.process.pid << '|' << metric.completionTime << '|'
                << metric.turnaroundTime << '|' << metric.waitingTime << '\n';
        out << "SUMMARY|" << result.elapsedTime << '|' << result.busyTime << '|'
            << result.averageTurnaround << '|' << result.averageWaiting << '|'
            << result.cpuUtilization << '|' << result.throughput << '\n';
        out << "END\n";
        line("Event log saved to simulation_events.log.", Terminal::Color::Green);
    }
    void replayLatest() {
        std::ifstream in("simulation_events.log");
        if (!in) { line("No timeline archive found (simulation_events.log).", Terminal::Color::Yellow); return; }
        std::string raw, policy, meta, summary;
        std::vector<std::string> events, segments, processes, metrics;
        std::vector<std::string> currentEvents, currentSegments, currentProcesses, currentMetrics;
        std::string currentPolicy, currentMeta, currentSummary;
        while (std::getline(in, raw)) {
            if (raw.rfind("RUN|", 0) == 0) {
                currentPolicy = raw.substr(4); currentMeta.clear(); currentSummary.clear();
                currentEvents.clear(); currentSegments.clear(); currentProcesses.clear(); currentMetrics.clear();
            } else if (raw.rfind("META|", 0) == 0) currentMeta = raw.substr(5);
            else if (raw.rfind("PROCESS|", 0) == 0) currentProcesses.push_back(raw.substr(8));
            else if (raw.rfind("EVENT|", 0) == 0) currentEvents.push_back(raw.substr(6));
            else if (raw.rfind("SEG|", 0) == 0) currentSegments.push_back(raw.substr(4));
            else if (raw.rfind("METRIC|", 0) == 0) currentMetrics.push_back(raw.substr(7));
            else if (raw.rfind("SUMMARY|", 0) == 0) currentSummary = raw.substr(8);
            else if (raw == "END") {
                policy = currentPolicy; meta = currentMeta; summary = currentSummary;
                events = currentEvents; segments = currentSegments; processes = currentProcesses; metrics = currentMetrics;
            }
        }
        if (policy.empty()) { line("The timeline archive is empty or malformed.", Terminal::Color::Yellow); return; }
        int control = 1;
        if (terminal_.animation() != Terminal::Animation::Off &&
            !readInt("Replay: 1. Timed  2. Step-controlled  0. Cancel: ", control)) { running_ = false; return; }
        if (control == 0) return;
        if (control == 2) pauseLine();
        line("DOOMSDAY // TIMELINE ARCHIVE // REPLAY", Terminal::Color::Green);
        line("Protocol: " + policy, Terminal::Color::Cyan);
        if (!meta.empty()) line("Run ID / policy / quantum: " + meta, Terminal::Color::Grey);
        if (!processes.empty()) {
            line("Saved workload (PID | arrival | burst | priority):", Terminal::Color::Blue);
            for (const auto& process : processes) line("  " + process, Terminal::Color::White);
        }
        std::size_t eventCursor = 0;
        std::vector<std::string> replayGantt;
        for (std::size_t i = 0; i < segments.size(); ++i) {
            std::istringstream row(segments[i]); int start = 0, end = 0, idle = 0, pid = 0;
            row >> start; row.ignore(1); row >> end; row.ignore(1); row >> idle; row.ignore(1); row >> pid;
            const std::string cell = std::to_string(start) + " -- " + (idle ? "IDLE" : "P" + std::to_string(pid)) + " -- " + std::to_string(end);
            replayGantt.push_back(cell);
            terminal_.write("TIMELINE: ", Terminal::Color::Blue);
            for (std::size_t j = 0; j < replayGantt.size(); ++j) {
                if (j) terminal_.write(" | ", Terminal::Color::Grey);
                terminal_.revealGantt(replayGantt[j], 0);
            }
            std::cout << '\n';
            while (eventCursor < events.size()) {
                std::istringstream eventRow(events[eventCursor]); int timestamp = 0, pidEvent = 0;
                std::string type;
                eventRow >> timestamp; eventRow.ignore(1); std::getline(eventRow, type, '|'); eventRow >> pidEvent;
                if (timestamp > end) break;
                std::string description = type;
                if (type == "ARRIVAL") description = "NEW PROCESS DETECTED";
                else if (type == "DISPATCH") description = "PROCESS DISPATCHED";
                else if (type == "COMPLETION") description = "PROCESS COMPLETED";
                else if (type == "PREEMPTION") description = policy == "Round Robin" ?
                    "TIME QUANTUM EXHAUSTED // PROCESS ROTATED" : "PROCESS PREEMPTED";
                std::string message = "[T=" + std::to_string(timestamp) + "] P" + std::to_string(pidEvent) + " " + description;
                Terminal::Color color = type == "ARRIVAL" || type == "PREEMPTION" ? Terminal::Color::Yellow :
                                        type == "COMPLETION" ? Terminal::Color::Grey : Terminal::Color::Green;
                line(message, color);
                ++eventCursor;
                terminal_.delay(Terminal::Timing::EventReplay);
            }
            if (control == 2) {
                bool paused = true;
                while (paused) {
                    std::string action;
                    std::cout << "Replay controls [Enter resume/next | + faster | - slower | q exit]: ";
                    std::getline(std::cin, action);
                    if (action == "q" || action == "Q") return;
                    if (action == "+") { terminal_.setAnimation(Terminal::Animation::Full); continue; }
                    if (action == "-") { terminal_.setAnimation(Terminal::Animation::Reduced); continue; }
                    paused = false;
                }
            } else terminal_.delay(Terminal::Timing::ReplaySegment);
        }
        while (eventCursor < events.size()) {
            line("[T=" + events[eventCursor].substr(0, events[eventCursor].find('|')) + "] " + events[eventCursor], Terminal::Color::Yellow);
            ++eventCursor;
        }
        line("FINAL SAVED GANTT", Terminal::Color::Green);
        for (const auto& cell : replayGantt) line("  " + cell, Terminal::Color::Cyan);
        if (!metrics.empty()) {
            line("SAVED METRICS (PID | completion | turnaround | waiting):", Terminal::Color::Green);
            for (const auto& metric : metrics) line("  " + metric, Terminal::Color::White);
        }
        if (!summary.empty()) line("SAVED SUMMARY (elapsed | busy | avg TAT | avg WT | CPU % | throughput): " + summary, Terminal::Color::Cyan);
        else line("Legacy archive: final metrics were not stored in this log entry.", Terminal::Color::Grey);
    }
    void showLive(const scheduler::SchedulingResult& result, const std::vector<scheduler::Process>& processes) {
        if (!result.valid) { showSchedule(result); return; }
        line("DOOMSDAY // TEMPORAL CORE", Terminal::Color::Green);
        std::string protocol = policyName(result.policy);
        if (result.policy == scheduler::Policy::FCFS) protocol += " // CHRONOLOGICAL PROTOCOL";
        else if (result.policy == scheduler::Policy::RoundRobin) protocol += " // TEMPORAL ROTATION // QUANTUM=" + std::to_string(result.quantum);
        else protocol += " // THREAT ASSESSMENT PROTOCOL";
        line("PROTOCOL: " + protocol, Terminal::Color::Cyan);
        std::size_t eventCursor = 0;
        std::vector<std::string> gantt;
        for (const auto& segment : result.timeline) {
            while (eventCursor < result.events.size() && result.events[eventCursor].timestamp <= segment.start) {
                const auto& event = result.events[eventCursor++];
                line("[T=" + std::to_string(event.timestamp) + "] P" + std::to_string(event.pid) + " " + eventMessage(event.type, result.policy),
                     event.type == scheduler::EventType::Arrival || event.type == scheduler::EventType::Preemption ? Terminal::Color::Yellow : Terminal::Color::Green, Terminal::Timing::EventHighlight);
            }
            line("SIMULATED TIME: " + std::to_string(segment.start), Terminal::Color::Cyan);
            line("RUNNING PROCESS: " + std::string(segment.idle ? "IDLE // WAITING FOR NEXT ARRIVAL" : "P" + std::to_string(segment.pid)) +
                 (segment.idle ? "" : " (" + std::to_string(segment.remainingBurst) + " remaining)"),
                 segment.idle ? Terminal::Color::Yellow : Terminal::Color::Green);
            if (result.policy == scheduler::Policy::NonPreemptivePriority && !segment.idle) {
                for (const auto& process : processes) if (process.pid == segment.pid)
                    line("HIGHEST PRIORITY PROCESS SELECTED // priority=" + std::to_string(process.priority), Terminal::Color::Yellow);
            }
            std::cout << terminal_.paint("READY QUEUE: [", Terminal::Color::Cyan);
            for (std::size_t i = 0; i < segment.readyQueue.size(); ++i) {
                if (i) std::cout << terminal_.paint("] [", Terminal::Color::Cyan);
                std::cout << terminal_.paint("P" + std::to_string(segment.readyQueue[i]), Terminal::Color::Cyan);
            }
            std::cout << terminal_.paint("]\n", Terminal::Color::Cyan);
            line("PROCESS STATES:", Terminal::Color::Grey);
            for (const auto& process : segment.processes) {
                const auto stateColor = process.state == scheduler::ProcessState::Running ? Terminal::Color::Green :
                    process.state == scheduler::ProcessState::Ready ? Terminal::Color::Cyan :
                    process.state == scheduler::ProcessState::Completed ? Terminal::Color::Grey : Terminal::Color::Yellow;
                line("  P" + std::to_string(process.pid) + " " + stateName(process.state) +
                     " // remaining=" + std::to_string(process.remainingBurst), stateColor);
            }
            const std::string cell = std::to_string(segment.start) + " -- " +
                (segment.idle ? "TIMELINE GAP // CPU IDLE" : "P" + std::to_string(segment.pid)) + " -- " + std::to_string(segment.end);
            gantt.push_back(cell);
            terminal_.write("TIMELINE: ", Terminal::Color::Blue);
            for (std::size_t i = 0; i < gantt.size(); ++i) {
                if (i) terminal_.write(" | ", Terminal::Color::Grey);
                terminal_.revealGantt(gantt[i], 0);
            }
            std::cout << '\n';
            terminal_.delay(Terminal::Timing::SegmentStep);
        }
        while (eventCursor < result.events.size()) {
            const auto& event = result.events[eventCursor++];
            line("[T=" + std::to_string(event.timestamp) + "] P" + std::to_string(event.pid) + " " + eventMessage(event.type, result.policy),
                 event.type == scheduler::EventType::Completion ? Terminal::Color::Grey : Terminal::Color::Green, Terminal::Timing::EventHighlight);
        }
        showSchedule(result);
        saveRun(result);
    }
    void showComparison(const scheduler::SchedulingResult& fcfs, const scheduler::SchedulingResult& rr,
                        const scheduler::SchedulingResult& priority) const {
        line("\nDOOMSDAY // MULTIVERSE COMPARISON", Terminal::Color::Green);
        line("Policy                     Avg TAT   Avg WT   CPU %   Throughput", Terminal::Color::Cyan);
        const scheduler::SchedulingResult* rows[] = {&fcfs, &rr, &priority};
        for (const auto* r : rows) {
            if (!r->valid) { line(std::string(policyName(r->policy)) + " // " + r->error, Terminal::Color::Red); continue; }
            std::ostringstream row;
            row << std::left << std::setw(27) << policyName(r->policy) << std::right << std::fixed << std::setprecision(2)
                << std::setw(8) << r->averageTurnaround << std::setw(9) << r->averageWaiting
                << std::setw(8) << r->cpuUtilization << std::setw(13) << r->throughput;
            line(row.str(), Terminal::Color::White, Terminal::Timing::ReportReveal);
        }
        for (const auto* r : rows) {
            if (!r->valid) continue;
            std::ostringstream timeline;
            timeline << policyName(r->policy) << ": ";
            if (r->timeline.empty()) timeline << "(no segments)";
            else {
                timeline << r->timeline.front().start;
                for (const auto& segment : r->timeline)
                    timeline << " -- " << (segment.idle ? "IDLE" : "P" + std::to_string(segment.pid)) << " -- " << segment.end;
            }
            line(timeline.str(), Terminal::Color::Cyan, Terminal::Timing::ReportReveal);
        }
    }
    void showSchedule(const scheduler::SchedulingResult& result) const {
        line("\nEXECUTION REPORT // " + std::string(policyName(result.policy)), Terminal::Color::Green);
        if (!result.valid) { line("Cannot schedule workload: " + result.error, Terminal::Color::Red); return; }
        terminal_.write("FINAL GANTT: ", Terminal::Color::Blue);
        if (result.timeline.empty()) terminal_.write("(no processes)", Terminal::Color::Grey);
        else {
            std::cout << result.timeline.front().start;
            for (const auto& segment : result.timeline)
                std::cout << terminal_.paint(" -- ", Terminal::Color::Grey)
                          << terminal_.paint(segment.idle ? "IDLE" : "P" + std::to_string(segment.pid),
                              segment.idle ? Terminal::Color::Yellow : Terminal::Color::Cyan)
                          << terminal_.paint(" -- " + std::to_string(segment.end), Terminal::Color::Blue);
        }
        std::cout << '\n';
        line("PID  AT  BT  Priority  CT  TAT  WT", Terminal::Color::Cyan, Terminal::Timing::ReportReveal);
        for (const auto& m : result.metrics)
            line(std::to_string(m.process.pid) + "    " + std::to_string(m.process.arrivalTime) + "   " +
                 std::to_string(m.process.burstTime) + "   " + std::to_string(m.process.priority) + "         " +
                 std::to_string(m.completionTime) + "   " + std::to_string(m.turnaroundTime) + "    " +
                 std::to_string(m.waitingTime), Terminal::Color::White, Terminal::Timing::ReportReveal);
        if (result.metrics.empty()) line("No process metrics available.", Terminal::Color::Grey);
        else {
            terminal_.delay(Terminal::Timing::ReportPause);
            std::ostringstream summary;
            summary << std::fixed << std::setprecision(2)
                    << "Average TAT: " << result.averageTurnaround << " | Average WT: " << result.averageWaiting
                    << " | CPU utilization: " << result.cpuUtilization << "% | Throughput: " << result.throughput << " processes/unit";
            line(summary.str(), Terminal::Color::Green, Terminal::Timing::ReportReveal);
        }
    }
    void schedulingMenu() {
        while (running_) {
            line("\n+----------------------------------------------------------+", Terminal::Color::Green);
            line("| DOOMSDAY // TEMPORAL SIMULATION                         |", Terminal::Color::Green);
            line("+----------------------------------------------------------+", Terminal::Color::Green);
            revealMenuLine("[1]", "FCFS", "First-Come, First-Served // CHRONOLOGICAL PROTOCOL");
            revealMenuLine("[2]", "Round Robin", "TEMPORAL ROTATION");
            revealMenuLine("[3]", "Priority Scheduling", "THREAT ASSESSMENT PROTOCOL");
            revealMenuLine("[4]", "Compare policies", "Multiverse Comparison");
            revealMenuLine("[5]", "Replay latest run", "Timeline Archives");
            revealMenuLine("[6]", "Load sample workload");
            revealMenuLine("[0]", "Back");
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; } if (!c) return;
            if (c == 5) { replayLatest(); continue; }
            if (c == 6) {
                processes_ = {{1, 0, 5, 2}, {2, 1, 3, 1}, {3, 6, 2, 3}};
                line("Sample workload loaded: P1(AT=0,BT=5,PR=2), P2(AT=1,BT=3,PR=1), P3(AT=6,BT=2,PR=3).", Terminal::Color::Green);
                continue;
            }
            int quantum = 1;
            if (c == 2 || c == 4) { if (!readInt("Round Robin quantum (> 0): ", quantum)) { running_ = false; return; } }
            if (c < 1 || c > 4) { std::cout << "Choose a listed option.\n"; continue; }
            if (c != 4) {
                int mode = terminal_.animation() == Terminal::Animation::Full ? 1 :
                           terminal_.animation() == Terminal::Animation::Reduced ? 2 : 3;
                if (!readInt("Animation: 1. Full  2. Reduced  3. Off (current " + std::to_string(mode) + "): ", mode)) {
                    running_ = false; return;
                }
                if (mode == 1) terminal_.setAnimation(Terminal::Animation::Full);
                else if (mode == 2) terminal_.setAnimation(Terminal::Animation::Reduced);
                else if (mode == 3) terminal_.setAnimation(Terminal::Animation::Off);
                else { line("Choose 1, 2, or 3.", Terminal::Color::Red); continue; }
            }
            if (processes_.empty()) std::cout << "No processes are registered; showing an empty workload.\n";
            if (c == 1) showLive(scheduler::Scheduler::fcfs(processes_), processes_);
            else if (c == 2) showLive(scheduler::Scheduler::roundRobin(processes_, quantum), processes_);
            else if (c == 3) showLive(scheduler::Scheduler::priority(processes_), processes_);
            else {
                // Every policy receives the same unchanged input collection.
                const auto fcfs = scheduler::Scheduler::fcfs(processes_);
                const auto rr = scheduler::Scheduler::roundRobin(processes_, quantum);
                const auto priority = scheduler::Scheduler::priority(processes_);
                showComparison(fcfs, rr, priority);
                saveRun(fcfs); saveRun(rr); saveRun(priority);
            }
        }
    }

    void comparisonMenu() {
        int quantum = 1;
        if (!readInt("Round Robin quantum (> 0): ", quantum)) { running_ = false; return; }
        const auto fcfs = scheduler::Scheduler::fcfs(processes_);
        const auto rr = scheduler::Scheduler::roundRobin(processes_, quantum);
        const auto priority = scheduler::Scheduler::priority(processes_);
        showComparison(fcfs, rr, priority);
        saveRun(fcfs); saveRun(rr); saveRun(priority);
    }

    void configurationMenu() {
        for (;;) {
            const int current = terminal_.animation() == Terminal::Animation::Full ? 1 :
                terminal_.animation() == Terminal::Animation::Reduced ? 2 : 3;
            line("\nDOOMSDAY // SYSTEM CONFIGURATION", Terminal::Color::Green);
            line("1. Full animation  2. Reduced animation  3. Off", Terminal::Color::Cyan);
            line(std::string("Animation mode: ") + (current == 1 ? "Full" : current == 2 ? "Reduced" : "Off"), Terminal::Color::Grey);
            line(std::string("ANSI color: ") + (terminal_.ansiEnabled() ? "enabled" : "plain-text fallback"), Terminal::Color::Grey);
            line("4. Toggle ANSI color when terminal supports it  0. Back", Terminal::Color::Yellow);
            int choice; if (!readInt("Choice: ", choice)) { running_ = false; return; }
            if (choice == 0) return;
            if (choice == 1) terminal_.setAnimation(Terminal::Animation::Full);
            else if (choice == 2) terminal_.setAnimation(Terminal::Animation::Reduced);
            else if (choice == 3) terminal_.setAnimation(Terminal::Animation::Off);
            else if (choice == 4 && terminal_.ansiEnabled()) terminal_.setAnsi(false);
            else if (choice == 4) line("ANSI is unavailable in this terminal; plain-text output remains active.", Terminal::Color::Yellow);
            else line("Choose a listed option.", Terminal::Color::Red);
        }
    }

    void processMenu() {
        while (running_) {
            line("\nDOOMSDAY // PROCESS REGISTRY", Terminal::Color::Green);
            line("Process Management // add, display, search, update, delete", Terminal::Color::Cyan);
            line("1. Add  2. Display all  3. Search PID  4. Update  5. Delete  0. Back", Terminal::Color::White, Terminal::Timing::MenuRow);
            int choice; if (!readInt("Choice: ", choice)) { running_ = false; return; }
            if (choice == 0) return;
            if (choice == 1) {
                scheduler::Process p;
                if (!readInt("PID (unique integer): ", p.pid) || !readInt("Arrival time (>= 0): ", p.arrivalTime) || !readInt("Burst time (> 0): ", p.burstTime) || !readInt("Priority (smaller is higher): ", p.priority)) { running_ = false; return; }
                bool duplicate = false; for (const auto& old : processes_) if (old.pid == p.pid) duplicate = true;
                if (duplicate) line("That PID already exists.", Terminal::Color::Red);
                else if (p.arrivalTime < 0 || p.burstTime <= 0) line("Arrival must be non-negative and burst must be positive.", Terminal::Color::Red);
                else { processes_.push_back(p); line("PROCESS REGISTERED", Terminal::Color::Green); }
            } else if (choice == 2) {
                if (processes_.empty()) std::cout << "No processes recorded.\n";
                for (const auto& p : processes_) { p.display(); std::cout << '\n'; }
            } else if (choice == 3 || choice == 4 || choice == 5) {
                int pid; if (!readInt("PID: ", pid)) { running_ = false; return; }
                std::size_t at = 0; while (at < processes_.size() && processes_[at].pid != pid) ++at;
                if (at == processes_.size()) { line("PROCESS NOT FOUND", Terminal::Color::Yellow); continue; }
                if (choice == 3) { processes_[at].display(); std::cout << '\n'; }
                else if (choice == 5) { processes_.erase(processes_.begin() + static_cast<std::ptrdiff_t>(at)); line("PROCESS REMOVED FROM REGISTRY", Terminal::Color::Green); }
                else {
                    int newPid, arrival, burst, priority;
                    if (!readInt("New PID (unique integer): ", newPid) || !readInt("New arrival time (>= 0): ", arrival) || !readInt("New burst time (> 0): ", burst) || !readInt("New priority: ", priority)) { running_ = false; return; }
                    bool duplicate = false;
                    for (std::size_t i = 0; i < processes_.size(); ++i) if (i != at && processes_[i].pid == newPid) duplicate = true;
                    if (duplicate) line("That PID already exists; process unchanged.", Terminal::Color::Red);
                    else if (arrival < 0 || burst <= 0) line("Invalid values; process unchanged.", Terminal::Color::Red);
                    else { processes_[at].pid = newPid; processes_[at].arrivalTime = arrival; processes_[at].burstTime = burst; processes_[at].priority = priority; line("PROCESS RECORD UPDATED", Terminal::Color::Green); }
                }
            } else std::cout << "Choose a listed option.\n";
        }
    }

    template <typename List>
    void listMenu(List& list, bool doubly) {
        while (running_) {
            line(doubly ? "\nDLL // BIDIRECTIONAL PROCESS CHAIN" : "\nSLL // PROCESS CHAIN INSPECTOR", Terminal::Color::Green);
            line("Linked List // indices are zero-based", Terminal::Color::Cyan);
            std::cout << "1. Insert beginning  2. Insert end  3. Insert at index\n4. Delete beginning  5. Delete end  6. Delete at index\n7. Search  8. Update  9. Traverse  10. Reverse  11. Empty?  12. Size";
            if (doubly) std::cout << "  13. Backward traversal";
            std::cout << "  0. Back\n";
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            if (!c) return;
            int v; std::size_t i;
            switch (c) {
                case 1: if (!readForStructure(v)) return; list.insertBeginning(v); line("Inserted at beginning.", Terminal::Color::Green); break;
                case 2: if (!readForStructure(v)) return; list.insertEnd(v); line("Inserted at end.", Terminal::Color::Green); break;
                case 3: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max() || !readForStructure(v)) { if (!running_) return; line("Invalid index.", Terminal::Color::Red); break; } line(list.insertAt(i, v) ? "Inserted." : "Index out of range.", list.size() > i ? Terminal::Color::Green : Terminal::Color::Red); break;
                case 4: showResult(list.deleteBeginning(), "Deleted"); break;
                case 5: showResult(list.deleteEnd(), "Deleted"); break;
                case 6: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max()) { std::cout << "Invalid index.\n"; break; } showResult(list.deleteAt(i), "Deleted"); break;
                case 7: if (!readForStructure(v)) return; { auto found = list.search(v); if (found) line("Found at index " + std::to_string(*found) + ".", Terminal::Color::Green); else line("Value not found.", Terminal::Color::Yellow); } break;
                case 8: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max() || !readForStructure(v)) { if (!running_) return; line("Invalid index.", Terminal::Color::Red); break; } line(list.update(i, v) ? "Updated." : "Index out of range.", i < list.size() ? Terminal::Color::Green : Terminal::Color::Red); break;
                case 9: displayList(list, false); break;
                case 10: list.reverse(); line("List reversed.", Terminal::Color::Green); break;
                case 11: std::cout << (list.isEmpty() ? "List is empty.\n" : "List is not empty.\n"); break;
                case 12: std::cout << "Size: " << list.size() << '\n'; break;
                case 13: if (doubly) displayList(list, true); else std::cout << "Choose a listed option.\n"; break;
                default: std::cout << "Choose a listed option.\n";
            }
        }
    }

    template <typename Stack>
    void stackMenu(Stack& stack) {
        while (running_) {
            line("\nSTACK // TEMPORAL EVENT STACK", Terminal::Color::Green);
            line("Stack // push, pop, peek, display, empty/full checks", Terminal::Color::Cyan);
            line("1. Push  2. Pop  3. Peek  4. Display  5. Empty?  6. Full?  7. Size  0. Back", Terminal::Color::White, Terminal::Timing::MenuRow);
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            int v;
            switch (c) {
                case 0: return;
                case 1: if (!readForStructure(v)) return; { const bool ok = stack.push(v); line(ok ? "Pushed." : "Stack is full; value not added.", ok ? Terminal::Color::Green : Terminal::Color::Yellow); } break;
                case 2: showResult(stack.pop(), "Popped"); break;
                case 3: showResult(stack.peek(), "Top value is"); break;
                case 4: stack.display(); break;
                case 5: std::cout << (stack.isEmpty() ? "Stack is empty.\n" : "Stack is not empty.\n"); break;
                case 6: std::cout << (stack.isFull() ? "Stack is full.\n" : "Stack is not full.\n"); break;
                case 7: std::cout << "Size: " << stack.size() << '\n'; break;
                default: std::cout << "Choose a listed option.\n";
            }
        }
    }

    template <typename Queue>
    void queueMenu(Queue& queue) {
        while (running_) {
            line("\nCIRCULAR QUEUE // ROUND-ROBIN ROTATION MONITOR", Terminal::Color::Green);
            line("Queue // enqueue, dequeue, front, display, empty/full checks", Terminal::Color::Cyan);
            line("1. Enqueue  2. Dequeue  3. Front  4. Display  5. Empty?  6. Full?  7. Size  0. Back", Terminal::Color::White, Terminal::Timing::MenuRow);
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            int v;
            switch (c) {
                case 0: return;
                case 1: if (!readForStructure(v)) return; { const bool ok = queue.enqueue(v); line(ok ? "Enqueued." : "Queue is full; value not added.", ok ? Terminal::Color::Green : Terminal::Color::Yellow); } break;
                case 2: showResult(queue.dequeue(), "Dequeued"); break;
                case 3: showResult(queue.front(), "Front value is"); break;
                case 4: queue.display(); break;
                case 5: std::cout << (queue.isEmpty() ? "Queue is empty.\n" : "Queue is not empty.\n"); break;
                case 6: std::cout << (queue.isFull() ? "Queue is full.\n" : "Queue is not full.\n"); break;
                case 7: std::cout << "Size: " << queue.size() << '\n'; break;
                default: std::cout << "Choose a listed option.\n";
            }
        }
    }

public:
    void run() {
        dsa::SLL sll; dsa::DLL dll; dsa::StackArray stackArray; dsa::StackLinked stackLinked;
        dsa::QueueArray queueArray; dsa::QueueLinked queueLinked;
        dsa::HashTable hashTable;
        storage::SequentialProcessFile processFile;
        bootSequence();
        while (running_) {
            line("\n+----------------------------------------------------------+", Terminal::Color::Green);
            line("# DOOMSDAY // TEMPORAL CORE", Terminal::Color::Green);
            line("CPU Process & Task Scheduling Simulator", Terminal::Color::Cyan);
            line("Every process has a timeline. Every decision changes the outcome.", Terminal::Color::Grey);
            line("+----------------------------------------------------------+", Terminal::Color::Green);
            revealMenuLine("[01]", "PROCESS REGISTRY", "Process Management");
            revealMenuLine("[02]", "TEMPORAL SIMULATION", "Scheduling Lab");
            revealMenuLine("[03]", "DSA TRAINING CHAMBER", "Data Structures and Algorithms");
            revealMenuLine("[04]", "TIMELINE ARCHIVES", "Saved events and replay");
            revealMenuLine("[05]", "MULTIVERSE COMPARISON", "Compare scheduling policies");
            revealMenuLine("[06]", "SYSTEM CONFIGURATION", "Animation and color settings");
            revealMenuLine("[00]", "SHUT DOWN CORE", "Exit");
            int c; if (!readInt("Choice: ", c)) break;
            if (c >= 0 && c <= 6) {
                const char* selected[] = {"SHUT DOWN CORE", "PROCESS REGISTRY", "TEMPORAL SIMULATION",
                    "DSA TRAINING CHAMBER", "TIMELINE ARCHIVES", "MULTIVERSE COMPARISON", "SYSTEM CONFIGURATION"};
                line("SELECTED OPTION // " + std::string(selected[c]), Terminal::Color::Green);
            }
            switch (c) {
                case 0: running_ = false; break;
                case 1: processMenu(); break;
                case 2: schedulingMenu(); break;
                case 3: {
                    bool inLab = true;
                    while (running_ && inLab) {
                        line("\nDOOMSDAY // DSA TRAINING CHAMBER", Terminal::Color::Green);
                        revealMenuLine("[1]", "SLL", "Singly Linked List // Process Chain Inspector");
                        revealMenuLine("[2]", "DLL", "Doubly Linked List // Bidirectional Process Chain");
                        revealMenuLine("[3]", "Array Stack", "Stack Using Array");
                        revealMenuLine("[4]", "Linked Stack", "Stack Using Linked List");
                        revealMenuLine("[5]", "Circular Queue", "Queue Using Circular Array");
                        revealMenuLine("[6]", "Linked Queue", "Queue Using Linked List");
                        revealMenuLine("[7]", "Search and Sort", "Linear/binary search, bubble/insertion/quick sort");
                        revealMenuLine("[8]", "Hash Table", "PID Lookup Index");
                        revealMenuLine("[9]", "Sequential File", "Process Record Archive");
                        revealMenuLine("[0]", "Back");
                        int d; if (!readInt("Choice: ", d)) { running_ = false; break; }
                        switch (d) {
                            case 0: inLab = false; break;
                            case 1: listMenu(sll, false); break;
                            case 2: listMenu(dll, true); break;
                            case 3: stackMenu(stackArray); break;
                            case 4: stackMenu(stackLinked); break;
                            case 5: queueMenu(queueArray); break;
                            case 6: queueMenu(queueLinked); break;
                            case 7: algorithmsMenu(); break;
                            case 8: hashMenu(hashTable); break;
                            case 9: fileMenu(processFile); break;
                            default: std::cout << "Choose a listed option.\n";
                        }
                    }
                    break;
                }
                case 4: replayLatest(); break;
                case 5: comparisonMenu(); break;
                case 6: configurationMenu(); break;
                default: line("Choose a listed option.", Terminal::Color::Red);
            }
        }
        line("CORE SHUTDOWN // Goodbye.", Terminal::Color::Green);
    }
};

}  // namespace ui
