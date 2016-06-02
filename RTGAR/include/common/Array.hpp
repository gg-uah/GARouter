#ifndef __ARRAY_HPP_
#define __ARRAY_HPP_

#include <MatrixException.hpp>
#include <iostream>

namespace common {

template <class Type>
class Array {
public:
    /**
     * Default constructor
     */
    Array();

    /**
     * Array parameterized constructor with elements initialized to zero value
     *
     * @param dim   Size of the array
     */
    Array(const unsigned short & dim)
        throw (MatrixException);

    /**
     * Array parameterized constructor with a given value
     *
     * @param dim   Size of the array
     * @param value Value used for initializing all the matrix elements
     */
    Array(const unsigned short & dim,
          const Type & value)
        throw (MatrixException);

    /**
     * Copy constructor
     *
     * @param other The other Array object to copy
     */
    Array(const Array & other)
        throw(MatrixException);

    /**
     * Destructor
     */
    virtual ~Array();

    /**
     * Return a reference to the element located at a position
     *
     * @param pos   The position of the element in the array
     * @return      The element located at the position
     */
    Type & operator() (const unsigned short & pos)
        throw(MatrixException);

    /**
     * Return a copy of the element located at a position
     *
     * @param pos   The position of the element in the array
     * @return      The element located at the position
     */
    Type operator() (const unsigned short & pos) const
        throw(MatrixException);

    /**
     * Return a reference to the element located at a position
     *
     * @param pos   The position of the element in the array
     * @return      The element located at the position
     */
    Type & operator[] (const unsigned short & pos)
        throw(MatrixException);

    /**
     * Return a copy of the element located at a position
     *
     * @param pos   The position of the element in the array
     * @return      The element located at the position
     */
    Type operator[] (const unsigned short & pos) const
        throw(MatrixException);

    /**
     * Assignment operator overloading
     *
     * @param other The other array from which to copy its elements
     * @return      A copy of the other array passed as a parameter
     */
    Array & operator= (Array const & other)
        throw(MatrixException);

    /**
     * 'Equal than' comparison operator overloading
     *
     * @param other The right hand array operand of the comparison
     * @return      True if this left hand array is equal than the right one, false otherwise
     */
    bool operator== (Array const & other) const
        throw(MatrixException);

    /**
     * Return the size of the array
     *
     * @return  The size of the array
     */
    unsigned short getSize() const {
        return this->size;
    }

    /**
     * Return a only-readable reference to the first element of the array
     *
     * @return  The reference to the first element of the array
     */
    const Type * begin() const {
        return const_cast<Type*>(this->pElements);
    }

    /**
     * Return a only-readable reference to the last element of the array
     *
     * @return  The reference to the last element of the array
     */
    const Type * end() const {
        return const_cast<Type*>(this->pElements + this->size);
    }

    /**
     * Check wheter an array is empty or not
     *
     * @return  True if the array has no elements, false otherwise
     */
    bool empty() const {
        if (this->size == 0) {
            return true;
        }
        return false;
    }

private:
    unsigned short size; /**< Number of elements of the array */

    Type * pElements;    /**< A pointer to the begin of the array of elements */

};



//................................................. Default constructor ...
template <class Type>
Array<Type>::Array() {
    this->size = 0;
    this->pElements = NULL;
}


//................................................. Parameterized default constructor ...
template <class Type>
Array<Type>::Array(const unsigned short & dim)
    throw(MatrixException) {

    this->size = dim;
    this->pElements = NULL;

    if (dim == 0) {
        throw MatrixException(MATEXC_BAD_DIMENSIONS);
    }

    this->pElements = new (std::nothrow) Type[dim];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < dim; i++) {
        this->pElements[i] = 0;
    }
}


//................................................. Parameterized initial value constructor ...
template <class Type>
Array<Type>::Array(const unsigned short & dim,
                   const Type & value)
    throw(MatrixException) {

    this->size = dim;
    this->pElements = NULL;

    if (dim == 0) {
        throw MatrixException(MATEXC_BAD_DIMENSIONS);
    }

    this->pElements = new (std::nothrow) Type[dim];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < dim; i++) {
        this->pElements[i] = value;
    }
}


//................................................. Copy constructor ...
template <class Type>
Array<Type>::Array(const Array<Type> & other)
    throw(MatrixException) {

    this->size = other.size;
    this->pElements = NULL;

    if(other.size == 0) {
        return;
    }

    this->pElements = new (std::nothrow) Type [other.size];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < other.size; i++) {
        this->pElements[i] = other.pElements[i];
    }
}


//................................................. Destructor ...
template <class Type>
Array<Type>::~Array() {
    if (this->pElements != NULL) {
        delete [] this->pElements;
    }
}


//................................................. Array element writable access ...
template <class Type>
Type & Array<Type>::operator() (const unsigned short & pos)
    throw(MatrixException) {

    if (pos >= this->size) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[pos];
}


//................................................. Array element read-only access ...
template <class Type>
Type Array<Type>::operator() (const unsigned short & pos) const
    throw(MatrixException){

    if (pos >= this->size) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[pos];
}


//................................................. Array element writable access ...
template <class Type>
Type & Array<Type>::operator[](const unsigned short & pos)
    throw(MatrixException) {

    if (pos >= this->size) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[pos];
}


//................................................. Array element read-only access ...
template <class Type>
Type Array<Type>::operator[] (const unsigned short & pos) const
    throw(MatrixException){

    if (pos >= this->size) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[pos];
}


//................................................. Assignment operator overloading ...
template <class Type>
Array<Type> & Array<Type>::operator= (Array<Type> const & other)
    throw(MatrixException) {

    if (this == &other) {
        return *this;
    }

    this->size = other.size;
    this->pElements = NULL;

    if(other.size == 0) {
        return *this;
    }

    this->pElements = new (std::nothrow) Type [other.size];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < other.size; i++) {
        this->pElements[i] = other.pElements[i];
    }

    return *this;
}


//................................................. 'Equal than' comparison operator overloading ...
template <class Type>
bool Array<Type>::operator== (Array<Type> const & other) const
    throw(MatrixException) {

    if(this->size != other.size) {
        return false;
    }

    for (int i = 0; i < other.size; i++) {
        if ((*this)(i) != other(i)) {
            return false;
        }
    }

    return true;
}

} /* namespace */
#endif /* __ARRAY_HPP_ */
