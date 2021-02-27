#pragma once

#include <iterator>

// todo: this has issues and isnt working
// may be used to parrallelize a std::for_each for every scanline

namespace ranges {

  class rangeIterator{
  private:
    size_t value;
  public:
    rangeIterator(size_t val) : value(val) {}

    bool operator!=(rangeIterator const& other) const {
      return value != other.value;
    }

    int const& operator*() const {
      return value;
    }

    rangeIterator& operator++(){
      ++value;
      return *this;
    }
    rangeIterator& operator++(int in) {
      value += in;
      return *this;
    }
  };

  class range{
  private:
    int const from;
    int const to;
  public:
    range(int from_, int to_) : from(from_), to(to_) {}
    range(int to_) : from(0), to(to_) {}

    rangeIterator begin() const {
      return rangeIterator(from);
    }

    rangeIterator end() const{
      return rangeIterator(to);
    }

  };
  
}

