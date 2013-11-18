/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/surfDBase.cxx
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
#include "surfDBase.h"

namespace ModelSupport
{

// ------------------------------------------
//            STATIC
// ------------------------------------------

int
surfDBase::replaceTokenWithSign(std::vector<Token>& TVec,
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
  bool flag;
  do
    {
      flag=0;
      for(std::ptrdiff_t index=0;
	  index<static_cast<std::ptrdiff_t>(TVec.size());index++)
	{
	  const Token& mc=*(TVec.begin()+index);
	  // If token is a surface and its sign matches:
	  if (mc.type==1 && (mc.num==SN || mc.num==-SN))
	    {
	      if (mc.num==-SN)
		{
		  TVec.insert(TVec.begin()+index+1,
			      CompInsVec.begin(),CompInsVec.end());
		}
	      else
		{
		  TVec.insert(TVec.begin()+index+1,
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
surfDBase::replaceToken(std::vector<Token>& TVec,const int SN,
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
  ELog::RegMethod RegA("surfDBase","replaceToken");

  if (!SN) return 0;
  for(size_t index=0;index<TVec.size();index++)
    {
      const Token& mc=TVec[index];
      if (mc.type==1 && (mc.num==SN || mc.num==-SN))
        {
	  TVec.insert(TVec.begin()+static_cast<std::ptrdiff_t>(index+1),
		      InsertVec.begin(),InsertVec.end());
	  TVec.erase(TVec.begin()+static_cast<std::ptrdiff_t>(index));
	  return 1;
	}
    }
  return 0;
}

void
surfDBase::removeToken(std::vector<Token>& TVec,const int SN)
 /*!
   Static function to remove a token from a stream
   \param TVec :: Token vector
   \param SN :: surface Number to remove
 */
{
  ELog::RegMethod RegA("surfDRule","removeToken");

  if (!SN) return;
  bool flag(0);
  do
    {
      flag=0;
      for(size_t i=0;i<TVec.size();i++)
        {
	  const std::ptrdiff_t pi(static_cast<std::ptrdiff_t>(i));
	  // Main loop conditions : found a N.
	  if (TVec[i].type==1 && (TVec[i].num==SN || TVec[i].num==-SN))
	    {
	      int minusI=(i!=0 && TVec[i-1].type==0 
			  && TVec[i-1].unit==':') ? 1 : 0;
	      int plusI=(i+1<TVec.size() && TVec[i+1].type==0 
			 && TVec[i+1].unit==':') ? 1 : 0;
	      if (minusI)
		TVec.erase(TVec.begin()+pi-1,TVec.begin()+pi);
	      else if (plusI)
		TVec.erase(TVec.begin()+pi,TVec.begin()+pi+1);
	      else
		TVec.erase(TVec.begin()+pi);
	      flag=1;
	    }
	  // Bracket delete
	  else if (i!=0 && (TVec[i].type==0 && TVec[i].unit==')')  &&
		   (TVec[i-1].type==0 && TVec[i-1].unit=='('))
	    {
	      TVec.erase(TVec.begin()+pi-1,TVec.begin()+pi);
	      flag=1;
	    }
	}
    } while(flag);

  return;
}

}  // NAMESPACE ModelSupport
