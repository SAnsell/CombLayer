/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LeadPipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "FrontBackCut.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"

#include "GeneralPipe.h"
#include "LeadPipe.h"

namespace constructSystem
{

LeadPipe::LeadPipe(const std::string& Key) :
  constructSystem::GeneralPipe(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


LeadPipe::~LeadPipe() 
  /*!
    Destructor
  */
{}

void
LeadPipe::populate(const FuncDataBase& Control)
    /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("LeadPipe","populate");
  
  GeneralPipe::populate(Control);
  
  claddingThick=Control.EvalVar<double>(keyName+"CladdingThick");
  claddingStep=Control.EvalVar<double>(keyName+"CladdingStep");

  claddingMat=ModelSupport::EvalMat<int>(Control,keyName+"CladdingMat");
  
  return;
}
  
void
LeadPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("LeadPipe","createSurfaces");

  GeneralPipe::createSurfaces();

  ExternalCut::makeShiftedSurf
    (SMap,"front",buildIndex+121,Y,(flangeALength+claddingStep));

  ExternalCut::makeShiftedSurf
    (SMap,"back",buildIndex+122,Y,-(flangeBLength+claddingStep));

  SurfMap::makeCylinder("CladdingCyl",SMap,buildIndex+27,
			Origin,Y,radius+pipeThick+claddingThick);

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
  HR=HeadRule(SMap,buildIndex,-7);
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // A FLANGE:
  if (flangeARadius>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 7");
      makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,HR*frontHR);
    }
  // FLANGE B
  if (flangeBRadius>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"102 -207 7");
      makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,HR*backHR);
    }



  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7 101 -102");
  makeCell("MainPipe",System,cellIndex++,pipeMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -122 -27 17");
  makeCell("Cladding",System,cellIndex++,claddingMat,0.0,HR);

  // create lead gap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -121 17 -27");
  makeCell("LeadAGap",System,cellIndex++,voidMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-102 122 17 -27");
  makeCell("LeadBGap",System,cellIndex++,voidMat,0.0,HR);

      
  HR=HeadRule(SMap,buildIndex,27);
  GeneralPipe::createOuterVoid(System,HR);
  

  /*
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
  */
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

  FixedComp::setConnect(3,Origin+Z*(radius+pipeThick),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(4,Origin+Z*(radius+pipeThick+claddingThick),Z);
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
