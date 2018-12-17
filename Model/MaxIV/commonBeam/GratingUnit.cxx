/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/GratingUnit.cxx
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
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "GrateHolder.h"
#include "GratingUnit.h"


namespace xraySystem
{

GratingUnit::GratingUnit(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  grateArray({
        std::make_shared<xraySystem::GrateHolder>(keyName+"Grate0"),
	std::make_shared<xraySystem::GrateHolder>(keyName+"Grate1"),
	std::make_shared<xraySystem::GrateHolder>(keyName+"Grate2")})
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


GratingUnit::~GratingUnit()
  /*!
    Destructor
   */
{}

void
GratingUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GratingUnit","populate");

  FixedRotate::populate(Control);

  grateIndex=Control.EvalDefVar<int>(keyName+"GrateIndex",0);
  
  grateTheta=Control.EvalVar<double>(keyName+"GrateTheta");
  mainGap=Control.EvalVar<double>(keyName+"MainGap");
  mainBarXLen=Control.EvalVar<double>(keyName+"MainBarXLen");
  mainBarDepth=Control.EvalVar<double>(keyName+"MainBarDepth");
  mainBarYWidth=Control.EvalVar<double>(keyName+"MainBarYWidth");

  slidePlateZGap=Control.EvalVar<double>(keyName+"SlidePlateZGap");
  slidePlateThick=Control.EvalVar<double>(keyName+"SlidePlateThick");
  slidePlateWidth=Control.EvalVar<double>(keyName+"SlidePlateWidth");
  slidePlateLength=Control.EvalVar<double>(keyName+"SlidePlateLength");
  
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  slideMat=ModelSupport::EvalMat<int>(Control,keyName+"SlideMat");


  return;
}

void
GratingUnit::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("GratingUnit","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // rotate about X axis
  FixedComp::applyAngleRotate(grateTheta,0.0,0.0);
  
  return;
}

void
GratingUnit::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("GratingUnit","createSurfaces");

  //  setCutSurf("innerFront",grateArray[0]->getSurf("Front"));
  //  setCutSurf("innerBack",grateArray[0]->getSurf("Back"));
  setCutSurf("innerFront",*grateArray[0],-1);
  setCutSurf("innerBack",*grateArray[0],-2);
  setCutSurf("innerLeft",*grateArray[0],-3);
  setCutSurf("innerRight",*grateArray[2],-4);
  setCutSurf("innerBase",*grateArray[0],-5);
  setCutSurf("innerTop",*grateArray[0],-6);


  ExternalCut::makeShiftedSurf
    (SMap,"innerFront",buildIndex+101,1,Y,-slidePlateLength);
  ExternalCut::makeShiftedSurf
    (SMap,"innerBack",buildIndex+201,-1,Y,-slidePlateLength);

  ExternalCut::makeShiftedSurf
    (SMap,"innerLeft",buildIndex+3,1,X,-slidePlateWidth);
  ExternalCut::makeShiftedSurf
    (SMap,"innerRight",buildIndex+4,-1,X,-slidePlateWidth);
 
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin+Z*slidePlateZGap,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(slidePlateZGap+slidePlateThick),Z);

  // Main support bars
  ExternalCut::makeShiftedSurf
    (SMap,"innerFront",buildIndex+1001,1,Y,-mainBarYWidth);
  ExternalCut::makeShiftedSurf
    (SMap,"innerBack",buildIndex+1002,-1,Y,-mainBarYWidth);
  
  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-X*(mainBarXLen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*(mainBarXLen/2.0),X);
    
  ModelSupport::buildPlane(SMap,buildIndex+1005,
			   Origin+Z*(slidePlateZGap-mainBarDepth),Z);

  
  return; 
}

void
GratingUnit::createObjects(Simulation& System)
  /*!
    Create the mono unit
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("GratingUnit","createObjects");

  const HeadRule frontHR=getRule("innerFront");
  const HeadRule frontOutHR=frontHR.complement();
  const HeadRule backHR=getRule("innerBack");
  const HeadRule backOutHR=backHR.complement();
  const HeadRule baseHR=getRule("innerBase");
  const HeadRule topHR=getRule("innerTop");

  HeadRule innerHR(frontHR);
  innerHR.addIntersection(backHR);
  innerHR.addIntersection(getRule("innerLeft"));
  innerHR.addIntersection(getRule("innerRight"));

  std::string Out;


  makeCell("InnerVoid",System,cellIndex++,0,0.0,
	   baseHR.display() + innerHR.display()+ topHR.display());
  innerHR.makeComplement();

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -201 3 -4 5 -6 ");
  makeCell("FBar",System,cellIndex++,mainMat,0.0,Out + innerHR.display());

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -201 3 -4 6 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,
	   Out+innerHR.display()+topHR.display());

  // outer void on edges
  HeadRule cutHR(getRule("innerLeft"));
  cutHR.addIntersection(getRule("innerRight"));
  cutHR.makeComplement();
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -5 ");
  Out+=cutHR.display()+frontHR.display()+backHR.display()+baseHR.display();

  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);


  // support bars

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1001  1003 -1004 1005 -5");
  makeCell("OuterFBar",System,cellIndex++,mainMat,0.0,Out+frontOutHR.display());
  addOuterSurf(Out+frontOutHR.display());
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -1002 1003 -1004 1005 -5");
  makeCell("OuterBBar",System,cellIndex++,mainMat,0.0,Out+backOutHR.display());
  addOuterUnionSurf(Out+backOutHR.display());

  // inner
  Out=ModelSupport::getComposite(SMap,buildIndex,"  101 -201 3 -4 ");
  addOuterUnionSurf(Out+baseHR.display()+topHR.display());
  

    //  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -201 3 -4 5 -6 ");
  //  makeCell("MidVoid",System,cellIndex++,0,0.0,Out);

  // support:
 
  // Out=ModelSupport::getComposite(SMap,buildIndex,"101 -202 3 -4 5 -6");
  // addOuterSurf(Out);


  
  return; 
}

void
GratingUnit::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("GratingUnit","createLinks");


  // // top surface going back down beamline to ring
  // FixedComp::setConnect(0,Origin,-Y);
  // FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+106));

  // // top surface going to experimental area
  // FixedComp::setConnect(1,Origin,Y);
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+205));

  return;
}

void
GratingUnit::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("GratingUnit","createAll");

  populate(System.getDataBase());
  /// insert later
  for(size_t i=0;i<3;i++)
    {
      ELog::EM<<"Grate Index == "<<grateIndex<<ELog::endDiag;
      grateArray[i]->setIndexPosition(static_cast<int>(i)-grateIndex);
      grateArray[i]->createAll(System,FC,sideIndex);
    }  


  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  /// insert later
  for(size_t i=0;i<3;i++)
    grateArray[i]->insertInCell(System,getCell("InnerVoid"));
  
  
  return;
}

}  // NAMESPACE xraySystem
