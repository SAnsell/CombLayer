/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/surfDBase.cxx
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
#include <memory>
# 
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
#include "Vec3D.h"
#include "Token.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfDBase.h"

namespace ModelSupport
{

// ------------------------------------------
//            STATIC
// ------------------------------------------

std::ostream&
operator<<(std::ostream& OX,const surfDBase& A)
  /*!
    Output operator
  */
{
  A.write(OX);
  return OX;
}

template<>
Geometry::Surface*
surfDBase::createSurf<Geometry::Plane,Geometry::Plane>
(const Geometry::Plane* pSurf,const Geometry::Plane* sSurf,
   const double fraction,int& newItem)
  /*!
    Divides two planes to the required fraction. Care is taken
    if the two planes have opposite signs.
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param pSurf :: primary surface
    \param sSurf :: secondary surface
    \param newItem :: Plane number to start with
    \param fraction :: Weight between the two surface
    \return plane number created
   */
{
  ELog::RegMethod RegA("surfDBase<Plane>","createSurf");

  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D NormA=pSurf->getNormal();
  const double DA=pSurf->getDistance();

  Geometry::Vec3D NormB=sSurf->getNormal();
  double DB=sSurf->getDistance();
  double dProd=NormA.dotProd(NormB);
  double normNeg=1.0;
  if (dProd<0)
    {
      normNeg=-1.0;
      NormB*=-1.0;
      DB*=-1.0;
    }
  NormB*=fraction;
  NormB+=NormA*(1.0-fraction);
  NormB.makeUnit();
  DB*=fraction;
  DB+=(1.0-fraction)*DA;
  
  // This needs to be checked / minimized
  Geometry::Plane* TP=SurI.createUniqSurf<Geometry::Plane>(newItem++);
  TP->setPlane(NormB*normNeg,DB*normNeg);
  return SurI.addSurface(TP);
}

template<>
Geometry::Surface*
surfDBase::createSurf<Geometry::Cylinder,Geometry::Cylinder>
(const Geometry::Cylinder* pSurf,const Geometry::Cylinder* sSurf,
   const double fraction,int& newItem)
  /*!
    Divides two cylinders to the required fraction. Care is taken
    if the two planes have opposite signs.
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param pSurf :: primary surface
    \param sSurf :: secondary surface
    \param newItem :: Plane number to start with
    \param fraction :: Weight between the two surface
    \return plane number created
   */
{
  ELog::RegMethod RegA("surfDBase<Cylinder,Cylinder>","createSurf");

  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D CentA=pSurf->getCentre();
  const Geometry::Vec3D NormA=pSurf->getNormal();
  const double RA=pSurf->getRadius();
  
  Geometry::Vec3D CentB=sSurf->getCentre();
  Geometry::Vec3D NormB=sSurf->getNormal();
  double RB=sSurf->getRadius();
  if (NormA.dotProd(NormB)<0)
    NormB*=-1.0;
  NormB*=fraction;
  NormB+=NormA*(1.0-fraction);
  CentB*=fraction;
  CentB+=CentA*(1.0-fraction);
  RB*=fraction;
  RB+=(1.0-fraction)*RA;
  
  Geometry::Cylinder* CPtr=
    SurI.createUniqSurf<Geometry::Cylinder>(newItem++);
  CPtr->setCylinder(CentB,NormB,RB);
  return SurI.addSurface(CPtr);
}

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
