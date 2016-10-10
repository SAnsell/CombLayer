/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/VacuumVessel.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "Qhull.h"
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
  attachSystem::CellMap(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(vacIndex+1),
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
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),length(A.length),
  radius(A.radius),thick(A.thick),backThick(A.backThick),
  doorStep(A.doorStep),doorThick(A.doorThick),voidMat(A.voidMat),
  mat(A.mat),CentPort(A.CentPort)
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
      cellIndex=A.cellIndex;
      length=A.length;
      radius=A.radius;
      thick=A.thick;
      backThick=A.backThick;
      doorStep=A.doorStep;
      doorThick=A.doorThick;
      voidMat=A.voidMat;
      mat=A.mat;
    }
  return *this;
}

VacuumVessel::~VacuumVessel()
  /*!
    Destructor
  */
{}

VacuumVessel*
VacuumVessel::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new VacuumVessel(*this);
}

void
VacuumVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("VacuumVessel","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  thick=Control.EvalVar<double>(keyName+"Thick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  doorThick=Control.EvalVar<double>(keyName+"DoorThick");
  doorStep=Control.EvalVar<double>(keyName+"DoorStep");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
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

  // inner/outer surfaces:
  ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+thick);

  ModelSupport::buildPlane(SMap,vacIndex+11,Origin-Y*(length/2.0+backThick),Y);

  const double sphereRadius=(doorStep*doorStep+radius*radius)/(2*doorStep);
  const double sphereY=length/2.0-(sphereRadius-doorStep);

  ModelSupport::buildSphere(SMap,vacIndex+8,Origin+Y*sphereY,sphereRadius);
  ModelSupport::buildSphere(SMap,vacIndex+18,Origin+Y*sphereY,
			    sphereRadius+doorThick);
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

  // Inner void
  Out=ModelSupport::getComposite(SMap,vacIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out));
  addCell("FrontVoid",cellIndex-1);


  // metal layers
  Out=ModelSupport::getComposite(SMap,vacIndex," 1 -2 -17 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("Skin",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex," 11 -1 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("Skin",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -17 -18 8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("Door",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,vacIndex," 11 -17 (-18 : -2)");
  addOuterSurf(Out);

  return; 
}

void
VacuumVessel::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("VacuumVessel","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2.0+backThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(vacIndex+11));

  FixedComp::setConnect(1,Origin+Y*(length/2.0+doorStep+doorThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+18));
  FixedComp::setBridgeSurf(1,SMap.realSurf(vacIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius+thick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(vacIndex+17));
  
  FixedComp::setConnect(3,Origin+X*(radius+thick),-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+17));

  FixedComp::setConnect(4,Origin-Z*(radius+thick),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(vacIndex+17));

  FixedComp::setConnect(5,Origin+Z*(radius+thick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+17));

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
  CentPort->addInsertCell(getCells("FrontVoid"));
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
