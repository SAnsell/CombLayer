/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/BeamWindow.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "BeamWindow.h"

namespace ts1System
{

BeamWindow::BeamWindow(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,2),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamWindow::BeamWindow(const BeamWindow& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::ExternalCut(A),
  incThick1(A.incThick1),waterThick(A.waterThick),
  incThick2(A.incThick2),heMat(A.heMat),
  inconelMat(A.inconelMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: BeamWindow to copy
  */
{}

BeamWindow&
BeamWindow::operator=(const BeamWindow& A)
  /*!
    Assignment operator
    \param A :: BeamWindow to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      incThick1=A.incThick1;
      waterThick=A.waterThick;
      incThick2=A.incThick2;
      heMat=A.heMat;
      inconelMat=A.inconelMat;
      waterMat=A.waterMat;
    }
  return *this;
}


BeamWindow::~BeamWindow() 
 /*!
   Destructor
 */
{}

void
BeamWindow::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("BeamWindow","populate");

  FixedRotate::populate(Control);
  // Master values

  incThick1=Control.EvalVar<double>(keyName+"IncThick1");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  incThick2=Control.EvalVar<double>(keyName+"IncThick2");
  
  // Materials
  inconelMat=ModelSupport::EvalMat<int>(Control,keyName+"InconelMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
BeamWindow::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamWindow","createSurface");

  // Thick
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*incThick1,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin+
			   Y*(incThick1+waterThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+
			   Y*(incThick1+waterThick+incThick2),Y);

  return;
}

void
BeamWindow::createObjects(Simulation& System)
  /*!
    Adds the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamWindow","createObjects");

  const HeadRule BoundaryHR=getRule("Boundary");
  
  HeadRule HR;

  // Master box: 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -4");
  addOuterSurf(HR);

  //  const std::string Boundary=getCompContainer();

  // Inconel1 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2  ");
  makeCell("Inconel",System,cellIndex++,inconelMat,0.0,HR*BoundaryHR);
  // Water 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -3 ");
  makeCell("Water",System,cellIndex++,waterMat,0.0,HR*BoundaryHR);
  // Inconel2 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 ");
  makeCell("Inconel",System,cellIndex++,inconelMat,0.0,HR*BoundaryHR);


  return;
}

void
BeamWindow::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*(incThick1+waterThick+incThick2),Y);

  return;
}

void
BeamWindow::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int targetFrontIndex)
  /*!
    Global creation of the BeamWindow
    \param System :: Simulation to add vessel to
    \param FC :: Fixed target object
    \param targetFrontIndex :: front point of the target
  */
{
  ELog::RegMethod RegA("BeamWindow","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,targetFrontIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       


  return;
}

  
}  // NAMESPACE t1System
