/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/DSTerm.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <sstream>
#include <complex>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <numeric>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "SrcData.h"
#include "DSTerm.h"

namespace SDef
{

DSBase::DSBase() :
  option('H')
  /*!
    Contructor
  */
{}

DSBase::DSBase(const std::string& K,const char O)  :
  option(O),Key(K)
  /*!
    Contructor
    \param K :: Key value
    \param O :: Option
  */
{}

DSBase::DSBase(const DSBase& A) :
  option(A.option),Key(A.Key)
  /*!
    Copy constructor
    \param A :: DSBase object to copy
   */
{}

DSBase&
DSBase::operator=(const DSBase& A) 
  /*!
    Assignment operator
    \param A :: DSBase object to copy
    \return *this
   */
{
  if (this!=&A)
    {
      option=A.option;
      Key=A.Key;
    }
  return *this;
}

void
DSBase::setType(const std::string& K,const char O)
  /*!
    General setting of type, to be replaced by
    specializations in DSTerm
    \param K :: Key value
    \param O :: Option
   */
{
  Key=K;
  option=O;
  return;
}
  

// -------------------------------------------
//               DSTERM
// -------------------------------------------

template<typename T>
DSTerm<T>::DSTerm() : DSBase()
 /*!
   Constructor 
  */
{}


template<typename T>
DSTerm<T>::DSTerm(const std::string& K,const char O) : 
  DSBase(K,O)
 /*!
   Constructor 
   \param K :: Key value [Needs checking]
   \param O :: Option value [Needs checking]
 */
{}

template<typename T>
DSTerm<T>::DSTerm(const DSTerm& A) : 
  DSBase(A),Values(A.Values)
 /*!
   Copy Constructor 
   \param A :: DSTerm item to copy
 */
{}

template<typename T>
DSTerm<T>&
DSTerm<T>::operator=(const DSTerm<T>& A) 
 /*!
   Assignment operator
   \param A :: DSTerm Object
   \return *this
 */
{
  if (this!=&A)
    {
      DSBase::operator=(A);
      Values=A.Values;
    }
  return *this;
}

template<typename T>
DSTerm<T>*
DSTerm<T>::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new DSTerm<T>(*this);
}

template<typename T>
DSTerm<T>::~DSTerm() 
  /*!
    Destructor
   */
{}

template<typename T>
void
DSTerm<T>::setData(const std::vector<T>& V) 
  /*!
    Sets the data as values
    \param V :: Values
   */
{
  Values=V;
  return;
}

template<typename T>
void
DSTerm<T>::addData(const T& V) 
  /*!
    Adds a data set the data as values
    \param V :: Value
  */
{
  Values.push_back(V);
  return;
}

template<>
void
DSTerm<double>::write(const size_t Index,std::ostream& OX) const
  /*!
    Write DS line to an MCNPX output
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  boost::format FMTStr("%1$11.5g ");
  std::ostringstream cx;
  cx<<"ds"<<Index<<" "<<DSBase::option<<" ";
  if (!Values.empty())
    {
      std::vector<double>::const_iterator vc;
      for(vc=Values.begin();vc!=Values.end();vc++)
	cx<<FMTStr % *vc;
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

template<>
void
DSTerm<int>::write(const size_t Index,std::ostream& OX) const
  /*!
    Write DS line to an MCNPX output
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  std::ostringstream cx;
  cx<<"ds"<<Index<<" "<<option<<" ";
  if (!Values.empty())
    {
      std::vector<int>::const_iterator vc;
      for(vc=Values.begin();vc!=Values.end();vc++)
	cx<<*vc<<" ";
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

template<>
void
DSTerm<std::string>::write(const size_t Index,std::ostream& OX) const
  /*!
    Write DS line to an MCNPX output
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  std::ostringstream cx;
  cx<<"ds"<<Index<<" "<<option<<" ";
  if (!Values.empty())
    {
      std::vector<std::string>::const_iterator vc;
      for(vc=Values.begin();vc!=Values.end();vc++)
	cx<<*vc<<" ";
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

// -------------------------------------------
//               DSIndex
// -------------------------------------------

DSIndex::DSIndex() : DSBase()
 /*!
   Constructor 
 */
{
  option='s';
}


DSIndex::DSIndex(const std::string& K) : 
  DSBase(K,'s')
 /*!
   Constructor 
   \param K :: Key value [Needs checking]
 */
{}

DSIndex::DSIndex(const DSIndex& A) : 
  DSBase(A),PMesh(A.PMesh)
 /*!
   Copy Constructor 
   \param A :: DSIndex item to copy
 */
{}

DSIndex&
DSIndex::operator=(const DSIndex& A) 
 /*!
   Assignment operator
   \param A :: DSIndex Object
   \return *this
 */
{
  if (this!=&A)
    {
      DSBase::operator=(A);
      PMesh=A.PMesh;
    }
  return *this;
}

DSIndex*
DSIndex::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new DSIndex(*this);
}

DSIndex::~DSIndex() 
  /*!
    Destructor
   */
{}


void
DSIndex::addData(const size_t I,const SrcInfo* SI,
		 const SrcBias* SB,const SrcProb* SP) 
  /*!
    Adds a data set the data as values
    \param I :: Index
    \param SI :: Index type
    \param SB :: Bias type
    \param SP :: Probability type
  */
{
  PMesh.push_back(SrcData(I));
  if (SI)
    PMesh.back().addUnit(*SI);
  if (SB)
    PMesh.back().addUnit(*SB);
  if (SP)
    PMesh.back().addUnit(*SP);
  
  return;
}

void
DSIndex::write(const size_t Index,std::ostream& OX) const
  /*!
    Write DS line to an MCNPX output
    \todo Check doesn't seem correct
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  std::ostringstream cx;
  cx<<"ds"<<Index<<" s";

  for(const SrcData& sd : PMesh)
    cx<<" "<<sd.getIndex();
  StrFunc::writeMCNPX(cx.str(),OX);

  for(const SrcData& sd : PMesh)
    sd.write(OX);
  
  return;
}

///\cond TEMPLATE
template class DSTerm<int>;
template class DSTerm<double>;
///\endcond TEMPLATE


}  // NAMESPACE SDef
