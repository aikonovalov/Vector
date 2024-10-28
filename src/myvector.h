#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <cstring>

template <typename ElemType, typename Allocator = std::allocator<ElemType>>
class Vector {
public:
	// Iterators
	class Iterator {
	public:
		Iterator(ElemType* it) : curr_(it) {
		}

		ElemType& operator*() {
			return *curr_;
		}

		ElemType* operator->() {
			return curr_;
		}

		Iterator& operator++() {
			++curr_;
			return *this;
		}
		Iterator operator++(int) {
			Iterator curr_copy = curr_;
			++curr_;
			return curr_copy;
		}
		
		Iterator& operator--() {
			--curr_;
			return *this;
		}
		Iterator operator--(int) {
			Iterator curr_copy = curr_;
			--curr_;
			return curr_copy;
		}

		bool operator==(const Iterator& rhs) {
			return curr_ == rhs.curr_;
		}

		bool operator!=(const Iterator& rhs) {
			return curr_ != rhs.curr_;
		}

	private:
		ElemType* curr_;
	};

	class ConstIterator {
	public:
		ConstIterator(ElemType* it) : curr_(it) {
		}

		const ElemType& operator*() const {
			return *curr_;
		}

		const ElemType* operator->() const {
			return curr_;
		}

		ConstIterator& operator++() {
			++curr_;
			return *this;
		}

		ConstIterator operator++(int) {
			ConstIterator curr_copy = curr_;
			++curr_;
			return curr_copy;
		}
		
		ConstIterator& operator--() {
			--curr_;
			return *this;
		}
		
		ConstIterator operator--(int) {
			ConstIterator curr_copy = curr_;
			--curr_;
			return curr_copy;
		}

		bool operator==(ConstIterator& rhs) {
			return curr_ == rhs.curr_;
		}

		bool operator!=(ConstIterator& rhs) {
			return curr_ != rhs.curr_;
		}

	private:
		ElemType* curr_;
	};

	class ReverseIterator {
	public:
		ReverseIterator(ElemType* it) : curr_(it) {
		}

		ElemType& operator*() {
			return *curr_;
		}

		ElemType* operator->() {
			return curr_;
		}

		ReverseIterator& operator++() {
			--curr_;
			return *this;
		}
		ReverseIterator operator++(int) {
			Iterator curr_copy = curr_;
			--curr_;
			return curr_copy;
		}
		
		ReverseIterator& operator--() {
			++curr_;
			return *this;
		}
		ReverseIterator operator--(int) {
			Iterator curr_copy = curr_;
			++curr_;
			return curr_copy;
		}

		bool operator==(const ReverseIterator& rhs) {
			return curr_ == rhs.curr_;
		}

		bool operator!=(const ReverseIterator& rhs) {
			return curr_ != rhs.curr_;
		}

	private:
		ElemType* curr_;
	};

	// Constructors
	Vector() : allocator_(), size_v(0), capacity_v(0) {
	}

	Vector(size_t new_size_v) : allocator_(), size_v(new_size_v), capacity_v(new_size_v) {
		if (new_size_v > Max_size()) {
			throw std::runtime_error("End_of_space");
		}
		if (new_size_v) {
			vec = new ElemType[new_size_v];
		}
	}

	Vector(size_t new_size_v, ElemType pattern) : allocator_(), size_v(new_size_v), capacity_v(new_size_v) {
		if (new_size_v > Max_size()) {
			throw std::runtime_error("End_of_space");
		}
		if (new_size_v) {
			vec = new ElemType[new_size_v];

			Initialize(begin(), new_size_v, pattern);
		}
	}

	Vector(const Vector<ElemType, Allocator>& other) : allocator_(), size_v(other.size()), capacity_v(other.capacity()) {
		if (!other.empty()) {
			vec = new ElemType[other.capacity()];
			for (size_t i = 0; i < other.size(); ++i) {
				*(data() + i) = other[i];
			}
		}
	}

	Vector(Vector<ElemType, Allocator>&& other) : vec(other.vec), allocator_(), size_v(other.size()), capacity_v(other.capacity()) {
		other.vec = nullptr;
		other.size_v = 0;
		other.capacity_v = 0;
	}

	Vector(const ElemType* begin_, const ElemType* end_) {
		size_t size_massive = 0;
		for (auto it = begin_; it != end_; ++it) {
			++size_massive; 
		}
		if (size_massive) {
			vec = new ElemType[size_massive];
			capacity_v = size_massive;
			size_v = size_massive;
			std::copy(begin_, end_, vec);
		}
	}

	Vector(std::initializer_list<ElemType> list) : allocator_() {
		try {
			MakeAllocation(list.size());
			capacity_v = list.size();

			for (auto it = list.begin(); it != list.end(); ++it) {
				vec[size()] = *it;
				++size_v;
			}

		} catch(std::exception& e) {
			clear();
			throw e;
		}
	}
	
	// Assignments
	Vector<ElemType, Allocator>& operator=(const Vector<ElemType, Allocator>& other) {
		if (this != &other) {
			clear();
			if (!other.empty()) {
				MakeAllocation(other.capacity());
				size_v = other.size();
				capacity_v = other.capacity();
				for (size_t i = 0; i < size(); ++i) {
					*(data() + i) = other[i];
				}
			}
		}
		return *this;
	}
	Vector<ElemType, Allocator> &operator=(Vector<ElemType, Allocator>&& other) {
		if (this != &other) {
			delete[] vec;

			this->vec = other.vec;
			this->capacity_v = other.capacity_v;
			this->size_v = other.size();

			other.vec = nullptr;
			other.size_v = 0;
			other.capacity_v = 0;
		}
		return *this;
	}

	// Setters
	void resize(size_t new_size, const ElemType& elem) {
		while (size() < new_size) {
			push_back(elem);
		}
		
		while (size() > new_size) {
			while (size() > new_size) {
				(vec + (size() - 1))->~ElemType();
				--size_v;
			}
		}
	}

	void resize(size_t new_size) {
		if (new_size <= size()) {
			while (size() > new_size) {
				(vec + (size() - 1))->~ElemType();
				--size_v;
			}
		} else {
			ElemType* new_vec = new ElemType[new_size];
			size_t counter = 0;

			try {
				while (counter != size()) {
					std::swap(vec[counter], new_vec[counter]);
					++counter;
				}
			} catch(std::exception& e) {
				for (size_t i = 0; i <= counter; ++i) {
					std::swap(vec[i], new_vec[i]);
				}
				delete[] new_vec;
				throw e;
			}

			if (vec) {
				delete[] vec;
			}
			vec = new_vec;
			capacity_v = new_size;
			size_v = new_size;
		}
	}

	void reserve(size_t new_capacity) {
		if (new_capacity > capacity()) {
			ElemType* new_vec = new ElemType[new_capacity];
			size_t counter = 0;

			try {
				while (counter != size()) {
					std::swap(vec[counter], new_vec[counter]);
					++counter;
				}
			} catch(std::exception& e) {
				for (size_t i = 0; i <= counter; ++i) {
					std::swap(vec[i], new_vec[i]);
				}
				delete[] new_vec;
				throw e;
			}

			if (vec) {
				delete[] vec;
			}
			vec = new_vec;
			capacity_v = new_capacity;
		}
	}

	void assign(size_t new_size, const ElemType& elem) {
		clear();
		vec = new ElemType[new_size];
		for (size_t i = 0; i < new_size; ++i) {
			vec[i] = elem;
		}
	}

	void shrink_to_fit() {
		if (size() < capacity()) {
			ElemType* new_vec = nullptr;
			if (size()) {
				new_vec = new ElemType[size()];
				size_t counter = 0;
				try {
					while (counter != size()) {
						std::swap(vec[counter], new_vec[counter]);
						++counter;
					}
				} catch(std::exception& e) {
					for (size_t i = 0; i <= counter; ++i) {
						std::swap(vec[i], new_vec[i]);
					}
					delete[] new_vec;
					throw e;
				}
			}
			if (vec) {
				delete[] vec;
			}
			vec = new_vec;
			capacity_v = size();
		}
	}

	void push_back(const ElemType& value) {
		if (size() == capacity()) {
			ReallocateIf();
		}
		vec[size()] = value;
		++this->size_v;
	}

	void push_back(ElemType&& value) {
		if (size() == capacity()) {
			ReallocateIf();
		}
		vec[size()] = std::forward<ElemType>(value);
		++this->size_v;
	}

	template<typename... Args>
	void emplace_back(Args... args) {
		if (size() == capacity()) {
			MakeAllocation(capacity() + 1);
		}
		vec[size()] = ElemType(std::forward<Args>(args)...);
		++size_v;
	}

	void pop_back() {
		if (this->size()) {
			(vec + (size_v - 1))->~ElemType();
			--this->size_v;
			ReallocateIf();
		}
	}

	void swap(Vector<ElemType, Allocator>& other) {
		std::swap(vec, other.vec);
		std::swap(size_v, other.size_v);
		std::swap(capacity_v, other.capacity_v);
		std::swap(allocator_, other.allocator_);
	}

	// Getters
	bool empty() const {
		return this->size() == 0;
	}

	size_t size() const {
		return this->size_v;
	}

	size_t capacity() const {
		return this->capacity_v;
	}

	ElemType& front() {
		return *vec;
	}
	const ElemType& front() const {
		return *vec;
	}

	ElemType& back() {
		return *(vec + (size_v - 1));
	}
	const ElemType& back() const {
		return *(vec + (size_v - 1));
	}

	Iterator begin() {
		return Iterator(vec);
	}

	Iterator end() {
		return Iterator(vec + size());
	}

	ConstIterator cbegin() {
		return ConstIterator(vec);
	}

	ConstIterator cend() {
		return ConstIterator(vec + size());
	}

	ReverseIterator rbegin() {
		return ReverseIterator(vec + (size() - 1));
	}

	ReverseIterator rend() {
		return ReverseIterator(vec);
	}

	ElemType& operator[](size_t index) {
		if (index >= size() || index < 0) {
			throw std::out_of_range("Out of range");
		}
		return *(vec + index);
	}

	const ElemType& operator[](size_t index) const {
		if (index >= size() || index < 0) {
			throw std::out_of_range("Out of range");
		}
		return *(vec + index);
	}
	template <typename T, typename Alloc>
	friend bool operator==(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);
	template <typename T, typename Alloc>
	friend bool operator!=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);
	template <typename T, typename Alloc>
	friend bool operator<(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);
	template <typename T, typename Alloc>
	friend bool operator>(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);
	template <typename T, typename Alloc>
	friend bool operator<=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);
	template <typename T, typename Alloc>
	friend bool operator>=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs);


	ElemType* data() {
		return vec;
	}

	const ElemType* data() const {
		return vec;
	}

	ElemType& at(size_t index) {
		if (index < 0 || index >= size()) {
			throw std::out_of_range("Invalid index");
		}
		return vec[index];
	}

	const ElemType& at(size_t index) const {
		if (index < 0 || index >= size()) {
			throw std::out_of_range("Invalid index");
		}
		return vec[index];
	}

	// Free
	void clear() {
		if (vec) {
			delete[] vec;
		}
		vec = nullptr;
		capacity_v = 0;
		size_v = 0;
	}

	~Vector() {
		if (vec) {
			delete[] vec;
		}
	}

private:
	static size_t Max_size() {
		return std::numeric_limits<size_t>::max();
	}

	bool SizeCheck(size_t n) {
		if (n > Max_size()) {
			throw std::runtime_error("Cannot make such a large vector");
		}

	}

	void MakeAllocation(size_t new_capacity) {
		ElemType* new_vec = nullptr;
		size_t counter = 0;
		if (new_capacity) {
			try {
				new_vec = new ElemType[new_capacity];
				while (counter != std::min(this->size(), new_capacity)) {
					std::swap(new_vec[counter], vec[counter]);
					++counter;
				}
			} catch(std::exception& e) {
				if (new_vec) {
					for (size_t i = 0; i <= counter; ++i) {
						std::swap(new_vec[i], vec[i]);
					}
					delete[] new_vec;
				}
				throw e;
			}
		}
		if (vec) {
			delete[] vec;
		}
		vec = new_vec;
		capacity_v = new_capacity;
	}

	void ReallocateIf() {
		if (size() * 4 <= capacity()) {
			MakeAllocation((capacity_v / 4) + 1);
		} else if (size() == capacity()) {
			try {
				MakeAllocation(size_v * 2);
			} catch(std::exception& e) {
				try {
					MakeAllocation(size_v + 1);
				} catch(std::exception& e) {
					throw;
				}
			}
		}
	}

	void Initialize(Iterator first, size_t n, ElemType& elem) {
		Iterator curr = first;
		size_t elem_counter = 0;
		try {
			for (size_t i = 0; i < n; ++i) {
				*curr = elem;
				++curr;
			}

		} catch(std::exception& e) {
			for (size_t i = 0; i < elem_counter; ++i) {
				delete &(*curr);
				--curr;
			}
			throw e;
		}
	}

private:
	ElemType* vec = nullptr;
	Allocator allocator_;
	size_t size_v = 0;
	size_t capacity_v = 0;
};

template <typename T, typename Alloc>
bool operator==(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	for (size_t i = 0; i < lhs.size(); ++i) {
		if (lhs.at(i) != rhs.at(i)) {
			return false;
		}
	}
	return true;
}

template <typename T, typename Alloc>
bool operator!=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator<(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator>(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
bool operator<=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	return !(lhs > rhs);
}

template <typename T, typename Alloc>
bool operator>=(const Vector<T, Alloc> &lhs, const Vector<T, Alloc> &rhs) {
	return !(lhs < rhs);
}