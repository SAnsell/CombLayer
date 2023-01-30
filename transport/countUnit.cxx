/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/countUnit.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <vector>

#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Exception.h"
#include "countUnit.h"

namespace Transport
{

countUnit::countUnit(const size_t NI,const size_t NE) :
  nIndex(NI),nEBin(NE)
  /*!
    Constructor 
    */
{
  if (nIndex*nEBin<1)
    {
      if (!nIndex)
	throw ColErr::EmptyValue<size_t>("countUnit::nIndex");
      else
	throw ColErr::EmptyValue<size_t>("countUnit::nEBin");
    }

  setMem();
  zeroMem();
}

countUnit::countUnit(const countUnit& A) :
  nIndex(A.nIndex),nEBin(A.nIndex)
  /*!
    Copy Constructor
    \param A :: Tensor to copy
  */
{
  setMem();
  copyMem(A);
}


countUnit::countUnit(countUnit&& A) :
  nIndex(A.nIndex),nEBin(A.nIndex)
  /*!
    Copy Constructor
    \param A :: Tensor to copy
  */
{
  A.data=nullptr;
  A.cnt=nullptr;
}

countUnit&
countUnit::operator=(const countUnit& A) 
  /*!
    Assignment operator
    \param A :: data to copy
    \return *this
  */
{
  if (this!=&A)
    {
      if (nIndex!=A.nIndex || nEBin!=A.nEBin)
	{
	  if (nIndex!=A.nIndex)
	    throw ColErr::IndexError<size_t>
	      (nIndex,A.nIndex,"countUnit::nIndex");
	  else
	    throw ColErr::IndexError<size_t>
	      (nEBin,A.nEBin,"countUnit::nEBin");
	}
      copyMem(A);
    }
  return *this;
}

countUnit&
countUnit::operator=(countUnit&& A) 
  /*!
    Move Assignment operator
    \param A :: data to copy
    \return *this
  */
{
  if (this!=&A)
    {
      if (nIndex!=A.nIndex || nEBin!=A.nEBin)
	{
	  if (nIndex!=A.nIndex)
	    throw ColErr::IndexError<size_t>
	      (nIndex,A.nIndex,"countUnit::nIndex");
	  else
	    throw ColErr::IndexError<size_t>
	      (nEBin,A.nEBin,"countUnit::nEBin");
	}
      data=std::move(A.data);
      cnt=std::move(A.cnt);
      A.data=nullptr;
      A.cnt=nullptr;
    }
  return *this;
}


countUnit::~countUnit() 
  /*!
    Destructor 
  */
{
  if (data)   // MUST CHECK: std::move will set nullptr
    {
      delete [] data[0];
      delete data;
      delete cnt;
    }
}

void
countUnit::copyMem(const countUnit& A)
  /*!
    simple function to copy blocks of memory
    Will be sped up after debug
  */
{
  for(size_t i=0;i<nIndex;i++)
    for(size_t j=0;i<nEBin;i++)
      data[i][j]=A.data[i][j];
  for(size_t i=0;i<nIndex;i++)
    cnt[i]=A.cnt[i];
  return;
}

void
countUnit::zeroMem()
  /*!
    Zero all the data
    (to be sped up after debug)
   */
{
  for(size_t i=0;i<nIndex;i++)
    for(size_t j=0;j<nEBin;j++)
      data[i][j]=0.0;
  for(size_t i=0;i<nIndex;i++)
    cnt[i]=0.0;
  return;
}

void
countUnit::setMem()
  /*! 
    Sets the memory held in data
   */
{
  double* tmpX=new double[nIndex*nEBin];
  data=new double*[nIndex];
  for (size_t i=0;i<nIndex;i++)
    data[i]=tmpX + (i*nEBin);
  cnt=new double[nEBin];
  return;
}



} // NAMESPACE Transport
