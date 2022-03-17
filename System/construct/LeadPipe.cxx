/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LeadPipe.cxx
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
#include <array>

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"

#include "LeadPipe.h"

namespace constructSystem
{

LeadPipe::LeadPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  outerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    Note that groups: FlangeA and FlangeB are only added
    if outerVoid is NOT set in method createObject.
    \param Key :: KeyName
  */
{
  // FixedComp::nameSideIndex(2,"innerPipe");
  // FixedComp::nameSideIndex(6,"outerPipe");
  // FixedComp::nameSideIndex(8,"pipeWall");
}


LeadPipe::~LeadPipe() 
  /*!
    Destructor
  */
{}

void
LeadPipe::populate(const FuncDataBase& Control)
{
  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  thick=Control.EvalVar<double>(keyName+"Thick");
  
  claddingThick=Control.EvalVar<double>(keyName+"CladdingThick");
  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");
  claddingStep=Control.EvalVar<double>(keyName+"CladdingStep");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  claddingMat=ModelSupport::EvalMat<int>(Control,keyName+"CladdingMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}
  
void
LeadPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("LeadPipe","createSurfaces");

  // Inner void
  if (!isActive("front"))
    {
      SurfMap::makePlane("Front",SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ELog::EM<<"Key "<<keyName<<" "<<Origin-Y*(length/2.0)<<ELog::endDiag;
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+11,Y,flangeALength);
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+21,
			       Y,(flangeALength+claddingStep));

  if (!isActive("back"))
    {
      SurfMap::makePlane("Back",SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+12,Y,-flangeBLength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+22,
			       Y,-(flangeBLength+claddingStep));

  SurfMap::makeCylinder("InnerCyl",SMap,buildIndex+7,Origin,Y,radius);

  SurfMap::makeCylinder("PipeCyl",SMap,buildIndex+17,
			Origin,Y,radius+thick);
  SurfMap::makeCylinder("CladdingCyl",SMap,buildIndex+27,
			Origin,Y,radius+thick+claddingThick);

  // FLANGE SURFACES FRONT/BACK:
  SurfMap::makeCylinder("FlangeARadius",SMap,buildIndex+107,
			Origin,Y,flangeARadius);
  SurfMap::makeCylinder("FlangeBRadius",SMap,buildIndex+207,
			Origin,Y,flangeBRadius);

  return;
}

void
LeadPipe::createObjects(Simulation& System)
  /*!
    Create the main pipe objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LeadPipe","createObjects");

  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  HeadRule HR;
  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // FLANGE Front:
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 -107 17 ");
  makeCell("FrontFlange",System,cellIndex++,flangeMat,0.0,HR*frontHR);

  // FLANGE Back:
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 -207 17 ");
  makeCell("BackFlange",System,cellIndex++,flangeMat,0.0,HR*backHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7");
  makeCell("MainPipe",System,cellIndex++,pipeMat,0.0,HR*frontHR*backHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 -27 17");
  makeCell("Cladding",System,cellIndex++,claddingMat,0.0,HR);

  // choose largest radius
  const int radiusIndex=(flangeARadius>=flangeBRadius) ?
    buildIndex :  buildIndex+100;

  if (outerVoid)
    {
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,radiusIndex,"11 -12 17 (-21:22:27) -107M ");
      makeCell("OuterGap",System,cellIndex++,voidMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,radiusIndex,"-107");
      addOuterSurf("Main",HR*frontHR*backHR);
    }
  else
    {      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 17 -27");
      makeCell("LeadAGap",System,cellIndex++,voidMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 17 -27");
      makeCell("LeadBGap",System,cellIndex++,voidMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -27");
      addOuterSurf("Main",HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -107");
      addOuterSurf("FlangeA",HR*frontHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -207");
      addOuterSurf("FlangeB",HR*backHR);
      
    }
  return;
}

void
LeadPipe::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("LeadPipe","createLinks");
  
  ExternalCut::createLink("front",*this,0,Origin,Y);  // Front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);   // Front and back
  
  FixedComp::setConnect(2,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));

  FixedComp::setConnect(3,Origin+Z*(radius+thick),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(4,Origin+Z*(radius+thick+claddingThick),Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+27));
  
  FixedComp::nameSideIndex(2,"inner");
  FixedComp::nameSideIndex(3,"pipe");
  FixedComp::nameSideIndex(4,"outer");


  
  return;
}


void
LeadPipe::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param sideIndex :: link Index
  */ 
{
  ELog::RegMethod RegA("LeadPipe","createAll(FC,index)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}

  
}  // NAMESPACE constructSystem
