#include <stdint.h>

template<typename T, uint32_t size>
class Queue {
public:
  //Queue() : count(0), head(0) {} 
  
  void clear() {
    _count = 0;
    _head = 0;
  }
  
  bool push(T item) {
    if (_count < size) {
      uint32_t tail = _head + _count;
      if (tail >= size) tail -= size;
      _buffer[tail] = item;
      _count++;      
      return true;
    }
    return false;
  }
  
  bool pop(T &item) {
    if (_count > 0) {
      item = _buffer[_head];
      _head++;
      if (_head == size) _head = 0;
      _count--;
      return true;
    }
    return false;
  }
  
  uint32_t count() {
    return _count;
  }
  
private:
  T         _buffer[size];
  uint32_t  _count;
  uint32_t  _head;
};

