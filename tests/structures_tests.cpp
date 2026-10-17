#include "../dsa/DLL.h"
#include "../dsa/QueueArray.h"
#include "../dsa/QueueLinked.h"
#include "../dsa/SLL.h"
#include "../dsa/StackArray.h"
#include "../dsa/StackLinked.h"

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
    std::cout << "All Day 1 structure checks passed.\n";
}
