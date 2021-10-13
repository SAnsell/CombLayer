/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/portItem.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "portItem.h"

namespace constructSystem
{

portItem::portItem(const std::string& baseKey,
		   const std::string& Key) :
  attachSystem::FixedComp(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  portBase(baseKey),
  statusFlag(0),outerFlag(0),
  externalLength(0.0),radius(0.0),wall(0.0),
  flangeRadius(0.0),flangeLength(0.0),capThick(0.0),
  voidMat(0),wallMat(0),capMat(-1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base name
    \param Key :: KeyName
  */
{}

portItem::portItem(const std::string& Key) :
  attachSystem::FixedComp(Key,8),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  portBase(keyName),
  statusFlag(0),outerFlag(0),
  externalLength(0.0),radius(0.0),wall(0.0),
  flangeRadius(0.0),flangeLength(0.0),capThick(0.0),
  windowRadius(0.0),windowThick(0.0),
  voidMat(0),wallMat(0),capMat(-1),windowMat(-1),
  outerVoidMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

portItem::portItem(const portItem& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  portBase(A.portBase),statusFlag(A.statusFlag),
  outerFlag(A.outerFlag),centreOffset(A.centreOffset),
  axisOffset(A.axisOffset),externalLength(A.externalLength),
  radius(A.radius),wall(A.wall),flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),capThick(A.capThick),
  windowRadius(A.windowRadius),windowThick(A.windowThick),
  voidMat(A.voidMat),wallMat(A.wallMat),capMat(A.capMat),
  windowMat(A.windowMat),outerVoidMat(A.outerVoidMat),
  outerCell(A.outerCell),refComp(A.refComp),exitPoint(A.exitPoint)
  /*!
    Copy constructor
    \param A :: portItem to copy
  */
{}

portItem&
portItem::operator=(const portItem& A)
  /*!
    Assignment operator
    \param A :: portItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      statusFlag=A.statusFlag;
      outerFlag=A.outerFlag;
      centreOffset=A.centreOffset;
      axisOffset=A.axisOffset;
      externalLength=A.externalLength;
      radius=A.radius;
      wall=A.wall;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      capThick=A.capThick;
      windowRadius=A.windowRadius;
      windowThick=A.windowThick;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      capMat=A.capMat;
      windowMat=A.windowMat;
      outerVoidMat=A.outerVoidMat;
      outerCell=A.outerCell;
      refComp=A.refComp;
      exitPoint=A.exitPoint;
    }
  return *this;
}

  
portItem::~portItem()
  /*!
    Destructor
  */
{}

void
portItem::setMain(const double L,const double R,const double WT)
  /*!
    Set parameters
    \param L :: external length
    \param R :: Internal radius
    \param WT :: Wall thick

  */
{
  externalLength=L;
  radius=R;
  wall=WT;
  return;
}

void
portItem::setFlange(const double FR,const double FT)
  /*!
    Set flange parameters
    \param FR :: Flange radius
    \param FT :: Flange thick
  */
{
  flangeRadius=FR;
  flangeLength=FT;
  return;
}

void
portItem::setCoverPlate(const double T,const int M)
  /*!
    Set flange parameters
    \param T :: Plate thickness
    \param M :: cover plate material [-ve for wall Mat]
  */
{
  capThick=T;
  capMat=(M<0) ? wallMat : M;

  return;
}

void
portItem::setMaterial(const int V,const int W,
		      const int PM)
  /*!
    Sets the materials
    \param V :: Void mat
    \param W :: Wall Mat.
    \param PM :: Plate Material [-ve for default]
   */
{
  voidMat=V;
  wallMat=W;
  if (PM<0 && capMat<0)
    capMat=wallMat;
  else if (PM>=0)
    capMat=PM;

  return;
}

void
portItem::populate(const FuncDataBase& Control)
  /*!
    Populate variables
    \param Control :: Variable DataBase
   */
{
  ELog::RegMethod RegA("portItem","populate");

  centreOffset=
    Control.EvalVar<Geometry::Vec3D>(keyName+"Centre");
  axisOffset=
    Control.EvalTail<Geometry::Vec3D>(keyName,portBase,"Axis");

  externalLength=Control.EvalTail<double>(keyName,portBase,"Length");
  radius=Control.EvalTail<double>(keyName,portBase,"Radius");
  wall=Control.EvalTail<double>(keyName,portBase,"Wall");

  flangeRadius=Control.EvalTail<double>(keyName,portBase,"FlangeRadius");
  flangeLength=Control.EvalTail<double>(keyName,portBase,"FlangeLength");
  capThick=Control.EvalDefTail<double>(keyName,portBase,"CapThick",0.0);

  windowThick=Control.EvalDefTail<double>(keyName,portBase,"WindowThick",0.0);

  windowRadius=Control.EvalDefTail<double>(keyName,portBase,"WindowRadius",0.0);
  
  voidMat=ModelSupport::EvalDefMat
    (Control,keyName+"VoidMat",portBase+"VoidMat",0);

  /// need to write a Eval<bool> version
  int oFlag(static_cast<int>(outerFlag));
  oFlag=Control.EvalDefTail<int>(keyName,portBase,"OuterVoid",oFlag);
  outerFlag=static_cast<bool>(oFlag);
	
  outerVoidMat=ModelSupport::EvalDefMat
    (Control,keyName+"OuterVoidMat",portBase+"OuterVoidMat",0);

  wallMat=ModelSupport::EvalMat<int>
    (Control,keyName+"WallMat",portBase+"WallMat");
  capMat=ModelSupport::EvalDefMat
    (Control,keyName+"CapMat",portBase+"CapMat",capMat);
  if (capMat<0) capMat=wallMat;

  windowMat=ModelSupport::EvalDefMat
    (Control,keyName+"WindowMat",portBase+"WindowMat",windowMat);
  if (windowMat<0) windowMat=capMat;

  return;
}

void
portItem::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("portItem","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  refComp=FC.getKeyName();
  statusFlag=1;
  return;
}


void
portItem::setCentLine(const attachSystem::FixedComp& FC,
		      const Geometry::Vec3D& Centre,
		      const Geometry::Vec3D& Axis)
  /*!
    Set position
    \param FC :: FixedComp to get inital orientation [origin]
    \param Centre :: centre point [in FC basis coordinates]
    \param Axis :: Axis direction
   */
{
  portItem::createUnitVector(FC,0);
  Origin+=X*Centre.X()+Y*Centre.Y()+Z*Centre.Z();
  const Geometry::Vec3D DVec=Axis.getInBasis(X,Y,Z);

  FixedComp::reOrientate(1,DVec.unit());
  return;
}

void
portItem::reNormZ(const Geometry::Vec3D& Axis)
  /*!
    Apply extra rotation to Z [if needed]
    \param Axis :: Z-Axis direction
   */
{
  FixedComp::reOrientate(2,Axis.unit());
  return;
}

void
portItem::addOuterCell(const int ON)
  /*!
    Add outer cell(s)
    \param ON :: cell number
   */
{
  outerCell.insert(ON);
  return;
}

void
portItem::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("portItem","createSurfaces");
  // divider surface if needeed :

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  if (flangeRadius-Geometry::zeroTol<=radius+wall)
    throw ColErr::SizeError<double>(flangeRadius,wall+radius,
				    "Wall Radius<FlangeRadius");
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wall);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);

  
  // Final outer
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+Y*externalLength,Y);

  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(externalLength-flangeLength),Y);

  const bool capFlag(capThick>Geometry::zeroTol);
  const bool windowFlag (capFlag &&
			 windowThick>Geometry::zeroTol &&
			 windowThick+Geometry::zeroTol <capThick &&
			 windowRadius>Geometry::zeroTol &&
			 windowRadius+Geometry::zeroTol < flangeRadius);
  if (!windowFlag) windowThick=-1.0;
  // 
  // This builds a window cap if required:
  // 
  if (capFlag)
    {
      Geometry::Vec3D capPt(Origin+Y*(externalLength+capThick));
      ModelSupport::buildPlane(SMap,buildIndex+202,capPt,Y);
      // if we have a cap we might have a window:
      if (windowFlag)
	{
	  capPt-= Y*(capThick/2.0);   // move to mid point
	  ModelSupport::buildPlane
	    (SMap,buildIndex+211,capPt-Y*(windowThick/2.0),Y);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+212,capPt+Y*(windowThick/2.0),Y);
	  ModelSupport::buildCylinder
	    (SMap,buildIndex+207,capPt,Y,windowRadius);
	}
    }
  /// ----  END : Cap/Window
 

  return;
}

void
portItem::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
    \param LT :: Line track
    \param AIndex :: start of high density material
    \param BIndex :: end of high density material
  */
{
  ELog::RegMethod RegA("portItem","createLinks");

  FixedComp::nameSideIndex(0,"BasePoint");
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(1,"OuterPlate");
  
  if (capThick>Geometry::zeroTol)
    {
      FixedComp::setConnect(1,Origin+Y*(externalLength+capThick),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+202));
    }
  else
    {
      FixedComp::setConnect(1,Origin+Y*externalLength,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
    }

  FixedComp::nameSideIndex(2,"InnerRadius");
  FixedComp::setConnect(2,Origin+Y*(externalLength/2.0)+X*radius,X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+7));
  FixedComp::setBridgeSurf(2,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(3,"WallRadius");
  FixedComp::setConnect(3,Origin+Y*(externalLength/2.0)+X*(wall+radius),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+17));
  FixedComp::setBridgeSurf(3,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(4,"InnerPlate");
  FixedComp::setConnect(4,Origin+Y*externalLength,-Y);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+2));

  FixedComp::nameSideIndex(5,"VoidRadius");
  FixedComp::setConnect(5,Origin+Y*externalLength,-Y);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+27));
  FixedComp::setBridgeSurf(5,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(6,"FlangePlate");
  const Geometry::Vec3D flangePoint=
    Origin+Y*(externalLength-flangeLength);
  
  FixedComp::setConnect(6,flangePoint,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+102));

  
  FixedComp::nameSideIndex(7,"OuterRadius");
  if (outerFlag)
    {
      FixedComp::setConnect(7,Origin+X*flangeRadius,X);
      FixedComp::setBridgeSurf(7,SMap.realSurf(buildIndex+27));
    }
  else
    {
      FixedComp::setConnect(7,Origin+X*(wall+radius),X);
      FixedComp::setBridgeSurf(7,SMap.realSurf(buildIndex+17));
    }

  return;
}

void
portItem::constructObject(Simulation& System,
			  const HeadRule& innerSurf,
			  const HeadRule& outerSurf)
/*!
    Construct a flange from the centre point
    \param System :: Simulation to use
    \param inner Surface of main cell to cut (into the void typically)
    \param wall Surface of main cell to cut 
  */
{
  ELog::RegMethod RegA("portItem","constructObject");

  const bool capFlag(capThick>Geometry::zeroTol);
  const bool windowFlag(windowThick>Geometry::zeroTol);
  
  // construct inner volume:
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -7 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -17 7 -2 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 102 -27 17 -2 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);

  if (capFlag)
    {
      // we have window AND flange:
      if (windowFlag)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 -211 2");
	  makeCell("BelowPlate",System,cellIndex++,voidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 212 -202");
	  makeCell("AbovePlate",System,cellIndex++,outerVoidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 211 -212");
	  makeCell("Plate",System,cellIndex++,windowMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 207 -202 2");
	  makeCell("PlateSurround",System,cellIndex++,capMat,0.0,HR);
	}
      else // just a cap
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," -27 2 -202 ");
	  makeCell("Plate",System,cellIndex++,capMat,0.0,HR);
	}
    }

  if (outerFlag)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 17  -27 -102");
      makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR*outerSurf);
      HR= (capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 1") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27  1");

      addOuterSurf(HR*outerSurf);
    }
  else
    {
      HR= (capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 102") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27 102");
      addOuterSurf(HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -102 1");
      addOuterUnionSurf(HR*outerSurf);
    }
  return;
}

void
portItem::addPortCut(MonteCarlo::Object* mainTube) const
  /*!
    Adds the port exclude to the main tube
    \param mainTube :: object for main tube
  */
{
  // Mid port exclude
  const HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex,"( 17 : -1 )");
  mainTube->addIntersection(HR);
  return;
}

void
portItem::addFlangeCut(MonteCarlo::Object* obj) const
  /*!
    Adds the port exclude to the main tube
    \param mainTube :: object for main tube
  */
{
  ELog::RegMethod RegA("portItem","addFlangeCut");
  if (!outerFlag)
    {
      const HeadRule HR
	(ModelSupport::getComposite(SMap,buildIndex," ( 27 : -102 )"));
      obj->addIntersection(HR);
    }
  return;
}

void
portItem::intersectPair(Simulation& System,
			portItem& Outer) const
  /*!
    Intersect two port
    \param Simulation :: Simulation to use
    \param Outer :: setcond port to intersect
  */
{
  ELog::RegMethod RegA("portItem","intersectPair");

  const HeadRule mainComp(getFullRule(3).complement());
  Outer.insertComponent(System,"Wall",mainComp);
  const HeadRule outerComp(Outer.getFullRule(3).complement());
  const HeadRule outerWallComp(Outer.getFullRule(4).complement());

  this->insertComponent(System,"Wall",outerWallComp);
  this->insertComponent(System,"Void",outerComp);

  const HeadRule voidRadius(getFullRule(6).complement());
  const HeadRule outerVoidRadius(Outer.getFullRule(6).complement());
  const HeadRule wallComp(getFullRule(4).complement());
  if (outerFlag && Outer.outerFlag)  // both
    {
      this->insertComponent(System,"OutVoid",outerVoidRadius);
      Outer.insertComponent(System,"OutVoid",wallComp);
    }
  return;
}

void
portItem::intersectVoidPair(Simulation& System,
			    const portItem& Outer) const
  /*!
    Intersect two ports outer only
    \param Simulation :: Simulation to use
    \param Outer :: second port to intersect
  */
{
  ELog::RegMethod RegA("portItem","intersectVoidPair");

  if (CellMap::hasItem("OutVoid"))
    {
      const HeadRule outerComp(Outer.getFullRule("VoidRadius").complement());
      this->insertComponent(System,"OutVoid",outerComp);
    }

  return;
}

void
portItem::constructTrack(Simulation& System,
			 MonteCarlo::Object* insertObj,
			 const HeadRule& innerSurf,
			 const HeadRule& outerSurf)
  /*!
    Construct a track system
    \param System :: Simulation of model
    \param insertObj :: Object to insert port cut into
    \param innerSurf :: HeadRule to inner surf
    \param outerSurf :: HeadRule to outer surf
  */
{
  ELog::RegMethod RegA("portItem","constructTrack(HR,HR)");

  if (!statusFlag)
    {
      ELog::EM<<"Failed to set orientation in port:"<<keyName<<ELog::endCrit;
      return;
    }
  createSurfaces();

  constructObject(System,innerSurf,outerSurf);
  
  addPortCut(insertObj);
  
  createLinks();
  return;
}

void
portItem::constructAxis(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Construct the centre line components
    \param System :: Simulation to use
    \param FC :: Fixed comp
    \param sideIndex :: Link point
   */
{
  ELog::RegMethod RegA("portItem","constructAxis");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  setCentLine(FC,centreOffset,axisOffset);
  return;
}


void
portItem::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Build the system assuming outer cells have been added
    \param System :: Simulation to use
    \param FC :: Fixed comp
    \param sideIndex :: Link point
   */
{
  ELog::RegMethod RegA("portItem","createAll");

  constructAxis(System,FC,sideIndex);
  
  return;
}



}  // NAMESPACE constructSystem
