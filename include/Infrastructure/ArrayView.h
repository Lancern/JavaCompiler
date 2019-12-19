//
// Created by Sirui Mu on 2019/12/18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

#ifndef JVC_ARRAYVIEW_H
#define JVC_ARRAYVIEW_H

#include <cassert>
#include <vector>
#include <array>

namespace jvc {

template <typename Element>
class array_view {
public:
  template <size_t N>
  array_view(Element (&arr)[N])
    : _begin(&arr[0]),
      _end(&arr[N])
  { }

  array_view(std::vector<Element>& v)
    : _begin(&v[0]),
      _end(&v[v.size()])
  { }

  template <size_t N>
  array_view(std::array<Element, N>& arr)
    : _begin(&arr[0]),
      _end(&arr[N])
  { }

  array_view(Element* begin, Element* end)
    : _begin(begin),
      _end(end)
  { }

  Element* begin() const { return _begin; }
  Element* end() const { return _end; }

  [[nodiscard]]
  size_t size() const { return _end - _begin; }

  Element& at(size_t index) {
    assert(index >= 0 && index < size() && "index is out of bound.");
    return _begin[index];
  }

  Element& operator[](size_t index) {
    return _begin[index];
  }

  array_view<Element> slice(size_t start) const {
    assert(start >= 0 && start <= size() && "start is out of bound.");
    return array_view<Element> { _begin + start, _end };
  }

  array_view<Element> slice(size_t start, size_t length) const {
    assert(start >= 0 && start <= size() && "start is out of bound.");
    assert(length >= 0 && "length is out of bound.");
    assert(start + length <= size() && "sub-slice is out of bound.");
    return array_view<Element> { _begin + start, _begin + start + length };
  }

private:
  Element* _begin;
  Element* _end;
}; // template class array_view

} // namespace jvc

#endif // JVC_ARRAYVIEW_H

#pragma clang diagnostic pop
