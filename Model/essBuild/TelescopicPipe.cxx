/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TelescopicPipe.cxx
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
#include <numeric>
#include <memory>

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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "TelescopicPipe.h"

namespace essSystem
{
  
TelescopicPipe::TelescopicPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedGroup(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Keyname
  */
{}

TelescopicPipe::TelescopicPipe(const TelescopicPipe& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::FrontBackCut(A),
  attachSystem::CellMap(A),
  nSec(A.nSec),radius(A.radius),length(A.length),zCut(A.zCut),
  thick(A.thick),inMat(A.inMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: TelescopicPipe to copy
  */
{}

   
TelescopicPipe&
TelescopicPipe::operator=(const TelescopicPipe& A)
  /*!
    Assignment operator
    \param A :: TelescopicPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      nSec=A.nSec;
      radius=A.radius;
      length=A.length;
      zCut=A.zCut;
      thick=A.thick;
      inMat=A.inMat;
      wallMat=A.wallMat;
    }
  return *this;
}
   
TelescopicPipe*  
TelescopicPipe::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
  */
{
  return new TelescopicPipe(*this);
}

TelescopicPipe::~TelescopicPipe()
  /*!
    Destructor
   */
{}

void
TelescopicPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TelescopicPipe","populate");

  nSec=Control.EvalVar<size_t>(keyName+"NSection");   
  double R,L(0.0),C,T;
  int Imat,Wmat;

  for(size_t i=0;i<nSec;i++)
    {
      const std::string Num(std::to_string(i+1));
      R=Control.EvalVar<double>(keyName+"Radius"+Num);
      L+=Control.EvalVar<double>(keyName+"Length"+Num);   
      C=Control.EvalVar<double>(keyName+"Zcut"+Num);   
      T=Control.EvalVar<double>(keyName+"WallThick"+Num);     
      Imat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat"+Num);   
      Wmat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat"+Num);   
      
      radius.push_back(R);
      length.push_back(L);
      zCut.push_back(C);
      thick.push_back(T);
      inMat.push_back(Imat);
      wallMat.push_back(Wmat);
    }

  return;
}

void
TelescopicPipe::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createSurfaces");
 

  int PT(buildIndex);
  for(size_t i=0;i<nSec;i++)
    {
     ModelSupport::buildCylinder(SMap,PT+7,Origin,Y,radius[i]);  
     ModelSupport::buildCylinder(SMap,PT+17,Origin,Y,radius[i]+thick[i]); 

     ModelSupport::buildPlane(SMap,PT+2,Origin+Y*length[i],Y);  
     if (zCut[i]>0.0)
       {
	 ModelSupport::buildPlane(SMap,PT+5,Origin-Z*(radius[i]-zCut[i]),Z);
	 ModelSupport::buildPlane(SMap,PT+6,Origin+Z*(radius[i]-zCut[i]),Z);  
       }
     PT+=100;
    }
  return; 
}

void
TelescopicPipe::createObjects(Simulation& System)
  /*!
    Adds the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createObjects");

  HeadRule HR,EndCap,FrontCap;

  int PT(buildIndex);
  attachSystem::ContainedGroup::addCC("Full");
  
  for(size_t i=0;i<nSec;i++)
    {
      const std::string SName="Sector"+std::to_string(i);

      FrontCap=(!i) ? getFrontRule() : HeadRule(SMap,PT-100,2);      
      EndCap=(i+1 == nSec) ? getBackRule() : HeadRule(SMap,PT,-2);
      FrontCap*=EndCap;
      
      if (thick[i]>Geometry::zeroTol)
	{
	  HR=ModelSupport::getSetHeadRule(SMap,PT,"7 -17 5 -6");
	  System.addCell(cellIndex++,wallMat[i],0.0,HR*FrontCap);
	}

      HR=ModelSupport::getSetHeadRule(SMap,PT,"-7 5 -6");
      System.addCell(cellIndex++,inMat[i],0.0,HR*FrontCap);

      HR=ModelSupport::getSetHeadRule(SMap,PT,"-17 5 -6");
      attachSystem::ContainedGroup::addCC(SName);
      addOuterSurf(SName,HR*FrontCap);
      addOuterUnionSurf("Full",HR*FrontCap);

      PT+=100;
    }
  return;
}


void
TelescopicPipe::createLinks()
  /*!
    Creates a full attachment set
  */
{ 
  ELog::RegMethod RegA("TelescopicPipe","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setNConnect(nSec+2);
  int PT(buildIndex);
  for(size_t i=0;i<nSec;i++)
    {
      FixedComp::setConnect(i+2,Origin+Y*length[i]/2.0,-X);
      FixedComp::setLinkSurf(i+2,-SMap.realSurf(PT+7));
      PT+=100;
    } 
  return;
}

void
TelescopicPipe::createAll(Simulation& System,
			  const attachSystem::FixedComp& TargetFC,
			  const long int tIndex)
  /*!
    Global creation of the pipe
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target outer surf
    \param tIndex :: Target plate surface [signed]
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createAll");
  //  populate(System);
  populate(System.getDataBase());

  createUnitVector(TargetFC,tIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System); 
  return;
}

}  // NAMESPACE essSystem
