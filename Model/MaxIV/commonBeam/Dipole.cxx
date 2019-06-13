/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Dipole.cxx
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "Dipole.h"

namespace xraySystem
{

Dipole::Dipole(const std::string& Key) :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(6,"Centre");
}

Dipole::Dipole(const std::string& Base,
	       const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Base)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


Dipole::~Dipole() 
  /*!
    Destructor
  */
{}

void
Dipole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Dipole","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
 
  poleAngle=Control.EvalVar<double>(keyName+"PoleAngle");
  poleGap=Control.EvalVar<double>(keyName+"PoleGap");
  poleWidth=Control.EvalVar<double>(keyName+"PoleWidth");

  coilGap=Control.EvalVar<double>(keyName+"CoilGap");
  coilLength=Control.EvalVar<double>(keyName+"CoilLength");
  coilWidth=Control.EvalVar<double>(keyName+"CoilWidth");

  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  
  poleRadius=length/(M_PI*poleAngle/180.0);
  return;
}

void
Dipole::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("Dipole","createUnitVector");

  // origin from start point
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
Dipole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Dipole","createSurfaces");

  // pole pieces  
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(-poleAngle,Z);

  const Geometry::Vec3D XPole=QR.makeRotate(X);
  const Geometry::Vec3D YPole=QR.makeRotate(Y);
  
  const Geometry::Vec3D cylCentre=Origin+X*poleRadius;

  // END plane
  const double xDisp=(1.0-cos(M_PI*poleAngle/180.0))*poleRadius;
  const double yDisp=sin(M_PI*poleAngle/180.0)*poleRadius;
  const Geometry::Vec3D cylEnd=Origin+X*xDisp+Y*yDisp;

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,cylEnd,YPole);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(poleGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(poleGap/2.0),Z);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,cylCentre-X*(poleWidth/2.0),Z,poleRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+27,cylCentre+X*(poleWidth/2.0),Z,poleRadius);

  
  // COILS:
  //coil angle is currently half of radius angle:
  const Geometry::Quaternion QCR=
    Geometry::Quaternion::calcQRotDeg(-poleAngle/2.0,Z);

  const Geometry::Vec3D XCoil=QCR.makeRotate(X);
  const Geometry::Vec3D YCoil=QCR.makeRotate(Y);
  
  const Geometry::Vec3D coilOrg=Origin+YCoil*(length/2.0);
  ModelSupport::buildPlane
    (SMap,buildIndex+101,coilOrg-YCoil*(coilLength/2.0),YCoil);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,coilOrg+YCoil*(coilLength/2.0),YCoil);

  ModelSupport::buildPlane(SMap,buildIndex+103,coilOrg-X*coilWidth,XCoil);
  ModelSupport::buildPlane(SMap,buildIndex+104,coilOrg+X*coilWidth,XCoil);

  ModelSupport::buildPlane
    (SMap,buildIndex+105,coilOrg-Z*(coilGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,coilOrg+Z*(coilGap/2.0),Z);
  
  // Coil end cylinders
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,coilOrg-YCoil*(coilLength/2.0),Z,coilWidth);
  ModelSupport::buildCylinder
    (SMap,buildIndex+108,coilOrg+YCoil*(coilLength/2.0),Z,coilWidth);

  // Cylinder cutters
  ModelSupport::buildPlane
    (SMap,buildIndex+201,coilOrg-YCoil*(coilLength/2.0+coilWidth),YCoil);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,coilOrg+YCoil*(coilLength/2.0+coilWidth),YCoil);

  FixedComp::setConnect(0,coilOrg-YCoil*(coilLength/2.0+coilWidth),YCoil);
  FixedComp::setConnect(1,coilOrg+YCoil*(coilLength/2.0+coilWidth),YCoil);
  FixedComp::setConnect(6,coilOrg,YCoil);

  return;
}

void
Dipole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Dipole","createObjects");


  std::string Out;

  if (isActive("MidSplit"))
    {
      const std::string ACell=getRuleStr("InnerA");
      const std::string BCell=getRuleStr("InnerB");
      const HeadRule& MSplit=getRule("MidSplit");
	    
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 15 -16 103 -104 201 ");
      makeCell("MidVoidA",System,cellIndex++,0,0.0,
	       Out+ACell+MSplit.complement().display());

      Out=ModelSupport::getComposite
	(SMap,buildIndex," 15 -16 103 -104 -202");
      makeCell("MidVoidB",System,cellIndex++,0,0.0,
	       Out+BCell+MSplit.display());
    }
  else
    {
      const std::string ACell=
	(isActive("Inner")) ? getRuleStr("Inner") : "";
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 15 -16 103 -104 201 -202");
      makeCell("MidVoid",System,cellIndex++,0,0.0,Out+ACell);
    }

  // side voids
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-1:2:17:-27) -15 105 103 -104 (-107:101) (-108:-102)");
  makeCell("BaseVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-1:2:17:-27) 16 -106 103 -104 (-107:101) (-108:-102)");
  makeCell("TopVoid",System,cellIndex++,0,0.0,Out);

  // Pole pieces
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -15 5 -17 27 ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 16 -6 -17 27 ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-107:101) (-108:-102) -105 5 103 -104 "
    " (-1:2:17:-27) ");
  makeCell("CoilA",System,cellIndex++,coilMat,0.0,Out);

  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-107:101) (-108:-102) 106 -6 103 -104 "
    " (-1:2:17:-27) ");
  makeCell("CoilB",System,cellIndex++,coilMat,0.0,Out);

  // Void ends
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 201 107 -101 5 -6 103 -104 (-15 : 16)");
  makeCell("FrontVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -202 108 102 5 -6 103 -104 (-15 : 16)");
  makeCell("BackVoid",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 5 -6 103 -104 201 -202 ");
  addOuterSurf(Out);
  
  return;
}

void 
Dipole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Dipole","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+201));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+202));
  
  return;
}

void
Dipole::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Dipole","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
