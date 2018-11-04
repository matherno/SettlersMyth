#pragma once
//
// Created by matt on 7/02/18.
//


class Timer
  {
private:
  long timeOut = 5000;
  long timer = 0;

public:
  void reset()
    {
    timer = timeOut;
    }

  void setTimeOut(long timeOut)
    {
    this->timeOut = timeOut;
    }

  bool incrementTimer(long time)
    {
    timer -= time;
    return timedOut();
    }

  bool timedOut() const
    {
    return timer <= 0;
    }
  };