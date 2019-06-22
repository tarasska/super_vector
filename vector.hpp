//
// Created by taras on 18.06.19.
//

#ifndef SUPER_VECTOR__VECTOR_HPP_
#define SUPER_VECTOR__VECTOR_HPP_

#include <variant>
#include <iterator>
#include <memory>
#include <assert.h>

template<typename T>
struct vector_iterator {
  typedef T value_type;
  typedef T &reference;
  typedef std::ptrdiff_t difference_type;
  typedef T *pointer;
  typedef std::random_access_iterator_tag iterator_category;

  template<typename> friend
  class vector;
  template<typename> friend
  class vector_const_iterator;

  vector_iterator() = default;
  vector_iterator(vector_iterator const &) = default;
  vector_iterator &operator=(vector_iterator const &) = default;

  vector_iterator &operator++() {
      ++ptr_;
      return *this;
  }

  const vector_iterator operator++(int) {
      vector_iterator result(*this);
      ++*this;
      return result;
  }

  vector_iterator &operator--() {
      --ptr_;
      return *this;
  }

  const vector_iterator operator--(int) {
      vector_iterator result(*this);
      --*this;
      return result;
  }

  reference operator*() const {
      return *ptr_;
  }

  pointer operator->() const {
      return ptr_;
  }

  bool operator==(vector_iterator const &other) const {
      return ptr_ == other.ptr_;
  }

  bool operator!=(vector_iterator const &other) const {
      return ptr_ != other.ptr_;
  }

  bool operator<(vector_iterator const &other) {
      return ptr_ < other.ptr_;
  }

  bool operator>(vector_iterator const &other) {
      return ptr_ > other.ptr_;
  }

  bool operator<=(vector_iterator const &other) {
      return ptr_ <= other.ptr_;
  }

  bool operator>=(vector_iterator const &other) {
      return ptr_ >= other.ptr_;
  }

  vector_iterator &operator+=(size_t n) {
      ptr_ += n;
      return *this;
  }

  vector_iterator &operator-=(size_t n) {
      ptr_ -= n;
      return *this;
  }

  reference operator[](size_t n) {
      return ptr_[n];
  }

  friend difference_type operator-(vector_iterator const &p, vector_iterator const &q) {
      return p.ptr_ - q.ptr_;
  }

  friend vector_iterator operator+(vector_iterator p, size_t n) {
      p += n;
      return p;
  }

  friend vector_iterator operator-(vector_iterator p, size_t n) {
      p -= n;
      return p;
  }

  friend vector_iterator operator+(size_t n, vector_iterator const &p) {
      p += n;
      return p;
  }

  private:
  explicit vector_iterator(pointer p) : ptr_(p) {}

  pointer ptr_ = nullptr;
};

template<typename T>
struct vector_const_iterator {
  typedef T value_type;
  typedef T const& reference;
  typedef std::ptrdiff_t difference_type;
  typedef T const* pointer;
  typedef std::random_access_iterator_tag iterator_category;


  template<typename> friend
  class vector;

  vector_const_iterator() = default;
  vector_const_iterator(vector_const_iterator const &) = default;
  vector_const_iterator(vector_iterator<T> const &other) : ptr_(other.ptr_) {}

  vector_const_iterator &operator=(vector_const_iterator const &) = default;

  vector_const_iterator &operator++() {
      ++ptr_;
      return *this;
  }

  const vector_const_iterator operator++(int) {
      vector_const_iterator result(*this);
      ++*this;
      return result;
  }

  vector_const_iterator &operator--() {
      --ptr_;
      return *this;
  }

  const vector_const_iterator operator--(int) {
      vector_const_iterator result(*this);
      --*this;
      return result;
  }

  reference operator*() const {
      return *ptr_;
  }

  pointer operator->() const {
      return ptr_;
  }

  bool operator==(vector_const_iterator const &other) const {
      return ptr_ == other.ptr_;
  }

  bool operator!=(vector_const_iterator const &other) const {
      return ptr_ != other.ptr_;
  }

  bool operator<(vector_const_iterator const &other) {
      return ptr_ < other.ptr_;
  }

  bool operator>(vector_const_iterator const &other) {
      return ptr_ > other.ptr_;
  }

  bool operator<=(vector_const_iterator const &other) {
      return ptr_ <= other.ptr_;
  }

  bool operator>=(vector_const_iterator const &other) {
      return ptr_ >= other.ptr_;
  }

  vector_const_iterator &operator+=(size_t n) {
      ptr_ += n;
      return *this;
  }

  vector_const_iterator &operator-=(size_t n) {
      ptr_ -= n;
      return *this;
  }

  reference operator[](size_t n) {
      return ptr_[n];
  }

  friend difference_type operator-(vector_const_iterator const &p,
                                   vector_const_iterator const &q) {
      return p.ptr_ - q.ptr_;
  }

  friend vector_const_iterator operator+(vector_const_iterator p, size_t n) {
      p += n;
      return p;
  }

  friend vector_const_iterator operator-(vector_const_iterator p, size_t n) {
      p -= n;
      return p;
  }

  friend vector_const_iterator operator+(size_t n, vector_const_iterator const &p) {
      p += n;
      return p;
  }

  private:
  explicit vector_const_iterator(pointer p) : ptr_(p) {}

  pointer ptr_ = nullptr;
};

template<typename T>
class vector {
  public:
  typedef T value_type;
  typedef T *pointer;
  typedef T const *const_pointer;
  typedef T &reference;
  typedef T const &const_reference;
  typedef vector_iterator<T> iterator;
  typedef vector_const_iterator<T> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  private:
  typedef char *mix_ptr;
  typedef char const *const_mix_ptr;
  typedef size_t size_type;
  static const size_type DEFAULT_CAPACITY_ = 2;

  std::variant<mix_ptr, value_type> variant_;
  static_assert(sizeof(variant_) <= sizeof(void *) + std::max(sizeof(T), sizeof(void *)));

  // _____________________________________________________________________________________________
  // service function

  mix_ptr allocate(size_type n) {
      return reinterpret_cast<mix_ptr>(operator new(n));
  }

  mix_ptr allocate_from_size_with_header(size_type n) {
      return reinterpret_cast<mix_ptr>(operator new(
          3 * sizeof(size_type) + n * sizeof(value_type)));
  }

  void free_empty_memory(mix_ptr ptr) {
      operator delete(static_cast<void *>(ptr));
  }

  size_type &vec_size_(mix_ptr ptr) noexcept {
      return *reinterpret_cast<size_type *>(ptr);
  }

  size_type const &vec_size_(mix_ptr ptr) const noexcept {
      return *reinterpret_cast<size_type *>(ptr);
  }

  size_type &vec_cap_(mix_ptr ptr) noexcept {
      return *reinterpret_cast<size_type *>(ptr + sizeof(size_type));
  }

  size_type const &vec_cap_(mix_ptr ptr) const noexcept {
      return *reinterpret_cast<size_type *>(ptr + sizeof(size_type));
  }

  size_type &vec_ref_(mix_ptr ptr) noexcept {
      return *reinterpret_cast<size_type *>(ptr + 2 * sizeof(size_type));
  }

  size_type const &vec_ref_(mix_ptr ptr) const noexcept {
      return *reinterpret_cast<size_type *>(ptr + 2 * sizeof(size_type));
  }

  pointer vec_data_(mix_ptr ptr) noexcept {
      return reinterpret_cast<pointer>(ptr + 3 * sizeof(size_type));
  }

  pointer vec_data_(mix_ptr ptr) const noexcept {
      return reinterpret_cast<pointer>(ptr + 3 * sizeof(size_type));
  }

  // noexcept if only if value_type destruction nothrow
  template<typename InputIt>
  void destruct(InputIt ptr, size_type obj_cnt) {
      std::destroy(ptr, ptr + obj_cnt);
  }

  // noexcept if only if value_type destruction nothrow
  template<typename InputIt>
  void destruct(InputIt first, InputIt last) {
      std::destroy(first, last);
  }

  void free_with_destruct(mix_ptr ptr) {
      destruct(vec_data_(ptr), vec_size_(ptr));
      free_empty_memory(ptr);
  }

  void construct(pointer ptr, const_reference value) {
      new(ptr) value_type(value);
  }

  void construct(pointer first, pointer last, const_reference value) {
      while (first != last) {
          construct(first, value);
          ++first;
      }
  }



  // _____________________________________________________________________________________________
  // helpful method

  reference val_() noexcept {
      assert(variant_.index() == 1);
      return std::get<1>(variant_);
  }

  const_reference val_() const noexcept {
      assert(variant_.index() == 1);
      return std::get<1>(variant_);
  }

  mix_ptr get_mix_ptr_() noexcept {
      assert(variant_.index() == 0);
      return std::get<0>(variant_);
  }

  const_mix_ptr get_mix_ptr_() const noexcept {
      assert(variant_.index() == 0);
      return std::get<0>(variant_);
  }

  size_type &size_() noexcept {
      assert(variant_.index() == 0);
      return vec_size_(std::get<0>(variant_));
  }

  size_type size_() const noexcept {
      assert(variant_.index() == 0);
      return vec_size_(std::get<0>(variant_));
  }

  size_type real_size_() const noexcept {
      return variant_.index() == 1 ? 1 : (is_empty() ? 0 : size_());
  }

  size_type &capacity_() noexcept {
      assert(variant_.index() == 0);
      return vec_cap_(std::get<0>(variant_));
  }

  size_type capacity_() const noexcept {
      assert(variant_.index() == 0);
      return vec_cap_(std::get<0>(variant_));
  }

  size_type real_capacity_() const noexcept {
      return variant_.index() == 1 ? 1 : (get_mix_ptr_() == nullptr ? 1 : capacity_());
  }

  size_type &ref_cnt_() noexcept {
      assert(variant_.index() == 0);
      return vec_ref_(std::get<0>(variant_));
  }

  size_type const &ref_cnt_() const noexcept {
      assert(variant_.index() == 0);
      return vec_ref_(std::get<0>(variant_));
  }

  pointer data_() noexcept {
      assert(variant_.index() == 0);
      return vec_data_(std::get<0>(variant_));
  }

  pointer data_() const noexcept {
      assert(variant_.index() == 0);
      return vec_data_(std::get<0>(variant_));
  }

  bool is_small() const noexcept {
      return variant_.index() == 1 || (variant_.index() == 0 && std::get<0>(variant_) == nullptr);
  }

  bool is_unique() const noexcept {
      return ref_cnt_() == 1;
  }

  bool is_empty() const noexcept {
      return (variant_.index() == 0 &&
          (std::get<0>(variant_) == nullptr
              || (std::get<0>(variant_) != nullptr && size_() == 0)));
  }

  void set_null() noexcept {
      variant_ = nullptr;
  }

  void cut_link_(mix_ptr ptr) noexcept {
      if (--vec_ref_(ptr) == 0) {
          destruct(vec_data_(ptr), vec_size_(ptr)); // noexcept
          free_empty_memory(ptr); // noexcept
      }
  }

  void set_header_(size_type sz, size_type cp, size_type ref = 1) {
      size_() = sz;
      capacity_() = cp;
      ref_cnt_() = ref;
  }

  void set_header_(mix_ptr ptr, size_type sz, size_type cp, size_type ref = 1) {
      vec_size_(ptr) = sz;
      vec_cap_(ptr) = cp;
      vec_ref_(ptr) = ref;
  }

  pointer get_unique_data() {
      if (is_small()) {
          return is_empty() ? nullptr : &val_();
      } else {
          make_copy_if_not_unique();
          return data_();
      }
  }


  const_pointer get_unique_const_data() const noexcept {
      if (is_small()) {
          return is_empty() ? nullptr : &val_();
      } else {
          return data_();
      }
  }

  // strong, big obj only
  mix_ptr copy_from_(mix_ptr src_ptr) {
      mix_ptr alloc_mem = nullptr;
      try {
          alloc_mem = allocate_from_size_with_header(vec_cap_(src_ptr));
          std::uninitialized_copy(&vec_size_(src_ptr),
                                  &vec_size_(src_ptr) + 3, &vec_size_(alloc_mem));
          std::uninitialized_copy(vec_data_(src_ptr), vec_data_(src_ptr) + vec_size_(src_ptr),
                                  vec_data_(alloc_mem));
          vec_ref_(alloc_mem) = 1; // attention
          return alloc_mem;
      } catch (...) {
          free_empty_memory(alloc_mem);
          throw;
      }
  }

  // strong, big obj only
  mix_ptr copy_from_(mix_ptr src_ptr, size_type new_cap) {
      mix_ptr alloc_mem = nullptr;
      try {
          alloc_mem = allocate_from_size_with_header(new_cap);
          std::uninitialized_copy(&vec_size_(src_ptr),
                                  &vec_size_(src_ptr) + 3, &vec_size_(alloc_mem));
          std::uninitialized_copy(vec_data_(src_ptr), vec_data_(src_ptr) + vec_size_(src_ptr),
                                  vec_data_(alloc_mem));
          vec_cap_(alloc_mem) = new_cap; // attention
          vec_ref_(alloc_mem) = 1; // attention
          return alloc_mem;
      } catch (...) {
          free_empty_memory(alloc_mem);
          throw;
      }
  }

  // strong, safety copy for big obj only
  void make_copy_if_not_unique() {
      if (is_unique()) {
          return;
      }
      mix_ptr new_mem = copy_from_(get_mix_ptr_());
      cut_link_(get_mix_ptr_());
      set_header_(new_mem, size_(), capacity_());
      variant_ = new_mem;
  }

  // strong
  void extend_(size_type new_cap) {
      if (is_small()) {
          mix_ptr alloc_mem = nullptr;
          try {
              alloc_mem = allocate_from_size_with_header(new_cap);
              if (!empty()) {
                  std::uninitialized_copy(&val_(), &val_() + 1, vec_data_(alloc_mem));
                  set_header_(alloc_mem, 1, new_cap);
              } else {
                  set_header_(alloc_mem, 0, new_cap);
              }
          } catch (...) {
              free_empty_memory(alloc_mem);
              throw;
          }
          variant_ = alloc_mem;
      } else {
          mix_ptr new_mem = copy_from_(get_mix_ptr_(), new_cap);
          set_header_(new_mem, size_(), new_cap);
          cut_link_(get_mix_ptr_());
          variant_ = new_mem;
      }
  }

  // strong, for big data only
  void shrink_() {
      mix_ptr new_mem = copy_from_(get_mix_ptr_(), size_());
      cut_link_(get_mix_ptr_());
      set_header_(new_mem, size_(), size_());
      variant_ = new_mem;
  }

  // strong
  void init_small(const_reference elem) {
      try {
          variant_ = elem;
      } catch (...) {
          variant_ = nullptr;
          throw;
      }
  }

  // strong
  void push_back_with_allocate(const_reference elem) {
      mix_ptr ptr = nullptr;
      if (is_small()) {
          try {
              ptr = allocate_from_size_with_header(DEFAULT_CAPACITY_);
              set_header_(ptr, 1, DEFAULT_CAPACITY_);
              construct(vec_data_(ptr), val_());
          } catch (...) {
              free_empty_memory(ptr);
              throw;
          }
          try {
              construct(vec_data_(ptr) + 1, elem);
          } catch (...) {
              free_with_destruct(ptr);
              throw;
          }
          variant_ = ptr;
      } else {
          ptr = copy_from_(get_mix_ptr_(), capacity_() * 2);
          try {
              construct(vec_data_(ptr) + vec_size_(ptr), elem);
          } catch (...) {
              free_with_destruct(ptr);
              throw;
          }
          cut_link_(get_mix_ptr_());
          variant_ = ptr;
      }
  }

  // strong
  void push_back_in_place(const_reference elem) {
      bool one = is_unique();
      mix_ptr ptr = one ? get_mix_ptr_() : copy_from_(get_mix_ptr_());
      try {
          construct(vec_data_(ptr) + vec_size_(ptr), elem);
      } catch (...) {
          if (!one) {
              free_with_destruct(ptr);
          }
          throw;
      }
      if (!one) {
          cut_link_(get_mix_ptr_());
          variant_ = ptr;
      }
  }

  // _____________________________________________________________________________________________
  // _____________________________________________________________________________________________
  // end of private zone

  public:
  vector() noexcept = default;

  ~vector() {
      clear();
  }

  // strong
  vector(vector const &other) {
      if (other.is_small()) {
          try {
              variant_ = other.variant_;
          } catch (...) {
              set_null();
              throw;
          }
      } else {
          variant_ = other.variant_; // noexcept
          ++ref_cnt_(); // noexcept
      }
  }

  // strong
  template<typename InputIterator>
  vector(InputIterator first, InputIterator last) {
      if (first + 1 == last) {
          try {
              variant_ = *first;
          } catch (...) {
              set_null();
              throw;
          }
      } else {
          mix_ptr new_mem = allocate_from_size_with_header(last - first);
          try {
              std::uninitialized_copy(first, last, vec_data_(new_mem));
              set_header_(new_mem, last - first, last - first);
          } catch (...) {
              free_empty_memory(new_mem);
              set_null();
              throw;
          }
          variant_ = new_mem;
      }
  }

  // basic, value_type depended
  vector &operator=(vector const &other) {
      if (this == &other) {
          return *this;
      }
      if (is_small()) {
          if (empty()) {
              try {
                  variant_ = other.variant_;
                  if (!other.is_small()) {
                      ++ref_cnt_();
                  }
              } catch (...) {
                  set_null();
                  throw;
              }
          } else {
              variant_ = other.variant_; // depends value_type guarantee
          }
      } else {
          mix_ptr old = get_mix_ptr_();
          try {
              variant_ = other.variant_;
              if (!other.is_small()) {
                  ++ref_cnt_();
              }
          } catch (...) {
              variant_ = old;
              throw;
          }
          cut_link_(old);
      }
      return *this;
  }

  // basic, value_type depended
  template<typename InputIterator>
  vector assign(InputIterator first, InputIterator last) {
      vector(first, last).swap(*this);
      return *this;
  }

  // strong
  reference operator[](size_type ind) {
      return get_unique_data()[ind];
  }

  const_reference operator[](size_type ind) const noexcept {
      return get_unique_const_data()[ind];
  }

  // strong
  reference front() {
      return get_unique_data()[0];
  }

  const_reference front() const noexcept {
      return get_unique_const_data()[0];
  }

  // strong
  reference back() {
      return get_unique_data()[size() - 1];
  }

  const_reference back() const noexcept {
      return get_unique_const_data()[size() - 1];
  }

  // strong
  void push_back(const_reference elem) {
      if (is_small() && empty()) {
          init_small(elem); // strong
          return;
      }
      if (size() == capacity()) {
          push_back_with_allocate(elem); // strong
      } else {
          push_back_in_place(elem); // strong
      }
      ++size_();
  }

  void pop_back() {
      if (is_small() && size() == 1) {
          variant_ = nullptr;
          return;
      }
      make_copy_if_not_unique();
      std::destroy(data() + size() - 1, data() + size());
      --size_();
  }

  pointer data() {
      return get_unique_data();
  }

  const_pointer data() const noexcept {
      return get_unique_const_data();
  }

  iterator begin() {
      return iterator(data());
  }

  const_iterator begin() const noexcept {
      return const_iterator(data());
  }

  const_iterator cbegin() const noexcept {
      return const_iterator(data());
  }

  iterator end() {
      return iterator(data() + size());
  }

  const_iterator end() const noexcept {
      return const_iterator(data() + size());
  }

  const_iterator cend() const noexcept {
      return const_iterator(data() + size());
  }

  reverse_iterator rbegin() {
      return reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const noexcept {
      return const_reverse_iterator(cend());
  }

  const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
  }

  reverse_iterator rend() {
      return reverse_iterator(begin());
  }

  const_reverse_iterator rend() const noexcept {
      return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator(cbegin());
  }

  bool empty() noexcept {
      return is_empty();
  }

  size_type size() const noexcept {
      return real_size_();
  }

  // strong
  void reserve(size_type new_cap) {
      if (new_cap > capacity()) {
          extend_(new_cap);
      }
  }

  size_type capacity() const noexcept {
      return real_capacity_();
  }

  // strong
  void shrink_to_fit() {
      if (size() < capacity()) {
          if (size() == 0) {
              clear();
          } else if (size() == 1) {
              mix_ptr old = get_mix_ptr_();
              try {
                  variant_ = vec_data_(old)[0];
              } catch (...) {
                  variant_ = old;
                  throw;
              }
              cut_link_(old);
          } else {
              shrink_();
          }
      }
  }

  // strong
  void resize(size_type new_size) {
      if (new_size == size()) {
          return;
      }
      if (new_size == 0) {
          clear();
      }
      if (is_small() && empty()) {
          if (new_size == 1) {
              try {
                  variant_ = value_type();
              } catch (...) {
                  set_null();
                  throw;
              }
          } else {
              mix_ptr ptr = nullptr;
              try {
                  ptr = allocate_from_size_with_header(new_size);
                  set_header_(ptr, new_size, new_size);
                  std::uninitialized_fill(vec_data_(ptr), vec_data_(ptr) + new_size, value_type());
              } catch (...) {
                  free_empty_memory(ptr);
                  throw;
              }
              variant_ = ptr;
          }
          return;
      }
      if (new_size < size()) {
          make_copy_if_not_unique();
          destruct(data() + new_size, data() + size());
      } else {
          mix_ptr ptr = copy_from_(get_mix_ptr_(), std::max(new_size, capacity()));
          try {
              std::uninitialized_fill(vec_data_(ptr) + size(),
                                      vec_data_(ptr) + new_size, value_type());
          } catch (...) {
              free_with_destruct(ptr);
              throw;
          }
          clear();
          variant_ = ptr;
          set_header_(new_size, std::max(new_size, capacity()));
      }
  }

  // noexcept if only if ~vaule_type() nothrow
  void clear() {
      if (!is_small()) {
          cut_link_(get_mix_ptr_());
      }
      set_null();
  }

  // basic, strong if "end insert"
  iterator insert(const_iterator pos, const_reference elem) {
      if (pos == end()) {
          push_back(elem);
          return end() - 1;
      }
      if (is_small()) {
          if (pos != begin()) {
              throw std::runtime_error("invalid iterator");
          }
          vector<value_type> buf;
          buf.push_back(*begin());
          buf.push_back(elem);
          swap(buf);
          return begin();
      } else {
          value_type insert_elem(elem);
          mix_ptr new_mem =
              allocate_from_size_with_header(size() < capacity() ? capacity() : capacity() * 2);
          pointer new_data_ptr = vec_data_(new_mem);
          try {
              std::uninitialized_copy(begin(), begin() + (pos - begin()), new_data_ptr);
              set_header_(new_mem, pos - begin(), capacity() * 2);
          } catch (...) {
              free_empty_memory(new_mem);
              throw;
          }
          new_data_ptr += pos - begin();
          iterator ret_iter = iterator(new_data_ptr);
          try {
              construct(new_data_ptr, insert_elem);
              ++vec_size_(new_mem);
              ++new_data_ptr;
          } catch (...) {
              free_with_destruct(new_mem);
              throw;
          }
          try {
              std::uninitialized_copy(begin() + (pos - begin()), end(), new_data_ptr);
              vec_size_(new_mem) += end() - pos;
          } catch (...) {
              free_with_destruct(new_mem);
              throw;
          }
          cut_link_(get_mix_ptr_());
          variant_ = new_mem;
          return ret_iter;
      }
  }

  // basic, strong if "end erase"
  iterator erase(const_iterator pos) {
      return erase(pos, pos + 1);
  }

  // basic, strong if "end erase"
  iterator erase(const_iterator first, const_iterator last) {
      if (first == last) {
          return begin() + (first - begin());
      }
      if (is_small()) {
          if (first != last) {
              pop_back();
          }
          return begin();
      }
      make_copy_if_not_unique();
      if (last == end()) {
          destruct(first.ptr_, last.ptr_);
          size_() -= last - first;
          return end();
      }
      typename const_iterator::difference_type sz_begin = first - begin();
      typename const_iterator::difference_type sz_erase = last - first;
      typename const_iterator::difference_type sz_end = end() - last;
      pointer ptr_begin = data();
      pointer ptr_erase = ptr_begin + sz_begin;
      pointer ptr_end = ptr_erase + sz_erase;
      if (sz_erase >= sz_end) {
          destruct(ptr_erase, ptr_erase + sz_end);
          try {
              std::uninitialized_copy(ptr_end, ptr_end + sz_end, ptr_erase);
              destruct(ptr_erase + sz_end, ptr_end + sz_end);
          } catch (...) {
              destruct(ptr_erase + sz_end, ptr_end + sz_end);
              size_() = sz_begin;
              throw;
          }
      } else {
          destruct(ptr_erase, ptr_erase + sz_erase);
          try {
              std::uninitialized_copy(ptr_end, ptr_end + sz_erase, ptr_erase);
          } catch (...) {
              destruct(ptr_end, ptr_end + sz_end);
              size_() = sz_begin;
              throw;
          }
          pointer dst;
          pointer src;
          try {
              for (dst = ptr_end, src = ptr_end + sz_erase; src < ptr_end + sz_end;
                   ++src, ++dst) {
                  *dst = *src;
              }
              destruct(dst, ptr_end + sz_end);
          } catch (...) {
              destruct(ptr_erase, ptr_end + sz_end);
              size_() = sz_begin;
              throw;
          }
      }
      size_() = sz_begin + sz_end;
      return begin() + sz_begin;
  }

  // basic
  void swap(vector &other) {
      size_type ind_this = variant_.index();
      size_type ind_other = other.variant_.index();
      if (ind_this == 0 && ind_other == 0) {
          variant_.swap(other.variant_);
      } else if (ind_this == 1 && ind_other == 1) {
          try {
              variant_.swap(other.variant_);
          } catch (...) {
              set_null();
              other.set_null();
              throw;
          }
      } else if (ind_this == 0 && ind_other == 1) {
          mix_ptr this_data = get_mix_ptr_();
          try {
              variant_.swap(other.variant_);
          } catch (...) {
              variant_ = this_data;
              other.set_null();
              throw;
          }
      } else {
          mix_ptr other_data = other.get_mix_ptr_();
          try {
              variant_.swap(other.variant_);
          } catch (...) {
              other.variant_ = other_data;
              set_null();
              throw;
          }
      }
  }

};

template<typename T>
bool operator==(vector<T> const &a, vector<T> const &b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

template<typename T>
bool operator!=(vector<T> const &a, vector<T> const &b) {
    return !(a == b);
}

template<typename T>
bool operator<(vector<T> const &a, vector<T> const &b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T>
bool operator>(vector<T> const &a, vector<T> const &b) {
    return b < a;
}

template<typename T>
bool operator<=(vector<T> const &a, vector<T> const &b) {
    return !(a > b);
}

template<typename T>
bool operator>=(vector<T> const &a, vector<T> const &b) {
    return !(a < b);
}

template<typename T>
void swap(vector<T> &a, vector<T> &b) {
    a.swap(b);
}

#endif //SUPER_VECTOR__VECTOR_HPP_
