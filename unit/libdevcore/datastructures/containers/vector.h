//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_VECTOR_H
#define UNIT_VECTOR_H

#include <vector>
#include "mutex"

namespace unit {
    template <class T, class Allocator = std::allocator<T>>
    class vector {
    public:
        typedef typename std::vector<T, Allocator>::iterator iterator;
        typedef typename std::vector<T, Allocator>::const_iterator const_iterator;
        typedef typename std::vector<T, Allocator>::reverse_iterator reverse_iterator;
        typedef typename std::vector<T, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::vector<T, Allocator>::allocator_type allocator_type;
        typedef typename std::vector<T, Allocator>::value_type value_type;
        typedef typename std::vector<T, Allocator>::size_type size_type;
        typedef typename std::vector<T, Allocator>::difference_type difference_type;
        explicit vector(const Allocator & alloc = Allocator()) : storage( alloc ) { }
        explicit vector(size_type n, const T & value = T(), const Allocator & alloc = Allocator()) : storage( n, value, alloc ) { }
        template <class InputIterator> vector(InputIterator first, InputIterator last, const Allocator & alloc = Allocator()) : storage( first, last, alloc ) { }
        vector( const unit::vector<T, Allocator> & x ) { std::lock_guard<std::mutex> lock( x.mutex ); storage = x.storage; }

        unit::vector<T, Allocator> & operator=( const unit::vector<T, Allocator> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); this->storage = x.storage; return *this;}

        ~vector<T, Allocator>() {}
        iterator begin() { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        const_iterator begin() const { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        iterator end() { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
        const_iterator end() const { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
        reverse_iterator rbegin() { std::lock_guard<std::mutex> lock( mutex ); return storage.rbegin(); }
        const_reverse_iterator rbegin() const { std::lock_guard<std::mutex> lock( mutex ); return storage.rbegin(); }
        reverse_iterator rend() { std::lock_guard<std::mutex> lock( mutex ); return storage.rend(); }
        const_reverse_iterator rend() const { std::lock_guard<std::mutex> lock( mutex ); return storage.rend(); }
        void remove(T &t) { std::lock_guard<std::mutex> lock( mutex ); std::remove(storage.begin(), storage.end(), t); }
        size_type size() const { std::lock_guard<std::mutex> lock( mutex ); return storage.size(); }
        size_type max_size() const { std::lock_guard<std::mutex> lock( mutex ); return storage.max_size(); }
        void resize( size_type n, T c = T() ) { std::lock_guard<std::mutex> lock( mutex ); storage.resize( n, c ); }
        size_type capacity() const { std::lock_guard<std::mutex> lock( mutex ); return storage.capacity(); }
        bool empty() const { std::lock_guard<std::mutex> lock( mutex ); return storage.empty(); }
        void reserve(size_type n) { std::lock_guard<std::mutex> lock( mutex ); this->storage.reserve(n); }
        T & operator[]( size_type n ) { std::lock_guard<std::mutex> lock( mutex ); return storage[n]; }
        const T & operator[]( size_type n ) const { std::lock_guard<std::mutex> lock( mutex ); return storage[n]; }
        T & at( size_type n ) { std::lock_guard<std::mutex> lock( mutex ); return storage.at( n ); }
        const T & at( size_type n ) const { std::lock_guard<std::mutex> lock( mutex ); return storage.at( n ); }
        T & front() { std::lock_guard<std::mutex> lock( mutex ); return storage.front(); }
        const T & front() const { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        T & back() { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        const T & back() const { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        void assign( size_type n, T & u ) { std::lock_guard<std::mutex> lock( mutex ); storage.assign( n, u ); }
        template <class InputIterator> void assign( InputIterator begin, InputIterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.assign( begin, end ); }
        void emplace_back(const T & u) { std::lock_guard<std::mutex> lock( mutex ); storage.emplace_back( u ); }
//        void push_back(const T & u) const { std::lock_guard<std::mutex> lock(mutex); storage.push_back(u); }
        void pop_back() { std::lock_guard<std::mutex> lock( mutex ); storage.pop_back(); }
        iterator insert( iterator pos, const T & u ) { std::lock_guard<std::mutex> lock( mutex ); return storage.insert( pos, u ); }
        void insert( iterator pos, size_type n, const T & u ) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( pos, n, u ); }
        template <class InputIterator> void insert( iterator pos, InputIterator begin, InputIterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( pos, begin, end ); }
        void erase( iterator pos ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( pos ); }
        void erase( iterator begin, iterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( begin, end ); }
        void swap( unit::vector<T, Allocator> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); storage.swap( x.storage ); }
        void clear() { std::lock_guard<std::mutex> lock( mutex ); storage.clear(); }
        allocator_type get_allocator() const { std::lock_guard<std::mutex> lock( mutex ); return storage.get_allocator(); }
    private:
        mutable std::mutex mutex;
        std::vector<T, Allocator> storage;
    };
}
#endif //UNIT_VECTOR_H
