/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/VacuumVessel.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "RingSeal.h"
#include "RingFlange.h"
#include "VacuumVessel.h"


namespace photonSystem
{
      
VacuumVessel::VacuumVessel(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  CentPort(new constructSystem::RingFlange(keyName+"CentPort"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(CentPort);
}

VacuumVessel::VacuumVessel(const VacuumVessel& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),  
  wallThick(A.wallThick),frontLength(A.frontLength),
  radius(A.radius),backLength(A.backLength),width(A.width),
  height(A.height),doorRadius(A.doorRadius),doorEdge(A.doorEdge),
  doorThick(A.doorThick),voidMat(A.voidMat),doorMat(A.doorMat),
  mat(A.mat),CentPort(new constructSystem::RingFlange(*A.CentPort))
  /*!
    Copy constructor
    \param A :: VacuumVessel to copy
  */
{}

VacuumVessel&
VacuumVessel::operator=(const VacuumVessel& A)
  /*!
    Assignment operator
    \param A :: VacuumVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      wallThick=A.wallThick;
      frontLength=A.frontLength;
      radius=A.radius;
      backLength=A.backLength;
      width=A.width;
      height=A.height;
      doorRadius=A.doorRadius;
      doorEdge=A.doorEdge;
      doorThick=A.doorThick;
      voidMat=A.voidMat;
      doorMat=A.doorMat;
      mat=A.mat;
      *CentPort= *A.CentPort;
    }
  return *this;
}

VacuumVessel::~VacuumVessel()
  /*!
    Destructor
  */
{}

void
VacuumVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("VacuumVessel","populate");

  FixedOffset::populate(Control);

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  radius=Control.EvalVar<double>(keyName+"Radius");
    
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Width");

  doorRadius=Control.EvalVar<double>(keyName+"DoorRadius");
  doorThick=Control.EvalVar<double>(keyName+"DoorThick");
  doorEdge=Control.EvalVar<double>(keyName+"DoorEdge");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  doorMat=ModelSupport::EvalMat<int>(Control,keyName+"DoorMat");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
VacuumVessel::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point surface to use as origin/basis.
  */
{
  ELog::RegMethod RegA("VacuumVessel","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
VacuumVessel::createSurfaces()
  /*!
    Create simple structures
  */
{
  ELog::RegMethod RegA("VacuumVessel","createSurfaces");

  // inner surfaces: 
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*backLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*frontLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
 
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);


  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // outer walls:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(backLength+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(frontLength+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*wallThick,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  // door
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,doorRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,doorRadius+doorEdge);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(frontLength+wallThick+doorThick),Y);

  
  return; 
}

void
VacuumVessel::createObjects(Simulation& System)
  /*!
    Create the tubed moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("VacuumVessel","createObjects");

  std::string Out;

  // Inner void (Cyl)
  Out=ModelSupport::getComposite(SMap,buildIndex,"  101 -2 -7 ");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addCell("Void",cellIndex-1);

  // Inner void (Box)
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -101 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addCell("Void",cellIndex-1);

  // Metal front
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -2 7 -17 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -101 111 -17 (-13 : 14 : -15 : 16 )");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -101  13 -14 15 -16 ( -1:-3:4:-5:6 )");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addCell("Wall",cellIndex-1);

  // DOOR
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 27");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addCell("Wall",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -27");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addCell("DoorVoid",cellIndex-1);

  // set forward door
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 -37 ");
  System.addCell(MonteCarlo::Object(cellIndex++,doorMat,0.0,Out));
  addCell("Door",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 37 -17");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addCell("Edge",cellIndex-1);

  
  Out=ModelSupport::getComposite(SMap,buildIndex," -22 -17 111");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 13 -14 15 -16 11");
  addOuterUnionSurf(Out);

  return; 
}

void
VacuumVessel::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("VacuumVessel","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(backLength+wallThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(1,Origin+Y*(frontLength+wallThick+doorThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+22));

  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  
  FixedComp::setConnect(4,Origin-Z*(radius+wallThick),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+17));
  
  FixedComp::setConnect(5,Origin+Z*(radius+wallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+17));
  

  return;
}

void
VacuumVessel::buildPorts(Simulation& System)
  /*!
    Build the external Ports/flanges
    \param System :: Simulation for building
   */
{
  ELog::RegMethod RegA("VacuumVessel","buildPorts");


  CentPort->addInsertCell(getInsertCells());   // main void
  CentPort->addInsertCell(getCells("Door"));
  CentPort->addInsertCell(getCells("DoorVoid"));
  CentPort->createAll(System,*this,2);

  return;
}

void
VacuumVessel::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("VacuumVessel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  buildPorts(System);
  insertObjects(System);       



  return;
}

}  // NAMESPACE photonSystem
