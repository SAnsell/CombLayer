/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BeamPair.cxx
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
#include "BeamPair.h"


namespace xraySystem
{

BeamPair::BeamPair(const std::string& Key) :
  attachSystem::ContainedGroup("BlockA","BlockB","SupportA","SupportB"),
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


BeamPair::~BeamPair()
  /*!
    Destructor
   */
{}

void
BeamPair::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeamPair","populate");

  FixedOffsetGroup::populate(Control);
  
  upFlag=Control.EvalDefVar<int>(keyName+"UpFlag",1);
  
  outLiftA=Control.EvalVar<double>(keyName+"OutLiftA");
  outLiftB=Control.EvalVar<double>(keyName+"OutLiftB");

  gapA=Control.EvalVar<double>(keyName+"GapA");
  gapB=Control.EvalVar<double>(keyName+"GapB");

  xStepA=Control.EvalVar<double>(keyName+"XStepA");
  yStepA=Control.EvalVar<double>(keyName+"YStepA");
  xStepB=Control.EvalVar<double>(keyName+"XStepB");
  yStepB=Control.EvalVar<double>(keyName+"YStepB");
  
  blockXYAngle=Control.EvalDefVar<double>(keyName+"BlockXYAngle",0.0);
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  blockMat=ModelSupport::EvalMat<int>(Control,keyName+"BlockMat");

  supportRadius=Control.EvalVar<double>(keyName+"SupportRadius");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");

  waterRadius=Control.EvalVar<double>(keyName+"WaterRadius");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  
  return;
}

void
BeamPair::createUnitVector(const attachSystem::FixedComp& centreFC,
			   const long int cIndex,
			   const attachSystem::FixedComp& flangeFC,
			   const long int fIndex)
  /*!
    Create the unit vectors.
    The first flangeFC is to set the X,Y,Z relative to the axis
    and the origin at the beam centre position.
    
    The beamFC axis are set so (a) Y ==> cIndex axis
                               (b) Z ==> mount axis
                               (c) X ==> correct other

    \param centreFC :: FixedComp for origin
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange centre
    \param fIndex :: direction for links
  */
{
  ELog::RegMethod RegA("BeamPair","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  mainFC.createUnitVector(flangeFC,fIndex);
  const Geometry::Vec3D& ZBeam=mainFC.getY();
  const Geometry::Vec3D YBeam=centreFC.getLinkAxis(cIndex);
  const Geometry::Vec3D XBeam=ZBeam*YBeam;
  Geometry::Vec3D BC(mainFC.getCentre());

  // need to remove directoin in Y of flange
  
  const double BY=BC.dotProd(ZBeam);
  const double MY=centreFC.getLinkPt(cIndex).dotProd(ZBeam);
  BC += ZBeam * (MY-BY);
  if (XBeam.abs()>0.5)
    beamFC.createUnitVector(BC,XBeam,YBeam,ZBeam);
  else
      beamFC.createUnitVector(flangeFC,fIndex);
  applyOffset();

  setDefault("Main","Beam");
  return;
}

void
BeamPair::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("BeamPair","createSurfaces");

  // Not can have a local rotation of the beam component
  // construct support

  if (!isActive("mountSurf"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
      setCutSurf("mountSurf",SMap.realSurf(buildIndex+101));
    }

  // BLOCK A : [UPPER]
  // action in Y direction:
  const Geometry::Vec3D bA(bOrigin+X*xStepA+Z*yStepA-Y*gapA);
  const Geometry::Vec3D wAxis=(bY*Y).unit();

  
  ModelSupport::buildPlane(SMap,buildIndex+1,bA-bY*(length/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+2,bA+bY*(length/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+3,bA-wAxis*(width/2.0),wAxis);
  ModelSupport::buildPlane(SMap,buildIndex+4,bA+wAxis*(width/2.0),wAxis);
  ModelSupport::buildPlane(SMap,buildIndex+5,bA-Y*height,Y);
  ModelSupport::buildPlane(SMap,buildIndex+6,bA,Y);

  // BLOCK B : [LOWER]
  // action in Y direction:
  const Geometry::Vec3D bB(bOrigin+X*xStepB+Z*yStepB+Y*gapB);
  
  ModelSupport::buildPlane(SMap,buildIndex+11,bB-bY*(length/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+12,bB+bY*(length/2.0),bY);
  ModelSupport::buildPlane(SMap,buildIndex+13,bB-wAxis*(width/2.0),wAxis);
  ModelSupport::buildPlane(SMap,buildIndex+14,bB+wAxis*(width/2.0),wAxis);
  ModelSupport::buildPlane(SMap,buildIndex+15,bB,Y);
  ModelSupport::buildPlane(SMap,buildIndex+16,bB+Y*(height),Y);

  // water if present
  ModelSupport::buildCylinder(SMap,buildIndex+107,bA,bZ,waterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,bA-wAxis*(width/4.0),bZ,waterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+127,bA+wAxis*(width/4.0),bZ,waterRadius);
  
  ModelSupport::buildCylinder(SMap,buildIndex+207,bB,bZ,waterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,bB-wAxis*(width/4.0),bZ,waterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,bB+wAxis*(width/4.0),bZ,waterRadius);
  
  
  // Support tube
  
  const Geometry::Vec3D cylA(bA+wAxis*(width/2.0-supportRadius));
  const Geometry::Vec3D cylB(bB-wAxis*(width/2.0-supportRadius));
  ModelSupport::buildCylinder(SMap,buildIndex+7,cylA,Y,supportRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,cylB,Y,supportRadius);
  return; 
}

void
BeamPair::createObjects(Simulation& System)
  /*!
    Create the paired object
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BeamPair","createObjects");

  const HeadRule mountSurfHR(ExternalCut::getRule("mountSurf"));

  HeadRule HR;

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6 107 117 127" );
  makeCell("BlockA",System,cellIndex++,blockMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 13 -14 15 -16 207 217 227" );
  makeCell("BlockB",System,cellIndex++,blockMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -107" );
  makeCell("TubeA",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -117" );
  makeCell("TubeA",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -127" );
  makeCell("TubeA",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 -207" );
  makeCell("TubeB",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 -217" );
  makeCell("TubeB",System,cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 -227" );
  makeCell("TubeB",System,cellIndex++,waterMat,0.0,HR);

  // outer surfaces
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6" );
  addOuterSurf("BlockA",HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 15 -16" );
  addOuterSurf("BlockB",HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -5" );
  makeCell("SupportA",System,cellIndex++,supportMat,0.0,HR*mountSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 -15 " );
  makeCell("SupportB",System,cellIndex++,supportMat,0.0,HR*mountSurfHR);
   
  // final exclude:
  //  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-117 -6");
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 -5 " );
  addOuterSurf("SupportA",HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 -15" );
  addOuterSurf("SupportB",HR);
  
  return; 
}

void
BeamPair::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("BeamPair","createLinks");
  
  return;
}

void
BeamPair::createAll(Simulation& System,
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
  ELog::RegMethod RegA("BeamPair","createAll");
  populate(System.getDataBase());

  createUnitVector(centreFC,cIndex,flangeFC,fIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
