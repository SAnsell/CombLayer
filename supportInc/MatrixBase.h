/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/MatrixBase.h
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
#ifndef MatrixBase_h 
#define MatrixBase_h


namespace mathLevel
{
  template<size_t index>
  class PolyVar;
}
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
  \class MatrixBase
  \brief Numerical MatrixBase class
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
class MatrixBase
{
 protected:

  size_t nx;    ///< Number of rows    (x coordinate) 
  size_t ny;    ///< Number of columns (y coordinate)   
  
  T** V;                   ///< Raw data
  
  void deleteMem();        ///< Helper function to delete memory

 public:

  MatrixBase(const size_t =0,const size_t =0);
  MatrixBase(const std::vector<T>&,const std::vector<T>&); 
  MatrixBase(const MatrixBase<T>&);
  MatrixBase(const MatrixBase<T>&,const size_t,const size_t);
  MatrixBase<T>& operator=(const MatrixBase<T>&); 
  virtual ~MatrixBase();

  const T* operator[](const size_t A) const { return V[A]; } ///< Ptr accessor
  T* operator[](const size_t A) { return V[A]; }             ///< Ptr accessor

  MatrixBase<T>& operator+=(const MatrixBase<T>&);  
  MatrixBase<T> operator+(const MatrixBase<T>&);    

  MatrixBase<T>& operator-=(const MatrixBase<T>&);  
  MatrixBase<T> operator-(const MatrixBase<T>&);    

  MatrixBase<T> operator*(const MatrixBase<T>&) const; 
  std::vector<T> operator*(const std::vector<T>&) const;
  MatrixBase<T> operator*(const T&) const;

  MatrixBase<T>& operator*=(const MatrixBase<T>&); 
  MatrixBase<T>& operator*=(const T&);             

  virtual bool operator==(const MatrixBase<T>&) const; 
  virtual bool operator!=(const MatrixBase<T>&) const;
  /// Item access
  T item(const size_t a,const size_t b) const { return V[a][b]; }   

  void print() const;
  void write(std::ostream&,const int =0) const;
  void writeGrid(std::ostream&) const;
  std::string str(const int =6) const;

  void zeroMatrix();    
  void identityMatrix();    
  T Trace() const;         

  std::vector<T> Diagonal() const;               
  // Forward multiply  D*this
  MatrixBase<T> fDiagonal(const std::vector<T>&) const; 
  // Backward multiply this*D
  MatrixBase<T> bDiagonal(const std::vector<T>&) const;   
  std::vector<T> Row(const size_t) const;              
  std::vector<T> Column(const size_t) const;              

  void setMem(const size_t,const size_t);
  /// Size values
  std::pair<size_t,size_t> size() const 
    { return std::pair<size_t,size_t>(nx,ny); }
  /// Smallest size
  size_t Ssize() const { return (nx>ny) ? ny : nx; }  
  /// Total size
  size_t itemCnt() const { return nx*ny; }

  void swapRows(const size_t,const size_t);    ///< Swap rows (first V index)
  void swapCols(const size_t,const size_t);    ///< Swap cols (second V index)


  MatrixBase<T>& Transpose();                   ///< Transpose the matrix
  MatrixBase<T> Tprime() const;                 ///< Transpose the matrix

  T laplaceDeterminate() const;
  T compSum() const;

}; 

template<typename T>
std::ostream&
operator<<(std::ostream&,const Geometry::MatrixBase<T>&);

template<int index>
std::ostream&
operator<<(std::ostream&,
	   const Geometry::MatrixBase<mathLevel::PolyVar<index> >&);

}  // NAMESPACE  Geometry



// template<typename X>
// std::ostream& operator<<(std::ostream&,const Geometry::MatrixBase<X>&);

#endif














