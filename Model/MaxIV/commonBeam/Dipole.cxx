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
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

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
 
  poleAngle=Control.EvalVar<double>(keyName+"PoleAngle");
  poleRadius=Control.EvalVar<double>(keyName+"PoleRadius");
  poleGap=Control.EvalVar<double>(keyName+"PoleGap");
  poleWidth=Control.EvalVar<double>(keyName+"PoleWidth");
  poleHeight=Control.EvalVar<double>(keyName+"PoleHeight");

  coilGap=Control.EvalVar<double>(keyName+"CoilGap");
  coilLength=Control.EvalVar<double>(keyName+"CoilLength");
  coilWidth=Control.EvalVar<double>(keyName+"CoilWidth");
  coilHeight=Control.EvalVar<double>(keyName+"CoilHeight");

  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  

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

  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(poleGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin-Z*(poleHeight+poleGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+25,Origin+Z*(poleGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+26,Origin+Z*(poleHeight+poleGap/2.0),Z);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,cylCentre-X*(poleWidth/2.0),Z,poleRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+27,cylCentre+X*(poleWidth/2.0),Z,poleRadius);

  

  // COILS:
  //coil angle is currently half of radius angle:
  const Geometry::Quaternion QCR=
    Geometry::Quaternion::calcQRotDeg(-poleAngle/2.0,Z);

  const Geometry::Vec3D XCoil=QR.makeRotate(X);
  const Geometry::Vec3D YCoil=QR.makeRotate(Y);
  
  const Geometry::Vec3D coilOrg=Origin+YCoil*(length/2.0);
  
  ModelSupport::buildPlane
    (SMap,buildIndex+101,coilOrg-(coidLength/2.0),YCoil);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,coilOrg+(coidLength/2.0),YCoil);

  ModelSupport::buildPlane(SMap,buildIndex+103,coilOrg-X*coilWidth,XCoil);
  ModelSupport::buildPlane(SMap,buildIndex+104,coilOrg+X*coilWidth,XCoil);

  ModelSupport::buildPlane
    (SMap,buildIndex+105,coilOrg-Z*(coilGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+115,coilOrg-Z*(coilHeight+coilGap/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+106,coilOrg+Z*(coilGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,coilOrg+Z*(coilHeight+coilGap/2.0),Z);
  
  // Coil end cylinders
  ModelSupport::buildCyldiner
    (SMap,buildIndex+107,coilOrg-(coidLength/2.0),Z,coilWidth);
  ModelSupport::buildPlane
    (SMap,buildIndex+108,coilOrg+(coidLength/2.0),Z,coilWidth);

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
  // Pole pieces
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2  ");
  makeCell("Frame",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "101 -1 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("FFrame",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-102 2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("BFrame",System,cellIndex++,0,0.0,Out);

  std::string TB[2];
  TB[0]=ModelSupport::getComposite(SMap,buildIndex," -105 5 ");
  TB[1]=ModelSupport::getComposite(SMap,buildIndex," 106 -6 ");
  for(size_t i=0;i<2;i++)
    {
      // Left  coil
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "101 -102 103 -1000 "
				     "(-117 : 111 : 113) "
				     "(-118 : -112 : 113) "
				     "(-127 : 111 : -114) "
				     "(-128 : -112 : -114) ");
      makeCell("CoilLowerLeft",System,cellIndex++,coilMat,0.0,Out+TB[i]);
      
      // corners for coils
      Out=ModelSupport::getComposite(SMap,buildIndex,"103 101 117 -111 -113 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,"103 -102 118 112 -113 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,"-1000 101 127 -111 114 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-1000 -102 128  112 114 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      
      // Right Lower coil
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "101 -102 -104 1000  "
				     "(-137 : 111 : -133) "
				     "(-138 : -112 : -133) "
				     "(-147 : 111 : 134) "
				     "(-148 : -112 : 134) ");
      makeCell("CoilLowerLeft",System,cellIndex++,coilMat,0.0,Out+TB[i]);
  
      // corners for coils
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-104 101 137 -111 133 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-104 -102 138 112 133 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "1000 101 147 -111 -134 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "1000 -102 148  112 -134 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);

    }
  // Now make edge voids:
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 3 -103 5 -6 ");
  makeCell("LEdge",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 104 -4 5 -6 ");
  makeCell("REdge",System,cellIndex++,0,0.0,Out);

  // Pole Pieces
  const std::string ICell=innerTube.display();
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 201 -202 203 -204 (206:-207) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 -2000 201 -202 1000 -104  (-203:204:(-206  207) )");
  makeCell("VoidPoleA",System,cellIndex++,0,0.0,Out+ICell);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 201 -202 303 -304 (306:-307) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 -2000 201 -202 -1000 103  (-303:304:(-306  307) )");
  makeCell("VoidPoleB",System,cellIndex++,0,0.0,Out+ICell);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 201 -202 403 -404 (406:-407) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 2000 201 -202 1000 -104  (-403:404:(-406  407) )");
  makeCell("VoidPoleC",System,cellIndex++,0,0.0,Out+ICell);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 201 -202 503 -504 (506:-507) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 2000 201 -202 -1000 103  (-503:504:(-506  507) )");
  makeCell("VoidPoleD",System,cellIndex++,0,0.0,Out+ICell);

  if (poleLength<coilLength-Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"105 -106 202 -102 103 -104 ");
      makeCell("ExtraPoleVoidA",System,cellIndex++,0,0.0,Out+ICell);
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"105 -106 101 -201 103 -104 ");
      makeCell("ExtraPoleVoidB",System,cellIndex++,0,0.0,Out+ICell);
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 13 -14 15 -16");  
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

  FixedComp::setConnect(0,Origin-Y*(coilLength/2.0),-Y);     
  FixedComp::setConnect(1,Origin+Y*(coilLength/2.0),Y);     
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);     
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);     
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);     
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+101));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+102));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  
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
