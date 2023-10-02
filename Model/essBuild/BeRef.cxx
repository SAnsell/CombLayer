/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BeRef.cxx
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
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "BeRefInnerStructure.h"
#include "BeRef.h"

namespace essSystem
{

BeRef::BeRef(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,12),
  attachSystem::CellMap(),
  engActive(0),
  InnerCompTop(new BeRefInnerStructure(Key+"TopInnerStructure")),
  InnerCompLow(new BeRefInnerStructure(Key+"LowInnerStructure")),
  lowVoidThick(-1.0),topVoidThick(-1.0),targSepThick(-1.0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR =
    ModelSupport::objectRegister::Instance();
  OR.addObject(InnerCompTop);
  OR.addObject(InnerCompLow);
}

BeRef::BeRef(const BeRef& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  
  engActive(A.engActive),
  InnerCompTop(new BeRefInnerStructure(*A.InnerCompTop)),
  InnerCompLow(new BeRefInnerStructure(*A.InnerCompLow)),
  radius(A.radius),height(A.height),depth(A.depth),
  wallThick(A.wallThick),wallThickLow(A.wallThickLow),
  lowVoidThick(A.lowVoidThick),
  topVoidThick(A.topVoidThick),targSepThick(A.targSepThick),
  topRefMat(A.topRefMat),lowRefMat(A.lowRefMat),
  topWallMat(A.topWallMat),lowWallMat(A.lowWallMat),
  targSepMat(A.targSepMat),voidCylRadius(A.voidCylRadius),
  voidCylDepth(A.voidCylDepth)
  /*!
    Copy constructor
    \param A :: BeRef to copy
  */
{}

BeRef&
BeRef::operator=(const BeRef& A)
  /*!
    Assignment operator
    \param A :: BeRef to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      engActive=A.engActive;
      *InnerCompTop = *A.InnerCompTop;
      *InnerCompLow = *A.InnerCompLow;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      wallThickLow=A.wallThickLow;
      lowVoidThick=A.lowVoidThick;
      topVoidThick=A.topVoidThick;
      targSepThick=A.targSepThick;
      topRefMat=A.topRefMat;
      lowRefMat=A.lowRefMat;
      topWallMat=A.topWallMat;
      lowWallMat=A.lowWallMat;
      targSepMat=A.targSepMat;
      voidCylRadius=A.voidCylRadius;
      voidCylDepth=A.voidCylDepth;
    }
  return *this;
}

BeRef::~BeRef()
  /*!
    Destructor
   */
{}

void
BeRef::setVoidThick(const double targetThick,
                    const double lowVThick,
                    const double topVThick)
  /*!
    Simple accessor to set primary thickness if not
    being set by variables
   */
{
  lowVoidThick=lowVThick;
  topVoidThick=topVThick;
  targSepThick=targetThick;
  return;
}

void
BeRef::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeRef","populateWithDef");

  FixedRotate::populate(Control);
  globalPopulate(Control);
  //  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  
  topRefMat=ModelSupport::EvalMat<int>(Control,keyName+"TopRefMat");   
  lowRefMat=ModelSupport::EvalMat<int>(Control,keyName+"LowRefMat");   
  topWallMat=ModelSupport::EvalMat<int>(Control,keyName+"TopWallMat");   
  lowWallMat=ModelSupport::EvalMat<int>(Control,keyName+"LowWallMat");   

  targSepMat=ModelSupport::EvalMat<int>(Control,keyName+"TargSepMat");
  
  lowVoidThick=(lowVoidThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"LowVoidThick") : lowVoidThick;
  topVoidThick=(topVoidThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"TopVoidThick") : topVoidThick;
  targSepThick=(targSepThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"TargetSepThick") : targSepThick;

  voidCylRadius=Control.EvalDefVar<double>(keyName+"VoidCylRadius",0.0);
  voidCylDepth=Control.EvalDefVar<double>(keyName+"VoidCylDepth",0.0);

  return;
}

void
BeRef::globalPopulate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeRef","globalPopulate");

  radius=Control.EvalVar<double>(keyName+"Radius");   
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");   
  wallThickLow=Control.EvalVar<double>(keyName+"WallThickLow");   
  
  return;
}

void
BeRef::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("BeRef","createSurfaces");

  // DIVIDER PLANES:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin,X);  
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);  
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);  

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);  
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(depth+wallThick),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height+wallThick),Z);  

  //define planes where the Be is substituted by Fe

  // Inner planes
  
  // wall and all gaps
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-
			   Z*(lowVoidThick+targSepThick/2.0+wallThickLow),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+
			   Z*(topVoidThick+targSepThick/2.0+wallThickLow),Z);  

  ModelSupport::buildPlane(SMap,buildIndex+115,Origin-
			   Z*(lowVoidThick+targSepThick/2.0),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+116,Origin+
			   Z*(topVoidThick+targSepThick/2.0),Z);  
  
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(targSepThick/2.0),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(targSepThick/2.0),Z);  

  // void volume (since we can't cool there)
  if (voidCylRadius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Z,voidCylRadius);
      ModelSupport::buildPlane(SMap,buildIndex+305,Origin-
			       Z*(lowVoidThick+targSepThick/2.0+
				  wallThickLow-voidCylDepth),Z);        
      // ModelSupport::buildShiftedPlane
      // (SMap,buildIndex+305,
      //       SMap.realPtr<Geometry::Plane>(buildIndex+105),
      //       -voidCylDepth);
    }

  
  return; 
}

void
BeRef::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("BeRef","createObjects");

  HeadRule HR;
  // low segment
  if (voidCylRadius>Geometry::zeroTol)
    {
      //  void volume
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-307 305 -105");
      makeCell("lowVoid",System,cellIndex++,0,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -105 (307:-305)");
      makeCell("lowBe",System,cellIndex++,lowRefMat,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -105");
      makeCell("lowBe",System,cellIndex++,lowRefMat,0.0,HR);
    }
  
  // low void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 115 -205");
  makeCell("lowVoid",System,cellIndex++,0,0.0,HR);

  // Target void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 205 -206");
  makeCell("targetVoid",System,cellIndex++,targSepMat,0.0,HR);
  
  // top Segment
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -116 206");
  makeCell("topVoid",System,cellIndex++,0,0.0,HR);

  // top segment
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -6 106");
  makeCell("topBE",System,cellIndex++,topRefMat,0.0,HR);

  if (wallThick>Geometry::zeroTol)
    {

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 15 -105 (7:-5)");
      makeCell("lowWall",System,cellIndex++,lowWallMat,0.0,HR);
      
      if (wallThickLow>Geometry::zeroTol)
	{
	  // divide layer
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 105 -115");
	  makeCell("lowWallDivider",System,cellIndex++,lowWallMat,0.0,HR);

	  // divide layer
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -106 116");
	  makeCell("lowWallDivider",System,cellIndex++,topWallMat,0.0,HR);
	}

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -16 106 (7:6)");
      makeCell("topWall",System,cellIndex++,topWallMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 15 -16");
    }
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 5 -6");

  addOuterSurf(HR);
  return; 

}

void
BeRef::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  ELog::RegMethod RegA("BeRef","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*radius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  FixedComp::addBridgeSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*radius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));
  FixedComp::addBridgeSurf(1,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::addBridgeSurf(2,-SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(3,Origin+X*radius,-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  FixedComp::addBridgeSurf(3,SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(4,Origin-Z*(depth+wallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height+wallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  FixedComp::setConnect(6,Origin-Z*depth,-Z);
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(7,Origin+Z*height,Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(8,Origin+Y*radius,-Y);
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+7));

  FixedComp::setConnect(9,Origin-Z*(lowVoidThick+targSepThick/2.0+
				    wallThickLow),-Z);
  FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+105));

  FixedComp::setConnect(10,Origin+Z*(lowVoidThick+targSepThick/2.0+
				     wallThickLow),Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+106));

  FixedComp::setConnect(11,Origin-Y*radius,-Y);
  FixedComp::setLinkSurf(11,SMap.realSurf(buildIndex+17));
  nameSideIndex(11,"OuterRadius");
  
  return;
}

void
BeRef::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
                 const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("BeRef","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  if (engActive)
    {
      InnerCompTop->setCutSurf("RefBase",*this,11);
      InnerCompTop->setCutSurf("RefTop",*this,8);
      
      InnerCompLow->setCutSurf("RefBase",*this,10);
      InnerCompLow->setCutSurf("RefTop",*this,7);

      InnerCompTop->setCell("ReflectorUnit",this->getCell("topBe"));
      InnerCompLow->setCell("ReflectorUnit",this->getCell("lowBe")); 
      
      
      InnerCompTop->createAll(System,*this,0);
      InnerCompLow->createAll(System,*this,0);
    }

  return;
}

  
}  // NAMESPACE essSystem
