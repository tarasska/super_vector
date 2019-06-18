//
// Created by taras on 18.06.19.
//

#ifndef SUPER_VECTOR__VECTOR_HPP_
#define SUPER_VECTOR__VECTOR_HPP_

#include <variant>
#include <iterator>
#include <assert.h>

template<typename T>
class vector {
  public:
    typedef T  value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef T const& const_reference;
  private:
    typedef char* mix_ptr;
    typedef size_t size_type;
    typedef size_t size_pointer;
    std::variant<mix_ptr, value_type> variant_;
    static_assert(sizeof(variant_) <= sizeof(void*) + std::max(sizeof(T), sizeof(void*)));

    mix_ptr allocate(size_type n) {
        return reinterpret_cast<mix_ptr>(operator new(n));
    }

    mix_ptr allocate_from_size_with_header(size_type n) {
        return reinterpret_cast<mix_ptr>(operator new(3 * sizeof(size_type) + n * sizeof(value_type)));
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

    value_type& vec_val_() noexcept {
        assert(variant_.index() == 1);
        return std::get<1>(variant_);
    }

    mix_ptr get_mix_data() noexcept {
        assert(variant_.index() == 0);
        return std::get<0>(variant_);
    }



    size_type& size_() noexcept {
        assert(variant_.index() == 0);
        return vec_size_(std::get<0>(variant_));
    }

    size_type& capacity_() noexcept {
        assert(variant_.index() == 0);
        return vec_cap_(std::get<0>(variant_));
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
        return variant_.index() == 1 || (variant_.index() == 0 && std::get<0>(variant_));
    }

    bool is_unique() noexcept {
        return ref_cnt_() == 1;
    }


    // _____________________________________________________________________________________________
    // _____________________________________________________________________________________________
    // end of private zone

  public:
    vector() noexcept = default;

    vector(vector const& other) {

    }
};

#endif //SUPER_VECTOR__VECTOR_HPP_
