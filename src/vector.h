#ifndef VECTOR
#define VECTOR
#include <iostream>
#include <vector>

void Kludje() { std::vector<int> a; }

template <typename T, typename Allocator = std::allocator<T>>
class Vector {
 private:
  T[] & vec;
  size_t size;
  size_t capacity;

 public:
  struct Iterator {
    T* curr;
  }

  Vector();
  Vector(size_t, T);
  Vector(const Vector<T, Allocator>&);
  Vector(const Vector<T, Allocator>&&);
  Vector<T, Allocator>& operator=(const Vector<T, Allocator>&);

  // Setters
  void resize(size_t);
  void assign(size_t, const T&);
  void shrink_to_fit();
  void push_back(const T&&);
  void pop_back();

  // Getters
  bool empty();
  T& front();
  T& back();
  Iterator begin();
  Iterator end();
  T& operator[](size_t);
  bool operator=();
  bool operator!=();

  void clear();
  ~Vector();
};

#endif  // VECTOR