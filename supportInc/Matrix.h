/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/Matrix.h
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
#ifndef Geometry_Matrix_h 
#define Geometry_Matrix_h

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
class Vec3D;


template<typename T>
class Matrix : public MatrixBase<T>
{
 private:

  void lubcmp(size_t*,int&);     ///< starts inversion process 
  void lubksb(const size_t*,T*);    
  void rotate(const T,const T,const size_t,
	      const size_t,const size_t,const size_t);

 public:

  Matrix(const size_t =0,const size_t =0);
  Matrix(const std::vector<T>&,const std::vector<T>&); 
  Matrix(const Matrix<T>&);
  Matrix(const MatrixBase<T>&);
  Matrix(const Matrix<T>&,const size_t,const size_t);
  Matrix<T>& operator=(const Matrix<T>&); 
  virtual ~Matrix();

  Matrix<T>& operator+=(const MatrixBase<T>&);  
  Matrix<T> operator+(const MatrixBase<T>&) const;    

  Matrix<T>& operator-=(const MatrixBase<T>&);  
  Matrix<T> operator-(const MatrixBase<T>&) const;    

  Matrix<T> operator*(const MatrixBase<T>&) const; 
  Matrix<T> operator*(const T&) const;
  std::vector<T> operator*(const std::vector<T>&) const;

  Vec3D operator*(const Vec3D&) const; 
  Matrix<T>& operator*=(const MatrixBase<T>&); 
  Matrix<T>& operator*=(const T&);             

  Matrix<T>& operator/=(const T&);            
  bool operator==(const Matrix<T>&) const;
  bool operator!=(const Matrix<T>&) const;

  Matrix<T> Tprime() const;     
  T Invert();                        ///< LU inversion routine
  T factor();                   
  T determinant() const;        
  void normVert();         ///< Vertical normalisation 
  void averSymmetric();     
  void sortEigen(Matrix<T>&);         
  int Diagonalise(Matrix<T>&,Matrix<T>&) const;  

   /// Polynomanal and inversion by Faddeev method.
  std::vector<T> Faddeev(Matrix<T>&);   

  int GaussJordan(Matrix<T>&);      ///< Create a Gauss-Jordan Invertion

}; 

}  // NAMESPACE  Geometry

#endif














