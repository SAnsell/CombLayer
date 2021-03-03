/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/FlangeDome.cxx
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
#include <array>

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

#include "FlangeDome.h"

namespace constructSystem
{

FlangeDome::FlangeDome(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


FlangeDome::~FlangeDome()
  /*!
    Destructor
  */
{}

void
FlangeDome::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlangeDome","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  curveRadius=Control.EvalVar<double>(keyName+"CurveRadius");
  curveStep=Control.EvalVar<double>(keyName+"CurveStep");
  plateThick=Control.EvalVar<double>(keyName+"WallThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");


  return;
}

void
FlangeDome::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FlangeDome","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*plateThick,Y);
  const double& y=curveStep;
  const double& L=curveRadius;
  const double Radius= (L*L+y*y)/(2.0*y);
  
  const Geometry::Vec3D rCentre=Origin-Y*(Radius-y); 
  
  ModelSupport::buildSphere(SMap,buildIndex+8,rCentre,Radius);
  ModelSupport::buildSphere(SMap,buildIndex+18,rCentre+Y*plateThick,Radius);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,curveRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeRadius);
  
  return;
}

void
FlangeDome::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FlangeDome","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getRule("front");
  
  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex," -8 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"(-18:-2) 8 -207 ");
  makeCell("Dome",System,cellIndex++,voidMat,0.0,HR*frontHR);

  

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"(-18:-2) -207");
  addOuterSurf(HR*frontHR);

  return;
}


void
FlangeDome::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlangeDome","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  return;
}


void
FlangeDome::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlangeDome","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
