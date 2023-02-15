//
// Created by Kirill Zhukov on 13.02.2023.
//

#ifndef UNIT_UNORDERED_SET_H
#define UNIT_UNORDERED_SET_H

#include "unordered_set"
#include "mutex"

namespace unit {
    template <class T>
    class unordered_set {
    public:
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;
        typedef typename std::unordered_set<T>::value_type value_type;
        typedef typename std::unordered_set<T>::size_type size_type;
        typedef typename std::unordered_set<T>::difference_type difference_type;
        explicit unordered_set(size_type n, const T & value = T()) : storage( n, value ) { }
        template <class InputIterator> unordered_set(InputIterator first, InputIterator last) : storage( first, last ) { }
        unordered_set( const unit::unordered_set<T> & x ) { std::lock_guard<std::mutex> lock( x.mutex ); storage = x.storage; }
        unit::unordered_set<T> & operator=( const unit::unordered_set<T> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); this->storage = x.storage; return *this;}
        ~unordered_set<T>() {}
        void remove(T &t) { std::lock_guard<std::mutex> lock( mutex ); std::remove(storage.begin(), storage.end(), t); }
        void erase(T &t) { storage.erase(t); }
        iterator begin() { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        const_iterator begin() const { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        iterator end() { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
        const_iterator end() const { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
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
        void insert(const T & u) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( u ); }
        void insert(const T * u) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( u ); }
        void pop_back() { std::lock_guard<std::mutex> lock( mutex ); storage.pop_back(); }
        void swap( unit::unordered_set<T> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); storage.swap( x.storage ); }
        void clear() { std::lock_guard<std::mutex> lock( mutex ); storage.clear(); }
    private:
        mutable std::mutex mutex;
        std::unordered_set<T> storage;
    };
}

#endif //UNIT_UNORDERED_SET_H
