/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/surfDRule.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Token.h"
#include "Surface.h"
#include "MergeSurf.h"
#include "surfDRule.h"

namespace ModelSupport
{




 
surfDRule::surfDRule() :
  innerDirection(1),pSurf(0),
  pSPtr(0),MR(0)
  /*!
    Constructor
  */
{}

surfDRule::surfDRule(const surfDRule& A) : 
  innerDirection(A.innerDirection),pSurf(A.pSurf),
  sSurf(A.sSurf),signSurfReplace(A.signSurfReplace),
  pSPtr(A.pSPtr),sSPtr(A.sSPtr),
  MR((A.MR) ? A.MR->clone() : 0)
  /*!
    Copy constructor
    \param A :: surfDRule to copy
  */
{}

surfDRule&
surfDRule::operator=(const surfDRule& A)
  /*!
    Assignment operator
    \param A :: surfDRule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      innerDirection=A.innerDirection;
      pSurf=A.pSurf;
      sSurf=A.sSurf;
      signSurfReplace=A.signSurfReplace;
      pSPtr=A.pSPtr;
      sSPtr=A.sSPtr;
      delete MR;
      MR=(A.MR) ? A.MR->clone() : 0;
    }
  return *this;
}



surfDRule::~surfDRule()
  /*!
    Destructor 
  */
{
  delete MR;
}

void
surfDRule::populate(const Geometry::Surface* ISurf,
		    const std::vector<const Geometry::Surface*>& OSurf)
  /*!
    Populate all the surfaces
    \param ISurf :: Internal surface
    \param OSurf :: Outer surfaces
  */
{
  ELog::RegMethod RegA("surfDRule","populate");
  pSPtr=ISurf;
  sSPtr=OSurf;
  MR->initSurfaces(ISurf,OSurf);
  MR->setDirection(sSurf);
  return;
}

void
surfDRule::setMergeRule(MergeSurf* MPtr)
 /*!
   Sets the merge rule
   \param MPtr :: Merge Point [Mangaged]
  */
{
  delete MR;
  MR=MPtr;
  return;
}

void 
surfDRule::setPrimarySurf(const int iDir,const int surfN)
  /*!
    Set the primary surface number / inner direction
    \param iDir :: Inner direction
    \param surfN :: Primary surface
   */
{
  innerDirection=iDir;
  pSurf=surfN;
  return;
}

void
surfDRule::addSecondarySurf(const int SN)
  /*!
    Add a secondary surface
    \param SN :: Signed surface number
  */
{
  sSurf.push_back(SN);
  return;
}

void
surfDRule::createDividedSurf(const double Frac,int& outSurfN)
  /*!
    Divide the surface according to the rule
    \param Frac :: Fraction to use
    \param outSurfN :: output surface number
  */
{
  MR->createSurf(outSurfN,Frac);
  return;
}

void
surfDRule::addString(std::string&,std::string&) const
  /*!
    Write out a mcnpx string
    \param inComp :: input string.
    \param outComp :: output string
  */
{
  // if (control==0 || control==1)
  //   {
  //     if (!inComp.empty()) 
  // 	inComp+=" : ";
  //     if (!outComp.empty()) 
  // 	outComp+=" ";
  //     std::stringstream cx;
  //     cx<<-MSign*oSign*dividedSurf;
  //     inComp+=cx.str();
  //     cx.str("");
  //     cx<<MSign*oSign*dividedSurf;
  //     outComp+=cx.str();
  //   }
  return;
}


void
surfDRule::write(std::ostream& OX) const
  /*!
    Write out a summary
    \param OX :: Output stream
   */
{
  // OX<<"M: "<<master<<" "<<control<<" "<<IOflag
  //   <<" P="<<innerSurf<<":"<<outerSurf
  //   <<"("<<oSign<<"x"<<MSign<<")";
  return;
}

// ------------------------------------------
//            STATIC
// ------------------------------------------

int
surfDRule::replaceTokenWithSign(std::vector<Token>& TVec,
				 const int SN,
				 const std::vector<Token>& InsertVec,
				 const std::vector<Token>& CompInsVec)
  /*!
    Replace a surface unit with the inserted Vec 
    - The Complemented Insert object is needed for cases were the sign
    is opposite 
    \param TVec :: input stream
    \param SN :: Surface to modifiy [signed]
    \param InsertVec :: Unit to add
    \param CompInsVec :: Unit to add [in complement]
    \return surface found boolean
   */
{
  ELog::RegMethod RegA("surfDivide","replaceTokenWithSign");

  if (!SN) return 0;
  int flag;
  do
    {
      flag=0;
      for(size_t index=0;index<TVec.size();index++)
	{
	  const Token& mc=TVec[index];
	  // If token is a surface and its sign matches:
	  if (mc.type==1 && (mc.num==SN || mc.num==-SN))
	    {
	      if (mc.num==-SN)
		{
		  TVec.insert(TVec.begin()+(index+1),
			      CompInsVec.begin(),CompInsVec.end());
		}
	      else
		{
		  TVec.insert(TVec.begin()+(index+1),
			      InsertVec.begin(),InsertVec.end());
		} 	    
	      TVec.erase(TVec.begin()+index);
	      flag=1;
	      break;
	    }
	}
    } while(flag);

  return 0;
}

int
surfDRule::replaceToken(std::vector<Token>& TVec,const int SN,
			const std::vector<Token>& InsertVec)
  /*!
    Replace a surface unit with the inserted Vec 
    -- Disregard the sign of the surface 
    \param TVec :: input stream
    \param SN :: Surface to modifiy
    \param InsertVec :: Unit to add
    \return 1 if work done						
   */
{
  ELog::RegMethod RegA("surfDRule","replaceToken");

  if (!SN) return 0;
  for(size_t index=0;index<TVec.size();index++)
    {
      const Token& mc=TVec[index];
      if (mc.type==1 && (mc.num==SN || mc.num==-SN))
        {
	  TVec.insert(TVec.begin()+(index+1),
		      InsertVec.begin(),InsertVec.end());
	  TVec.erase(TVec.begin()+index);
	  return 1;
	}
    }
  return 0;
}

void
surfDRule::removeToken(std::vector<Token>& TVec,const int SN)
 /*!
   Static function to remove a token from a stream
   \param TVec :: Token vector
   \param SN :: surface Number to remove
 */
{
  ELog::RegMethod RegA("surfDRule","removeToken");

  if (!SN) return;
  int flag(0);
  do
    {
      flag=0;
      for(size_t i=0;i<TVec.size();i++)
        {
	  // Main loop conditions : found a N.
	  if (TVec[i].type==1 && (TVec[i].num==SN || TVec[i].num==-SN))
	    {
	      int minusI=(i!=0 && TVec[i-1].type==0 
			  && TVec[i-1].unit==':') ? 1 : 0;
	      int plusI=(i+1<TVec.size() && TVec[i+1].type==0 
			 && TVec[i+1].unit==':') ? 1 : 0;
	      if (minusI)
		TVec.erase(TVec.begin()+i-1,TVec.begin()+i);
	      else if (plusI)
		TVec.erase(TVec.begin()+i,TVec.begin()+i+1);
	      else
		TVec.erase(TVec.begin()+i);
	      flag=1;
	    }
	  
	  // Bracket delete
	  else if (i!=0 && (TVec[i].type==0 && TVec[i].unit==')')  &&
		   (TVec[i-1].type==0 && TVec[i-1].unit=='('))
	    {
	      TVec.erase(TVec.begin()+i-1,TVec.begin()+i);
	      flag=1;
	    }
	}
    } while(flag);

  return;
}

///\cond TEMPLATE
// template void surfDRule::createRule<Geometry::Plane,Geometry::Plane>();
// template void surfDRule::createRule<Geometry::Cylinder,Geometry::Cylinder>();
// template void surfDRule::createRule<Geometry::Cylinder,Geometry::Plane>();
///\endcond TEMPLATE

} // NAMESPACE ModelSupport
