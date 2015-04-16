/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/Tensor.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

#include "Tensor.h"

template<typename T>
Tensor<T>::Tensor() :
  nx(0),ny(0),nz(0),U(0)
  /*!
    Default constructor
  */
{}

template<typename T>
Tensor<T>::Tensor(const ITYPE a,const ITYPE b,
		  const ITYPE c) :
  nx(0),ny(0),nz(0),U(0)
  /*!
    Constructor with pre-set sizes
    \param a :: First item
    \param b :: Second item
    \param c :: Third item
  */
{
  setMem(a,b,c);
}

template<typename T>
Tensor<T>::~Tensor() 
  /*!
    Destructor 
  */
{
  if (U)
    {
      delete U[0][0];
      delete U[0];
      delete U;
    }

}

template<typename T>
Tensor<T>::Tensor(const Tensor<T>& A) :
  nx(0),ny(0),nz(0),U(0)
  /*!
    Copy Constructor
    \param A :: Tensor to copy
  */
{
  setMem(A.nx,A.ny,A.nz);
  copyMem(A.U);
}

template<typename T>
Tensor<T>&
Tensor<T>::operator=(const Tensor<T>& A) 
  /*!
    Assignment operator
    \param A :: Tensor to copy
    \return *this
  */
{
  if (this!=&A)
    {
      setMem(A.nx,A.ny,A.nz);
      copyMem(A.U);
    }
  return *this;
}

template<typename T>
void
Tensor<T>::setMem(const ITYPE a, 
		  const ITYPE b,
		  const ITYPE c)
  /*! 
    Sets the memory held in U 
    \param a :: new x-size
    \param b :: new y-size
    \param c :: new z-size
   */
{
  if (a==nx && b==ny && c==nz) 
    return;
  
  deleteMem();
  if (a<=0 || b<=0 || c<=0)
    return;

  nx=a;
  ny=b;
  nz=c;
  if (a*b*c)
    {
      T* tmpX=new T[a*b*c];
      T** tmpY=new T*[a*b];
      U=new T**[a];
      for(ITYPE i=0;i<nx*ny;i++)
	tmpY[i]=tmpX+i*nz;
      for (ITYPE i=0;i<nx;i++)
	U[i]=tmpY + (i*ny);
    }
  return;
}  

template<typename T>
void
Tensor<T>::copyMem(const Tensor<T>& A)
  /*!
    Copy Tensor memory from A and resize
    the memory of this if required
    \param A :: Object to copy the tensor memory from
  */
{
  if (&A!=this)
    {
      setMem(A.nx,A.ny,A.nz);
      if (U)
	{
	  T* Wu=U[0][0];           //Write point 
	  T* Ru=A.U[0][0];           //Read point
	  for(ITYPE i=0;i<nx*ny*nz;i++)
	    {
	      *Wu = *Ru;
	      Ru++;
	      Wu++;
	    }
	}
    }
  return;
}

template<typename T>
void
Tensor<T>::copyMem(T*** const  AU)
  /*!
    This copies the memory from a dereferenced pointer
    \param AU :: Pointer object to copy from
  */
{
  if (U && AU)
    {
      T* Wu=U[0][0];           //Write point 
      const T* Ru=AU[0][0];           //Read point
      for(ITYPE i=0;i<nx*ny*nz;i++)
	{
	  *Wu = *Ru;
	  Ru++;
	  Wu++;
	}
    }
  return;
}

template<typename T>
void
Tensor<T>::deleteMem()
  /*!
    Memory deletion function
  */
{
  nx=0;
  ny=0;
  nz=0;
  if (U)
    {
      delete [] U[0][0];
      delete [] U[0];
      delete [] U;
      U=0;
    }
  return;
}


template class Tensor<double>;
