/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuild/Magnet.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "Magnet.h"

namespace epbSystem
{

Magnet::Magnet(const std::string& Key,const size_t Index) : 
  attachSystem::FixedComp(StrFunc::makeString(Key,Index),6),
  attachSystem::ContainedComp(),baseName(Key),
  magIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(magIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Magnet::~Magnet() 
  /*!
    Destructor
  */
{}

void
Magnet::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Magnet","populate");

  const FuncDataBase& Control=System.getDataBase();

  segIndex=Control.EvalVar<size_t>(keyName+"SegIndex");
  segLen=Control.EvalDefVar<size_t>(keyName+"SegLen",1);

  xStep=Control.EvalPair<double>(keyName,baseName,"XStep");
  yStep=Control.EvalPair<double>(keyName,baseName,"YStep");
  zStep=Control.EvalPair<double>(keyName,baseName,"ZStep");
  xyAngle=Control.EvalPair<double>(keyName,baseName,"XYangle");
  zAngle=Control.EvalPair<double>(keyName,baseName,"Zangle");

  length=Control.EvalPair<double>(keyName,baseName,"Length");
  height=Control.EvalPair<double>(keyName,baseName,"Height");
  width=Control.EvalPair<double>(keyName,baseName,"Width");

  feMat=Control.EvalPair<int>(keyName,baseName,"FeMat");

  return;
}

void
Magnet::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: TwinComp to attach to
  */
{
  ELog::RegMethod RegA("Magnet","createUnitVector");

  Geometry::Vec3D Axis;
  for(size_t i=0;i<segLen;i++)
    Axis+=FC.getLU(segIndex+i).getAxis();
  Axis/=static_cast<double>(segLen);

  beamAxis=FC.getLU(segIndex+segLen-1).getAxis();

  const attachSystem::LinkUnit& LU=FC.getLU(segIndex);
  FixedComp::createUnitVector(LU.getConnectPt(),Axis,
			      FC.getX()*Axis);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  return;
}

void
Magnet::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Magnet","createSurface");

  ModelSupport::buildPlane(SMap,magIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,magIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,magIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,magIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,magIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,magIndex+6,Origin+Z*(height/2.0),Z);
  return;
}

void
Magnet::createObjects(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param FC :: inner guide
   */
{
  ELog::RegMethod RegA("Magnet","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,magIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      
  for(size_t i=0;i<segLen;i++)
    Out+=FC.getLinkString(segIndex+i);
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));

  return;
}

void 
Magnet::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Magnet","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);     
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);     
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);     
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);     
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);     
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(magIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(magIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(magIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(magIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(magIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(magIndex+6));
  
  return;
}


void
Magnet::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track ( epbline)
  */
{
  ELog::RegMethod RegA("Magnet","createAll");
  
  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System,FC);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem
