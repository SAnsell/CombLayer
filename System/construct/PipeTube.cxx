/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PipeTube.cxx
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
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
#include "portItem.h"

#include "VirtualTube.h"
#include "PipeTube.h"

namespace constructSystem
{

PipeTube::PipeTube(const std::string& Key) :
  constructSystem::VirtualTube(Key)  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"FlangeA");
  nameSideIndex(3,"FlangeB");
  ContainedGroup::addCC("FlangeA");
  ContainedGroup::addCC("FlangeB");
}

  
PipeTube::~PipeTube() 
  /*!
    Destructor
  */
{}

void
PipeTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PipeTube","populate");
  
  VirtualTube::populate(Control);

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");

  flangeACapThick=Control.EvalDefPair<double>(keyName+"FlangeACapThick",
					 keyName+"FlangeCapThick",0.0);
  flangeBCapThick=Control.EvalDefPair<double>(keyName+"FlangeBCapThick",
					 keyName+"FlangeCapThick",0.0);
  

  return;
}

void
PipeTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PipeTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  const double frontLength(length/2.0+flangeACapThick);
  const double backLength(length/2.0+flangeACapThick);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*frontLength,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*backLength,Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+101,Y,
			       flangeALength+flangeACapThick);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+102,Y,
			       -(flangeBLength+flangeBCapThick));

  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+201,Y,
			       flangeACapThick);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+202,Y,
			       -(flangeBCapThick));
  
  // void space:
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  SurfMap::addSurf("VoidCyl",-SMap.realSurf(buildIndex+7));
    
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  SurfMap::addSurf("OuterCyl",SMap.realSurf(buildIndex+17));


  // flange:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  SurfMap::addSurf("FlangeACyl",SMap.realSurf(buildIndex+107));

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);
  SurfMap::addSurf("FlangeBCyl",SMap.realSurf(buildIndex+207));
  
  return;
}

HeadRule
PipeTube::makeOuterVoid(Simulation& System)
  /*!
    Build outer void and return the outer volume
    \param System :: Simulation to build in
    \return Outer exclude volume
  */
{
  ELog::RegMethod RegA("PipeTube","makeOuterVoid");
  
  HeadRule HR;
  const HeadRule frontHR(frontRule());
  const HeadRule backHR(backRule());

  if (flangeARadius>flangeBRadius+Geometry::zeroTol)
    {
      ELog::EM<<"Code unwritten"<<ELog::endErr;
    }
  else if (flangeBRadius>flangeARadius+Geometry::zeroTol)
    {
      ELog::EM<<"Code unwritten"<<ELog::endErr;
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"17 -107 101 -102");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);

      SurfMap::addSurf("OuterRadius",SMap.realSurf(buildIndex+107));
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107")*frontHR*backHR;

    }
  return HR;
}

void
PipeTube::createObjects(Simulation& System)
  /*!
    Builds the pipe
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PipeTube","createObjects");

  const HeadRule frontHR=getFrontRule();
  const HeadRule backHR=getBackRule();

  const HeadRule frontVoidHR=
    (flangeACapThick<Geometry::zeroTol) ? frontHR :
    ModelSupport::getHeadRule(SMap,buildIndex,"201");
  const HeadRule backVoidHR=
    (flangeBCapThick<Geometry::zeroTol) ? backHR :
    ModelSupport::getHeadRule(SMap,buildIndex,"-202");
  
  
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,
	   HR*frontVoidHR*backVoidHR);
  // main walls
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 7 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,
	   HR*frontVoidHR*backVoidHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 17 -107 -101 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,HR*frontVoidHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 17 -207 102 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR*backVoidHR);

  if (flangeACapThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," -201 -107 ");
      makeCell("FrontCap",System,cellIndex++,capMat,0.0,HR*frontHR);	    
    }
  
  if (flangeBCapThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 -207");
      makeCell("BackCap",System,cellIndex++,capMat,0.0,HR*backHR);
    }

  if (outerVoid)
    {
      HR=makeOuterVoid(System);
      addOuterSurf("Main",HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -17");
      addOuterSurf("Main",HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107 -101");
      addOuterSurf("FlangeA",HR*frontHR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 102");
      addOuterSurf("FlangeB",HR*backHR);
    }
  return;
}

void
PipeTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("PipeTube","createLinks");

  // port centre
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  // getlinke points
  FixedComp::setConnect(2,FixedComp::getLinkPt(1),-Y);
  FixedComp::setConnect(3,FixedComp::getLinkPt(2),Y);

  // make a composite flange
  HeadRule HR;


  const HeadRule frontSurf(getFrontRule());
  const HeadRule backSurf(getBackRule());
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
  FixedComp::setLinkComp(2,HR*frontSurf);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207");
  FixedComp::setLinkComp(3,HR*backSurf);

  // inner links
  int innerFrontSurf, innerBackSurf;
  Geometry::Vec3D innerFrontVec, innerBackVec;
  if (flangeACapThick<Geometry::zeroTol)
    {
      innerFrontSurf = getFrontRule().getPrimarySurface();
      innerFrontVec = Origin-Y*(length/2.0);
    }
  else
    {
      innerFrontSurf = buildIndex+201;
      innerFrontVec = Origin-Y*(length/2.0-flangeACapThick);
    }

  if (flangeBCapThick<Geometry::zeroTol)
    {
      innerBackSurf  = getBackRule().getPrimarySurface();
      innerBackVec  = Origin+Y*(length/2.0);
    } else
    {
      innerBackSurf  = buildIndex+202;
      innerBackVec  = Origin+Y*(length/2.0-flangeBCapThick);
    }
  
  FixedComp::setConnect(4,innerFrontVec,Y);
  FixedComp::setLinkSurf(4,innerFrontSurf);
  nameSideIndex(4,"InnerFront");

  FixedComp::setConnect(5,innerBackVec,Y);
  FixedComp::setLinkSurf(5,-innerBackSurf);
  nameSideIndex(5,"InnerBack");

  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+7));
  nameSideIndex(6,"InnerSide");

  if (!outerVoid)
    {
      FixedComp::setConnect(8,Origin+Z*(radius+wallThick),Z);
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));
    }
  else
    {
      FixedComp::setConnect(8,Origin+Z*flangeBRadius,Z);
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+107));
    }
  nameSideIndex(8,"OuterRadius");

  
  return;
}

void
PipeTube::createPorts(Simulation& System)
  /*!
    Create the ports [for outside ports]
    \param System :: Simulation for model
   */
{
  ELog::RegMethod RegA("PipeTube","createPorts");
  // both OUTWARD
  MonteCarlo::Object* OPtr=
    CellMap::getCellObject(System,"MainTube");

  const HeadRule innerSurf(SurfMap::getSurfRules("#VoidCyl"));
  const HeadRule outerSurf(SurfMap::getSurfRules("OuterCyl"));

  if (outerVoid)
    {
      const HeadRule flangeSurf(SurfMap::getSurfRules("FlangeACyl"));
      createPorts(System,OPtr,innerSurf,flangeSurf);
      HeadRule OVoidHR=SurfMap::getSurfRule("#OuterRadius");
      OVoidHR.populateSurf();
      for(size_t i=0;i<Ports.size();i++)
	{
	  const Geometry::Vec3D Pt = Ports[i]->getLinkPt("FlangePlate");
	  if (OVoidHR.isValid(Pt))
	    Ports[i]->addFlangeCut(CellMap::getCellObject(System,"OuterVoid"));

	}
    }
  else
    createPorts(System,OPtr,innerSurf,outerSurf);
}

  
  
}  // NAMESPACE constructSystem
