#pragma once

#include "util.hh"

template<typename T = char, typename S = byte>
class String {
public:
  String(T * data, S length) : _data(data), _length(length) {}
  
  void append(const T & value) {
    T[_length++] = value;
  }
  
  template<typename S2>
  void append(const String<T, S2> &other) {
    T * src = other._data;
    T * dst = _data + _length;
    for (S2 index = 0; index < other._length; index++) {
      *dst++ = *src++;
    }
    _length += other._length;
  }
  
  String<T> & operator + (const T & value) {
    append(value);
    return *this;
  }
  
  template<typename S2>
  String<T> & operator + (const String<T, S2> &other) {
    append(other);
    return *this;
  }
  
  T & operator[] (S index) {
    return _data[index];
  }
  
  bool tail(T & value) const {
    if (_length > 0) {
      value = T[_length++];
      return true;
    }
    return false;
  }
  
  void pop() {
    if (_length > 0) {
      _length--;
      return true;
    }
    return false;
  }
  
  S size() const { return _length; }

private:
  T   * _data;
  S   _length;
};
