/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SrcData.cxx
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

SrcData::SrcData(const size_t I) :
  Index(I),SI(0),SB(0),SP(0),DS(0)
 /*!
   Constructor 
   \param I :: Index value
 */
{}

SrcData::SrcData(const SrcData& A) :
  Index(A.Index),
  SI(A.SI ? new SrcInfo(*A.SI) : 0),
  SB(A.SB ? new SrcBias(*A.SB) : 0),
  SP(A.SP ? new SrcProb(*A.SP) : 0),
  DS(A.DS ? A.DS->clone() : 0)
 /*!
   Copy Constructor 
   \param A :: SrcData Object
 */
{}


SrcData&
SrcData::operator=(const SrcData& A) 
 /*!
   Assignment operator
   \param A :: SrcData Object
   \return *this
 */
{
  if (this!=&A)
    {
      Index=A.Index;
      delete SI;
      delete SB;
      delete SP;
      delete DS;
      SI=A.SI ? new SrcInfo(*A.SI) : 0;
      SB=A.SB ? new SrcBias(*A.SB) : 0;
      SP=A.SP ? new SrcProb(*A.SP) : 0;
      DS=A.DS ? A.DS->clone() : 0;
    }
  return *this;
}

SrcData::~SrcData()
  /*!
    Deletion
  */
{
  delete SI;
  delete SB;
  delete SP;
  delete DS;
}

SrcInfo*
SrcData::getInfo()
  /*!
    Create/access SrcInfo object
    \return SI ptr
  */
{
  if (!SI)
    SI=new SrcInfo;
  return SI;
}

SrcBias*
SrcData::getBias()
  /*!
    Create/access SrcBias object
    \return SB ptr
  */
{
  if (!SB)
    SB=new SrcBias;
  return SB;
}


SrcProb*
SrcData::getProb()
  /*!
    Create/access SrcProb object
    \return SP ptr
  */
{
  if (!SP)
    SP=new SrcProb;
  return SP;
}

template<typename T>
DSTerm<T>*
SrcData::getDS()
  /*!
    Create/access DSTerm object
    \return DS ptr
  */
{
  
  DSTerm<T>* DPtr=dynamic_cast<DSTerm<T>*>(DS);
  if (DPtr)
    return DPtr;

  delete DS;
  DPtr=new DSTerm<T>;
  DS=DPtr;
  return DPtr;
}

std::string
SrcData::getDep() const
  /*!
    Get dependent string
    \return Dep String / null
   */
{
  if (!DS) return std::string();
  return DS->getKey();
}

void
SrcData::addUnit(const SrcInfo& A) 
  /*!
    Add a value
    \param A :: SI object ot add
  */
{
  delete SI;
  SI=new SrcInfo(A);
  return;
}

void
SrcData::addUnit(const SrcBias& A) 
  /*!
    Add a value
    \param A :: SB object ot add
  */
{
  delete SB;
  SB=new SrcBias(A);
  return;
}

void
SrcData::addUnit(const SrcProb& A) 
  /*!
    Add a value
    \param A :: SP object ot add
  */
{
  delete SP;
  SP=new SrcProb(A);
  return;
}

void
SrcData::addUnit(const DSBase* A) 
  /*!
    Add a DSBase object [ note a check is made as A can be zero]
    \param A :: SP object ot add
  */
{
  delete DS;
  DS= (A) ? A->clone() : 0;
  return;
}

void
SrcData::cutValue(const double VCut)
  /*!
    Apply an energy/value cut
    \param VCut :: Cut off value
  */
{
  ELog::RegMethod RegA("SrcData","cutValue");

  if (!SI || !SP) return;

  std::vector<double>& E=SI->getData();
  std::vector<double>& P=SP->getData();
  if (E.empty()) return;
  if (P.size()!=E.size())
    {
      ELog::EM<<"Mismatch in SI and SP cards "<<ELog::endErr;
      return;
    }
  // trick to move -1 to a large nuber
  size_t Id=static_cast<size_t>(indexPos(E,VCut));
  if (Id>=E.size()-1) return;

  const double frac=(E[Id+1]-VCut)/(E[Id+1]-E[Id]);
  P[Id]*=frac;

  if (fabs(frac)<1e-6)
    Id++;
  else
    E[Id]=VCut;
  
  E.erase(E.begin(),E.begin()+static_cast<std::ptrdiff_t>(Id));
  P.erase(P.begin(),P.begin()+static_cast<std::ptrdiff_t>(Id));

  // Renormalize
  renormalize();

  return;
}

void
SrcData::renormalize()
  /*!
    Renormalize probabilites to 1.0
   */
{
  if (SP && !SP->getFMinus() )
    {
      std::vector<double>& P=SP->getData();
      if (!P.empty())
        {
	  const double IV=
	    accumulate(P.begin(),P.end(),0.0,std::plus<double>());
	  if (IV>1e-6) 
	    transform(P.begin(),P.end(),P.begin(),
		      std::bind2nd(std::divides<double>(),IV));
	}
    }
  if (DS)
    {
      DSTerm<double>* DPtr=dynamic_cast<DSTerm<double>*>(DS);
      if (DPtr)
	{
	  std::vector<double>& P=DPtr->getData();
	  if (!P.empty())
	    {
	      const double IV=
		accumulate(P.begin(),P.end(),0.0,std::plus<double>());
	      if (IV>1e-6) 
		transform(P.begin(),P.end(),P.begin(),
			  std::bind2nd(std::divides<double>(),IV));
	    }
	}
    }
  return;
}

void
SrcData::write(std::ostream& OX) const
  /*!
    Write out the data form
    \param OX :: Output stream
   */
{
  if (SI)
    SI->write(Index,OX);
  if (SP)
    SP->write(Index,OX);
  if (SB)
    SB->write(Index,OX);
  if (DS)
    DS->write(Index,OX);
  return;
}

// -------------------------------------------
//               SRCINFO
// -------------------------------------------

SrcInfo::SrcInfo() :
  option('H')
 /*!
   Constructor 
 */
{}


SrcInfo::SrcInfo(const char O) : 
  option(O)
 /*!
   Constructor 
   \param O :: Option value [Needs checking]
 */
{}

SrcInfo::SrcInfo(const SrcInfo& A) : 
  option(A.option),Values(A.Values),
  Str(A.Str)
 /*!
   Copy Constructor 
   \param A :: SrcInfo item to copy
 */
{}

SrcInfo&
SrcInfo::operator=(const SrcInfo& A) 
 /*!
   Assignment operator
   \param A :: SrcInfo Object
   \return *this
 */
{
  if (this!=&A)
    {
      option=A.option;
      Values=A.Values;
      Str=A.Str;
    }
  return *this;
}

void
SrcInfo::setData(const std::vector<double>& V) 
  /*!
    Sets the data as values
    \param V :: Values
   */
{
  Values=V;
  Str.clear();
  return;
}

void
SrcInfo::setData(const std::vector<std::string>& V) 
  /*!
    Sets the data as values
    \param V :: Values
   */
{
  Str=V;
  Values.clear();
  return;
}

void
SrcInfo::addData(const double& V) 
  /*!
    Adds a data set the data as values
    \param V :: Value
   */
{
  Values.push_back(V);
  return;
}

void
SrcInfo::addData(const std::string& V) 
  /*!
    Adds a data set the data as values
    \param V :: Value
   */
{
  Str.push_back(V);
  return;
}

void
SrcInfo::write(const size_t Index,std::ostream& OX) const
  /*!
    Write SI line to an MCNPX output
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  ELog::RegMethod RegItem("SrcInfo","write");

  boost::format FMTStr("%1$.5g ");
  std::ostringstream cx;
  cx<<"si"<<Index<<" "<<option<<" ";
  if (!Values.empty())
    {
      std::vector<double>::const_iterator vc;
      for(vc=Values.begin();vc!=Values.end();vc++)
	cx<<FMTStr % *vc;
    }
  else
    {
      copy(Str.begin(),Str.end(),
	   std::ostream_iterator<std::string>(cx," "));
    }
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

// -------------------------------------------
//               SRCPROB
// -------------------------------------------

SrcProb::SrcProb() : option('D')
 /*!
   Constructor 
 */
{}

SrcProb::SrcProb(const char O) :
  option(isalpha(O) ? O : 0)
 /*!
   Constructor 
   \param O :: Option character
 */
{}

SrcProb::SrcProb(const SrcProb& A) : 
  option(A.option),
  minusF(A.minusF),Values(A.Values)
 /*!
   Copy Constructor 
   \param A :: SrcProb Data Object
 */
{}

SrcProb&
SrcProb::operator=(const SrcProb& A) 
 /*!
   Assignment operator
   \param A :: SrcProb Object
   \return *this
 */
{
  if (this!=&A)
    {
      option=A.option;
      minusF=A.minusF;
      Values=A.Values;
    }
  return *this;
}


int
SrcProb::MFcount(const int F)
  /*!
    Function to count number of items required
    for a -f value in a SP/SB card
    \param F :: Prob-type [see MCNPX manual]
    \return max item count
  */
{
  switch (F) 
  {
  case -2:            // 
    return 1;
  case -3:
    return 2;
  case -4:
    return 2;
  case -5:
    return 1;
  case -6:
    return 2;
  case -21:          // power law p(x) = c|x|^a
    return 1;
  case -31:          // exponential p(x)=c exp(a|x|)
    return 1;
  case -41:          // gaussian p(x)=c exp(-(1.665*(x-a)/b))
    return 2;
  }
  // UNKNOWN:
  return -1;
}


void
SrcProb::setFminus(const int F,const double a)
  /*!
    Set an Fminus output
    \param F :: -ve fMinus value
    \param a :: first parameter
  */
{
  const int NC(MFcount(F));
  if (NC<1)
    throw ColErr::InContainerError<int>(F,"SrcProb::setFminus(F,a)");
  Values.resize(1);
  Values[0]=a;
  minusF=F;
  return;
}

void
SrcProb::setFminus(const int F,const double a,const double b)
  /*!
    Set an Fminus output
    \param F :: -ve fMinus value
    \param a :: first parameter
    \param b :: second parameter
  */
{
  const int NC(MFcount(F));
  if (NC!=2)
    throw ColErr::InContainerError<int>(F,"SrcProb::setFminus(F,a,b)");
  Values.resize(2);
  Values[0]=a;
  Values[1]=b;
  minusF=F;
  return;
}


void
SrcProb::setData(const std::vector<double>& V) 
  /*!
    Sets the data
    \param V :: Values
   */
{
  Values=V;
  minusF=0;
  return;
}

void
SrcProb::addData(const double& V) 
  /*!
    Adds some data
    \param V :: Value
   */
{
  Values.push_back(V);
  minusF=0;
  return;
}

void
SrcProb::write(const size_t Index,std::ostream& OX) const
  /*!
    Write SI line to an MCNPX output
    \param Index :: Index number
    \param OX :: Output stream
  */   
{
  ELog::RegMethod RegItem("SrcProb","write");

  boost::format FMTStr("%1$.5g ");
  std::ostringstream cx;
  cx<<"sp"<<Index<<" ";

  if (minusF)
    cx<<minusF<<" ";
  else if (option)
    cx<<option<<" ";

  for(const double& V: Values)
    cx<<FMTStr % V;


  StrFunc::writeMCNPX(cx.str(),OX);

  return;
}

// -------------------------------------------
//               SRCBIAS
// -------------------------------------------

SrcBias::SrcBias() : 
  option('D')
  /*!
    Constructor 
  */
{}


SrcBias::SrcBias(const char O) : 
  option(O)
 /*!
   Constructor 
   \param O :: Option value [Needs checking]
 */
{}

SrcBias::SrcBias(const SrcBias& A) : 
  option(A.option),minusF(A.minusF),
  Values(A.Values)
 /*!
   Copy Constructor 
   \param A :: SrcBias item to copy
 */
{}

SrcBias&
SrcBias::operator=(const SrcBias& A) 
 /*!
   Assignment operator
   \param A :: SrcBias Object
   \return *this
 */
{
  if (this!=&A)
    {
      option=A.option;
      minusF=A.minusF;
      Values=A.Values;
    }
  return *this;
}


void
SrcBias::setData(const std::vector<double>& V) 
  /*!
    Sets the data as values
    \param V :: Values
   */
{
  minusF=0;
  Values=V;
  return;
}

void
SrcBias::addData(const double& V) 
  /*!
    Adds a data set the data as values
    \param V :: Value
   */
{
  Values.push_back(V);
  return;
}

void
SrcBias::setFminus(const int F,const double a,const double b)
  /*!
    Set an Fminus output
    \param F :: -ve fMinus value
    \param a :: first parameter
    \param b :: second parameter
  */
{
  ELog::RegMethod RegA("SrcBias","setFminus");
  if (F!=21 && F!=-31)
    ELog::EM<<"SB card only allows -21 and -31"<<ELog::endErr;

  Values.resize(2);
  Values[0]=a;
  Values[1]=b;
  minusF=F;
  return;
}

void
SrcBias::write(const size_t Index,std::ostream& OX) const
  /*!
    Write SI line to an MCNPX output
    \param Index :: Index of the D number
    \param OX :: Output stream
  */   
{
  boost::format FMTStr("%1$11.5g ");
  std::ostringstream cx;
  if (!minusF)
    {
      cx<<"sb"<<Index<<" "<<option<<" ";
      std::vector<double>::const_iterator vc;
      for(vc=Values.begin();vc!=Values.end();vc++)
	cx<<FMTStr % *vc;
    }
  else
    {
      cx<<"sb"<<Index<<" "<<minusF<<" ";
      copy(Values.begin(),Values.end(),
	   std::ostream_iterator<double>(cx," "));
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

///\cond TEMPLATE
template DSTerm<int>* SrcData::getDS<int>();
template DSTerm<double>* SrcData::getDS<double>();
///\endcond TEMPLATE

}  // NAMESPACE SDef
