//
// Created by Kirill Zhukov on 07.09.2022.
//

#ifndef UVM_LIST_H
#define UVM_LIST_H

#include <list>
#include "mutex"

namespace unit {
    template < class T, class Allocator = std::allocator<T> >
    class list {
    public:
        typedef typename std::list<T, Allocator>::iterator iterator;
        typedef typename std::list<T, Allocator>::const_iterator const_iterator;
        typedef typename std::list<T, Allocator>::reverse_iterator reverse_iterator;
        typedef typename std::list<T, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::list<T, Allocator>::allocator_type allocator_type;
        typedef typename std::list<T, Allocator>::value_type value_type;
        typedef typename std::list<T, Allocator>::size_type size_type;
        typedef typename std::list<T, Allocator>::difference_type difference_type;

        explicit list( const Allocator & alloc = Allocator() ) : storage( alloc ) { }
        explicit list(std::list<T, Allocator> list) : storage( list ) { }
        explicit list( size_type n, const T & value = T(), const Allocator & alloc = Allocator() ) : storage( n, value, alloc ) { }
        template <class InputIterator> list( InputIterator first, InputIterator last, const Allocator & alloc = Allocator() ) : storage( first, last, alloc ) { }
        list( const unit::list<T, Allocator> & x ) { std::lock_guard<std::mutex> lock(x.mutex ); storage = x.storage; }
        unit::list<T,Allocator>& operator=(const unit::list<T,Allocator>& x ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage = x.storage; return *this;}
        ~list<T,Allocator>() = default;
        iterator begin() { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        const_iterator begin() const { std::lock_guard<std::mutex> lock( mutex ); return storage.begin(); }
        iterator end() { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
        const_iterator end() const { std::lock_guard<std::mutex> lock( mutex ); return storage.end(); }
        reverse_iterator rbegin() { std::lock_guard<std::mutex> lock( mutex ); return storage.rbegin(); }
        const_reverse_iterator rbegin() const { std::lock_guard<std::mutex> lock( mutex ); return storage.rbegin(); }
        reverse_iterator rend() { std::lock_guard<std::mutex> lock( mutex ); return storage.rend(); }
        const_reverse_iterator rend() const { std::lock_guard<std::mutex> lock( mutex ); return storage.rend(); }
        size_type size() const { std::lock_guard<std::mutex> lock( mutex ); return storage.size(); }
        size_type max_size() const { std::lock_guard<std::mutex> lock( mutex ); return storage.max_size(); }
        void resize( size_type n, T c = T() ) { std::lock_guard<std::mutex> lock( mutex ); storage.resize( n, c ); }
        bool empty() const { std::lock_guard<std::mutex> lock( mutex ); return storage.empty(); }
        T & front() { std::lock_guard<std::mutex> lock( mutex ); return storage.front(); }
        const T & front() const { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        T & back() { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        const T & back() const { std::lock_guard<std::mutex> lock( mutex ); return storage.back(); }
        void assign( size_type n, T & u ) { std::lock_guard<std::mutex> lock( mutex ); storage.assign( n, u ); }
        template <class InputIterator> void assign( InputIterator begin, InputIterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.assign( begin, end ); }
        void emplace_back(const T & u ) { std::lock_guard<std::mutex> lock(mutex ); storage.template emplace_back(u ); }
        void pop_back() { std::lock_guard<std::mutex> lock( mutex ); storage.pop_back(); }
        void push_front( const T & u ) { std::lock_guard<std::mutex> lock( mutex ); storage.push_front( u ); }
        void pop_front() { std::lock_guard<std::mutex> lock( mutex ); storage.pop_front(); }
        iterator insert( iterator pos, const T & u ) { std::lock_guard<std::mutex> lock( mutex ); return storage.insert( pos, u ); }
        void insert( iterator pos, size_type n, const T & u ) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( pos, n, u ); }
        template <class InputIterator> void insert( iterator pos, InputIterator begin, InputIterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.insert( pos, begin, end ); }
        void erase( iterator pos ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( pos ); }
        void erase( iterator begin, iterator end ) { std::lock_guard<std::mutex> lock( mutex ); storage.erase( begin, end ); }
        void swap(unit::list<T, Allocator> & x ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.swap(x.storage ); }
        void clear() { std::lock_guard<std::mutex> lock( mutex ); storage.clear(); }
        void splice (iterator position, unit::list<T,Allocator>& x ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.splice(position, x.storage ); }
        void splice (iterator position, unit::list<T,Allocator>& x, iterator i ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.splice(position, x.storage, i ); }
        void splice (iterator position, unit::list<T,Allocator>& x, iterator first, iterator last ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.splice(position, x.storage, first, last ); }
        void remove ( const T& value ) { std::lock_guard<std::mutex> lock( mutex ); storage.remove( value ); }
        template <class Predicate> void remove_if ( Predicate pred ) { std::lock_guard<std::mutex> lock( mutex ); storage.remove_if( pred ); }
        void unique () { std::lock_guard<std::mutex> lock( mutex ); storage.unique(); }
        template <class BinaryPredicate> void unique ( BinaryPredicate binary_pred ) { std::lock_guard<std::mutex> lock( mutex ); storage.unique( binary_pred ); }
        void merge (unit::list<T,Allocator>& x ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.merge(x.storage() ); }
        template <class Compare> void merge (unit::list<T,Allocator>& x, Compare comp ) { std::lock_guard<std::mutex> lock(mutex ); std::lock_guard<std::mutex> lock2(x.mutex ); storage.merge(x.storage, comp ); }
        void sort () { std::lock_guard<std::mutex> lock( mutex ); storage.sort(); }
        template <class Compare> void sort ( Compare comp ) { std::lock_guard<std::mutex> lock( mutex ); storage.sort( comp ); }
        void reverse() { std::lock_guard<std::mutex> lock( mutex ); }
        allocator_type get_allocator() const { std::lock_guard<std::mutex> lock( mutex ); return storage.get_allocator(); }
    private:
        std::list<T, Allocator> storage;
        mutable std::mutex mutex;
    };
}

#endif //UVM_LIST_H
