/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/RectanglePipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "GeneralPipe.h"
#include "RectanglePipe.h"

namespace constructSystem
{

RectanglePipe::RectanglePipe(const std::string& Key) :
  GeneralPipe(Key,11)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

RectanglePipe::RectanglePipe(const RectanglePipe& A) :
  GeneralPipe(A)
  /*!
    Copy constructor
    \param A :: RectanglePipe to copy
  */
{}

RectanglePipe&
RectanglePipe::operator=(const RectanglePipe& A)
  /*!
    Assignment operator
    \param A :: RectanglePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralPipe::operator=(A);
    }
  return *this;
}

RectanglePipe::~RectanglePipe()
  /*!
    Destructor
  */
{}

void
RectanglePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("RectanglePipe","populate");

  GeneralPipe::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  
  return;
}

void
RectanglePipe::createSurfaces()
  /*!
    Create surfaces for general pipe
   */
{
  ELog::RegMethod RegA("RectanglePipe","createSurface");  
  GeneralPipe::createCommonSurfaces();

  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(width/2.0+pipeThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(width/2.0+pipeThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(height/2.0+pipeThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(height/2.0+pipeThick),Z);

  return;
}
  
void
RectanglePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("RectanglePipe","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3:4:-5:6");
  GeneralPipe::createFlange(System,HR);

  const HeadRule windowAHR=getRule("AWindow").complement();
  const HeadRule windowBHR=getRule("BWindow").complement();

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 13 -14 15 -16 (-3:4:-5:6)");
  makeCell("Steel",System,cellIndex++,pipeMat,0.0,HR);
  addCell("MainSteel",cellIndex-1);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-13:14:-15:16");
  GeneralPipe::createOuterVoid(System,HR);

  return;
}


void
RectanglePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("RectanglePipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  // Round pipe
  FixedComp::setConnect(2,Origin-X*(pipeThick+width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(pipeThick+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(pipeThick+height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(pipeThick+height/2.0),Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));
  
  FixedComp::setConnect(7,Origin-Z*(height/2.0+pipeThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(height/2.0+pipeThick),Z);
  HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-13:14:-15:16");
    
  FixedComp::setLinkSurf(7,HR);
  FixedComp::setLinkSurf(8,HR);
  
  FixedComp::nameSideIndex(7,"outerPipe");
  FixedComp::nameSideIndex(7,"pipeOuterBase");
  FixedComp::nameSideIndex(8,"pipeOuterTop");

  
  // MID Point: [NO SURF]
  const Geometry::Vec3D midPt=
    (getLinkPt(1)+getLinkPt(2))/2.0;

  FixedComp::setConnect(6,midPt,Y);

  FixedComp::setConnect(9,Origin-Z*flangeARadius,-Z);
  FixedComp::setConnect(10,Origin+Z*flangeARadius,Z);
  
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+107));
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+107));


  return;
}


void
RectanglePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("RectanglePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
