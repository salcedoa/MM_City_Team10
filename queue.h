#include <LinkedList.h>
#include "cell.h"

// Define a queue using LinkedList to store maze elements (Cell pointer type)
class queue {
private:
  LinkedList<Cell*> queue;

public:
  queue() {}

  void push(Cell* item) {
    queue.add(item);
  }

  bool empty() {
    return queue.size() == 0;
  }

  void pop() {
    if (!empty()) {
      queue.remove(0);
    }
  }

  Cell* front() {
    if (!empty()) {
      return queue.get(0);
    } else {
      return nullptr; // Returning nullptr when the queue is empty
    }
  }
};
