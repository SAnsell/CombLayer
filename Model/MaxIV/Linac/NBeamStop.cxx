/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/NBeamStop.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "Exception.h"
#include "BaseModVisit.h"
#include "Importance.h"
#include "Object.h"

#include "NBeamStop.h"

namespace tdcSystem
{

coreUnit::coreUnit(const coreUnit& A) :
  radius(A.radius),length(A.length),mat(A.mat)
  /*
    Copy copstructor
    \param A :: coreUnit to copy
  */
{}

coreUnit&
coreUnit::operator=(const coreUnit& A)
  /*!
    Standard assignement operator
    \param A :: coreUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      radius=A.radius;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}

void
coreUnit::addUnit(const double L,const int M)
  /*!
    Add a full unit
   */
{
  length.push_back(L);
  mat.push_back(M);
  return;
}

void
coreUnit::addMat(const int M)
  /*!
    Add a extra material (last)
   */
{
  mat.push_back(M);
  return;
}
  
NBeamStop::NBeamStop(const std::string& Key)  :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  nLayers(0),fullLength(0.0),outerRadius(0.0)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

NBeamStop::NBeamStop(const NBeamStop& A) :
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  nLayers(A.nLayers),fullLength(A.fullLength),
  outerRadius(A.outerRadius),units(A.units)
  /*!
    Copy constructor
    \param A :: NBeamStop to copy
  */
{}

NBeamStop&
NBeamStop::operator=(const NBeamStop& A)
  /*!
    Assignment operator
    \param A :: NBeamStop to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      nLayers=A.nLayers;
      fullLength=A.fullLength;
      outerRadius=A.outerRadius;
      units=A.units;
    }
  return *this;
}

NBeamStop::~NBeamStop()
  /*!
    Destructor
  */
{}

void
NBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("NBeamStop","populate");

  FixedRotate::populate(Control);
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  fullLength=Control.EvalVar<double>(keyName+"Length");
  outerRadius=0.0;
  
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string layerName(keyName+"Layer"+std::to_string(i));
      const double R=Control.EvalVar<double>(layerName+"Radius");

      if (R<outerRadius)
	throw ColErr::SizeError<double>
	  (R,outerRadius,"Radius too small/out of order");
      
      size_t index(0);
      double totalLength(0.0);
      double L;
      int M;
      std::string UNum("0");
      coreUnit cu(R);
      while(totalLength<fullLength-Geometry::zeroTol &&
	    Control.hasVariable(layerName+"Mat"+UNum))
	{
	  L=Control.EvalDefVar<double>(layerName+"Length"+UNum,-1.0);
	  M=ModelSupport::EvalMat<int>(Control,layerName+"Mat"+UNum);
	  if (L<=0.0 || totalLength+L > fullLength)
	    {
	      totalLength=10.0+L;
	      cu.addMat(M);
	    }
	  else
	    {
	      cu.addUnit(L,M);
	    }
	  
	  index++;
	  UNum=std::to_string(index);
	}
      
      if (totalLength>fullLength-Geometry::zeroTol)
	throw ColErr::SizeError<double>
	  (totalLength,L,"Length of unit:"+layerName);
      units.push_back(cu);
      outerRadius=cu.radius;
    }
  return;
}

void
NBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("NBeamStop","createSurfaces");

  if (!isActive("front"))
    {
      makePlane("front",SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      makePlane("back",SMap,buildIndex+2,Origin+Y*fullLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  int BI(buildIndex+100);
  for(const coreUnit& cu : units)
    {
      ModelSupport::buildCylinder(SMap,BI+7,Origin,Y,cu.radius);
      int PI(BI);
      for(const double L : cu.length)
	{
	  ModelSupport::buildPlane(SMap,PI+1,Origin+Y*L,Y);
	  PI+=10;
	}
      BI+=100;
    }

  return;
}

void
NBeamStop::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("NBeamStop","createObjects");

  const HeadRule& frontHR=ExternalCut::getRule("front");
  const HeadRule& backHR=ExternalCut::getRule("back");
  const HeadRule& baseHR=ExternalCut::getRule("base");
  
  HeadRule rInnerHR,rOuterHR;
  int BI(buildIndex+100);
  int index(0);
  for(const coreUnit& cu : units)
    {
      const std::string lName="Layer"+std::to_string(index);
      rInnerHR=rOuterHR.complement();
      rOuterHR=ModelSupport::getSetHeadRule(SMap,BI,"-7");
      HeadRule aHR=frontHR;
      int PI(BI);
      size_t cuI(1);
      for(cuI=1;cuI<cu.mat.size();cuI++)
	{
	  const HeadRule bHR=ModelSupport::getHeadRule(SMap,PI,"-1");
	  makeCell(lName,System,cellIndex++,
		   cu.mat[cuI-1],0.0,aHR*bHR*rOuterHR*rInnerHR*baseHR);
	  aHR=bHR.complement();
	  PI+=10;
	}
      // last cell:
      makeCell(lName,System,cellIndex++,
	       cu.mat[cuI-1],0.0,aHR*backHR*rOuterHR*rInnerHR*baseHR);
      BI+=100;
      index++;
    }

  addOuterSurf(rOuterHR*frontHR*backHR);
  return;
}

void
NBeamStop::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("NBeamStop","createLinks");

  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*fullLength,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}


void
NBeamStop::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("NBeamStop","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
