//-----------------------------
// James Payor - Evolution RTS
// Created 30/05/13
//-----------------------------
//
//-------------
// Atomic.hpp |
//-------------------------------------------
// Implements the atomic template class,
// which allows thread-safe access and
// modification of data.
//-------------------------------------------

#ifndef ATOMIC_H
#define ATOMIC_H

// Required header:
#include "threading.hpp"



template<class T>
class atomic {
private:
	// The data and mutex involved:
	volatile T data;
	Threading::Mutex lock;

public:
	// Constructors:
	atomic() {}
	atomic(const atomic<T>& other) : data(other.get()) {}
	atomic(const T& data) : data(data) {}

	// Simple get and cast operations:
	inline T get() const {
		lock.Lock();
		T res = data;
		lock.Unlock();
		return res;
	}
	inline operator T() {
		return get();
	}

	// Reserves the data and retrieves it:
	inline T resget() const {
		lock.Lock();
		return data;
	}

	// Sets the data:
	inline void set(const T& value) {
		lock.Lock();
		data = value;
		lock.Unlock();
	}

	// Overloaded equal operators:
	inline atomic<T>& operator=(const atomic<T>& other) {
		set(other.get());
		return *this;
	}
	inline atomic<T>& operator=(const T& other) {
		set(other);
		return *this;
	}

	// Helper functions for controlling the state of locks:
	inline bool reserve() const {
		return lock.Lock();
	}
	inline bool release() const {
		return lock.Unlock();
	}

	// To overload a lot of operators, I define a macro that overloads something= operators:
#define ops(op) \
	inline atomic<T>& operator op (const atomic<T>& other) { \
		lock.Lock(); \
		data op other.get(); \
		lock.Unlock(); \
		return *this; \
	} \
	inline atomic<T>& operator op (const T& other) { \
		lock.Lock(); \
		data op other; \
		lock.Unlock(); \
		return *this; \
	}

	// And apply the macro to everything we can think of!  This expands out into the code above repeated several times.
	ops(+=);
	ops(-=);
	ops(*=);
	ops(/=);
	ops(&=);
	ops(|=);
	ops(<<=);
	ops(>>=);

	// Other useful overloaded operators:
	inline T operator-() const {
		return -get();
	}
	inline T operator~() const {
		return ~get();
	}
	inline T operator++() const {
		T tmp = resget();
		set(++tmp);
		return tmp;
	}
};


// To overload many other operators, another macro to handle gets and sets:
#define operators(op) \
	template<class T> \
	inline T operator op(const atomic<T>& lhs, const atomic<T>& rhs) { \
		return lhs.get() op rhs.get(); \
	} \
	template<class T> \
	inline T operator op(const atomic<T>& lhs, const T& rhs) { \
		return lhs.get() op rhs; \
	} \
	template<class T> \
	inline T operator op (const T& lhs, const atomic<T>& rhs) { \
		return lhs op rhs.get(); \
	}

// And applying it to all the things:
operators(+);
operators(-);
operators(*);
operators(/);
operators(^);
operators(&);
operators(&&);
operators(|);
operators(||);
operators(==);
operators(!=);
operators(<<);
operators(>>);

#endif
