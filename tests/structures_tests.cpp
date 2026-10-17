#include "../dsa/DLL.h"
#include "../dsa/QueueArray.h"
#include "../dsa/QueueLinked.h"
#include "../dsa/SLL.h"
#include "../dsa/StackArray.h"
#include "../dsa/StackLinked.h"
#include "../dsa/SearchSort.h"
#include "../dsa/HashTable.h"
#include "../scheduler/Scheduler.h"
#include "../storage/SequentialProcessFile.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    {
        dsa::SLL list;
        assert(list.isEmpty() && !list.deleteBeginning() && !list.deleteEnd());
        list.insertEnd(2); list.insertBeginning(1); list.insertAt(2, 3); list.insertAt(2, 3);
        assert(list.size() == 4 && list.search(3) == 2 && !list.search(9));
        assert(!list.update(4, 0) && list.update(3, 4));
        list.reverse(); assert(list.search(1) == 3);
        dsa::SLL copied(list); copied.deleteBeginning(); assert(copied.size() == 3 && list.size() == 4);
        assert(list.deleteAt(1) == 3 && list.deleteEnd() == 1);
    }
    {
        dsa::DLL list;
        assert(!list.deleteEnd());
        list.insertEnd(1); list.insertEnd(3); list.insertAt(1, 2);
        assert(list.search(2) == 1 && !list.update(3, 4));
        list.reverse(); assert(list.search(1) == 2);
        list.deleteBeginning(); list.deleteEnd(); assert(list.size() == 1);
        list.deleteEnd(); assert(list.isEmpty()); list.insertEnd(8); list.deleteBeginning(); assert(list.isEmpty());
    }
    {
        dsa::StackArray stack(2);
        assert(!stack.pop() && stack.push(1) && stack.push(2) && !stack.push(3));
        assert(stack.peek() == 2 && stack.pop() == 2 && stack.pop() == 1 && !stack.pop());
        dsa::StackLinked linked;
        assert(linked.isEmpty() && !linked.pop()); linked.push(5); linked.push(6);
        assert(linked.peek() == 6 && linked.pop() == 6 && linked.pop() == 5 && linked.isEmpty() && !linked.isFull());
    }
    {
        dsa::QueueArray queue(3);
        assert(!queue.dequeue() && queue.enqueue(1) && queue.enqueue(2) && queue.enqueue(3) && !queue.enqueue(4));
        assert(queue.dequeue() == 1 && queue.dequeue() == 2 && queue.enqueue(4) && queue.enqueue(5));
        assert((queue.values() == std::vector<int>{3, 4, 5})); // FIFO snapshot remains correct across wraparound
        assert(queue.front() == 3 && queue.dequeue() == 3 && queue.dequeue() == 4 && queue.dequeue() == 5);
        assert(queue.isEmpty() && !queue.front() && queue.enqueue(6) && queue.dequeue() == 6);
        dsa::QueueArray zero(0); assert(zero.isFull() && !zero.enqueue(1));
        dsa::QueueLinked linked; assert(!linked.dequeue()); linked.enqueue(7); assert(linked.front() == 7);
        assert(linked.dequeue() == 7 && linked.isEmpty()); linked.enqueue(8); assert(linked.dequeue() == 8 && !linked.isFull());
    }
    {
        const std::vector<int> values{1, 2, 2, 5, 9};
        assert(dsa::linearSearch(values, 2) == 1 && dsa::linearSearch(values, 7) == -1);
        assert(dsa::binarySearch(values, 5) == 3 && dsa::binarySearch(values, 7) == -1);
        assert(dsa::linearSearch({}, 1) == -1 && dsa::binarySearch({}, 1) == -1);
        const std::vector<int> expected{1, 2, 2, 5, 9};
        std::vector<int> bubble{9, 2, 5, 2, 1}; dsa::bubbleSort(bubble); assert(bubble == expected);
        std::vector<int> insertion{9, 2, 5, 2, 1}; dsa::insertionSort(insertion); assert(insertion == expected);
        std::vector<int> quick{9, 2, 5, 2, 1}; dsa::quickSort(quick); assert(quick == expected);
        std::vector<int> empty; dsa::bubbleSort(empty); dsa::insertionSort(empty); dsa::quickSort(empty); assert(empty.empty());
        std::vector<int> single{4}; dsa::bubbleSort(single); dsa::insertionSort(single); dsa::quickSort(single); assert(single == std::vector<int>{4});
    }
    {
        dsa::HashTable table(5);
        assert(table.insert(1) && table.insert(6) && table.insert(11));
        assert(!table.insert(6) && table.contains(11) && !table.contains(16));
        assert(table.erase(6) && !table.contains(6) && table.contains(11));
        assert(table.insert(16) && table.size() == 3);
        assert(table.insert(2) && table.insert(3) && !table.insert(4));
        assert(table.erase(1) && table.insert(4) && table.contains(4));
        dsa::HashTable noSlots(0); assert(!noSlots.insert(1) && !noSlots.contains(1) && !noSlots.erase(1));
    }
    {
        const std::string path = "day5_process_records_test.csv";
        std::remove(path.c_str()); std::remove((path + ".tmp").c_str()); std::remove((path + ".bak").c_str());
        storage::SequentialProcessFile file(path);
        std::vector<scheduler::Process> records;
        assert(file.display(records) && records.empty());
        assert(file.append({31, 0, 4, 2}) && file.append({32, 3, 2, 1}));
        assert(!file.append({31, 8, 1, 0}) && !file.append({33, -1, 1, 0}) && !file.append({34, 0, 0, 0}));
        assert(file.display(records) && records.size() == 2 && records[1].pid == 32);
        scheduler::Process found; assert(file.find(32, found) && found.burstTime == 2 && !file.find(99, found));
        bool erased = false; assert(file.erase(31, erased) && erased);
        assert(file.display(records) && records.size() == 1 && records[0].pid == 32);
        assert(file.erase(99, erased) && !erased);
        { std::ofstream malformed(path, std::ios::trunc); malformed << "not,a,valid,record,extra\n"; }
        assert(!file.display(records) && !file.append({35, 0, 1, 0}));
        { std::ifstream malformed(path); std::string line; assert(std::getline(malformed, line) && line == "not,a,valid,record,extra"); }
        std::remove(path.c_str()); std::remove((path + ".tmp").c_str()); std::remove((path + ".bak").c_str());
    }
    {
        const std::vector<scheduler::Process> workload{{1, 0, 5, 2}, {2, 1, 3, 1}};
        const auto fcfs = scheduler::Scheduler::fcfs(workload);
        assert(fcfs.valid && fcfs.metrics.size() == 2);
        assert(fcfs.metrics[0].completionTime == 5 && fcfs.metrics[0].turnaroundTime == 5 && fcfs.metrics[0].waitingTime == 0);
        assert(fcfs.metrics[1].completionTime == 8 && fcfs.metrics[1].turnaroundTime == 7 && fcfs.metrics[1].waitingTime == 4);
        const auto rr = scheduler::Scheduler::roundRobin(workload, 2);
        assert(rr.valid && rr.timeline.size() == 5);
        assert(rr.timeline[0].pid == 1 && rr.timeline[0].readyQueue.empty());
        assert(rr.timeline[1].pid == 2 && rr.timeline[1].readyQueue.size() == 1 && rr.timeline[1].readyQueue[0] == 1);
        assert(rr.timeline[2].pid == 1 && rr.timeline[2].readyQueue.size() == 1 && rr.timeline[2].readyQueue[0] == 2);
        assert(rr.timeline[1].processes.size() == 2 && rr.timeline[1].processes[0].state == scheduler::ProcessState::Ready &&
               rr.timeline[1].processes[1].state == scheduler::ProcessState::Running);
        assert(rr.metrics[0].completionTime == 8 && rr.metrics[0].turnaroundTime == 8 && rr.metrics[0].waitingTime == 3);
        assert(rr.metrics[1].completionTime == 7 && rr.metrics[1].turnaroundTime == 6 && rr.metrics[1].waitingTime == 3);
        assert(rr.averageWaiting == 3.0 && rr.averageTurnaround == 7.0 && rr.cpuUtilization == 100.0);
        bool sawArrival = false, sawDispatch = false, sawPreemption = false, sawCompletion = false;
        for (const auto& event : rr.events) {
            sawArrival = sawArrival || event.type == scheduler::EventType::Arrival;
            sawDispatch = sawDispatch || event.type == scheduler::EventType::Dispatch;
            sawPreemption = sawPreemption || event.type == scheduler::EventType::Preemption;
            sawCompletion = sawCompletion || event.type == scheduler::EventType::Completion;
        }
        assert(sawArrival && sawDispatch && sawPreemption && sawCompletion);
        const auto priority = scheduler::Scheduler::priority(workload);
        assert(priority.valid && priority.metrics[0].completionTime == 5 && priority.metrics[1].completionTime == 8);
        const auto idle = scheduler::Scheduler::fcfs({{9, 3, 2, 0}});
        assert(idle.valid && idle.timeline.size() == 2 && idle.timeline[0].idle && idle.elapsedTime == 5);
        assert(idle.timeline[0].processes.size() == 1 && idle.timeline[0].processes[0].state == scheduler::ProcessState::New);
        const auto priorityTie = scheduler::Scheduler::priority({{3, 0, 2, 2}, {8, 0, 1, 1}, {5, 0, 1, 1}});
        assert(priorityTie.valid && priorityTie.timeline[0].pid == 5 && priorityTie.timeline[1].pid == 8 && priorityTie.timeline[2].pid == 3);
        const auto boundaryArrival = scheduler::Scheduler::roundRobin({{1, 0, 4, 0}, {2, 2, 1, 0}}, 2);
        assert(boundaryArrival.valid && boundaryArrival.timeline.size() == 3);
        assert(boundaryArrival.timeline[0].pid == 1 && boundaryArrival.timeline[1].pid == 2 && boundaryArrival.timeline[2].pid == 1);
        assert(!scheduler::Scheduler::roundRobin(workload, 0).valid);
        assert(!scheduler::Scheduler::fcfs({{1, 0, 1, 0}, {1, 2, 1, 0}}).valid);
    }
    std::cout << "DSA, storage, and scheduling checks passed.\n";
}
