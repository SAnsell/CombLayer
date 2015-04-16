/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endf/SQWtable.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <complex>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "mathSupport.h"
#include "RefCon.h"
#include "ENDF.h"
#include "SQWtable.h"

namespace ENDF
{
  
SQWtable::SQWtable() : 
  nAlpha(0),nBeta(0)
  /*!
    Constructor
  */
{}			

SQWtable::SQWtable(const SQWtable& A) : 
  nAlpha(A.nAlpha),nBeta(A.nBeta),
  Alpha(A.Alpha),Beta(A.Beta),
  SAB(A.SAB),alphaInterp(A.alphaInterp),
  alphaIBoundary(A.alphaIBoundary),
  betaInterp(A.betaInterp),
  betaIBoundary(A.betaIBoundary)
  /*!
    Copy Constructor
    \param A :: SQWtable to copy
  */
{}			

SQWtable&
SQWtable::operator=(const SQWtable& A) 
  /*!
    Assignment operator
    \param A :: SQWtable to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nAlpha=A.nAlpha;
      nBeta=A.nBeta;
      Alpha=A.Alpha;
      Beta=A.Beta;
      SAB=A.SAB;
      alphaInterp=A.alphaInterp;
      alphaIBoundary=A.alphaIBoundary;
      betaInterp=A.betaInterp;
      betaIBoundary=A.betaIBoundary;
    }
  return *this;
}			
  
void 
SQWtable::setNAlpha(const size_t NA)
  /*!
    Set the table size
    \param NA :: Alpha size
  */
{
  nAlpha=NA;
  Alpha.resize(NA);
  if (nBeta*nAlpha)
    SAB.resize(boost::extents
	       [static_cast<long int>(nAlpha)]
	       [static_cast<long int>(nBeta)]);
  return;
}

void 
SQWtable::setNBeta(const size_t NB)
  /*!
    Set the table size
    \param NB :: Beta size
  */
{
  nBeta=NB;
  Beta.resize(NB);
  if (nBeta*nAlpha)
    {
      SAB.resize(boost::extents[nAlpha][nBeta]);
    }
  return;
}

int
SQWtable::alphaType(const long int Index) const
  /*!
    Given an index determine the type
    \param Index :: Index to find
    \return Interpolation type
  */
{
  std::vector<int>::const_iterator vc=
    lower_bound(alphaIBoundary.begin(),alphaIBoundary.end(),Index);
  return (vc!=alphaIBoundary.end()) ? 
    alphaInterp[vc-alphaIBoundary.begin()] : alphaIBoundary.back();
}

int
SQWtable::betaType(const long int Index) const
  /*!
    Given an index determine the type
    \param Index :: Index to find
    \return Interpolation type
  */
{
  std::vector<int>::const_iterator vc=
    lower_bound(betaIBoundary.begin(),betaIBoundary.end(),Index);
  return (vc!=betaIBoundary.end()) ? 
    betaInterp[vc-betaIBoundary.begin()] : betaIBoundary.back();
}

int
SQWtable::checkAlpha(const std::vector<int>& IB,
		     const std::vector<int>& II,
		     const std::vector<double>& Acomp) const
  /*!
    Checks to see if the alpha and boundaries are the same
    \param IB :: Boundaries
    \param II :: iterporation types
    \param Acomp :: Alpha values
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("SQWtable","checkAlpha");

  if (static_cast<int>(Acomp.size())!=nAlpha ||
      IB.size()!=alphaIBoundary.size() ||
      II.size()!=alphaInterp.size() )
    {
      ELog::EM<<"Size mismatch"<<ELog::endErr;
      return -1;
    }
  for(size_t i=0;i<Acomp.size();i++)
    if (fabs(Acomp[i]-Alpha[i])>1e-5)
      {
	ELog::EM<<"Alpha channel["<<i<<"] mismatch"<<ELog::endErr;
	return -2;
      }
  for(size_t i=0;i<II.size();i++)
    if (II[i]!=alphaInterp[i] || IB[i]!=alphaIBoundary[i])
      {
	ELog::EM<<"Interp channel["<<i<<"] mismatch"<<std::endl;
	ELog::EM<<"II:"<<II[i]<<" == "<<alphaInterp[i]<<std::endl;
	ELog::EM<<"IB:"<<IB[i]<<" == "<<alphaIBoundary[i]<<ELog::endErr;
	return -2;
      }
  return 0;
}

void
SQWtable::setData(const size_t index,const std::vector<double>& aVec, 
		  const std::vector<double>& sVec) 
  /*!
    Sets the data
    \param index :: Beta index value
    \param aVec :: Alpha values
    \param sVec :: Data
  */
{
  if (index>=nBeta)
    throw ColErr::IndexError<size_t>(index,nBeta,"SQWtable::setData");
  if (static_cast<int>(sVec.size())!=nAlpha)
    throw ColErr::MisMatch<size_t>(sVec.size(),nAlpha,"SQWtable::setData");

  if (index==0)
    {
      if (aVec.size()!=Alpha.size())
	throw ColErr::MisMatch<size_t>(aVec.size(),Alpha.size(),
					 "SQWtable::setData"); 
      copy(aVec.begin(),aVec.end(),Alpha.begin());
    }
  const long int LI(static_cast<long int>(index));
  for(size_t i=0;i<sVec.size();i++)
    SAB[static_cast<long int>(i)][LI]=sVec[i];

  return;
}

int 
SQWtable::isValidRangePt(const double& alphaV,const double& betaV,
			 long int& aInt,long int& bInt) const
  /*!
    Determine if the point is valid
    \param alphaV :: alpha value
    \param betaV :: beta value
    \param aInt :: index of alpha
    \param bInt :: index of beta
    \return 0 on failure and 1 on success
  */
{
  aInt=mathFunc::binSearch(Alpha.begin(),Alpha.end(),alphaV);
  bInt=mathFunc::binSearch(Beta.begin(),Beta.end(),betaV);
  // No extreme cases:
  if (!(aInt*bInt) || aInt>=nAlpha-1 || bInt>=nBeta-1)
    return 0;
  aInt--;
  bInt--;
  return 1;
}

double
SQWtable::Sab(const double alphaV,const double betaV) const
  /*!
    Calculate S(q,omega) for a neutron of energy E.
    \param alphaV :: Q-values
    \param betaV :: energy transfer
    \return S(Q,w)
  */
{
  ELog::RegMethod RegA("SQWtable","Sab");

  long int aInt,bInt;
  if (!isValidRangePt(alphaV,betaV,aInt,bInt))
    {
      // ELog::EM<<"Returning value but Material def required:"
      // 	      <<alphaV<<" "<<betaV<<ELog::endErr;
      return 0.0;
    }

  const int aType=alphaType(aInt);
  const int bType=betaType(bInt);

  const double Alow=loglinear(Alpha[aInt],Alpha[aInt+1],
			      SAB[aInt][bInt],SAB[aInt+1][bInt],
			      alphaV);

  const double Ahigh=loglinear(Alpha[aInt],Alpha[aInt+1],
			       SAB[aInt][bInt+1],SAB[aInt+1][bInt+1],
			       alphaV);

  // ELog::EM<<Alow<<" "<<Ahigh<<" :: "<<alphaV<<" "
  //  	  <<Alpha[aInt]<<" "<<Alpha[aInt+1]<<ELog::endCrit;
  // ELog::EM<<Beta[bInt]<<" "<<Beta[bInt+1]<<" "<<betaV<<ELog::endCrit;
  // ELog::EM<<SAB[aInt][bInt]<<" "<<SAB[aInt+1][bInt]<<" "
  //  	  <<SAB[aInt][bInt+1]<<" "<<SAB[aInt+1][bInt+1]<<ELog::endCrit;

  const double SAB=loglinear(Beta[bInt],Beta[bInt+1],
			     Alow,Ahigh,betaV);  
  //  ELog::EM<<ELog::endCrit;

  return SAB;
}



} // NAMESPACE ENDF

