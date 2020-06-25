/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2020 André Pedro
 *
 *  This file is part of rtmlib.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MONITOR_EVENT_H_
#define _MONITOR_EVENT_H_

#include <time.h>

#include "time_compat.h"

/**
 * Event data container that keeps a timestamp of the event.
 *
 * Timestamps are parameterized.
 *
 * @author André Pedro
 * @date
 */
template <typename T> class Event {
private:
  /** Data container for this Event. */
  T data;
  /** Creation time for this Event */
  timespan time;

public:
  typedef T data_t;

  /**
   * Instantiates a new event.
   *
   * Instantiates a new event, nothing is zeroed out.
   */
  Event();

  /**
   * Instantiates a new event with data and time parameters.
   *
   * Both  data and  time are instantiated using copy constructors.
   *
   * @param data a constant reference to the data being copied.
   * @param time constant reference to the time being copied.
   */
  Event(const T &data, const timespan &time);

  /**
   * Gets the event current data.
   *
   * @return a reference to this events Data.
   */
  T &getData();

  /**
   * Gets the event creation time.
   *
   * @return the event current time.
   */
  timespan getTime();

  /**
   * Gets the event creation time.
   *
   * @return the event current time.
   */
  const timespan &getTime() const;

  /**
   * Set event
   */
  void set(T &, timespan &);

  /**
   * Set time of the event
   */
  void setTime(timespan &);

  /**
   * Set data of the event
   */
  void setData(T &);

  /**
   * Checks if this object is null.
   *
   * An object is determined to be null if the current time is zeroed out,
   * that is if time.tv_sec is 0 and time_.tv_nsec is 0.
   *
   * @return whether this is a nullObject.
   */
  bool isNull() const;

  /**
   * Copies the Events data pointed by event to this object.
   *
   * @param event The volatile data to copy to this object.
   */
  Event<T> &operator=(const Event<T> *event);

  /**
   * Checks if this event is < than Event  event.
   *
   * @param event Another event.
   * @return whether this event is < than Event  event.
   */
  bool operator<(const Event &event) const;

  /**
   * Checks if this event is <= than Event  event.
   *
   * @param event Another event.
   * @return whether this event is <= than Event  event.
   */
  bool operator<=(const Event &event) const;

  /**
   * Checks if this event is > than Event  event.
   *
   * @param event Another event.
   * @return whether this event is > than Event  event.
   */
  bool operator>(const Event &event) const;

  /**
   * Checks if this event is >= than Event  event.
   *
   * @param event Another event.
   * @return whether this event is >= than Event  event.
   */
  bool operator>=(const Event &event) const;

  /**
   * Checks if this event is == to Event  event.
   *
   * @param event Another event.
   * @return whether this event is == to Event  event.
   */
  bool operator==(const Event &event) const;

  /**
   * Checks if this event is != than Event  event.
   *
   * @param event Another event.
   * @return whether this event is != than Event  event.
   */
  bool operator!=(const Event &event) const;

  /**
   * Checks if this event is < than timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is < than timespec  time.
   */
  bool operator<(const timespan &time) const;

  /**
   * Checks if this event is <= than timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is <= than timespec  time.
   */
  bool operator<=(const timespan &time) const;

  /**
   * Checks if this event is > than timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is > than timespec  time.
   */
  bool operator>(const timespan &time) const;

  /**
   * Checks if this event is >= than timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is >= than timespec  time.
   */
  bool operator>=(const timespan &time) const;

  /**
   * Checks if this event is == to timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is == to timespec  time.
   */
  bool operator==(const timespan &time) const;

  /**
   * Checks if this event is != than timespec  time.
   *
   * @param time a timespec as defined by time.h
   * @return whether this event is != than timespec  time.
   */
  bool operator!=(const timespan &time) const;

  /**
   * Checks if timespec time is < than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is < than Event  event.
   */
  template <typename D>
  friend bool operator<(const timespan &time, const Event<D> &event);

  /**
   * Checks if timespec time is < than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is < than Event  event.
   */
  template <typename D>
  friend bool operator<=(const timespan &time, const Event<D> &event);

  /**
   * Checks if timespec  time is <= than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is <= than Event  event.
   */
  template <typename D>
  friend bool operator>(const timespan &time, const Event<D> &event);

  /**
   * Checks if timespec  time is > than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is > than Event  event.
   */
  template <typename D>
  friend bool operator>=(const timespan &time, const Event<D> &event);

  /**
   * Checks if timespec  time is == than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is == than Event  event.
   */
  template <typename D>
  friend bool operator==(const timespan &time, const Event<D> &event);

  /**
   * Checks if timespec  time is != than Event  event.
   *
   * @param time a timespec as defined by time.h
   * @param event an event
   *
   * @return whether timespec  time is != than Event  event.
   */
  template <typename D>
  friend bool operator!=(const timespan &time, const Event<D> &event);
};

template <typename T> Event<T>::Event() : data(0), time(0) {}

template <typename T>
Event<T>::Event(const T &ddata, const timespan &ttime)
    : data(ddata), time(ttime) {}

template <typename T> T &Event<T>::getData() { return data; }

template <typename T> timespan Event<T>::getTime() { return time; }

template <typename T> void Event<T>::set(T &d, timespan &t) {
  time = t;
  data = d;
}

template <typename T> void Event<T>::setTime(timespan &t) { time = t; }

template <typename T> void Event<T>::setData(T &d) { data = d; }

template <typename T> const timespan &Event<T>::getTime() const { return time; }

template <typename T> bool Event<T>::isNull() const {
  // [TODO] this function is incorrect
  return time == 0;
}

template <typename T> Event<T> &Event<T>::operator=(const Event *event) {
  data = event->data;
  time = event->time;
  return *this;
}

template <typename T> bool Event<T>::operator<(Event const &event) const {
  return time < event.time;
}

template <typename T> bool Event<T>::operator<=(Event const &event) const {
  return time <= event.time;
}

template <typename T> bool Event<T>::operator>(Event const &event) const {
  return time > event.time;
}

template <typename T> bool Event<T>::operator>=(Event const &event) const {
  return time >= event.time;
}

template <typename T> bool Event<T>::operator==(Event<T> const &event) const {
  return time == event.time && data == event.data;
}

template <typename T> bool Event<T>::operator!=(Event<T> const &event) const {
  return time != event.time;
}

template <typename T> bool Event<T>::operator<(const timespan &ttime) const {
  return this->time < ttime;
}

template <typename T> bool Event<T>::operator<=(const timespan &ttime) const {
  return this->time <= ttime;
}

template <typename T> bool Event<T>::operator>(const timespan &ttime) const {
  return this->time > ttime;
}

template <typename T> bool Event<T>::operator>=(const timespan &ttime) const {
  return this->time >= ttime;
}

template <typename T> bool Event<T>::operator==(const timespan &ttime) const {
  return this->time == ttime;
}

template <typename T> bool Event<T>::operator!=(const timespan &ttime) const {
  return this->time != ttime;
}

template <typename T>
bool operator<(const timespan &ttime, const Event<T> &event) {
  return ttime < event.time;
}

template <typename T>
bool operator<=(const timespan &ttime, const Event<T> &event) {
  return ttime <= event.time;
}

template <typename T>
bool operator>(const timespan &ttime, const Event<T> &event) {
  return ttime > event.time;
}

template <typename T>
bool operator>=(const timespan &ttime, const Event<T> &event) {
  return ttime >= event.time;
}

template <typename T>
bool operator==(const timespan &ttime, const Event<T> &event) {
  return ttime == event.time;
}

template <typename T>
bool operator!=(const timespan &ttime, const Event<T> &event) {
  return ttime != event.time;
}

#endif //_MONITOR_EVENT_H_
