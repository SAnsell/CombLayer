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
#include "groupRange.h"
#include "objectGroups.h"
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

#include "JawFlange.h" 

namespace constructSystem
{

JawFlange::JawFlange(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",2,"Beam",2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  cutCell(0)
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

  // If length -ve then extra from beam-flange length
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  jYStep=Control.EvalVar<double>(keyName+"JYStep");
  jOpen=Control.EvalVar<double>(keyName+"JOpen");
  jThick=Control.EvalVar<double>(keyName+"JThick");
  jHeight=Control.EvalVar<double>(keyName+"JHeight");
  jWidth=Control.EvalVar<double>(keyName+"JWidth");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  jawMat=ModelSupport::EvalMat<int>(Control,keyName+"JawMat");
  
  return;
}

void
JawFlange::setFillRadius(const attachSystem::FixedComp& portFC,
			 const long int radiusIndex,
			 const int cellNumber)
  /*!
    Give a cylindical (or contained) link surface and the 
    cell that will be completely removed. 
    \param portFC :: Port FixedComp
    \param radiusIndex :: link point to a radial surface 
    \param cellNubmer :: cell to remove
  */
{
  ELog::RegMethod Rega("JawFlange","setFillRadius");

  cylRule=portFC.getMainRule(radiusIndex);
  cutCell=cellNumber;
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
  ELog::RegMethod RegA("JawFlange","calcBeamCentre");
  
  attachSystem::FixedComp& mainFC=getKey("Main");
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
  // make Y vertical (going in)
  if ((MC-cPt).dotProd(mainFC.getY())<0.0)
    mainFC.reOrientate(1,-mainFC.getY());
  
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

  const attachSystem::FixedComp& mainFC=getKey("Main");
  const attachSystem::FixedComp& beamFC=getKey("Beam");
  
  if (!frontActive())
    {

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,-Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));	    
    }

  // back planes
  const double defLength(Origin.Distance(beamFC.getCentre()));
    
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin-Y*(defLength+length),-Y);
  FrontBackCut::setBack(SMap.realSurf(buildIndex+2));

  if (!cutCell)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
      cylRule.procSurfNum(-SMap.realSurf(buildIndex+7));
    }

  // calc position
  const Geometry::Vec3D& beamY=beamFC.getY();
  const Geometry::Vec3D& mainY=mainFC.getY();
  const Geometry::Vec3D xVec=(beamY*mainY).unit();

    
  const Geometry::Vec3D beamOrg=beamFC.getCentre()+beamY*jYStep;

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   beamOrg-beamY*(jThick/2.0),beamY);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   beamOrg+beamY*(jThick/2.0),beamY);
  ModelSupport::buildPlane(SMap,buildIndex+103,
			   beamOrg-xVec*(jWidth/2.0),xVec);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   beamOrg+xVec*(jWidth/2.0),xVec);

  ModelSupport::buildPlane(SMap,buildIndex+105,
			   beamOrg-mainY*(jOpen/2.0),mainY);
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   beamOrg-mainY*(jHeight+jOpen/2.0),mainY);

  ModelSupport::buildPlane(SMap,buildIndex+106,
			   beamOrg+mainY*(jOpen/2.0),mainY);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   beamOrg+mainY*(jHeight+jOpen/2.0),mainY);

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

  const std::string frontStr=frontRule();
  const std::string cylStr=cylRule.display();  

  // Jaws
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 -105 115");
  makeCell("BaseJaw",System,cellIndex++,jawMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 106 -116");
  makeCell("TopJaw",System,cellIndex++,jawMat,0.0,Out);

  if (jOpen>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 101 -102 103 -104 105 -106");
      makeCell("JawGap",System,cellIndex++,voidMat,0.0,Out);
    }

  // Void
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-2 (-101 : 102 : -103 : 104 : -115 : 116)");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+cylStr);

  // create jaws
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 ");  
  addOuterSurf(Out+cylStr);
  if (cutCell)
    System.removeCell(cutCell);
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
  
  return;
}
  
}  // NAMESPACE constructSystem
