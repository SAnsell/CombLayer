/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomMonitor.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h" 
#include "ContainedComp.h"
#include "ZoomMonitor.h"

namespace zoomSystem
{

ZoomMonitor::ZoomMonitor(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomMonitor::ZoomMonitor(const ZoomMonitor& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  viewThick(A.viewThick),
  wallThick(A.wallThick),length(A.length),radius(A.radius),
  mat(A.mat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: ZoomMonitor to copy
  */
{}

ZoomMonitor&
ZoomMonitor::operator=(const ZoomMonitor& A)
  /*!
    Assignment operator
    \param A :: ZoomMonitor to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      viewThick=A.viewThick;
      wallThick=A.wallThick;
      length=A.length;
      radius=A.radius;
      mat=A.mat;
      wallMat=A.wallMat;
    }
  return *this;
}

ZoomMonitor::~ZoomMonitor() 
  /*!
    Destructor
  */
{}


void
ZoomMonitor::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ZoomMonitor","populate");

  FixedOffset::populate(Control);
  
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Depth");
  viewThick=Control.EvalVar<double>(keyName+"ViewThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ZoomMonitor::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  FixedComp::setConnect(0,Origin-Y*(length/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setConnect(1,Origin+Y*(length/2.0+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setConnect(4,Origin-Z*(radius+wallThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(radius+wallThick),Z);

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+17));
  return;
}

void
ZoomMonitor::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomMonitor","createSurface");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y); 
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y); 
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius); 

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(length/2.0+wallThick),Y); 
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length/2.0+wallThick),Y); 
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,
			      radius+wallThick); 


  return;
}

void
ZoomMonitor::createObjects(Simulation& System)
 /*!
    Adds the zoom monitor
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomMonitor","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -17");
  addOuterSurf(Out);      

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -17 (-1 : 2 : 7)");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  return;
}

void
ZoomMonitor::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Unit for origin
    \param linkIndex :: Index for orientation [0 for central origin]
    \param Region :: Contained Object to build monitor in
  */
{
  ELog::RegMethod RegA("ZoomMonitor","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,linkIndex);
  createSurfaces();
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem
