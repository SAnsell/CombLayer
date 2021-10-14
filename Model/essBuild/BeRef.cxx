/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BeRef.cxx
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
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
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
  attachSystem::FixedOffsetUnit(Key,11),
  attachSystem::CellMap(),
  engActive(0),
  InnerCompTop(new BeRefInnerStructure(Key+"TopInnerStructure")),
  InnerCompLow(new BeRefInnerStructure(Key+"LowInnerStructure"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(InnerCompTop);
  OR.addObject(InnerCompLow);
}

BeRef::BeRef(const BeRef& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffsetUnit(A),
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
      attachSystem::FixedOffset::operator=(A);
      CellMap::operator=(A);
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
BeRef::populateWithDef(const FuncDataBase& Control,
                       const double targetThick,
                       const double lowVThick,
                       const double topVThick)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
    \param targetThick :: thickness of the target
    \param topVThick :: thickness of the premod-void
    \param lowVThick :: thickness of the premod-void
  */
{
  ELog::RegMethod RegA("BeRef","populateWithDef");

  FixedOffset::populate(Control);
  globalPopulate(Control);
  //  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  
  topRefMat=ModelSupport::EvalMat<int>(Control,keyName+"TopRefMat");   
  lowRefMat=ModelSupport::EvalMat<int>(Control,keyName+"LowRefMat");   
  topWallMat=ModelSupport::EvalMat<int>(Control,keyName+"TopWallMat");   
  lowWallMat=ModelSupport::EvalMat<int>(Control,keyName+"LowWallMat");   

  targSepMat=ModelSupport::EvalMat<int>(Control,keyName+"TargSepMat");
  
  lowVoidThick=(lowVThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"LowVoidThick") : lowVThick;
  topVoidThick=(topVThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"TopVoidThick") : topVThick;
  targSepThick=(targetThick<Geometry::zeroTol) ?
    Control.EvalVar<double>(keyName+"TargetSepThick") : targetThick;

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

  std::string Out;
  // low segment
  if (voidCylRadius>Geometry::zeroTol)
    {
      //  void volume
      Out=ModelSupport::getComposite(SMap,buildIndex," -307 305 -105 ");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

      Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -105 (307:-305) ");
      System.addCell(MonteCarlo::Object(cellIndex++,lowRefMat,0.0,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -105 ");
      System.addCell(MonteCarlo::Object(cellIndex++,lowRefMat,0.0,Out));
    }
  setCell("lowBe",cellIndex-1);
  
  // low void
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 115 -205");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  setCell("lowVoid",cellIndex-1);
  // Target void
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 205 -206");
  System.addCell(MonteCarlo::Object(cellIndex++,targSepMat,0.0,Out));
  setCell("targetVoid",cellIndex-1);
  
  // top Segment
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 -116 206");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  setCell("topVoid",cellIndex-1);

  // top segment
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -6 106");
  System.addCell(MonteCarlo::Object(cellIndex++,topRefMat,0.0,Out));
  setCell("topBe",cellIndex-1);
  
  if (wallThick>Geometry::zeroTol)
    {

      Out=ModelSupport::getComposite(SMap,buildIndex," -17 15 -105 (7:-5)");
      System.addCell(MonteCarlo::Object(cellIndex++,lowWallMat,0.0,Out));
      setCell("lowWall",cellIndex-1);
      
      if (wallThickLow>Geometry::zeroTol)
	{
	  // divide layer
	  Out=ModelSupport::getComposite(SMap,buildIndex," -17 105 -115 ");
	  System.addCell(MonteCarlo::Object(cellIndex++,lowWallMat,0.0,Out));
	  setCell("lowWallDivider",cellIndex-1);
	  
	  // divide layer
	  Out=ModelSupport::getComposite(SMap,buildIndex," -17 -106 116 ");
	  System.addCell(MonteCarlo::Object(cellIndex++,topWallMat,0.0,Out));
	}

      Out=ModelSupport::getComposite(SMap,buildIndex," -17 -16 106 (7:6)");
      System.addCell(MonteCarlo::Object(cellIndex++,topWallMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,buildIndex," -17 15 -16 ");
    }
  else
    Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -6 ");

  addOuterSurf(Out);
  return; 

}

void
BeRef::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  FixedComp::setConnect(0,Origin+Y*radius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*radius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(1,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(2,Origin+Y*radius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(2,-SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(3,Origin+Y*radius,-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(3,SMap.realSurf(buildIndex+2));
  
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

  return;
}


void
BeRef::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
                 const long int sideIndex,
		 const double tThick,
		 const double lpThick,
		 const double tpThick)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Link point
    \param tThick :: Thickness of target void for exact cutting
    \param lpThick :: Thickness of lower-preMod
    \param tpThick :: Thickness of top-preMod
  */
{
  ELog::RegMethod RegA("BeRef","createAll");

  populateWithDef(System.getDataBase(),tThick,lpThick,tpThick);
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
      InnerCompLow->setCell("ReflectorUnit",this->getCell("topBe"));
      
      
      InnerCompTop->createAll(System,*this,0);
      InnerCompLow->createAll(System,*this,0);
    }

  return;
}

  
}  // NAMESPACE essSystem
