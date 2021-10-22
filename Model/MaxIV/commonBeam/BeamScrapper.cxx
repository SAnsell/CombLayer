/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/BeamScrapper.cxx
 *
 * Copyright (c) 2004-2021 Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "Surface.h"

#include "BeamScrapper.h"

namespace xraySystem
{

BeamScrapper::BeamScrapper(const std::string& Key)  :
  attachSystem::ContainedGroup("Payload","Connect","Outer"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BeamScrapper::~BeamScrapper()
  /*!
    Destructor
  */
{}

void
BeamScrapper::calcImpactVector()
  /*!
    Calculate the impact points of the main beam  on the screen surface:
    We have the beamAxis this must intersect the screen and mirror closest to 
    their centre points. It DOES NOT need to hit the centre points as the mirror
    system is confined to moving down the Y axis of the object. 
    [-ve Y from flange  to beam centre]
  */
{
  ELog::RegMethod RegA("BeamScrapper","calcImpactVector");

  // defined points:

  // This point is the beam centre point between the main axis:

  std::tie(std::ignore,beamCentre)=
    beamAxis.closestPoints(Geometry::Line(Origin,Y));

  // Thread point
  plateCentre=beamCentre-Y*plateOffset;

  return;
}


void
BeamScrapper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BeamScrapper","populate");

  FixedRotate::populate(Control);

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeOffset=Control.EvalVar<double>(keyName+"TubeOffset");
  tubeWall=Control.EvalVar<double>(keyName+"TubeWall");

  plateOffset=Control.EvalVar<double>(keyName+"PlateOffset");
  plateAngle=Control.EvalVar<double>(keyName+"PlateAngle");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateHeight=Control.EvalVar<double>(keyName+"PlateHeight");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  tubeWidth=Control.EvalVar<double>(keyName+"TubeWidth");
  tubeHeight=Control.EvalVar<double>(keyName+"TubeHeight");

  inletZOffset=Control.EvalVar<double>(keyName+"InletZOffset");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  copperMat=ModelSupport::EvalMat<int>(Control,keyName+"CopperMat");

  return;
}

void
BeamScrapper::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamScrapper","createSurfaces");

  calcImpactVector();

  // Ring box outer [note extra for tubewall clearance]:
  const double ringThick(tubeRadius+1.02*tubeWall);

  const Geometry::Quaternion QP=
    Geometry::Quaternion::calcQRotDeg(plateAngle,Z);
  const Geometry::Vec3D PX=QP.makeRotate(X);
  const Geometry::Vec3D PY=QP.makeRotate(Y);
  const Geometry::Vec3D PZ=QP.makeRotate(Z);
  
  if (!isActive("FlangePlate"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("FlangePlate",SMap.realSurf(buildIndex+1));
    }

  const Geometry::Vec3D inletCentre=beamCentre+Z*inletZOffset;
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,inletCentre-X*tubeOffset,Y,tubeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+8,inletCentre+X*tubeOffset,Y,tubeRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,inletCentre-X*tubeOffset,Y,tubeRadius+tubeWall);
  ModelSupport::buildCylinder
    (SMap,buildIndex+18,inletCentre+X*tubeOffset,Y,tubeRadius+tubeWall);

  // container cylinder
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,inletCentre,Y,tubeOffset+ringThick);
  
  // screen+mirror thread
  ModelSupport::buildPlane(SMap,buildIndex+201,plateCentre,PY);

  // grid of water:
  const Geometry::Vec3D vxA(plateCentre-PX*(tubeWidth/2.0)-PZ*(tubeHeight/2.0));
  const Geometry::Vec3D vxB(plateCentre+PX*(tubeWidth/2.0)-PZ*(tubeHeight/2.0));
  const Geometry::Vec3D vzA(plateCentre-PX*(tubeWidth/2.0)+PZ*(tubeHeight/2.0));
  const Geometry::Vec3D vzB(plateCentre+PX*(tubeWidth/2.0)+PZ*(tubeHeight/2.0));

  const Geometry::Vec3D axisA((vzB-vxA).unit());
  const Geometry::Vec3D axisB((vxB-vzA).unit());

  ModelSupport::buildCylinder(SMap,buildIndex+1107,vxA,PX,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1117,vxA,PX,tubeRadius+tubeWall);

  ModelSupport::buildCylinder(SMap,buildIndex+1108,vzA,PX,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1118,vzA,PX,tubeRadius+tubeWall);

  ModelSupport::buildCylinder(SMap,buildIndex+1207,vxA,PZ,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1217,vxA,PZ,tubeRadius+tubeWall);
  
  ModelSupport::buildCylinder(SMap,buildIndex+1208,vzB,PZ,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1218,vzB,PZ,tubeRadius+tubeWall);

  // dividers:
  // +ve X then +ve z
  ModelSupport::buildPlane(SMap,buildIndex+1101,vxA,axisB);
  ModelSupport::buildPlane(SMap,buildIndex+1102,vxB,axisA);

  const double maxX(std::max(tubeWidth/2.0+ringThick,plateLength/2.0));
  const double maxZ(std::max(tubeHeight/2.0+ringThick,plateLength/2.0));
  ModelSupport::buildPlane(SMap,buildIndex+1111,plateCentre-PY*ringThick,PY);
  ModelSupport::buildPlane(SMap,buildIndex+1112,plateCentre+PY*ringThick,PY);
  ModelSupport::buildPlane(SMap,buildIndex+1113,plateCentre-PX*maxX,PX);
  ModelSupport::buildPlane(SMap,buildIndex+1114,plateCentre+PX*maxX,PX);
  ModelSupport::buildPlane(SMap,buildIndex+1115,plateCentre-PZ*maxZ,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+1116,plateCentre+PZ*maxZ,PZ);  

  // main plate
  const Geometry::Vec3D mainC=plateCentre+PY*(ringThick+plateThick/2.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+2002,mainC+PY*(plateThick/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+2003,mainC-PX*(plateLength/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+2004,mainC+PX*(plateLength/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+2005,mainC-PZ*(plateHeight/2.0),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+2006,mainC+PZ*(plateHeight/2.0),PZ);

  return;
}

void
BeamScrapper::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamScrapper","createObjects");

  const HeadRule plateHR=getRule("FlangePlate");

  HeadRule HR,HRbox;

  HRbox=
    ModelSupport::getHeadRule(SMap,buildIndex,"-1111 -107");
  addOuterSurf("Connect",HRbox*plateHR);  

  // remove pipes
  HRbox*=
    ModelSupport::getHeadRule(SMap,buildIndex,"18 17");
  makeCell("supplyBox",System,cellIndex++,voidMat,0.0,HRbox*plateHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -201 1217 1117");
  makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR*plateHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 -201 1218 1117");
  makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR*plateHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -17 -201 1217 1117");
  makeCell("supplyPipe",System,cellIndex++,copperMat,0.0,HR*plateHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"8 -18 -201 1218 1117");
  makeCell("supplyPipe",System,cellIndex++,copperMat,0.0,HR*plateHR);
  
  // Note the odd order of this: I need to remove the ring
  // pipes from the Box. So the HR for the box is created
  // and then the each pipe is remove, and then the Box-cell is
  // createed

  // ring pipes:
  HRbox=ModelSupport::getHeadRule(SMap,buildIndex,
				  "1111 -1112 1113 -1114 1115 -1116");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1101 -1102 -1107");
  makeCell("ringWater",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1101 -1102 -1117 1107");
  makeCell("ringPipe",System,cellIndex++,copperMat,0.0,HR);
  HRbox*=ModelSupport::getHeadRule(SMap,buildIndex,"-1101:1102:1117");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1101 -1102 -1207");
  makeCell("ringWater",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1101 -1102 -1217 1207");
  makeCell("ringPipe",System,cellIndex++,copperMat,0.0,HR);
  HRbox*=ModelSupport::getHeadRule(SMap,buildIndex,"1101:1102:1217");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1102 -1101 -1108");
  makeCell("ringWater",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1102 -1101 -1118 1108");
  makeCell("ringPipe",System,cellIndex++,copperMat,0.0,HR);
  HRbox*=ModelSupport::getHeadRule(SMap,buildIndex,"1101:-1102:1118");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1101 1102 -1208");
  makeCell("ringWater",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1101 1102 -1218 1208");
  makeCell("ringPipe",System,cellIndex++,copperMat,0.0,HR);
  HRbox*=ModelSupport::getHeadRule(SMap,buildIndex,"-1101:-1102:1218");

  // Final box-cell containter:
  // main pipes
  HRbox*=ModelSupport::getHeadRule(SMap,buildIndex,"(201:(18 17))");
  makeCell("ringBox",System,cellIndex++,0,0.0,HRbox);

  HRbox=ModelSupport::getHeadRule(SMap,buildIndex,
				  "1111 -2002 1113 -1114 1115 -1116");
  addOuterSurf("Payload",HRbox);


  // main deflector plate:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "1112 -2002 2003 -2004 2005 -2006");
  makeCell("deflectorPlate",System,cellIndex++,copperMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1112 -2002 1113 -1114 1115 -1116 "
     "(-2003:2004:-2005:2006)");
  makeCell("deflectorPlate",System,cellIndex++,voidMat,0.0,HR);

  
  return;
}


void
BeamScrapper::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("BeamScrapper","createLinks");

  return;
}

void
BeamScrapper::setBeamAxis(const attachSystem::FixedComp& FC,
			    const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(FC)");

  beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			  FC.getLinkAxis(sIndex));


  return;
}

void
BeamScrapper::setBeamAxis(const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& Axis)
  /*!
    Set the screen centre
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(Vec3D)");

  beamAxis=Geometry::Line(Org,Axis);
  return;
}

void
BeamScrapper::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BeamScrapper","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
