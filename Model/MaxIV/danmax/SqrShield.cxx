/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/danmax/SqrShield.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "SqrShield.h"

namespace xraySystem
{

SqrShield::SqrShield(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
}

SqrShield::~SqrShield()
  /*!
    Destructor
  */
{}

void
SqrShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SqrShield","populate");

  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalDefVar<double>(keyName+"Lenght",0.0);
  thick=Control.EvalVar<double>(keyName+"Thick");
  
  skinThick=Control.EvalVar<double>(keyName+"SkinThick");


  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");


  return;
}

void
SqrShield::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SqrShield","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  int BI(buildIndex);
  double T(0.0);
  for(const double extra : {0.0,skinThick,thick,skinThick})
    {
      T+=extra;
      ModelSupport::buildPlane(SMap,BI+3,Origin-X*(T+width/2.0),X);
      ModelSupport::buildPlane(SMap,BI+4,Origin+X*(T+width/2.0),X);
      ModelSupport::buildPlane(SMap,BI+5,Origin-Z*(T+height/2.0),Z);
      ModelSupport::buildPlane(SMap,BI+6,Origin+Z*(T+height/2.0),Z);
      BI+=10;
    }
  return;
}

HeadRule
SqrShield::getInnerVoid() const
  /*!
    Get the inner void psace
    \return HeadRule of inner void [minus front/back]
   */
{
  const std::string Out=ModelSupport::getComposite
    (SMap,buildIndex," 3 -4 5 -6");  
  return HeadRule(Out);
}  


void
SqrShield::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SqrShield","createObjects");

  std::string Out;
  const std::string frontStr(frontRule());
  const std::string backStr(backRule());


  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 13 -14 15 -16 (-3:4:-5:6)");
  CellMap::makeCell("Inner",System,cellIndex++,skinMat,0.0,
		    Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 23 -24 25 -26 (-13:14:-15:16)");
  CellMap::makeCell("Wall",System,cellIndex++,mat,0.0,
		    Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 33 -34 35 -36 (-23:24:-25:26)");
  CellMap::makeCell("Outer",System,cellIndex++,skinMat,0.0,
		    Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 33 -34 35 -36 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
SqrShield::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("SqrShield","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
SqrShield::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SqrShield","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
