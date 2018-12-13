/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/GratingMonoUnit.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "GrateHolder.h"
#include "GratingMonoUnit.h"


namespace xraySystem
{

GratingMonoUnit::GratingMonoUnit(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
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


GratingMonoUnit::~GratingMonoUnit()
  /*!
    Destructor
   */
{}

void
GratingMonoUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GratingMonoUnit","populate");

  FixedOffset::populate(Control);
  
  grateTheta=Control.EvalVar<double>(keyName+"GrateTheta");
  
  mainGap=Control.EvalVar<double>(keyName+"MainGap");
  mainBarXLen=Control.EvalVar<double>(keyName+"MainBarXLen");
  mainBarDepth=Control.EvalVar<double>(keyName+"MainBarDepth");
  mainBarYWidth=Control.EvalVar<double>(keyName+"MainBarYWidth");
  
  slidePlateThick=Control.EvalVar<double>(keyName+"SlidePlateThick");
  slidePlateLength=Control.EvalVar<double>(keyName+"SlidePlateLength");
  slidePlateGapWidth=Control.EvalVar<double>(keyName+"SlidePlateGapWidth");
  slidePlateGapLength=Control.EvalVar<double>(keyName+"SlidePlateGapLength");
  
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  slideMat=ModelSupport::EvalMat<int>(Control,keyName+"SlideMat");


  return;
}

void
GratingMonoUnit::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("GratingMonoUnit","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
GratingMonoUnit::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("GratingMonoUnit","createSurfaces");

  setCutSurf("innerFront",grateArray[0]->getSurf("Front"));
  setCutSurf("innerBack",grateArray[0]->getSurf("Back"));
  setCutSurf("innerLeft",grateArray[0]->getSurf("Left"));
  setCutSurf("innerRight",grateArray[2]->getSurf("Right"));

  ExternalCut::makeShiftedSurf
    (SMap,"innerFront",buildIndex+101,-1,-Y,mainBarYWidth);
  ExternalCut::makeShiftedSurf
    (SMap,"innerBack",buildIndex+201,-1,Y,mainBarYWidth);

 
  // bar common:
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(mainBarXLen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(mainBarXLen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(mainBarDepth),Z);

  return; 
}

void
GratingMonoUnit::createObjects(Simulation& System)
  /*!
    Create the mono unit
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("GratingMonoUnit","createObjects");

  const std::string front=getComplementStr("innerFront");
  const std::string back=getComplementStr("innerBack");
  std::string Out;
  ELog::EM<<"Front == "<<front<<ELog::endDiag;
  ELog::EM<<"Back  == "<<back<<ELog::endDiag;
  ELog::EM<<"FR == "<<SMap.realSurf(buildIndex+101)<<Elog::endDiag;
  // main bars
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 3 -4 5 -6 ");
  makeCell("FBar",System,cellIndex++,mainMat,0.0,Out + front);
  addOuterSurf(Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -201 3 -4 5 -6 ");
  makeCell("BBar",System,cellIndex++,mainMat,0.0,Out + back);
  addOuterUnionSurf(Out);

  //  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -201 3 -4 5 -6 ");
  //  makeCell("MidVoid",System,cellIndex++,0,0.0,Out);

  // support:
 
  // Out=ModelSupport::getComposite(SMap,buildIndex,"101 -202 3 -4 5 -6");
  // addOuterSurf(Out);


  
  return; 
}

void
GratingMonoUnit::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("GratingMonoUnit","createLinks");


  // // top surface going back down beamline to ring
  // FixedComp::setConnect(0,Origin,-Y);
  // FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+106));

  // // top surface going to experimental area
  // FixedComp::setConnect(1,Origin,Y);
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+205));

  return;
}

void
GratingMonoUnit::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("GratingMonoUnit","createAll");

  /// insert later
  for(size_t i=0;i<3;i++)
    grateArray[i]->createAll(System,FC,sideIndex);
  
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  
  return;
}

}  // NAMESPACE xraySystem
