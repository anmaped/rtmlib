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

#ifndef _RMTLD3_PATTERN_H_
#define _RMTLD3_PATTERN_H_

#include "rmtld3.h"

template <typename B, size_t N> class RMTLD3_Pattern {

  B memory[N];

public:
  RMTLD3_Pattern();

  /**
   * Get value from memory
   */
  three_valued_type getValue(size_t, size_t);

  /**
   * Map t to memory index
   */
  size_t mapt(timespan &);
};

template <typename B, size_t N> RMTLD3_Pattern<B, N>::RMTLD3_Pattern() {}

template <typename B, size_t N>
three_valued_type RMTLD3_Pattern<B, N>::getValue(size_t i, size_t id) {
  three_valued_type value;
  memory[id].read(value, i);
  return value;
}

template <typename B, size_t N> size_t RMTLD3_Pattern<B, N>::mapt(timespan &t) {
  return (size_t)t;
}

#endif //_RMTLD3_PATTERN_H_
