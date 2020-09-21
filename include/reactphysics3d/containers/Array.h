/********************************************************************************
* ReactPhysics3D physics library, http://www.reactphysics3d.com                 *
* Copyright (c) 2010-2020 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

#ifndef REACTPHYSICS3D_ARRAY_H
#define REACTPHYSICS3D_ARRAY_H

// Libraries
#include <reactphysics3d/configuration.h>
#include <reactphysics3d/memory/MemoryAllocator.h>
#include <cassert>
#include <cstring>
#include <iterator>
#include <memory>

namespace reactphysics3d {

// Class Array
/**
 * This class represents a simple dynamic array with custom memory allocator.
 */
template<typename T>
class Array {

    private:

        // -------------------- Attributes -------------------- //

        /// Buffer for the array elements
        T* mBuffer;

        /// Number of elements in the array
        uint32 mSize;

        /// Number of allocated elements in the array
        uint32 mCapacity;

        /// Memory allocator
        MemoryAllocator& mAllocator;

    public:

        /// Class Iterator
        /**
         * This class represents an iterator for the array
         */
        class Iterator {

            private:

                uint32 mCurrentIndex;
                T* mBuffer;
                uint32 mSize;

            public:

                // Iterator traits
                using value_type = T;
                using difference_type = std::ptrdiff_t;
                using pointer = T*;
                using const_pointer = T const*;
                using reference = T&;
                using const_reference = const T&;
                using iterator_category = std::random_access_iterator_tag;

                /// Constructor
                Iterator() = default;

                /// Constructor
                Iterator(void* buffer, uint32 index, uint32 size)
                     :mCurrentIndex(index), mBuffer(static_cast<T*>(buffer)), mSize(size) {

                }

                /// Copy constructor
                Iterator(const Iterator& it)
                     :mCurrentIndex(it.mCurrentIndex), mBuffer(it.mBuffer), mSize(it.mSize) {

                }

                /// Deferencable
                reference operator*() {
                    assert(mCurrentIndex >= 0 && mCurrentIndex < mSize);
                    return mBuffer[mCurrentIndex];
                }

                /// Const Deferencable
                const_reference operator*() const {
                    assert(mCurrentIndex >= 0 && mCurrentIndex < mSize);
                    return mBuffer[mCurrentIndex];
                }

                /// Deferencable
                const_pointer operator->() const {
                    assert(mCurrentIndex >= 0 && mCurrentIndex < mSize);
                    return &(mBuffer[mCurrentIndex]);
                }

                /// Post increment (it++)
                Iterator& operator++() {
                    assert(mCurrentIndex < mSize);
                    mCurrentIndex++;
                    return *this;
                }

                /// Pre increment (++it)
                Iterator operator++(int number) {
                    assert(mCurrentIndex < mSize);
                    Iterator tmp = *this;
                    mCurrentIndex++;
                    return tmp;
                }

                /// Post decrement (it--)
                Iterator& operator--() {
                    assert(mCurrentIndex > 0);
                    mCurrentIndex--;
                    return *this;
                }

                /// Pre decrement (--it)
                Iterator operator--(int number) {
                    assert(mCurrentIndex > 0);
                    Iterator tmp = *this;
                    mCurrentIndex--;
                    return tmp;
                }
               
                /// Plus operator
                Iterator operator+(const difference_type& n) {
                    return Iterator(mBuffer, mCurrentIndex + n, mSize);
                }
               
                /// Plus operator
                Iterator& operator+=(const difference_type& n) {
                    mCurrentIndex += n;
                    return *this;
                }
               
                /// Minus operator
                Iterator operator-(const difference_type& n) {
                    return Iterator(mBuffer, mCurrentIndex - n, mSize);
                }
                
                /// Minus operator
                Iterator& operator-=(const difference_type& n) {
                    mCurrentIndex -= n;
                    return *this;
                }

                /// Difference operator
                difference_type operator-(const Iterator& iterator) const {
                   return mCurrentIndex - iterator.mCurrentIndex;
                }

                /// Comparison operator
                bool operator<(const Iterator& other) const {
                    return mCurrentIndex < other.mCurrentIndex;
                }

                /// Comparison operator
                bool operator>(const Iterator& other) const {
                    return mCurrentIndex > other.mCurrentIndex;
                }

                /// Comparison operator
                bool operator<=(const Iterator& other) const {
                    return mCurrentIndex <= other.mCurrentIndex;
                }

                /// Comparison operator
                bool operator>=(const Iterator& other) const {
                    return mCurrentIndex >= other.mCurrentIndex;
                }

                /// Equality operator (it == end())
                bool operator==(const Iterator& iterator) const {
                    assert(mCurrentIndex >= 0 && mCurrentIndex <= mSize);

                    // If both iterators points to the end of the array
                    if (mCurrentIndex == mSize && iterator.mCurrentIndex == iterator.mSize) {
                        return true;
                    }

                    return &(mBuffer[mCurrentIndex]) == &(iterator.mBuffer[iterator.mCurrentIndex]);
                }

                /// Inequality operator (it != end())
                bool operator!=(const Iterator& iterator) const {
                    return !(*this == iterator);
                }

                /// Frienship
                friend class Array;

        };

        // -------------------- Methods -------------------- //

        /// Constructor
        Array(MemoryAllocator& allocator, uint32 capacity = 0)
            : mBuffer(nullptr), mSize(0), mCapacity(0), mAllocator(allocator) {

            if (capacity > 0) {

                // Allocate memory
                reserve(capacity);
            }
        }

        /// Copy constructor
        Array(const Array<T>& array) : mBuffer(nullptr), mSize(0), mCapacity(0), mAllocator(array.mAllocator) {

            // If we need to allocate more memory
            if (array.mCapacity > 0) {
                reserve(array.mCapacity);
            }

            // All all the elements of the array to the current one
            addRange(array);
        }

        /// Destructor
        ~Array() {

            // If elements have been allocated
            if (mCapacity > 0) {

                // Clear the array
                clear(true);
            }
        }

        /// Allocate memory for a given number of elements
        void reserve(uint32 capacity) {

            if (capacity <= mCapacity) return;

            // Allocate memory for the new array
            void* newMemory = mAllocator.allocate(capacity * sizeof(T));
            T* destination = static_cast<T*>(newMemory);

            if (mBuffer != nullptr) {

                if (mSize > 0) {

                    // Copy the elements to the new allocated memory location
                    std::uninitialized_copy(mBuffer, mBuffer + mSize, destination);

                    // Destruct the previous items
                    for (uint32 i=0; i<mSize; i++) {
                        mBuffer[i].~T();
                    }
                }

                // Release the previously allocated memory
                mAllocator.release(mBuffer, mCapacity * sizeof(T));
            }

            mBuffer = destination;
            assert(mBuffer != nullptr);

            mCapacity = capacity;
        }

        /// Add an element into the array
        void add(const T& element) {

            // If we need to allocate more memory
            if (mSize == mCapacity) {
                reserve(mCapacity == 0 ? 1 : mCapacity * 2);
            }

            // Use the copy-constructor to construct the element
            new (reinterpret_cast<void*>(mBuffer + mSize)) T(element);

            mSize++;
        }

        /// Add an element into the array by constructing it directly into the array (in order to avoid a copy)
        template<typename...Ts>
        void emplace(Ts&&... args) {

            // If we need to allocate more memory
            if (mSize == mCapacity) {
                reserve(mCapacity == 0 ? 1 : mCapacity * 2);
            }

            // Construct the element directly at its location in the array
            new (reinterpret_cast<void*>(mBuffer + mSize)) T(std::forward<Ts>(args)...);

            mSize++;
        }

        /// Add a given numbers of elements at the end of the array but do not init them
        void addWithoutInit(uint32 nbElements) {

            // If we need to allocate more memory
            if ((mSize + nbElements) > mCapacity) {
                reserve(mCapacity == 0 ? nbElements : (mCapacity + nbElements) * 2);
            }

            mSize += nbElements;
        }

        /// Try to find a given item of the array and return an iterator
        /// pointing to that element if it exists in the array. Otherwise,
        /// this method returns the end() iterator
        Iterator find(const T& element) {

            for (uint32 i=0; i<mSize; i++) {
                if (element == mBuffer[i]) {
                    return Iterator(mBuffer, i, mSize);
                }
            }

            return end();
        }

        /// Look for an element in the array and remove it
        Iterator remove(const T& element) {
           return remove(find(element));
        }

        /// Remove an element from the array and return a iterator
        /// pointing to the element after the removed one (or end() if none)
        Iterator remove(const Iterator& it) {
           assert(it.mBuffer == mBuffer);
           return removeAt(it.mCurrentIndex);
        }

        /// Remove an element from the array at a given index (all the following items will be moved)
        Iterator removeAt(uint32 index) {

          assert(index < mSize);

          // Call the destructor
          mBuffer[index].~T();

          mSize--;

          if (index != mSize) {

              // Move the elements to fill in the empty slot
              void* dest = reinterpret_cast<void*>(mBuffer + index);
              std::uintptr_t src = reinterpret_cast<std::uintptr_t>(dest) + sizeof(T);
              std::memmove(dest, reinterpret_cast<const void*>(src), (mSize - index) * sizeof(T));
          }

          // Return an iterator pointing to the element after the removed one
          return Iterator(mBuffer, index, mSize);
        }

        /// Remove an element from the list at a given index and replace it by the last one of the list (if any)
        void removeAtAndReplaceByLast(uint32 index) {

            assert(index < mSize);

            mBuffer[index] = mBuffer[mSize - 1];

           // Call the destructor of the last element
            mBuffer[mSize - 1].~T();

            mSize--;
        }

        /// Remove an element from the array at a given index and replace it by the last one of the array (if any)
        /// Append another array at the end of the current one
        void addRange(const Array<T>& array, uint32 startIndex = 0) {

            assert(startIndex <= array.size());

            // If we need to allocate more memory
            if (mSize + (array.size() - startIndex) > mCapacity) {

                // Allocate memory
                reserve(mSize + array.size() - startIndex);
            }

            // Add the elements of the array to the current one
            for(uint32 i=startIndex; i<array.size(); i++) {

                new (reinterpret_cast<void*>(mBuffer + mSize)) T(array[i]);
                mSize++;
            }
        }

        /// Clear the array
        void clear(bool releaseMemory = false) {

            // Call the destructor of each element
            for (uint32 i=0; i < mSize; i++) {
                mBuffer[i].~T();
            }

            mSize = 0;

            // If we need to release all the allocated memory
            if (releaseMemory && mCapacity > 0) {

                // Release the memory allocated on the heap
                mAllocator.release(mBuffer, mCapacity * sizeof(T));

                mBuffer = nullptr;
                mCapacity = 0;
            }
        }

        /// Return the number of elements in the array
        uint32 size() const {
            return mSize;
        }

        /// Return the capacity of the array
        uint32 capacity() const {
            return mCapacity;
        }

        /// Overloaded index operator
        T& operator[](const uint32 index) {
           assert(index >= 0 && index < mSize);
           return mBuffer[index];
        }

        /// Overloaded const index operator
        const T& operator[](const uint32 index) const {
           assert(index >= 0 && index < mSize);
           return mBuffer[index];
        }

        /// Overloaded equality operator
        bool operator==(const Array<T>& array) const {

           if (mSize != array.mSize) return false;

            for (uint32 i=0; i < mSize; i++) {
                if (mBuffer[i] != array[i]) {
                    return false;
                }
            }

            return true;
        }

        /// Overloaded not equal operator
        bool operator!=(const Array<T>& array) const {

            return !((*this) == array);
        }

        /// Overloaded assignment operator
        Array<T>& operator=(const Array<T>& array) {

            if (this != &array) {

                // Clear all the elements
                clear();

                // Add all the elements of the array to the current one
                addRange(array);
            }

            return *this;
        }

        /// Return a begin iterator
        Iterator begin() const {
            return Iterator(mBuffer, 0, mSize);
        }

        /// Return a end iterator
        Iterator end() const {
            return Iterator(mBuffer, mSize, mSize);
        }
};

}

#endif
