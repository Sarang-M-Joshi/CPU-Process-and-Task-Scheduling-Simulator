#pragma once

#include "../dsa/DLL.h"
#include "../dsa/QueueArray.h"
#include "../dsa/QueueLinked.h"
#include "../dsa/SLL.h"
#include "../dsa/StackArray.h"
#include "../dsa/StackLinked.h"
#include "../models/Process.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace ui {

class Menu {
    std::vector<scheduler::Process> processes_;
    bool running_{true};

    static bool readInt(const std::string& prompt, int& value) {
        for (;;) {
            std::cout << prompt;
            if (std::cin >> value) return true;
            if (std::cin.eof()) return false;
            std::cout << "Please enter a whole number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    static bool readIndex(const std::string& prompt, std::size_t& value) {
        int raw{};
        if (!readInt(prompt, raw)) return false;
        if (raw < 0) { std::cout << "Index must be non-negative.\n"; value = std::numeric_limits<std::size_t>::max(); }
        else value = static_cast<std::size_t>(raw);
        return true;
    }
    static void pauseLine() { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
    bool readForStructure(int& x) { if (!readInt("Value: ", x)) { running_ = false; return false; } return true; }
    bool readListIndex(std::size_t& i) { if (!readIndex("Index (zero-based): ", i)) { running_ = false; return false; } return true; }
    static void showResult(const dsa::Maybe<int>& result, const char* action) {
        if (result) std::cout << action << " value " << *result << ".\n";
        else std::cout << "Operation unavailable: the structure is empty or the index is invalid.\n";
    }
    static void displayList(dsa::SLL& list, bool) { list.display(); }
    static void displayList(dsa::DLL& list, bool backward) { if (backward) list.displayBackward(); else list.displayForward(); }

    void processMenu() {
        while (running_) {
            std::cout << "\nProcess Management\n1. Add  2. Display all  3. Search PID  4. Update  5. Delete  0. Back\n";
            int choice; if (!readInt("Choice: ", choice)) { running_ = false; return; }
            if (choice == 0) return;
            if (choice == 1) {
                scheduler::Process p;
                if (!readInt("PID (unique integer): ", p.pid) || !readInt("Arrival time (>= 0): ", p.arrivalTime) || !readInt("Burst time (> 0): ", p.burstTime) || !readInt("Priority (smaller is higher): ", p.priority)) { running_ = false; return; }
                bool duplicate = false; for (const auto& old : processes_) if (old.pid == p.pid) duplicate = true;
                if (duplicate) std::cout << "That PID already exists.\n";
                else if (p.arrivalTime < 0 || p.burstTime <= 0) std::cout << "Arrival must be non-negative and burst must be positive.\n";
                else { processes_.push_back(p); std::cout << "Process added.\n"; }
            } else if (choice == 2) {
                if (processes_.empty()) std::cout << "No processes recorded.\n";
                for (const auto& p : processes_) { p.display(); std::cout << '\n'; }
            } else if (choice == 3 || choice == 4 || choice == 5) {
                int pid; if (!readInt("PID: ", pid)) { running_ = false; return; }
                std::size_t at = 0; while (at < processes_.size() && processes_[at].pid != pid) ++at;
                if (at == processes_.size()) { std::cout << "PID not found.\n"; continue; }
                if (choice == 3) { processes_[at].display(); std::cout << '\n'; }
                else if (choice == 5) { processes_.erase(processes_.begin() + static_cast<std::ptrdiff_t>(at)); std::cout << "Process deleted.\n"; }
                else {
                    int newPid, arrival, burst, priority;
                    if (!readInt("New PID (unique integer): ", newPid) || !readInt("New arrival time (>= 0): ", arrival) || !readInt("New burst time (> 0): ", burst) || !readInt("New priority: ", priority)) { running_ = false; return; }
                    bool duplicate = false;
                    for (std::size_t i = 0; i < processes_.size(); ++i) if (i != at && processes_[i].pid == newPid) duplicate = true;
                    if (duplicate) std::cout << "That PID already exists; process unchanged.\n";
                    else if (arrival < 0 || burst <= 0) std::cout << "Invalid values; process unchanged.\n";
                    else { processes_[at].pid = newPid; processes_[at].arrivalTime = arrival; processes_[at].burstTime = burst; processes_[at].priority = priority; std::cout << "Process updated.\n"; }
                }
            } else std::cout << "Choose a listed option.\n";
        }
    }

    template <typename List>
    void listMenu(List& list, bool doubly) {
        while (running_) {
            std::cout << "\nLinked List (indices are zero-based)\n1. Insert beginning  2. Insert end  3. Insert at index\n4. Delete beginning  5. Delete end  6. Delete at index\n7. Search  8. Update  9. Traverse  10. Reverse  11. Empty?  12. Size";
            if (doubly) std::cout << "  13. Backward traversal";
            std::cout << "  0. Back\n";
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            if (!c) return;
            int v; std::size_t i;
            switch (c) {
                case 1: if (!readForStructure(v)) return; list.insertBeginning(v); break;
                case 2: if (!readForStructure(v)) return; list.insertEnd(v); break;
                case 3: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max() || !readForStructure(v)) { if (!running_) return; std::cout << "Invalid index.\n"; break; } std::cout << (list.insertAt(i, v) ? "Inserted.\n" : "Index out of range.\n"); break;
                case 4: showResult(list.deleteBeginning(), "Deleted"); break;
                case 5: showResult(list.deleteEnd(), "Deleted"); break;
                case 6: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max()) { std::cout << "Invalid index.\n"; break; } showResult(list.deleteAt(i), "Deleted"); break;
                case 7: if (!readForStructure(v)) return; { auto found = list.search(v); if (found) std::cout << "Found at index " << *found << ".\n"; else std::cout << "Value not found.\n"; } break;
                case 8: if (!readListIndex(i)) return; if (i == std::numeric_limits<std::size_t>::max() || !readForStructure(v)) { if (!running_) return; std::cout << "Invalid index.\n"; break; } std::cout << (list.update(i, v) ? "Updated.\n" : "Index out of range.\n"); break;
                case 9: displayList(list, false); break;
                case 10: list.reverse(); std::cout << "List reversed.\n"; break;
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
            std::cout << "\nStack\n1. Push  2. Pop  3. Peek  4. Display  5. Empty?  6. Full?  7. Size  0. Back\n";
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            int v;
            switch (c) {
                case 0: return;
                case 1: if (!readForStructure(v)) return; std::cout << (stack.push(v) ? "Pushed.\n" : "Stack is full; value not added.\n"); break;
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
            std::cout << "\nQueue\n1. Enqueue  2. Dequeue  3. Front  4. Display  5. Empty?  6. Full?  7. Size  0. Back\n";
            int c; if (!readInt("Choice: ", c)) { running_ = false; return; }
            int v;
            switch (c) {
                case 0: return;
                case 1: if (!readForStructure(v)) return; std::cout << (queue.enqueue(v) ? "Enqueued.\n" : "Queue is full; value not added.\n"); break;
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
        while (running_) {
            std::cout << "\n========================================\n CPU PROCESS AND TASK SCHEDULING SIMULATOR\n========================================\n"
                      << "1. Process Management\n2. Singly Linked List\n3. Doubly Linked List\n4. Stack Using Array\n5. Stack Using Linked List\n6. Queue Using Circular Array\n7. Queue Using Linked List\n0. Exit\n";
            int c; if (!readInt("Choice: ", c)) break;
            switch (c) {
                case 0: running_ = false; break;
                case 1: processMenu(); break;
                case 2: listMenu(sll, false); break;
                case 3: listMenu(dll, true); break;
                case 4: stackMenu(stackArray); break;
                case 5: stackMenu(stackLinked); break;
                case 6: queueMenu(queueArray); break;
                case 7: queueMenu(queueLinked); break;
                default: std::cout << "Choose a listed option.\n";
            }
        }
        std::cout << "Goodbye.\n";
    }
};

}  // namespace ui
