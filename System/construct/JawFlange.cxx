/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawFlange
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
#include <array>

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
#include "Cylinder.h"
#include "Line.h"
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
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "JawUnit.h"
#include "JawFlange.h" 

namespace constructSystem
{

JawFlange::JawFlange(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",2,"Beam",2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  JItem(Key+"Jaw")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


JawFlange::~JawFlange() 
  /*!
    Destructor
  */
{}

void
JawFlange::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawFlange","populate");
  
  attachSystem::FixedOffsetGroup::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);


  return;
}


void
JawFlange::createUnitVector(const attachSystem::FixedComp& mFC,
			    const long int mainIndex,
			    const attachSystem::FixedComp& bFC,
			    const long int beamIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param mFC :: Fixed component of central axis
    \param mainIndex :: Link point and direction [0 for origin]
    \param bFC :: Fixed component of beam
    \param beamIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("JawFlange","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  mainFC.FixedComp::createUnitVector(mFC,mainIndex);
  beamFC.FixedComp::createUnitVector(bFC,beamIndex);

  calcBeamCentre();
  applyOffset();
  
  setDefault("Main");
  return;
}


void
JawFlange::calcBeamCentre()
  /*!
    We set the beam coordinates and the main coordinate
    The jaw centre is placed at the closest point of
    the beam centre to the main axis
  */
{
  const attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  const Geometry::Vec3D& MC(mainFC.getCentre());
  const Geometry::Vec3D& YAxis(mainFC.getY());

  const Geometry::Vec3D& BC(beamFC.getCentre());
  const Geometry::Vec3D& BYAxis(beamFC.getY());

  const Geometry::Line mainLine(MC,YAxis);
  const Geometry::Line beamLine(BC,BYAxis);

  // point on beamLine that is closest to mainLine
  // closetPt returns pair:
  const Geometry::Vec3D cPt=
    mainLine.closestPoints(beamLine).second;
  
  beamFC.setCentre(cPt);

  return;    
}

void
JawFlange::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("JawFlange","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  
  // back planes
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  FrontBackCut::setBack(SMap.realSurf(buildIndex+2));

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  return;
}

void
JawFlange::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("JawFlange","createObjects");

  std::string Out;

  const std::string frontStr=frontRule();  // 101
  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr);

  addOuterSurf(Out);
  return;
}
  
void
JawFlange::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("JawFlange","createLinks");

  //stufff for intersection
  attachSystem::FixedComp& mainFC=getKey("Main");
  //attachSystem::FixedComp& beamFC=getKey("Beam");

  
  FrontBackCut::createLinks(mainFC,Origin,Y);  //front and back

  return;
}

void
JawFlange::createJaws(Simulation& System)
  /*!
    Create the jaws
    \param System :: Simuation to use
   */
{
  ELog::RegMethod RegA("JawFlange","creatJaws");

  const attachSystem::FixedComp& beamFC=getKey("Beam");
  JItem.addInsertCell(this->getCells("Void"));
  
  JItem.createAll(System,beamFC,0);
  return;
}
  
void
JawFlange::createAll(Simulation& System,
		     const attachSystem::FixedComp& portFC,
		     const long int portIndex,
		     const attachSystem::FixedComp& beamFC,
		     const long int beamIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param portFC :: FixedComp
    \param portIndex :: Fixed Index
    \param beamFC :: FixedComp for beam axis
    \param beamIndex :: Fixed Index link point
  */
{
  ELog::RegMethod RegA("JawFlange","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(portFC,portIndex,beamFC,beamIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  createJaws(System);
  
  return;
}
  
}  // NAMESPACE constructSystem
