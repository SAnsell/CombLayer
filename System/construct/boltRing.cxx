/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/boltRing.cxx
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
#include "Quaternion.h"
#include "surfRegister.h"
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "boltRing.h"

namespace constructSystem
{

boltRing::boltRing(const std::string& BKey,
		   const std::string& PKey) :
  attachSystem::FixedRotate(BKey+PKey,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  baseName(BKey),
  populated(0),innerExclude(0),
  NBolts(0),innerRadius(1.0),sealRadius(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param BKey :: Base KeyName
    \param PKey :: primary name
  */
{}

boltRing::boltRing(const boltRing& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  baseName(A.baseName),
  populated(A.populated),innerExclude(A.innerExclude),
  NBolts(A.NBolts),boltRadius(A.boltRadius),innerRadius(A.innerRadius),
  outerRadius(A.outerRadius),thick(A.thick),angOffset(A.angOffset),
  sealRadius(A.sealRadius),sealThick(A.sealThick),
  sealDepth(A.sealDepth),boltMat(A.boltMat),mainMat(A.mainMat),
  sealMat(A.sealMat)
  /*!
    Copy constructor
    \param A :: boltRing to copy
  */
{}

boltRing&
boltRing::operator=(const boltRing& A)
  /*!
    Assignment operator
    \param A :: boltRing to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      populated=A.populated;
      innerExclude=A.innerExclude;
      NBolts=A.NBolts;
      boltRadius=A.boltRadius;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      thick=A.thick;
      angOffset=A.angOffset;
      sealRadius=A.sealRadius;
      sealThick=A.sealThick;
      sealDepth=A.sealDepth;
      boltMat=A.boltMat;
      mainMat=A.mainMat;
      sealMat=A.sealMat;
    }
  return *this;
}

boltRing::~boltRing() 
  /*!
    Destructor
  */
{}


void
boltRing::setDimensions(const size_t NB,const double IR,const double OR,
			const double TK,const double BR,const double AO)
  /*!
    Set the demensions of the system
    \param NB :: Number of bolts
    \param IR :: Inner radius
    \param OR :: Outer radius
    \param TK :: Thickness
    \param BR :: Bolt-Radius
    \param AO :: Angle offset
   */
{
  ELog::RegMethod RegA("boltRing","setDimensions");

  NBolts=NB;

  if (OR-IR<Geometry::zeroTol || OR<Geometry::zeroTol ||
      IR<Geometry::zeroTol)
    throw ColErr::OrderError<double>(IR,OR,"Inner Radius >= Outer Radius");
  if (TK<Geometry::zeroTol)
    throw ColErr::SizeError<double>(TK,0.0,"Thickness");

  innerRadius=IR;
  outerRadius=OR;
  thick=TK;
  
  if (NBolts && (BR<Geometry::zeroTol || BR>(OR-IR)/2.0))
    throw ColErr::SizeError<double>(BR,0.0,"Bolt radius");
  
  boltRadius=BR;
  angOffset=AO;

  populated |= 1;
  return;
}

void
boltRing::setMaterials(const std::string& boltM,const std::string& mainM)
  /*!
    Set the materials
    \param boltM :: bolt material
    \param mainM :: Main material
   */
{
  ELog::RegMethod RegA("boltRing","setMaterials");
  
  boltMat=ModelSupport::EvalMatName(boltM);
  mainMat=ModelSupport::EvalMatName(mainM);
  ELog::EM<<"Port Mat incorrect here " <<ELog::endWarn;
  mainMat++;
  populated |= 2;
  return;
}
  
void
boltRing::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("boltRing","populate");

  FixedRotate::populate(Control);
  if (!(populated & 1))
    {
      const size_t NB=
	Control.EvalDefTail<size_t>(keyName,baseName,"NBolts",0);
      const double BR=
	Control.EvalDefTail<double>(keyName,baseName,"BoltRadius",0.0);
      const double IR=Control.EvalTail<double>(keyName,baseName,"InnerRadius");
      const double OR=Control.EvalTail<double>(keyName,baseName,"OuterRadius");

      double TK(1.0);  // default value to ignore if active/front/back
      if (!backActive() || !frontActive())
	TK=Control.EvalTail<double>(keyName,baseName,"Thickness");
      
      const double AO=
	Control.EvalDefTail<double>(keyName,baseName,"AngleOffset",0.0);

      setDimensions(NB,IR,OR,TK,BR,AO);
    }
  if (!(populated & 2))
    {
      boltMat=ModelSupport::EvalDefMat(Control,keyName+"BoltMat",
					    baseName+"BoltMat",0);
      mainMat=ModelSupport::EvalMat<int>(Control,
					 keyName+"MainMat",
					 baseName+"MainMat");
      populated |= 2;
    }
  if (!(populated & 4))
    {
      sealRadius=Control.EvalDefTail<double>
	(keyName,baseName,"SealRadius",0.0);
      sealThick=Control.EvalDefTail<double>
	(keyName,baseName,"SealThick",0.2);
      sealDepth=Control.EvalDefTail<double>
	(keyName,baseName,"SealDepth",sealThick);
      sealMat=ModelSupport::EvalDefMat(Control,
					    keyName+"SealMat",
					    baseName+"SealMat",0);
      populated |= 4;
    }

  return;
}

void
boltRing::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("boltRing","createSurfaces");
   // Construct surfaces:

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  addSurf("innerRing",SMap.realSurf(buildIndex+7));

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,outerRadius);
  addSurf("outerRing",SMap.realSurf(buildIndex+17));
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(thick/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(thick/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+2));      
    }

  // BOLTS:
  if (NBolts>0)
    {
      const double BRadius=(innerRadius+outerRadius)/2.0;
      const double angleR=360.0/static_cast<double>(NBolts);
      Geometry::Vec3D DPAxis(X);
      Geometry::Vec3D BAxis(Z*BRadius);
      const Geometry::Quaternion QStartSeg=
        Geometry::Quaternion::calcQRotDeg(angOffset,Y);
      const Geometry::Quaternion QHalfSeg=
        Geometry::Quaternion::calcQRotDeg(angleR/2.0,Y);
      const Geometry::Quaternion QSeg=
        Geometry::Quaternion::calcQRotDeg(angleR,Y);
      
      // half a segment rotation to start:
      QStartSeg.rotate(DPAxis);
      QStartSeg.rotate(BAxis);
      QHalfSeg.rotate(DPAxis);
      
      int boltIndex(buildIndex+100);
      for(size_t i=0;i<NBolts;i++)
        {
          const Geometry::Vec3D boltC(Origin+BAxis);          
          ModelSupport::buildCylinder(SMap,boltIndex+7,boltC,Y,boltRadius);
	  
          ModelSupport::buildPlane(SMap,boltIndex+3,Origin,DPAxis);
          QSeg.rotate(DPAxis);
          QSeg.rotate(BAxis);
          boltIndex+=10;
        }
    }
	  
  if (sealRadius>innerRadius && sealRadius>Geometry::zeroTol)
    {
      
      const Geometry::Vec3D FPoint=FrontBackCut::frontInterPoint(Origin,Y);
      const Geometry::Vec3D BPoint=FrontBackCut::backInterPoint(Origin,Y);
      const Geometry::Vec3D MidPt((FPoint+BPoint)/2.0);
      const Geometry::Vec3D MidAxis((BPoint-FPoint).unit());

      ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,sealRadius);
      ModelSupport::buildCylinder(SMap,buildIndex+1017,Origin,Y,
				  sealRadius+sealThick);      
      ModelSupport::buildPlane(SMap,buildIndex+1001,
			       MidPt-MidAxis*sealDepth,MidAxis);
      ModelSupport::buildPlane(SMap,buildIndex+1002,
			       MidPt+MidAxis*sealDepth,MidAxis);
    }
    
  
  return;
}
  
void
boltRing::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("boltRing","createObjects");

  HeadRule HR;

  const bool sealFlag(sealRadius>innerRadius);

  HeadRule sealUnit;
  if (sealFlag)
    sealUnit=ModelSupport::getHeadRule
      (SMap,buildIndex,"1001 -1002 1007 -1017");
  
  int boltIndex(buildIndex+100);
  int prevBoltIndex(boltIndex+static_cast<int>(10*NBolts)-10);
  const HeadRule EdgeHR=
    ModelSupport::getHeadRule(SMap,buildIndex," 7 -17 ");
  const HeadRule frontBackHR=getFrontRule()*getBackRule();

  for(size_t i=0;i<NBolts;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,boltIndex,"-7");
      makeCell("Bolts",System,cellIndex++,boltMat,0.0,HR*frontBackHR);
      
      HR=ModelSupport::getHeadRule
	(SMap,prevBoltIndex,boltIndex," 3 -3M 7M ");
          
      makeCell("Ring",System,cellIndex++,mainMat,0.0,
	       HR*frontBackHR+EdgeHR*sealUnit.complement());
      
      if (sealFlag)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,prevBoltIndex,boltIndex,"3 -3M");
	  makeCell("Seal",System,cellIndex++,sealMat,0.0,HR*sealUnit);
	}
      prevBoltIndex=boltIndex;
      boltIndex+=10;
    }

  if (!NBolts)
    {
      makeCell("Ring",System,cellIndex++,mainMat,0.0,
	       frontBackHR*EdgeHR*sealUnit.complement());
      if (sealFlag)
	makeCell("Seal",System,cellIndex++,sealMat,0.0,sealUnit);
    }

  if (innerExclude)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 ");      
      addOuterSurf(HR*frontBackHR);
    }
  else
    addOuterSurf(EdgeHR*frontBackHR);
  
  return;
}



void
boltRing::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("boltRing","createLinks");

  
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  
  return;
}

void
boltRing::createAll(Simulation& System,
		    const attachSystem::FixedComp& mainFC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param mainFC :: FixedComp at the beam centre
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("boltRing","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(mainFC,sideIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
