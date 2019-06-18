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
    typedef T& reference;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef std::random_access_iterator_tag iterator_category;

    template<typename> friend
    class vector;
    template<typename> friend
    class vector_const_iterator;

    vector_iterator() = default;
    vector_iterator(vector_iterator const&) = default;
    vector_iterator& operator=(vector_iterator const&) = default;

    vector_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    const vector_iterator operator++(int) {
        vector_iterator result(*this);
        ++*this;
        return result;
    }

    vector_iterator& operator--() {
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

    bool operator==(vector_iterator const& other) const {
        return ptr_ == other.ptr_;
    }

    bool operator!=(vector_iterator const& other) const {
        return ptr_ != other.ptr_;
    }

    bool operator<(vector_iterator const& other) {
        return ptr_ < other.ptr_;
    }

    bool operator>(vector_iterator const& other) {
        return ptr_ > other.ptr_;
    }

    bool operator<=(vector_iterator const& other) {
        return ptr_ <= other.ptr_;
    }

    bool operator>=(vector_iterator const& other) {
        return ptr_ >= other.ptr_;
    }

    vector_iterator& operator+=(size_t n) {
        ptr_ += n;
        return *this;
    }

    vector_iterator& operator-=(size_t n) {
        ptr_ -= n;
        return *this;
    }

    reference operator[](size_t n) {
        return ptr_[n];
    }

    friend difference_type operator-(vector_iterator const& p, vector_iterator const& q) {
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

    friend vector_iterator operator+(size_t n, vector_iterator const& p) {
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
    typedef T* pointer;
    typedef std::random_access_iterator_tag iterator_category;

    template<typename> friend
    class vector;

    vector_const_iterator() = default;
    vector_const_iterator(vector_const_iterator const&) = default;
    vector_const_iterator(vector_iterator<T> const& other) : ptr_(other.ptr_) {}

    vector_const_iterator& operator=(vector_const_iterator const&) = default;

    vector_const_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    const vector_const_iterator operator++(int) {
        vector_const_iterator result(*this);
        ++*this;
        return result;
    }

    vector_const_iterator& operator--() {
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

    bool operator==(vector_const_iterator const& other) const {
        return ptr_ == other.ptr_;
    }

    bool operator!=(vector_const_iterator const& other) const {
        return ptr_ != other.ptr_;
    }

    bool operator<(vector_const_iterator const& other) {
        return ptr_ < other.ptr_;
    }

    bool operator>(vector_const_iterator const& other) {
        return ptr_ > other.ptr_;
    }

    bool operator<=(vector_const_iterator const& other) {
        return ptr_ <= other.ptr_;
    }

    bool operator>=(vector_const_iterator const& other) {
        return ptr_ >= other.ptr_;
    }

    vector_const_iterator& operator+=(size_t n) {
        ptr_ += n;
        return *this;
    }

    vector_const_iterator& operator-=(size_t n) {
        ptr_ -= n;
        return *this;
    }

    reference operator[](size_t n) {
        return ptr_[n];
    }

    friend difference_type operator-(vector_const_iterator const& p,
                                     vector_const_iterator const& q) {
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

    friend vector_const_iterator operator+(size_t n, vector_const_iterator const& p) {
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
    typedef T* pointer;
    typedef T& reference;
    typedef T const& const_reference;
    typedef vector_iterator<T> iterator;
    typedef vector_const_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  private:
    typedef char* mix_ptr;
    typedef size_t size_type;
    static const size_type DEFAULT_CAPACITY_ = 128;

    std::variant<mix_ptr, value_type> variant_;
    static_assert(sizeof(variant_) <= sizeof(void*) + std::max(sizeof(T), sizeof(void*)));

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
        operator delete(static_cast<void*>(ptr));
    }

    size_type& vec_size_(mix_ptr ptr) noexcept {
        return *reinterpret_cast<size_type*>(ptr);
    }

    size_type& vec_cap_(mix_ptr ptr) noexcept {
        return *reinterpret_cast<size_type*>(ptr + sizeof(size_type));
    }

    size_type& vec_ref_(mix_ptr ptr) noexcept {
        return *reinterpret_cast<size_type*>(ptr + 2 * sizeof(size_type));
    }

    pointer vec_data_(mix_ptr ptr) noexcept {
        return *reinterpret_cast<size_type*>(ptr + 3 * sizeof(size_type));
    }

    // noexcept if only if value_type destruction nothrow
    void destruct(pointer ptr, size_type obj_cnt) noexcept {
        std::destroy(ptr, ptr + obj_cnt);
    }

    // noexcept if only if value_type destruction nothrow
    void destruct(pointer first, pointer last) noexcept {
        std::destroy(first, last);
    }

    void free_with_destruct(mix_ptr ptr) noexcept {
        destruct(vec_data_(ptr), vec_size_(ptr));
    }


    // _____________________________________________________________________________________________
    // helpful method

    value_type& val_() noexcept {
        assert(variant_.index() == 1);
        return std::get<1>(variant_);
    }

    mix_ptr get_mix_ptr_() noexcept {
        assert(variant_.index() == 0);
        return std::get<0>(variant_);
    }

    size_type& size_() noexcept {
        assert(variant_.index() == 0);
        return vec_size_(std::get<0>(variant_));
    }

    size_type real_size_() noexcept {
        return variant_.index() == 1 ? 1 : (is_empty() ? 0 : size_());
    }

    size_type& capacity_() noexcept {
        assert(variant_.index() == 0);
        return vec_cap_(std::get<0>(variant_));
    }

    size_type real_capacity_() noexcept {
        return variant_.index() == 1 ? 1 : (get_mix_ptr_() == nullptr ? 1 : capacity_());
    }

    size_type& ref_cnt_() noexcept {
        assert(variant_.index() == 0);
        return vec_ref_(std::get<0>(variant_));
    }

    pointer data_() noexcept {
        assert(variant_.index() == 0);
        return vec_data_(std::get<0>(variant_));
    }

    bool is_small() noexcept {
        return variant_.index() == 1 || (variant_.index() == 0 && std::get<0>(variant_) == nullptr);
    }

    bool is_unique() noexcept {
        return ref_cnt_() == 1;
    }

    bool is_empty() noexcept {
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

    pointer get_unique_data() {
        if (is_small()) {
            return &val_;
        } else {
            make_copy_if_not_unique();
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
        set_header_(size_(), capacity_());
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
                    set_header_(1, new_cap);
                } else {
                    set_header_(0, new_cap);
                }
            } catch (...) {
                free_empty_memory(alloc_mem);
                throw;
            }
            variant_ = alloc_mem;
        } else {
            mix_ptr new_mem = copy_from_(get_mix_ptr_(), new_cap);
            cut_link_(get_mix_ptr_());
            set_header_(size_(), new_cap);
            variant_ = new_mem;
        }
    }

    // strong, for big data only
    void shrink_() {
        mix_ptr new_mem = copy_from_(get_mix_ptr_(), size_());
        cut_link_(get_mix_ptr_());
        set_header_(size_(), size_());
        variant_ = new_mem;
    }

    // _____________________________________________________________________________________________
    // _____________________________________________________________________________________________
    // end of private zone

  public:
    vector() noexcept = default;

    // strong
    vector(vector const& other) {
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
        // TODO
    }

    // basic, value_type depended
    vector& operator=(vector const& other) {
        if (*this == &other) {
            return *this;
        }
        if (is_small()) {
            if (empty()) {
                try {
                    variant_ = other.variant_;
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
    }

    // basic, value_type depended
    template<typename InputIterator>
    vector assign(InputIterator first, InputIterator last) {
        // TODO
    }

    // strong
    reference operator[](size_type ind) {
        return get_unique_data()[ind];
    }

    // strong
    reference front() {
        return get_unique_data()[0];
    }

    // strong
    reference back() {
        return get_unique_data()[size_() - 1];
    }

    void push_back() {
        // TODO
    }

    void pop_back() {
        // TODO 
    }

    pointer data() {
        return get_unique_data();
    }

    iterator begin() {
        return iterator(data());
    }

    const_iterator cbegin() const {
        return const_iterator(data());
    }

    iterator end() {
        return iterator(data() + size());
    }

    const_iterator cend() const {
        return const_iterator(data() + size());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
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

    void resize(size_type new_size) {
        if (new_size == size()) {
            return;
        }
        if (new_size == 0) {
            clear();
        }
        if (new_size == 1 && is_small() && empty()) {
            try {
                variant_ = value_type();
            } catch (...) {
                set_null();
                throw;
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

    void clear() noexcept {
        if (!is_small()) {
            cut_link_(get_mix_ptr_());
        }
        set_null();
    }
};

#endif //SUPER_VECTOR__VECTOR_HPP_
