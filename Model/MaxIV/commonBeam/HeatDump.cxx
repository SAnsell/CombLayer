/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/HeatDump.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "HeatDump.h"


namespace xraySystem
{

HeatDump::HeatDump(const std::string& Key) :
  attachSystem::ContainedGroup("Inner","Outer"),
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",4),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

HeatDump::HeatDump(const HeatDump& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedOffsetGroup(A),
  attachSystem::ExternalCut(A),attachSystem::CellMap(A),
  upFlag(A.upFlag),radius(A.radius),height(A.height),
  width(A.width),thick(A.thick),lift(A.lift),cutHeight(A.cutHeight),
  cutAngle(A.cutAngle),cutDepth(A.cutDepth),topInnerRadius(A.topInnerRadius),
  topFlangeRadius(A.topFlangeRadius),topFlangeLength(A.topFlangeLength),
  bellowLength(A.bellowLength),bellowThick(A.bellowThick),
  outRadius(A.outRadius),outLength(A.outLength),
  waterRadius(A.waterRadius),waterZStop(A.waterZStop),
  mat(A.mat),flangeMat(A.flangeMat),bellowMat(A.bellowMat),
  waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: HeatDump to copy
  */
{}

HeatDump&
HeatDump::operator=(const HeatDump& A)
  /*!
    Assignment operator
    \param A :: HeatDump to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      upFlag=A.upFlag;
      radius=A.radius;
      height=A.height;
      width=A.width;
      thick=A.thick;
      lift=A.lift;
      cutHeight=A.cutHeight;
      cutAngle=A.cutAngle;
      cutDepth=A.cutDepth;
      topInnerRadius=A.topInnerRadius;
      topFlangeRadius=A.topFlangeRadius;
      topFlangeLength=A.topFlangeLength;
      bellowLength=A.bellowLength;
      bellowThick=A.bellowThick;
      outRadius=A.outRadius;
      outLength=A.outLength;
      waterRadius=A.waterRadius;
      waterZStop=A.waterZStop;
      mat=A.mat;
      flangeMat=A.flangeMat;
      bellowMat=A.bellowMat;
      waterMat=A.waterMat;
    }
  return *this;
}

HeatDump::~HeatDump()
  /*!
    Destructor
   */
{}

void
HeatDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("HeatDump","populate");

  FixedOffsetGroup::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  thick=Control.EvalVar<double>(keyName+"Thick");
  lift=Control.EvalVar<double>(keyName+"Lift");
  upFlag=Control.EvalDefVar<int>(keyName+"UpFlag",1);

  cutHeight=Control.EvalVar<double>(keyName+"CutHeight");
  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");
  cutDepth=Control.EvalVar<double>(keyName+"CutDepth");

  topInnerRadius=Control.EvalVar<double>(keyName+"TopInnerRadius");
  topFlangeRadius=Control.EvalVar<double>(keyName+"TopFlangeRadius");
  topFlangeLength=Control.EvalVar<double>(keyName+"TopFlangeLength");

  bellowLength=Control.EvalVar<double>(keyName+"BellowLength");
  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");

  outLength=Control.EvalVar<double>(keyName+"OutLength");
  outRadius=Control.EvalVar<double>(keyName+"OutRadius");

  waterRadius=Control.EvalVar<double>(keyName+"WaterRadius");
  waterZStop=Control.EvalVar<double>(keyName+"WaterZStop");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  bellowMat=ModelSupport::EvalMat<int>(Control,keyName+"BellowMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  
  return;
}

void
HeatDump::createUnitVector(const attachSystem::FixedComp& centreFC,
			   const long int cIndex,
			   const attachSystem::FixedComp& flangeFC,
			   const long int fIndex)
  /*!
    Create the unit vectors.
    The first beamFC is to set the X,Y,Z relative to the beam
    and the origin at the beam centre position.

    \param centreFC :: FixedComp for origin
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange centre
    \param fIndex :: direction for links
  */
{
  ELog::RegMethod RegA("HeatDump","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  beamFC.createUnitVector(centreFC,cIndex);
  mainFC.createUnitVector(flangeFC,fIndex);
  
  applyOffset();
  if (upFlag)
    beamFC.applyShift(0,0,lift);  // only beam offset
  setDefault("Main","Beam");
  return;
}

void
HeatDump::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("HeatDump","createSurfaces");
  setDefault("Main","Beam");
  
  ModelSupport::buildPlane(SMap,buildIndex+3,bOrigin-bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+4,bOrigin+bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+5,bOrigin-bZ*cutHeight,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,bOrigin+bZ*
			   (height-cutHeight),bZ);
  ModelSupport::buildCylinder(SMap,buildIndex+7,bOrigin,bZ,radius);

  const Geometry::Vec3D ZCut(bOrigin+bY*cutDepth); 

  ModelSupport::buildPlane(SMap,buildIndex+15,ZCut,bZ);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+16,ZCut,bZ,
				  bX,-cutAngle);
  // water cut
  ModelSupport::buildPlane(SMap,buildIndex+305,ZCut+bZ*waterZStop,bZ);
  ModelSupport::buildCylinder(SMap,buildIndex+307,bOrigin,bZ,waterRadius);
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
HeatDump::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("HeatDump","createObjects");

  const std::string mountSurf
    (ExternalCut::getRuleStr("mountSurf"));

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 3 -4 -7 5 -6 (-15:16) (307:-305) " );
  makeCell("Dump",System,cellIndex++,mat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"305 -307 -6");
  makeCell("Water",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -7 15 -16 " );
  makeCell("Cut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -7 5 -6 " );
  addOuterSurf("Inner",Out);


  // create Flange:
  Out=ModelSupport::getComposite(SMap,buildIndex," -102 -117 107 " );
  makeCell("MountFlange",System,cellIndex++,flangeMat,0.0,Out+mountSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -127 107 -201");
  makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-207 201 -202 (-3:4:7)");
  makeCell("Topflange",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 (-3:4:7) -201");
  makeCell("LiftVoid",System,cellIndex++,0,0.0,Out+mountSurf);

  // Add outer voids
  Out=ModelSupport::getComposite(SMap,buildIndex,"127 102 -201 -117");
  makeCell("BellowVoid",System,cellIndex++,0,0.0,Out);

  if (topFlangeRadius+Geometry::zeroTol>outRadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"201 -202 -117 207");
      makeCell("TopVoid",System,cellIndex++,0,0.0,Out);
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,"202 -6 -117 (-3:4:7)");
  makeCell("OutVoid",System,cellIndex++,0,0.0,Out);


  // final exclude:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-117 -6");
  addOuterSurf("Outer",mountSurf+Out);
  
  
  return; 
}

std::vector<Geometry::Vec3D>
HeatDump::calcEdgePoints() const
  /*!
    Get points to test for ContainedComp
    \return Points
   */
{
  ELog::RegMethod RegA("HeatDump","calcEdgePoints");
  
  std::vector<Geometry::Vec3D> Out;
  Out.push_back(Origin);
  Out.push_back(Origin-Z*(height/2.0));
  Out.push_back(Origin+Z*(height/2.0));
  return Out;
}
  
void
HeatDump::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("HeatDump","createLinks");

  //  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  // Beam position is lift/no-lift so 1-2 are no-lift 3-4 are lifted

  // currently no divider plane provided -- use with caution
  beamFC.setLinkSurf(0,SMap.realSurf(buildIndex+7));
  beamFC.setLinkSurf(1,SMap.realSurf(buildIndex+7));
  beamFC.setLinkSurf(2,SMap.realSurf(buildIndex+7));
  beamFC.setLinkSurf(3,SMap.realSurf(buildIndex+7));

  beamFC.setConnect(0,bOrigin-bY*radius-bZ*lift,-bY);
  beamFC.setConnect(1,bOrigin+bY*radius-bZ*lift,bY);
  beamFC.setConnect(2,bOrigin-bY*radius,-bY);
  beamFC.setConnect(3,bOrigin+bY*radius,bY);
    
  return;
}

void
HeatDump::createAll(Simulation& System,
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
  ELog::RegMethod RegA("HeatDump","createAll");
  populate(System.getDataBase());

  createUnitVector(centreFC,cIndex,flangeFC,fIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
