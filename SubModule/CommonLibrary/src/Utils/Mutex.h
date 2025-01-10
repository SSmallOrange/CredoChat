#pragma once
#include <mutex>
#include <atomic>

#include "NoCopyable.h"

namespace CommonModule {

	template<class T>
	class ScopedLock {
	public:
		ScopedLock(T& mutex) : _mutex(mutex) {
			_mutex.lock();
			_locked = true;
		}
		~ScopedLock() {
			unlock();
		}
		void lock() {
			if (!_locked) {
				_mutex.lock();
				_locked = true;
			}
		}
		void unlock() {
			if (_locked) {
				_locked = false;
				_mutex.unlock();
			}
		}
	private:
		T& _mutex;
		bool _locked;
	};

	class CASLock : public Noncopyable {
	public:
		using Lock = ScopedLock<CASLock>;
		CASLock() {
			_mutex.clear();
		}
		~CASLock() {
		}
		void lock() {
			while (std::atomic_flag_test_and_set_explicit(&_mutex, std::memory_order_acquire));
		}
		void unlock() {
			std::atomic_flag_clear_explicit(&_mutex, std::memory_order_release);
		}
	private:
		volatile std::atomic_flag _mutex;
	};
	
	class Mutex : public Noncopyable {
	public:
		using Lock = ScopedLock<Mutex>;
		Mutex() = default;
		~Mutex() = default;
		void lock() {
			_mutex.lock();
		}
		void unlock() {
			_mutex.unlock();
		}
	private:
		std::mutex _mutex;
	};
}