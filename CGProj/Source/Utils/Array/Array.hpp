#pragma once
#include <algorithm>
#include <thread>

#include "Array.h"

template <typename T>
Array<T>::Array(const Array& array)
    : size_(array.size_), lenght_(array.lenght_)
{
    validateArray();
    copyFromTo(array.array_, array_);
}

template <typename T>
Array<T>::Array(Array&& other)
    : size_(other.size_), lenght_(other.lenght_), array_(other.array_)
{
    other.toValidNull();
}

template <typename T>
Array<T>& Array<T>::operator=(const Array& other)
{
    if (*this == other) return *this;

    Array temp(other);
    swap(*this, temp);
    return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(Array&& other)
{
    if (*this == other) return *this;

    swap(*this, other);
    other.toValidNull();
    return *this;
}


template <typename T>
int32_t Array<T>::insert(const T& value)
{
    return insert(lenght_, value);
}

template <typename T>
int32_t Array<T>::insert(int32_t index, const T& value)
{
    preInsert(index);
    new(&array_[index]) T(value);
    return index;
}

template <typename T>
int32_t Array<T>::insert(T&& value)
{
    return insert(lenght_, std::move(value));
}

template <typename T>
int32_t Array<T>::insert(int32_t index, T&& value)
{
    preInsert(index);
    new(&array_[index]) T(std::move(value));
    return index;
}

template <typename T>
void Array<T>::preInsert(int32_t& index)
{
    validateArray();

    if (lenght_ + 1 > size_) expand();
    lenght_++;

    index = index >= 0 && index < lenght_ ? index : lenght_ - 1;

    for (int32_t i = lenght_ - 2; i >= index; i--)
    {
        new(array_ + i + 1) T(std::move(array_[i]));
        array_[i].~T();
    }
}

template <typename T>
void Array<T>::removeAt(int32_t index)
{
    validateArray();
    if (index < 0 || index >= lenght_) return;
    array_[index].~T();
    for (int32_t i = index; i < lenght_ - 1; i++)
    {
        new(array_ + i) T(std::move(array_[i + 1]));
        array_[i + 1].~T();
    }
    lenght_--;
}

template <typename T>
void Array<T>::remove(const T& element)
{
    validateArray();

    int32_t i = 0;
    for (; i < lenght_; i++)
    {
        if (array_[i] == element)
        {
            removeAt(i);
            return;
        }
    }    
}

template <typename T>
void Array<T>::clear()
{
    for (int32_t i = 0; i < lenght_;)
    {
        removeAt(i);
    }
}

template <typename T>
bool Array<T>::contains(const T& element) const
{
    for ( int32_t i = 0; i < lenght_; i++)
    {
        if (array_[i] == element) return true;
    }
    return false;
}

template <typename T>
const T& Array<T>::operator[](int32_t index) const
{
    //validateArray();
    //if (index < 0 || index >= lenght_) return &T();
    return array_[index];
}

template <typename T>
T& Array<T>::operator[](int32_t index)
{
    validateArray();
    //if (index < 0 || index >= lenght_) return T();
    return array_[index];
}

template <typename T>
int32_t Array<T>::size() const
{
    return lenght_;
}

template <typename T>
void Array<T>::validateArray()
{
    if (!array_) array_ = static_cast<T*>(malloc(sizeof(T) * size_));
}

template <typename T>
void Array<T>::toValidNull()
{
    size_ = 0;
    lenght_ = 0;
    array_ = nullptr;
}

template <typename T>
void Array<T>::expand()
{
    size_ *= kExpandCoef;
    T* new_array = static_cast<T*>(malloc(sizeof(T) * size_));
    copyFromTo(array_, new_array);
    free(array_);
    array_ = new_array;
    new_array = nullptr;
}

template <typename T>
void Array<T>::swap(Array& lhs, Array& rhs)
{
    std::swap(lhs.size_, rhs.size_);
    std::swap(lhs.lenght_, rhs.lenght_);
    std::swap(lhs.array_, rhs.array_);
}

template <typename T>
void Array<T>::copyFromTo(T* from, T* to)
{
    for (int32_t i = 0; i < lenght_; i++)
    {
        new(&to[i]) T(std::move(from[i]));
        from[i].~T();
    }
}
