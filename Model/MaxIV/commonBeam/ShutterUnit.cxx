/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/ShutterUnit.cxx
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
#include "Line.h"
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
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "ShutterUnit.h"


namespace xraySystem
{

ShutterUnit::ShutterUnit(const std::string& Key) :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


ShutterUnit::~ShutterUnit()
  /*!
    Destructor
   */
{}

void
ShutterUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ShutterUnit","populate");

  FixedOffsetGroup::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  thick=Control.EvalVar<double>(keyName+"Thick");
  lift=Control.EvalVar<double>(keyName+"Lift");
  upFlag=Control.EvalDefVar<int>(keyName+"UpFlag",1);

  topInnerRadius=Control.EvalVar<double>(keyName+"TopInnerRadius");
  topFlangeRadius=Control.EvalVar<double>(keyName+"TopFlangeRadius");
  topFlangeLength=Control.EvalVar<double>(keyName+"TopFlangeLength");

  bellowLength=Control.EvalVar<double>(keyName+"BellowLength");
  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");

  outLength=Control.EvalVar<double>(keyName+"OutLength");
  outRadius=Control.EvalVar<double>(keyName+"OutRadius");

  blockMat=ModelSupport::EvalMat<int>(Control,keyName+"BlockMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  bellowMat=ModelSupport::EvalMat<int>(Control,keyName+"BellowMat");
  
  return;
}

void
ShutterUnit::createUnitVector(const attachSystem::FixedComp& centreFC,
			      const long int cIndex,
			      const attachSystem::FixedComp& flangeFC,
			      const long int fIndex)
  /*!
    Create the unit vectors.
    The first beamFC is to set the X,Y,Z relative to the beam
    and the origin at the beam centre position.
    The origin is corrected so that the flange centre + alpha*Y'
    where Y' is its primary direction is closeest to 
    beamCenter + Y'*beta. The flange centre point is taken.

    \param centreFC :: FixedComp for origin at beam height
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange centre
    \param fIndex :: direction for flange axis
  */
{
  ELog::RegMethod RegA("ShutterUnit","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  beamFC.createUnitVector(centreFC,cIndex);
  mainFC.createUnitVector(flangeFC,fIndex);
  applyOffset();
  setDefault("Main");
  setSecondary("Beam");
  
  // Now construct new centre point:
  const Geometry::Line beamL(bOrigin,bY);
  const Geometry::Line mainL(Origin,Y);
  const std::pair<Geometry::Vec3D,Geometry::Vec3D> CP=
    beamL.closestPoints(mainL);

  beamFC.setCentre(CP.second);
  
  if (upFlag)
    beamFC.applyShift(0,0,lift);  // only beam offset
  
  setDefault("Main");
  setSecondary("Beam");
  ELog::EM<<"Origin == "<<Origin<<ELog::endDiag;
  ELog::EM<<"BOrigin == "<<bOrigin<<ELog::endDiag;
  return;
}

void
ShutterUnit::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("ShutterUnit","createSurfaces");



  ModelSupport::buildPlane(SMap,buildIndex+1,bOrigin-bY*(thick/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+2,bOrigin+bY*(thick/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+3,bOrigin-bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+4,bOrigin+bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+5,bOrigin-bZ*(height/2.0),bZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,bOrigin+bZ*(height/2.0),bZ);

  // bellow outer 
  ModelSupport::buildCylinder(SMap,buildIndex+7,bOrigin,Z,
			      liftScrew);

  // construct surround [Y is upwards]
  if (!isActive("mountSurf"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
      setCutSurf("mountSurf",SMap.realSurf(buildIndex+101));
    }
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*topFlangeLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,topInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,topFlangeRadius);

  // bellow outer 
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,Y,
			      topInnerRadius+bellowThick);


  // bellows flange
  const double BL((upFlag) ? bellowLength+lift : bellowLength);

    
  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin+Y*(topFlangeLength+BL),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(topFlangeLength+BL+outLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,outRadius);
			   
  return; 
}

void
ShutterUnit::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("ShutterUnit","createObjects");

  const std::string mountSurf
    (ExternalCut::getRuleStr("mountSurf"));

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4  5 -6  " );
  makeCell("Dump",System,cellIndex++,blockMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4  5 -6 " );
  addOuterSurf("Inner",Out);
  // create Flange:
  Out=ModelSupport::getComposite(SMap,buildIndex," -102 -117 107 " );
  makeCell("MountFlange",System,cellIndex++,flangeMat,0.0,Out+mountSurf);
  addOuterSurf("Outer",mountSurf+Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -127 107 -201");
  makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
  addOuterUnionSurf("Outer",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-207 201 -202 ");
  makeCell("Topflange",System,cellIndex++,flangeMat,0.0,Out);
  addOuterUnionSurf("Outer",Out);
  return;

  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 (-1:2:-3:4) -201");
  makeCell("LiftVoid",System,cellIndex++,0,0.0,Out+mountSurf);

  // Add outer voids
  Out=ModelSupport::getComposite(SMap,buildIndex,"127 102 -201 -117");
  makeCell("BellowVoid",System,cellIndex++,0,0.0,Out);

  if (topFlangeRadius+Geometry::zeroTol>outRadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"201 -202 -117 207");
      makeCell("TopVoid",System,cellIndex++,0,0.0,Out);
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,"202 -6 -117 (-1:2:-3:4)");
  makeCell("OutVoid",System,cellIndex++,0,0.0,Out);


  // final exclude:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-117 -6");
  addOuterSurf("Outer",mountSurf+Out);
  
  
  return; 
}

std::vector<Geometry::Vec3D>
ShutterUnit::calcEdgePoints() const
  /*!
    Get points to test for ContainedComp
    \return Points
   */
{
  ELog::RegMethod RegA("ShutterUnit","calcEdgePoints");
  
  std::vector<Geometry::Vec3D> Out;
  Out.push_back(Origin);
  Out.push_back(Origin-Z*(height/2.0));
  Out.push_back(Origin+Z*(height/2.0));
  return Out;
}
  
void
ShutterUnit::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("ShutterUnit","createLinks");
  
  return;
}

void
ShutterUnit::createAll(Simulation& System,
		    const attachSystem::FixedComp& centreFC,
		    const long int cIndex,
		    const attachSystem::FixedComp& flangeFC,
		    const long int fIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param centreFC :: FixedComp for beam origin
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange center
    \param fIndex :: direction for links
   */
{
  ELog::RegMethod RegA("ShutterUnit","createAll");
  populate(System.getDataBase());

  createUnitVector(centreFC,cIndex,flangeFC,fIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
