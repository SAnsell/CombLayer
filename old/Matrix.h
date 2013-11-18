/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/Matrix.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef Matrix_h 
#define Matrix_h

/*!
   \namespace Geometry
   \brief Classes for geometric handlers
   \version 1.0
   \author S. Ansell
   \date February 2006

*/

namespace Geometry
{

/*!
  \class Matrix
  \brief Numerical Matrix class
  \version 1.0
  \author S. Ansell
  \date August 2005

  Holds a matrix of variable type
  and size. Should work for real and 
  complex objects. Carries out eigenvalue
  and inversion if the matrix is square
*/

template<typename T>
class Matrix
{
 private:

  int nx;      ///< Number of rows    (x coordinate) 
  int ny;      ///< Number of columns (y coordinate)   
  
  T** V;                      ///< Raw data
  
  void deleteMem();           ///< Helper function to delete memory
  void lubcmp(int*,int&);     ///< starts inversion process 
  void lubksb(const int*,double*);    
  void rotate(const double,const double,const int,
	      const int,const int,const int);

 public:

  Matrix(const int =0,const int =0);
  Matrix(const std::vector<T>&,const std::vector<T>&); 
  Matrix(const Matrix<T>&);
  Matrix<T>& operator=(const Matrix<T>&); 
  ~Matrix();

  const T* operator[](const int A) const { return V[A]; }
  T* operator[](const int A) { return V[A]; }

  Matrix<T>& operator+=(const Matrix<T>&);       ///< Basic addition operator
  Matrix<T> operator+(const Matrix<T>&);         ///< Basic addition operator

  Matrix<T>& operator-=(const Matrix<T>&);       ///< Basic subtraction operator
  Matrix<T> operator-(const Matrix<T>&);         ///< Basic subtraction operator

  Matrix<T> operator*(const Matrix<T>&) const;    ///< Basic matrix multiply 
  std::vector<T> operator*(const std::vector<T>&) const; ///< Multiply M*Vec 
  Matrix<T> operator*(const T&) const;              ///< Multiply by constant 

  Matrix<T>& operator*=(const Matrix<T>&);            ///< Basic matrix multipy
  Matrix<T>& operator*=(const T&);                 ///< Multiply by constant
  Matrix<T>& operator/=(const T&);                 ///< Divide by constant

  int operator==(const Matrix<T>&) const; 
  T item(const int a,const int b) const { return V[a][b]; }   // disallows access

  void print() const;
  void write(std::ostream&,const int =0) const;
  std::string str() const;

  void zeroMatrix();      ///< Set the matrix to zero
  void idenityMatrix();
  void normVert();         ///< Vertical normalisation 
  T Trace() const;         ///< Trace of the matrix  

  std::vector<T> Diagonal() const;                  ///< Returns a vector of the diagonal
  Matrix<T> fDiagonal(const std::vector<T>&) const;    ///< Forward multiply  D*this
  Matrix<T> bDiagonal(const std::vector<T>&) const;    ///< Backward multiply this*D
  void setMem(const int,const int);
  std::pair<int,int> size() const { return std::pair<int,int>(nx,ny); }
  int Ssize() const { return (nx>ny) ? ny : nx; }

  void swapRows(const int,const int);        ///< Swap rows (first V index)
  void swapCols(const int,const int);        ///< Swap cols (second V index)

  T Invert();                           ///< LU inversion routine
  std::vector<T> Faddeev(Matrix&);      ///< Polynominal and inversion by Faddeev method.
  void averSymmetric();                 ///< make Matrix symmetric
  int Diagonalise(Matrix<T>&,Matrix<T>&) const;  ///< (only Symmetric matrix)
  void sortEigen(Matrix<T>&);                    ///< Sort eigenvectors
  Matrix<T>& Transpose();                        ///< Transpose the matrix

  T factor();              ///< Calculate the factor
  T determinant() const;   ///< Calculate the determinant 

  int GaussJordan(Matrix<T>&);      ///< Create a Gauss-Jordan Invertion

};

template<typename T>
std::ostream&
operator<<(std::ostream&,const Geometry::Matrix<T>&);

};

// template<typename X>
// std::ostream& operator<<(std::ostream&,const Geometry::Matrix<X>&);

#endif














