#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <MatrixException.hpp>
#include <iostream>

namespace common {

template <class Type>
class Matrix {
public:
    /**
     * Default constructor
     */
    Matrix();

    /**
     * Matrix parameterized constructor with elements initialized to zero value
     *
     * @param rows  Number of rows
     * @param cols  Number of columns
     */
    Matrix(const unsigned short & rows,
           const unsigned short & cols)
        throw (MatrixException);

    /**
     * Matrix parameterized constructor with a given value
     *
     * @param rows  Number of rows
     * @param cols  Number of columns
     * @param value Value used for initializing all the matrix elements
     */
    Matrix(const unsigned short & rows,
           const unsigned short & cols,
           const Type & value)
        throw (MatrixException);

    /**
     * Copy constructor
     *
     * @param other The other Matrix object to copy
     */
    Matrix(const Matrix & other)
        throw(MatrixException);

    /**
     * Destructor
     */
    virtual ~Matrix();

    /**
     * Return a reference to the element located in a row and column crossing
     *
     * @param row   The number of the row
     * @param col   The number of the column
     * @return      The element located in the row and column crossing
     */
    Type & operator() (const unsigned short & row,
                       const unsigned short & col)
        throw(MatrixException);

    /**
     * Return a copy of the element located in a row and column crossing
     *
     * @param row   The number of the row
     * @param col   The number of the column
     * @return      The element located in the row and column crossing
     */
    Type operator() (const unsigned short & row,
                     const unsigned short & col) const
        throw(MatrixException);

    /**
     * Assignment operator overloading
     *
     * @param other The other matrix from which to copy its elements
     * @return      A copy of the other matrix passed as a parameter
     */
    Matrix & operator= (Matrix const & other)
        throw(MatrixException);

    /**
     * 'Equal than' comparison operator overloading
     *
     * @param other The right hand matrix operand of the comparison
     * @return      True if this left hand matrix is equal than the right one, false otherwise
     */
    bool operator== (Matrix const & other) const
        throw(MatrixException);

    /**
     * Return the number of rows
     *
     * @return  The number of rows
     */
    unsigned short getNumRows() const {
        return this->numRows;
    }

    /**
     * Return the number of columns
     *
     * @return  The number of columns
     */
    unsigned short getNumCols() const {
        return this->numCols;
    }

private:
    unsigned short numRows; /**< Number of rows of the matrix */

    unsigned short numCols; /**< Number of columns of the matrix */

    Type * pElements;       /**< A pointer to the begin of the list of the matrix elements */

};



//................................................. Default constructor ...
template <class Type>
Matrix<Type>::Matrix() {
    this->numRows = 0;
    this->numCols = 0;
    this->pElements = NULL;
}


//................................................. Parameterized default constructor ...
template <class Type>
Matrix<Type>::Matrix(const unsigned short & rows,
                     const unsigned short & cols)
    throw(MatrixException) {

    this->numRows = rows;
    this->numCols = cols;
    this->pElements = NULL;

    if (rows == 0 || cols == 0) {
        throw MatrixException(MATEXC_BAD_DIMENSIONS);
    }

    this->pElements = new (std::nothrow) Type[rows * cols];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            this->pElements[cols * i + j] = 0;
        }
    }
}


//................................................. Parameterized initial value constructor ...
template <class Type>
Matrix<Type>::Matrix(const unsigned short & rows,
                 const unsigned short & cols,
                 const Type & value)
    throw(MatrixException) {

    this->numRows = rows;
    this->numCols = cols;
    this->pElements = NULL;

    if (rows == 0 || cols == 0) {
        throw MatrixException(MATEXC_BAD_DIMENSIONS);
    }

    this->pElements = new (std::nothrow) Type[rows * cols];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            this->pElements[cols * i + j] = value;
        }
    }
}


//................................................. Copy constructor ...
template <class Type>
Matrix<Type>::Matrix(const Matrix<Type> & other)
    throw(MatrixException) {

    this->numRows = other.numRows;
    this->numCols = other.numCols;
    this->pElements = NULL;

    if(other.numRows == 0 && other.numCols == 0) {
        return;
    }

    this->pElements = new (std::nothrow) Type [other.numRows * other.numCols];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < other.numRows; i++) {
        for (int j = 0; j < other.numCols; j++) {
            this->pElements[other.numCols * i + j] = other.pElements[other.numCols * i + j];
        }
    }
}


//................................................. Destructor ...
template <class Type>
Matrix<Type>::~Matrix() {
    if (this->pElements != NULL) {
        delete [] this->pElements;
    }
}


//................................................. Matrix element writable access ...
template <class Type>
Type & Matrix<Type>::operator() (const unsigned short & row,
                                 const unsigned short & col)
    throw(MatrixException) {

    if (row >= this->numRows || col >= this->numCols) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[this->numCols * row + col];
}


//................................................. Matrix element read-only access ...
template <class Type>
Type Matrix<Type>::operator() (const unsigned short & row,
                               const unsigned short & col) const
    throw(MatrixException){

    if (row >= this->numRows || col >= this->numCols) {
        throw MatrixException(MATEXC_BAD_ACCESS);
    }

    return this->pElements[this->numCols * row + col];
}


//................................................. Assignment operator overloading ...
template <class Type>
Matrix<Type> & Matrix<Type>::operator= (Matrix<Type> const & other)
    throw(MatrixException) {

    if (this == &other) {
        return *this;
    }

    this->numRows = other.numRows;
    this->numCols = other.numCols;
    this->pElements = NULL;

    if(other.numRows == 0 && other.numCols == 0) {
        return *this;
    }

    this->pElements = new (std::nothrow) Type [other.numRows * other.numCols];

    if (this->pElements == NULL) {
        throw MatrixException(MATEXC_ALLOC_ERROR);
    }

    for (int i = 0; i < other.numRows; i++) {
        for (int j = 0; j < other.numCols; j++) {
            this->pElements[other.numCols * i + j] = other.pElements[other.numCols * i + j];
        }
    }

    return *this;
}


//................................................. 'Equal than' comparison operator overloading ...
template <class Type>
bool Matrix<Type>::operator== (Matrix<Type> const & other) const
    throw(MatrixException) {

    if(this->numRows != other.numRows  ||  this->numCols != other.numCols) {
        return false;
    }

    for (int i = 0; i < other.numRows; i++) {
        for (int j = 0; j < other.numCols; j++) {
            if ((*this)(i,j) != other(i,j)) {
                return false;
            }
        }
    }

    return true;
}

} /* namespace */
#endif /* CMATRIX_HPP_ */
