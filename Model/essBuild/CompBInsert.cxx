/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CompBInsert.cxx
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
#include <numeric>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CompBInsert.h"

namespace essSystem
{

CompBInsert::CompBInsert(const std::string& Key)  :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()

  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CompBInsert::CompBInsert(const CompBInsert& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::FrontBackCut(A),
  NBox(A.NBox),width(A.width),height(A.height),
  length(A.length),mat(A.mat),NWall(A.NWall),wallThick(A.wallThick),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: CompBInsert to copy
  */
{}

CompBInsert&
CompBInsert::operator=(const CompBInsert& A)
  /*!
    Assignment operator
    \param A :: CompBInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      NBox=A.NBox;
      width=A.width;
      height=A.height;
      length=A.length;
      mat=A.mat;
      NWall=A.NWall;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}

CompBInsert::~CompBInsert() 
  /*!
    Destructor
  */
{}

void
CompBInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CompBInsert","populate");

  FixedRotate::populate(Control);

  NBox=Control.EvalVar<size_t>(keyName+"NBox");

  double H,W,L;
  int M;
  for(size_t index=0;index<NBox;index++)
    {
      const std::string SNum=std::to_string(index);
      H=Control.EvalPair<double>(keyName+"Height"+SNum,keyName+"Height");
      W=Control.EvalPair<double>(keyName+"Width"+SNum,keyName+"Width");
      L=Control.EvalPair<double>(keyName+"Length"+SNum,keyName+"Length");
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+SNum,keyName+"Mat");

      height.push_back(H);
      width.push_back(W);
      length.push_back(L);
      mat.push_back(M);
    }
  
  NWall=Control.EvalVar<size_t>(keyName+"NWall");
  double T;
  for(size_t index=0;index<NWall;index++)
    {
      const std::string SNum=std::to_string(index);
      T=Control.EvalPair<double>(keyName+"WallThick"+SNum,keyName+"WallThick");
      M=ModelSupport::EvalMat<int>(Control,keyName+"WallMat"+SNum,keyName+"WallMat");
      wallThick.push_back(T);
      wallMat.push_back(M);
    }
  wallThick.push_back(0.0);
  
  return;
}
  
  
void
CompBInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CompBInsert","createSurface");

  int SI(buildIndex);
  double L(0.0);
  for(size_t index=0;index<NBox;index++)
    {
      double T(0.0);
      int SWI(SI);
      for(size_t wallIndex=0;wallIndex<=NWall;wallIndex++)
        {
	  ModelSupport::buildPlane(SMap,SWI+1,Origin+Y*(T+L),Y);
          ModelSupport::buildPlane(SMap,SWI+3,Origin-X*(T+width[index]/2.0),X);
          ModelSupport::buildPlane(SMap,SWI+4,Origin+X*(T+width[index]/2.0),X);
          ModelSupport::buildPlane(SMap,SWI+5,Origin-Z*(T+height[index]/2.0),Z);
          ModelSupport::buildPlane(SMap,SWI+6,Origin+Z*(T+height[index]/2.0),Z);
          SWI+=10;
          T+=wallThick[wallIndex];
        }
      L+=length[index];
      SI+=100;
    }
	    
  ModelSupport::buildPlane(SMap,SI+1,Origin+Y*L,Y);
  if (!frontActive())
    setFront(SMap.realSurf(buildIndex+1));
  if (!backActive())
    setBack(-SMap.realSurf(SI+1));

  return;
}
  
void
CompBInsert::createObjects(Simulation& System)
/*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CompBInsert","createObjects");

  HeadRule HR;

  int SI(buildIndex);

  HeadRule frontBackHR;
  for(size_t index=0;index<NBox;index++)
    {
      HeadRule innerHR;
      int SWI(SI);
      frontBackHR= (index) ? HeadRule(SMap,SI,1) : getFrontRule();
      frontBackHR*= ((index+1)!=NBox) ?
	HeadRule(SMap,SI,-101) : getBackRule();

      for(size_t wallIndex=0;wallIndex<=NWall;wallIndex++)
        {
          HR=ModelSupport::getHeadRule(SMap,SWI,"3 -4 5 -6");
	  if (!wallIndex)
	    makeCell("Item",System,cellIndex++,mat[index],0.0,
		     HR*innerHR*frontBackHR);
	  else
	    makeCell("Wall",System,cellIndex++,wallMat[wallIndex-1],0.0,
		     HR*innerHR*frontBackHR);
          innerHR=HR.complement();
          SWI+=10;
        }
      addOuterUnionSurf(HR*frontBackHR);
      SI+=100;
    }
  return;
}
  
void
CompBInsert::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("CompBInsert","createLinks");

  setNConnect(NBox*6+1);

  FrontBackCut::createLinks(*this,Origin,Y);
  if (!frontActive())
    {
      FixedComp::setConnect(0,Origin,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
    }
  
  size_t LOffset(0);
  double L(0.0);
  const int WIOffset(static_cast<int>(NWall)*10);
  int SI(buildIndex);

  const double T=std::accumulate(wallThick.begin(),wallThick.end(),0.0);

  for(size_t index=0;index<NBox;index++)
    {
      FixedComp::setConnect(LOffset+2,Origin-X*(T+width[index]/2.0),-X);
      FixedComp::setConnect(LOffset+3,Origin+X*(T+width[index]/2.0),X);
      FixedComp::setConnect(LOffset+4,Origin-Z*(T+height[index]/2.0),-Z);
      FixedComp::setConnect(LOffset+5,Origin+Z*(T+height[index]/2.0),Z);

      FixedComp::setLinkSurf(2,SMap.realSurf(SI+WIOffset+3));
      FixedComp::setLinkSurf(3,-SMap.realSurf(SI+WIOffset+4));
      FixedComp::setLinkSurf(4,SMap.realSurf(SI+WIOffset+5));
      FixedComp::setLinkSurf(5,-SMap.realSurf(SI+WIOffset+6));

      L+=length[index];
      LOffset+=6;
      SI+=100;
    }
  
  if (!backActive())
    {
      FixedComp::setConnect(1,Origin+Y*L,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+1));
    }
  // Mid point [useful for guides etc]
  FixedComp::setConnect(6,Origin+Y*(L/2.0),Y);
  
  return;
}


void
CompBInsert::createAll(Simulation& System,
                       const attachSystem::FixedComp& FC,
                       const long int sideIndex)
  
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link for origin
  */
{
  ELog::RegMethod RegA("CompBInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  createObjects(System);
  createLinks();
  
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
