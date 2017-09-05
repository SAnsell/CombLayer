/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/SpecialSurf.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "SurInter.h"
#include "SpecialSurf.h"

namespace ModelSupport
{

int
addPlane(const int ID,const Geometry::Vec3D& Norm,
	 const int S1,const int S2,const int S3,const int TSurf)
  /*!
    Given three planes/surfaces finds the best intersection
    point and then uses that so a surface with the appropiate
    normalisation vector goes through it.
    \param ID :: Plane ID
    \param Norm :: Normal vector of the plane
    \param S1 :: Surface A
    \param S2 :: Surface B
    \param S3 :: Surface C
    \param TSurf :: Test surface (+/-)
    \retval 0 :: failed
    \retval 1 :: success
  */
{
  ELog::RegMethod RegA("SpecialSurf[F]","addPlane");
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  std::vector<Geometry::Vec3D> Out;
  std::vector<const Geometry::Surface*> SVec;
  int Snum[]={S1,S2,S3,abs(TSurf)};


  Geometry::Surface* SPtr(0);
  Geometry::Surface* TSurfPtr(0);
  for(int i=0;i<4;i++)
    {
      if (i<3 || Snum[i])
	{
	  SPtr=SurI.getSurf(Snum[i]);
	  if (!SPtr)
	    {
	      ELog::EM<<"Surface not found:"<<Snum[i]<<ELog::endErr;
	      return 0;
	    }
	  if (i<3)
	    SVec.push_back(SPtr);
	  else
	    TSurfPtr=SPtr;
	}
    }

  Out=SurInter::processPoint(SVec[0],SVec[1],SVec[2]);
  if (Out.empty())
    {
      ELog::EM<<"Surfaces Parallel for :"
	      <<S1<<" "<<S2<<" "<<S3<<ELog::endErr;
      return 0;
    }

  std::vector<Geometry::Vec3D>::const_iterator vxc;
  for(vxc=Out.begin();vxc!=Out.end();vxc++)
    {
      int flag(1);
      if (TSurfPtr)
	{
	  const int side=TSurfPtr->side(*vxc);
	  flag= (side*TSurf>0) ? 1 : 0;
	}
      if (flag)
	{
	  Geometry::Plane* M=SurI.createSurf<Geometry::Plane>(ID);
	  M->setPlane(*vxc,Norm);
	  return 1;
	}
    }
  return 0;
}

int
addPlane(const int ID,const Geometry::Vec3D& Norm,
	 const int S1,const int S2,const int S3)
  /*!
    Given three planes/surfaces finds the best intersection
    point and then uses that so a surface with the appropiate
    normalisation vector goes through it.
    \param ID :: Plane ID
    \param Norm :: Norm of the plane
    \param S1 :: Surface A
    \param S2 :: Surface B
    \param S3 :: Surface C

    \retval 0 :: failed
    \retval 1 :: success
  */
{
  return addPlane(ID,Norm,S1,S2,S3,0);
}

int
addPlane(const int ID,const Geometry::Vec3D& Norm,
	 const Geometry::Vec3D& Intercept)
  /*!
    Given the normal and the intercept determine the plane
    \param ID :: Plane ID
    \param Norm :: Norm of the plane
    \param Intercept :: Norm of the plane
    \retval 0 :: failed
    \retval 1 :: success
  */
{
  ELog::RegMethod RegA("SpecialSurf[F]","addPlane");
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::Plane* M=SurI.createUniqSurf<Geometry::Plane>(ID);
  M->setPlane(Intercept,Norm);
  if (M->getName()!=ID)
    ELog::EM<<"Surface Name change ::"<<ID<<" "<<M->getName()<<ELog::endWarn; 
  
  return 1;
}

size_t
calcVertex(const int SA,const int SB,const int SC,
	   Geometry::Vec3D& Out,
	   const Geometry::Vec3D& nearPoint) 
  /*!
    Calculates the intercept of three surfaces
    \param SA :: layer for intercept
    \param SB :: layer for intercept
    \param SC :: layer for intercept
    \param Out :: output stream
    \param nearPoint :: Extra check to decide the side
    \return Number of sides
  */
{
  ELog::RegMethod RegA("SpecialSurf[F]","calcVertex");
  
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();


  std::vector<const Geometry::Surface*> SVec;
  const int SNum[3]={SA,SB,SC};

  for(int i=0;i<3;i++)
    {
      const Geometry::Surface* SPtr=SurI.getSurf(SNum[i]);
      if (!SPtr)
	{
	  ELog::EM<<"Surface not found:"<<SNum[i]<<ELog::endErr;
	  return 0;
	}
      SVec.push_back(SPtr);
    }

  const std::vector<Geometry::Vec3D> OutPts=
    SurInter::processPoint(SVec[0],SVec[1],SVec[2]);
  if (OutPts.empty()) return 0;
  Out=SurInter::nearPoint(OutPts,nearPoint);
  return 1;
}

size_t
calcVertex(const int SA,const int SB,const int SC,
	   std::vector<Geometry::Vec3D>& Out,
	   const int planeExtra) 
  /*!
    Calculates the intercept of three surfaces
    \param SA :: layer for intercept
    \param SB :: layer for intercept
    \param SC :: layer for intercept
    \param Out :: output stream
    \param planeExtra :: Extra check to decide the side
    \return Number of sides
  */
{
  ELog::RegMethod RegA("SpecialSurf","calcVertex(plane)");
  
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  Out.clear();
  std::vector<const Geometry::Surface*> SVec;
  const int SNum[3]={SA,SB,SC};

  for(int i=0;i<3;i++)
    {
      Geometry::Surface* SPtr=SurI.getSurf(SNum[i]);
      if (!SPtr)
	{
	  ELog::EM<<"Surface not found:"<<SNum[i]<<ELog::endErr;
	  return 0;
	}
      SVec.push_back(SPtr);
    }

  Out=SurInter::processPoint(SVec[0],SVec[1],SVec[2]);
  if (planeExtra)
    {
      const int pE((planeExtra>0) ? planeExtra : -planeExtra);
      Geometry::Surface* SPtr=SurI.getSurf(pE);
      if (!SPtr) 
	{
	  ELog::EM<<"Surfaces = "<<SA<<" "<<SB<<" "<<SC<<ELog::endTrace;
	  ELog::EM<<"Extra Surface not found:"<<planeExtra<<ELog::endErr;
	  return 0;
	}
      std::vector<Geometry::Vec3D> XOut;
      std::vector<Geometry::Vec3D>::const_iterator oc;
      for(oc=Out.begin();oc!=Out.end();oc++)
        {
	  if (SPtr->side(*oc)*planeExtra>0)
	      XOut.push_back(*oc);
	}
      Out=XOut;
    }

  return Out.size();
}

void
listSurfaces() 
  /*!
    List the surface units 
  */ 
{
  ELog::RegMethod RegA("SpecialSurf","listSurfaces");

  typedef std::map<int,Geometry::Surface*> STYPE;
  const STYPE& SurMap=ModelSupport::surfIndex::Instance().surMap();

  ELog::EM<<"Start of surface list -- "<<SurMap.size()<<ELog::endBasic;
  for(const STYPE::value_type& SI : SurMap)
    SI.second->write(ELog::EM.Estream());
  
  return;
}

int
checkSurface(const int surfNum,const Geometry::Vec3D& Pt)
  /*!
    Returns the value of the surface sense realtive to the point.
    Used exclusively for debugging.
    \param surfNum :: Surface number
    \param Pt :: Point to check side against
    \retval 1 :: same side
    \retval 0 :: on the surface
    \retval -1 :: other side
  */
{
  ELog::RegMethod RegA("SpecialSurf[F]","checkSurface");
  const int pE((surfNum>0) ? surfNum : -surfNum);
  Geometry::Surface* SPtr=ModelSupport::surfIndex::Instance().getSurf(pE);
  if (!SPtr)
    {
      ELog::EM<<"Failed to find surface :"<<surfNum<<ELog::endErr;
      return -1000;
    }

  return (surfNum>0) ? SPtr->side(Pt) : -SPtr->side(Pt);
}

} // NAMESPACE ModelSupport
