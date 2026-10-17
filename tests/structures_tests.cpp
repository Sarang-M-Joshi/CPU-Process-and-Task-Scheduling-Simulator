#include "../dsa/DLL.h"
#include "../dsa/QueueArray.h"
#include "../dsa/QueueLinked.h"
#include "../dsa/SLL.h"
#include "../dsa/StackArray.h"
#include "../dsa/StackLinked.h"
#include "../scheduler/Scheduler.h"

#include <cassert>
#include <iostream>

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
        assert(queue.front() == 3 && queue.dequeue() == 3 && queue.dequeue() == 4 && queue.dequeue() == 5);
        assert(queue.isEmpty() && !queue.front() && queue.enqueue(6) && queue.dequeue() == 6);
        dsa::QueueArray zero(0); assert(zero.isFull() && !zero.enqueue(1));
        dsa::QueueLinked linked; assert(!linked.dequeue()); linked.enqueue(7); assert(linked.front() == 7);
        assert(linked.dequeue() == 7 && linked.isEmpty()); linked.enqueue(8); assert(linked.dequeue() == 8 && !linked.isFull());
    }
    {
        const std::vector<scheduler::Process> workload{{1, 0, 5, 2}, {2, 1, 3, 1}};
        const auto fcfs = scheduler::Scheduler::fcfs(workload);
        assert(fcfs.valid && fcfs.metrics.size() == 2);
        assert(fcfs.metrics[0].completionTime == 5 && fcfs.metrics[0].turnaroundTime == 5 && fcfs.metrics[0].waitingTime == 0);
        assert(fcfs.metrics[1].completionTime == 8 && fcfs.metrics[1].turnaroundTime == 7 && fcfs.metrics[1].waitingTime == 4);
        const auto rr = scheduler::Scheduler::roundRobin(workload, 2);
        assert(rr.valid && rr.timeline.size() == 5);
        assert(rr.metrics[0].completionTime == 8 && rr.metrics[0].turnaroundTime == 8 && rr.metrics[0].waitingTime == 3);
        assert(rr.metrics[1].completionTime == 7 && rr.metrics[1].turnaroundTime == 6 && rr.metrics[1].waitingTime == 3);
        assert(rr.averageWaiting == 3.0 && rr.averageTurnaround == 7.0 && rr.cpuUtilization == 100.0);
        const auto priority = scheduler::Scheduler::priority(workload);
        assert(priority.valid && priority.metrics[0].completionTime == 5 && priority.metrics[1].completionTime == 8);
        const auto idle = scheduler::Scheduler::fcfs({{9, 3, 2, 0}});
        assert(idle.valid && idle.timeline.size() == 2 && idle.timeline[0].idle && idle.elapsedTime == 5);
        const auto priorityTie = scheduler::Scheduler::priority({{3, 0, 2, 2}, {8, 0, 1, 1}, {5, 0, 1, 1}});
        assert(priorityTie.valid && priorityTie.timeline[0].pid == 5 && priorityTie.timeline[1].pid == 8 && priorityTie.timeline[2].pid == 3);
        const auto boundaryArrival = scheduler::Scheduler::roundRobin({{1, 0, 4, 0}, {2, 2, 1, 0}}, 2);
        assert(boundaryArrival.valid && boundaryArrival.timeline.size() == 3);
        assert(boundaryArrival.timeline[0].pid == 1 && boundaryArrival.timeline[1].pid == 2 && boundaryArrival.timeline[2].pid == 1);
        assert(!scheduler::Scheduler::roundRobin(workload, 0).valid);
        assert(!scheduler::Scheduler::fcfs({{1, 0, 1, 0}, {1, 2, 1, 0}}).valid);
    }
    std::cout << "Day 1 structures and Day 3 scheduling checks passed.\n";
}
