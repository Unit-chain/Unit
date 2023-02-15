//
// Created by Kirill Zhukov on 13.02.2023.
//

#ifndef UNIT_QUEUE_H
#define UNIT_QUEUE_H
#if 0
#include "queue"
#include "mutex"

namespace unit {
    template <class T>
    class queue {
    public:
        typedef typename std::queue<T>::iterator iterator;
        typedef typename std::queue<T>::size_type size_type;
        explicit queue(size_type n, const T & value = T()) : storage( n, value ) { }
        template <class InputIterator> queue(InputIterator first, InputIterator last) : storage( first, last) { }
        queue( const unit::queue<T> & x ) { std::lock_guard<std::mutex> lock( x.mutex ); storage = x.storage; }
        unit::queue<T> & operator=( const unit::queue<T> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); this->storage = x.storage; return *this;}
        ~queue<T>() {}
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
        void pop_back() { std::lock_guard<std::mutex> lock( mutex ); storage.pop_back(); }
        void erase( iterator pos ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( pos ); }
        void erase( iterator begin, iterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( begin, end ); }
        void swap( unit::queue<T> & x ) { std::lock_guard<std::mutex> lock( mutex ); std::lock_guard<std::mutex> lock2( x.mutex ); storage.swap( x.storage ); }
        void clear() { std::lock_guard<std::mutex> lock( mutex ); storage.clear(); }
    private:
        mutable std::mutex mutex;
        std::queue<T> storage;
    };
}
#endif
#endif //UNIT_QUEUE_H
