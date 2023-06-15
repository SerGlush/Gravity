#pragma once
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <compare>
#include <memory>

namespace evo
{
	template<typename T> 
	class Array {
	public:
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using size_type = size_t;

		template<typename TPointer, typename TReference>
		class iterator_impl {
		public:
			using difference_type = typename std::ptrdiff_t;
			using value_type = typename Array::value_type;
			using reference = TReference;
			using pointer = TPointer;
			using iterator_category = std::random_access_iterator_tag;

			reference operator*() const {
				return *m_pointer;
			}
			pointer operator->() const {
				return m_pointer;
			}

			iterator_impl& operator++() {
				++m_pointer;
				return *this;
			}
			iterator_impl operator++(int) {
				return m_pointer++;
			}
			iterator_impl& operator--() {
				--m_pointer;
				return *this;
			}
			iterator_impl operator--(int) {
				return m_pointer--;
			}
			iterator_impl& operator+=(size_type a) {
				m_pointer += a;
				return *this;
			}
			iterator_impl& operator-=(size_type a) {
				m_pointer -= a;
				return *this;
			}

			iterator_impl operator+(size_type a) const {
				return iterator_impl(m_pointer + a);
			}
			friend iterator_impl operator+(size_type a, iterator_impl it) {
				return iterator_impl(it.m_pointer + a);
			}
			iterator_impl operator-(size_type a) const {
				return iterator_impl(m_pointer - a);
			}
			difference_type operator-(iterator_impl it) const {
				return m_pointer - it.m_pointer;
			}

			constexpr auto operator<=>(const iterator_impl&) const noexcept = default;

			iterator_impl() : m_pointer(nullptr) { }

			friend class Array;
		private:
			pointer m_pointer;

			iterator_impl(pointer p) : m_pointer(p) { }
		};

		using iterator = iterator_impl<pointer, reference>;
		using const_iterator = iterator_impl<const_pointer, const_reference>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		~Array() { free(); }

		constexpr explicit Array(size_type size) requires std::is_default_constructible_v<T> 
			: m_size(size), m_pointer(new T[size]) { }

		template<typename... TArgs> requires std::is_constructible_v<T, TArgs...>
		constexpr Array(size_type size, TArgs&&... args) 
			: m_size(size), m_pointer(static_cast<pointer>(operator new[](sizeof(T) * size))) 
		{ 
			if constexpr (!std::is_trivially_constructible_v<T>) {
				for (size_type i = 0; i < size; ++i) {
					new ( &m_pointer[i] ) T{std::forward<TArgs>(args)...};
				}
			}
		}
		
		constexpr Array(const Array& arr) requires std::is_copy_constructible_v<T>
			: m_size(arr.m_size), m_pointer(static_cast<pointer>(operator new[](sizeof(T) * arr.m_size))) 
		{ 
			for (size_t i = 0; i < m_size; ++i) {
				new ( &m_pointer[i] ) T(arr[i]);
			}
		}

		template<std::input_iterator I> 
		requires std::constructible_from<T, typename std::iterator_traits<I>::value_type>
		constexpr Array(I first, I last) : m_size(std::distance(first, last)) 
		{
			m_pointer = static_cast<pointer>( operator new[](sizeof(T) * m_size) );
			for (auto p = m_pointer; first != last; ++first, ++m_pointer) new ( p ) T { *first };
		}

		constexpr Array(Array&& arr) noexcept : m_size(arr.m_size), m_pointer(arr.m_pointer) { 
			arr.m_pointer = nullptr; 
			arr.m_size = 0;
		}

		constexpr Array& operator=(const Array& arr) {
			free();
			m_size = arr.m_size;			
			m_pointer = operator new[](sizeof(T)* arr.m_size);
			for (size_t i = 0; i < m_size; ++i) {
				new ( &m_pointer[i] ) T(arr[i]);
			}
			return *this;
		}

		constexpr Array& operator=(Array&& arr) {
			free();
			m_size = arr.m_size;
			m_pointer = arr.m_pointer;
			arr.m_pointer = nullptr;
			arr.m_size = 0;
			return *this;
		}

		constexpr reference operator[](size_type i) { 
#ifndef NDEBUG
			if (i >= m_size) throw std::out_of_range("evo::Array subscript out of range.");
#endif
			return m_pointer[i];
		}
		constexpr const_reference operator[](size_type i) const {
#ifndef NDEBUG
			if (i >= m_size) throw std::out_of_range("evo::Array subscript out of range.");
#endif
			return m_pointer[i];
		}

		constexpr reference at(size_type i) {
			if (i >= m_size) throw std::out_of_range("evo::Array subscript out of range.");
			return m_pointer[i];
		}
		constexpr const_reference at(size_type i) const {
			if (i >= m_size) throw std::out_of_range("evo::Array subscript out of range.");
			return m_pointer[i];
		}

		constexpr size_t size() const noexcept {
			return m_size;
		}

		constexpr std::make_signed_t<size_t> ssize() const noexcept {
			return static_cast<std::make_signed_t<size_t>>(m_size);
		}

		constexpr size_t bytes() const noexcept {
			return m_size * sizeof(T);
		}

		constexpr [[nodiscard]] bool empty() const noexcept {
			return m_size == 0;
		}

		constexpr pointer data() noexcept {
			return m_pointer;
		}
		constexpr const_pointer data() const noexcept {
			return m_pointer;
		}

		constexpr bool operator==(const Array& arr) const noexcept {
			return m_pointer == arr.m_pointer;
		}
		constexpr bool operator!=(const Array& arr) const noexcept {
			return m_pointer != arr.m_pointer;
		}

		constexpr friend void swap(Array& l, Array& r) noexcept {
			using std::swap;
			swap(l.m_pointer, r.m_pointer);
			swap(l.m_size, r.m_size);
		}

		iterator begin()					{ return iterator(m_pointer);			}
		iterator end()						{ return iterator(m_pointer + m_size);	}
		reverse_iterator rbegin()			{ return reverse_iterator(end());		}
		reverse_iterator rend()				{ return reverse_iterator(begin());		}

		const_iterator cbegin()	const			{ return const_iterator(m_pointer);				}
		const_iterator cend() const				{ return const_iterator(m_pointer + m_size);	}
		const_reverse_iterator crbegin() const	{ return const_reverse_iterator(cend());		}
		const_reverse_iterator crend() const	{ return const_reverse_iterator(cbegin());		}

	private:
		pointer m_pointer;
		size_type m_size;

		void free() {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (std::make_signed_t<size_type> i = m_size - 1; i >= 0; --i) std::destroy_at(m_pointer + i);
			}

			operator delete[](m_pointer);
		}
	};
}