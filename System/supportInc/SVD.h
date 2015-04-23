/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/SVD.h
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
#ifndef SVD_h
#define SVD_h

namespace Geometry
{
/*!
  \class SVD 
  \brief Carries out single value decomposition
  \version 1.0
  \author S. Ansell
  \date October 2006
  
  Loosely based on the cmdview code that does
  polynominal fitting.
  
  The code converts
  \f[
  \left( A \right)  = \left (U \right)  \cdot \left( S \right) \cdot \left( V^T \right) 
  \f]
  where A is the Design matrix 
*/

class SVD
{
private:

  size_t mRows;          ///< Number of rows in the Design matrix (A)
  size_t nCols;          ///< Number of columns in the Design matrix (A)

  Matrix<double> A;      ///< Design matrix A (m x n)
  Matrix<double> S;      ///< Diagonal Matrix 
  Matrix<double> U;      ///< matrix  (m x n)
  Matrix<double> V;      ///< Orthongonal matrix (n x n)
    
public:

  SVD();
  SVD(const SVD&);
  SVD& operator=(const SVD&);
  ~SVD();

  void calcDecomp();
  int removeSingular(const double);

  void setMatrix(const Matrix<double>&);
  /// Access A matrix const
  const Matrix<double>& getDesign() const { return A; }
  Matrix<double>& getDesign() { return A; }         ///< Access A matrix

  const Matrix<double>& getS() const { return S; }  ///< Access W matrix
  const Matrix<double>& getU() const { return U; }  ///< Access U matrix
  const Matrix<double>& getV() const { return V; }  ///< Access V matrix
  
  std::vector<double> linearEQ(const std::vector<double>&) const;
};

}

#endif












