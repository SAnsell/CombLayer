/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/InnerPort.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "InnerPort.h"

namespace constructSystem
{

InnerPort::InnerPort(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  portIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(portIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}



InnerPort::~InnerPort() 
  /*!
    Destructor
  */
{}

void
InnerPort::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("InnerPort","populate");

  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  
  nBolt=Control.EvalDefVar<size_t>(keyName+"NBolt",0);
  boltStep=Control.EvalDefVar<double>(keyName+"BoltStep",0.0);
  boltRadius=Control.EvalDefVar<double>(keyName+"BoltRadius",0.0);
  boltMat=ModelSupport::EvalDefMat<int>(Control,keyName+"BoltMat",0);
  
  sealStep=Control.EvalDefVar<double>(keyName+"Step",0.0);
  sealThick=Control.EvalDefVar<double>(keyName+"Thick",0.0);
  sealMat=ModelSupport::EvalDefMat<int>(Control,keyName+"SealMat",0);
    
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
InnerPort::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("InnerPort","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
InnerPort::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("InnerPort","createSurfaces");
  
  ModelSupport::buildPlane(SMap,portIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,portIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,portIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,portIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,portIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,portIndex+6,Origin+Z*(height/2.0),Z);
    
  if (sealStep>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,portIndex+11,
                               Origin-Y*(sealStep+length/2.0),Y);
      ModelSupport::buildPlane(SMap,portIndex+12,Origin+Y*(sealStep+length/2.0),Y);
      ModelSupport::buildPlane(SMap,portIndex+13,Origin-X*(sealStep+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+14,Origin+X*(sealStep+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+15,Origin-Z*(sealStep+height/2.0),Z);
      ModelSupport::buildPlane(SMap,portIndex+16,Origin+Z*(sealStep+height/2.0),Z);
  
      const double angleR=360.0/static_cast<double>(NSection);
      const Geometry::Quaternion QHalfSeg=
        Geometry::Quaternion::calcQRotDeg(angleR/2.0,Y);
      const Geometry::Quaternion QSeg=
        Geometry::Quaternion::calcQRotDeg(angleR,Y);
      Geometry::Vec3D DPAxis(X);
      QHalfSeg.rotate(DPAxis);
      int DI(portIndex);
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
InnerPort::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("InnerPort","createObjects");

  
  std::string Out,SealStr;

  SealStr=ModelSupport::getComposite(SMap,portIndex," 1 -2 7 -17");
  if (NSection>1)
    {
      // start from segment:1 and do seg:0 at end 
      int prevRingIndex(portIndex);
      for(size_t i=1;i<NSection-1;i++)
        {
          Out=ModelSupport::getComposite(SMap,prevRingIndex," 3 -13 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+SealStr));
          addCell("Ring",cellIndex-1);
          prevRingIndex+=10;
        }

      Out=ModelSupport::getComposite(SMap,prevRingIndex,portIndex,
                                     " 3 -3M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+SealStr));
      addCell("Ring",cellIndex-1);
    }
  else  // one ring
    {
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,SealStr));
      addCell("Ring",cellIndex-1);
    }
  addOuterSurf(SealStr);    
  return;
}

void
InnerPort::generateInsert(Simulation& System)
  /*!
    Create the links for the object
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("InnerPort","generateInsert");
  
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
InnerPort::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("InnerPort","createLinks");

  setConnect(0,Origin-Y*(thick/2.0),-Y);
  setConnect(1,Origin+Y*(thick/2.0),Y);
  setConnect(2,Origin-X*(radius+deltaRad/2.0),-X);
  setConnect(3,Origin+X*(radius+deltaRad/2.0),X);
  setConnect(4,Origin-Z*(radius+deltaRad/2.0),-Z);
  setConnect(5,Origin+Z*(radius+deltaRad/2.0),Z);

  setLinkSurf(0,-SMap.realSurf(portIndex+1));
  setLinkSurf(1,SMap.realSurf(portIndex+2));
  setLinkSurf(2,SMap.realSurf(portIndex+17));
  setLinkSurf(3,SMap.realSurf(portIndex+17));
  setLinkSurf(4,SMap.realSurf(portIndex+17));
  setLinkSurf(5,SMap.realSurf(portIndex+17));
  
  return;
}

void
InnerPort::createAll(Simulation& System,
                     const attachSystem::FixedComp& beamFC,
                     const long int FIndex,
                     const std::string& boundary)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
    \param boundary :: Boundary string
  */
{
  ELog::RegMethod RegA("InnerPort","createAll");

  System.populateCells();
  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  generateInsert(System);       // Done here so that cells not invalid
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
