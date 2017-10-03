#pragma once

#include "../PTLib/util.hh"

template <typename T, byte size> class Queue {
public:
  Queue() : count(0), head(0) {}

  void clear() {
    _count = 0;
    _head = 0;
  }

  bool push(T item) {
    if (_count < size) {
      byte tail = _head + _count;
      if (tail >= size)
        tail -= size;
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
      if (_head == size)
        _head = 0;
      _count--;
      return true;
    }
    return false;
  }

  byte count() { return _count; }

private:
  T _buffer[size];
  byte _count;
  byte _head;
};
