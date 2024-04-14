#pragma once
#include <cstdint>
#include <cstdlib>


template <typename T>
class Array
{
public:
    explicit Array(int32_t capacity = 8)
    {
        toValidNull();
        capacity = capacity ? capacity : 0;
        size_ = capacity;
        validateArray();
        lenght_ = 0;
    }

    ~Array()
    {
        for (int32_t i = 0; i < lenght_; i++)
        {
            array_[i].~T();
        }
        if (array_) free(array_);
        size_ = -1;
        lenght_ = -1;
    }

    Array(const Array& other);
    Array(Array&& other);

    Array& operator=(const Array& other);
    Array& operator=(Array&& other);

    bool operator==(const Array& rhs) { return array_ == rhs.array_; }

    int32_t insert(const T& value);
    int32_t insert(int32_t index, const T& value);
    int32_t insert(T&& value);
    int32_t insert(int32_t index, T&& value);
    void remove(int32_t index);
    void clear();
    const T& operator[](int32_t index) const;
    T& operator[](int32_t index);
    int32_t size() const;
    bool isEmpty() const { return lenght_ == 0; }

    class Iterator;
    class ConstIterator;

    Iterator iterator() { return Iterator(array_, lenght_, false); }
    Iterator begin() { return iterator(); }
    Iterator iterator() const { return Iterator(array_, lenght_, false); }
    //ConstIterator iterator() const { return ConstIterator(array_, lenght_, false); }
    Iterator begin() const { return iterator(); }
    Iterator reverseIterator() { return Iterator(array_, lenght_, true); }
    Iterator end() { return Iterator(array_, lenght_, false, lenght_); }
    Iterator end() const { return Iterator(array_, lenght_, false, lenght_); }
    //ConstIterator end() const { return ConstIterator(array_, lenght_, false, lenght_); }
    ConstIterator reverseIterator() const { return ConstIterator(array_, lenght_, true); }

private:
    T* array_ = nullptr;
    int32_t size_;
    int32_t lenght_;
    const float kExpandCoef = 1.6f;

    void validateArray();
    void toValidNull();
    void swap(Array& lhs, Array& rhs);
    void copyFromTo(T* from, T* to);
    void expand();
    void preInsert(int32_t& index);

    class Iterator
    {
    public:
        Iterator(T* arr, int32_t lenght, bool is_reverse, int32_t end_pos = -1)
            : arr_(arr), lenght_(lenght), is_reverse_(is_reverse)
        {
            cur_index_ = end_pos != -1
                             ? end_pos
                             : is_reverse
                             ? lenght_ - 1
                             : 0;
        }

        T get() const { return arr_[cur_index_]; }
        void set(const T& value) { arr_[cur_index_] = value; }
        void next() { if (hasNext()) cur_index_ += is_reverse_ ? -1 : 1; }
        bool hasNext() const { return is_reverse_ ? cur_index_ >= 0 : cur_index_ < lenght_; }
        T& operator*() { return arr_[cur_index_]; }
        bool operator!=(const Iterator& rhs) { return cur_index_ != rhs.cur_index_; }

        Iterator& operator++()
        {
            cur_index_++;
            return *this;
        }

    private:
        T* arr_;
        int32_t lenght_;
        bool is_reverse_;
        int32_t cur_index_;
    };

    class ConstIterator
    {
    public:
        ConstIterator(const T* arr, int32_t lenght, bool is_reverse)
            : arr_(arr), lenght_(lenght), is_reverse_(is_reverse),
              cur_index_(is_reverse ? lenght - 1 : 0)
        {
        }

        const T get() const { return arr_[cur_index_]; }
        void next() { if (hasNext()) cur_index_ += is_reverse_ ? -1 : 1; }
        bool hasNext() const { return is_reverse_ ? cur_index_ >= 0 : cur_index_ < arr_.lenght_; }
        const T& operator*() { return arr_[cur_index_]; }

    private:
        const T* arr_;
        int32_t lenght_;
        bool is_reverse_;
        int32_t cur_index_;
    };
};

#include "Array.hpp"
