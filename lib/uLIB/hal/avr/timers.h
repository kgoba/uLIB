#pragma once
#include "Arduino.h"

class Timer1 {
public:
  const uint16_t TIMER_MAX = 65535;

  enum ClockSource {
    ClkStop   = 0,
    ClkDiv1   = 1,
    ClkDiv8   = 2,
    ClkDiv64  = 3,
    ClkDiv256 = 4,
    ClkDiv1024          = 5,
    ClkExternalFalling  = 6,
    ClkExternalRising   = 7
  };

  enum Interrupt {
    IntCapture,
    IntCompareA,
    IntCompareB,
    IntCompareC,
    IntOverflow
  };
  
  static void setMode(uint8_t mode) {
    TCCR1A = (mode & 3);
    TCCR1B = (TCCR1B & 0xE7) | ((mode  & 0x0C) << 1);
  }

  static void setClock(ClockSource clock) {
    TCCR1B = (TCCR1B & 0xF8) | (clock & 0x07);
  }

  static ClockSource findClock(uint32_t frequency) {
    if (F_CPU / frequency < TIMER_MAX) return ClkDiv1;
    if (F_CPU / frequency / 8 < TIMER_MAX) return ClkDiv8;
    if (F_CPU / frequency / 64 < TIMER_MAX) return ClkDiv64;
    if (F_CPU / frequency / 256 < TIMER_MAX) return ClkDiv256;
    if (F_CPU / frequency / 1024 < TIMER_MAX) return ClkDiv1024;
    return ClkStop;
  }

  static uint16_t getPrescaler(ClockSource clock) {
    switch (clock) {
      case ClkDiv1: return 1;
      case ClkDiv8: return 8;
      case ClkDiv64: return 64;
      case ClkDiv256: return 256;
      case ClkDiv1024: return 1024;
      default: return 0;
    }
  }

  static void setICR(uint16_t value) {
    ICR1 = value;
  }

  static void enableInterrupt(Interrupt interrupt);
  static void disableInterrupt(Interrupt interrupt);
};

class PeriodicTimer {
public:
  typedef void (* TimerCallback) (void);

  void setPeriodMillis(uint16_t milliseconds);
  void setPeriodMicros(uint16_t microseconds);

  void attach(TimerCallback callback);
};

