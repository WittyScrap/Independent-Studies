#pragma once
#include <stdlib.h>

/// <summary>
/// Represents a list of a generic type.
/// </summary>
template<typename T>
class List
{
public:
	/// <summary>
	/// Creates an empty list.
	/// </summary>
	inline List();

	/// <summary>
	/// Creates a list with a starting capacity.
	/// </summary>
	inline List(size_t capacity);

	/// <summary>
	/// Copies one list into this one.
	/// </summary>
	inline List(const List<T>& copy);

	// Destructor
	//
	inline ~List();


	/// <summary>
	/// Adds an item to the list.
	/// </summary>
	inline size_t Add(T item);

	/// <summary>
	/// Removes an item from the list.
	/// </summary>
	inline void Remove(size_t item);


	/// <summary>
	/// Finds an item in the list.
	/// </summary>
	inline size_t Find(const T& item);

	/// <summary>
	/// Accesses the itemth item in the list.
	/// </summary>
	__forceinline const T& operator[](size_t item);

	/// <summary>
	/// The length of the list.
	/// </summary>
	__forceinline size_t GetLength()
	{
		return this->len;
	}

private:
	size_t capacity;
	size_t len;

	T* buff;

};

/// <summary>
/// Creates an empty list.
/// </summary>
template<typename T>
inline List<T>::List()
	: capacity{ 1 }, len{ 0 }, buff{ new T[1] }
{ }

/// <summary>
/// Creates a list with a starting capacity.
/// </summary>
template<typename T>
inline List<T>::List(size_t capacity)
	: capacity{ capacity }, len{ 0 }, buff{ new T[capacity] }
{ }

/// <summary>
/// Copies one list into this one.
/// </summary>
template<typename T>
inline List<T>::List(const List<T>& copy)
	: capacity{ copy.capacity }, len{ copy.len }, buff{ new T[capacity] }
{
	memcpy(this->buff, copy.buff, sizeof(T) * capacity);
}

// Destructor
//
template<typename T>
inline List<T>::~List()
{
	delete this->buff;
}

/// <summary>
/// Adds an item to the list.
/// </summary>
template<typename T>
inline size_t List<T>::Add(T item)
{
	this->len += 1;

	if (this->len > this->capacity)
	{
		T* newbuff = new T[this->capacity * 2];
		memcpy(newbuff, this->buff, sizeof(T) * this->capacity);
		this->capacity *= 2;
		delete this->buff;
		this->buff = newbuff;
	}

	this->buff[this->len - 1] = item;
	return this->len - 1;
}

/// <summary>
/// Removes an item from the list.
/// </summary>
template<typename T>
inline void List<T>::Remove(size_t item)
{
	memcpy(this->buff + item, this->buff + item + 1, this->len - (item + 1));
	this->len -= 1;
}

/// <summary>
/// Finds an item in the list.
/// </summary>
template<typename T>
inline size_t List<T>::Find(const T& item)
{
	for (int i = 0; i < this->len; i += 1)
	{
		if (this->buff[i] == item)
		{
			return i;
		}
	}

	return -1;
}

/// <summary>
/// Accesses the itemth item in the list.
/// </summary>
template<typename T>
__forceinline const T& List<T>::operator[](size_t item)
{
	return this->buff[item];
}
