/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/Tensor.h
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
#ifndef Tensor_h
#define Tensor_h

/*!
  \class Tensor
  \brief Basic Tensor for
  \version 1.0
  \date August 2005
  \author S. Ansell

  Holds a simple tensor object. 
  Currently has nothing beyond accessor and creation
  objects.
  \todo replace with Boost::array
*/
template<typename T>
class Tensor
{
 private: 

  typedef unsigned long int ITYPE;   ///< Integer form

  ITYPE nx;             ///< first coordinates
  ITYPE ny;             ///< second coordinates
  ITYPE nz;             ///< third coordinates
  T*** U;             ///< Storage Space
  
  void deleteMem();
  void copyMem(const Tensor<T>&);
  void copyMem(T*** const);

 public:

  Tensor();
  Tensor(const ITYPE,const ITYPE =0,const ITYPE=0);
  Tensor(const Tensor<T>&);
  Tensor& operator=(const Tensor<T>&);
  ~Tensor();

  T** operator[](const ITYPE A) const { return U[A]; }   ///< Accessor function (const)
  T** operator[](const ITYPE A) { return U[A]; }               ///< Accesor function
  T item(const ITYPE a,const ITYPE b,const ITYPE c) const 
    { return U[a][b][c]; }   ///<Item accessor
  
  void setMem(const ITYPE,const ITYPE,const ITYPE);
};

#endif
