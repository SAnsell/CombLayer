/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/RingSeal.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "RingSeal.h"

namespace constructSystem
{

RingSeal::RingSeal(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  standardInsert(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

RingSeal::RingSeal(const RingSeal& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  NSection(A.NSection),NTrack(A.NTrack),radius(A.radius),
  deltaRad(A.deltaRad),thick(A.thick),mat(A.mat),
  standardInsert(A.standardInsert)
  /*!
    Copy constructor
    \param A :: RingSeal to copy
  */
{}

RingSeal&
RingSeal::operator=(const RingSeal& A)
  /*!
    Assignment operator
    \param A :: RingSeal to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      NSection=A.NSection;
      NTrack=A.NTrack;
      radius=A.radius;
      deltaRad=A.deltaRad;
      thick=A.thick;
      mat=A.mat;
      standardInsert=A.standardInsert;
    }
  return *this;
}

RingSeal::~RingSeal() 
  /*!
    Destructor
  */
{}

void
RingSeal::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("RingSeal","populate");

  FixedRotate::populate(Control);

  NSection=Control.EvalVar<size_t>(keyName+"NSection");
  NTrack=Control.EvalDefVar<size_t>(keyName+"NTrack",NSection);
  radius=Control.EvalVar<double>(keyName+"Radius");
  thick=Control.EvalVar<double>(keyName+"Thick");
  deltaRad=Control.EvalDefVar<double>(keyName+"DeltaRad",thick);
  deltaAngle=Control.EvalDefVar<double>(keyName+"DeltaAngle",0.0);
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  
  return;
}

void
RingSeal::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("RingSeal","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
RingSeal::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("RingSeal","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(thick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(thick/2.0),Y);

  if (!isActive("Inner"))
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
      setCutSurf("Inner",SMap.realSurf(buildIndex+7));
    }
  if (!isActive("Outer"))
    {
      ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+deltaRad);
      setCutSurf("Outer",-SMap.realSurf(buildIndex+17));
    }
    
  if (NSection>1)
    {
      const double angleR=360.0/static_cast<double>(NSection);
      const Geometry::Quaternion QStartSeg=
        Geometry::Quaternion::calcQRotDeg(deltaAngle+angleR/2.0,Y);
      const Geometry::Quaternion QSeg=
        Geometry::Quaternion::calcQRotDeg(angleR,Y);
      Geometry::Vec3D DPAxis(X);
      QStartSeg.rotate(DPAxis);
      int DI(buildIndex);
      for(size_t i=0;i<NSection;i++)
        {
          ModelSupport::buildPlane(SMap,DI+3,Origin,DPAxis);
          QSeg.rotate(DPAxis);
          DI+=10;
        }
    }
  return;
}
  
void
RingSeal::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RingSeal","createObjects");

  const HeadRule IOHR=getRule("Inner")*getRule("Outer");
  HeadRule HR;
  HeadRule SealHR;

  SealHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2")*IOHR;

  if (NSection>1)
    {
      // start from segment:1 and do seg:0 at end 
      int prevRingIndex(buildIndex);
      for(size_t i=1;i<NSection;i++)
        {
          HR=ModelSupport::getHeadRule(SMap,prevRingIndex,"3 -13");
          makeCell("Ring",System,cellIndex++,mat,0.0,HR*SealHR);
          prevRingIndex+=10;
        }

      HR=ModelSupport::getHeadRule
	(SMap,prevRingIndex,buildIndex,"3 -3M");
      makeCell("Ring",System,cellIndex++,mat,0.0,HR*SealHR);
    }
  else  // one ring
    {
      makeCell("Ring",System,cellIndex++,mat,0.0,SealHR);
    }
  addOuterSurf(SealHR);    
  return;
}

void
RingSeal::generateInsert(Simulation& System)
  /*!
    Create the links for the object
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("RingSeal","generateInsert");

  if (standardInsert) return;

  //System.populateCells();
  //  System.validateObjSurfMap();

  const size_t maxN(std::max<size_t>(NTrack,12));
  const double angleR=360.0/static_cast<double>(maxN);
  const Geometry::Quaternion QSeg=
    Geometry::Quaternion::calcQRotDeg(angleR,Y);

  MonteCarlo::Object* OPtr(0);
  std::set<int> cellActive;
  Geometry::Vec3D BAxis(Z*radius);
  for(size_t i=0;i<maxN;i++)
    {
      OPtr=System.findCell(BAxis+Origin,OPtr);
      if (!OPtr)
        throw ColErr::InContainerError<Geometry::Vec3D>
          (BAxis+Origin,"Cell not found");
      cellActive.insert(OPtr->getName());
      QSeg.rotate(BAxis);
    }


  for(const int CN : cellActive)
    addInsertCell(CN);

  return;
}  

void
RingSeal::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("RingSeal","createLinks");

  setConnect(0,Origin-Y*(thick/2.0),-Y);
  setConnect(1,Origin+Y*(thick/2.0),Y);
  setConnect(2,Origin-X*(radius+deltaRad/2.0),-X);
  setConnect(3,Origin+X*(radius+deltaRad/2.0),X);
  setConnect(4,Origin-Z*(radius+deltaRad/2.0),-Z);
  setConnect(5,Origin+Z*(radius+deltaRad/2.0),Z);

  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));
  setLinkSurf(2,SMap.realSurf(buildIndex+17));
  setLinkSurf(3,SMap.realSurf(buildIndex+17));
  setLinkSurf(4,SMap.realSurf(buildIndex+17));
  setLinkSurf(5,SMap.realSurf(buildIndex+17));
  
  return;
}
  
void
RingSeal::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("RingSeal","createAll(FC)");

  System.populateCells();
  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  generateInsert(System);           // Done here so that cells not invalid
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
