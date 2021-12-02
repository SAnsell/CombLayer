/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/InnerPort.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "BaseModVisit.h"
#include "Vec3D.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "InnerPort.h"

namespace constructSystem
{

InnerPort::InnerPort(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

InnerPort::InnerPort(const InnerPort& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::ExternalCut(A),  
  width(A.width),height(A.height),length(A.length),
  nBolt(A.nBolt),boltStep(A.boltStep),
  boltRadius(A.boltRadius),boltMat(A.boltMat),
  sealStep(A.sealStep),sealThick(A.sealThick),
  sealMat(A.sealMat),window(A.window),windowMat(A.windowMat),
  mat(A.mat),activeCells(A.activeCells)
  /*!
    Copy constructor
    \param A :: InnerPort to copy
  */
{}

InnerPort&
InnerPort::operator=(const InnerPort& A)
  /*!
    Assignment operator
    \param A :: InnerPort to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      width=A.width;
      height=A.height;
      length=A.length;
      nBolt=A.nBolt;
      boltStep=A.boltStep;
      boltRadius=A.boltRadius;
      boltMat=A.boltMat;
      sealStep=A.sealStep;
      sealThick=A.sealThick;
      sealMat=A.sealMat;
      window=A.window;
      windowMat=A.windowMat;
      mat=A.mat;
      activeCells=A.activeCells;
    }
  return *this;
}

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

  window=Control.EvalVar<double>(keyName+"Window");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");

  nBolt=Control.EvalDefVar<size_t>(keyName+"NBolt",0);
  boltStep=Control.EvalDefVar<double>(keyName+"BoltStep",0.0);
  boltRadius=Control.EvalDefVar<double>(keyName+"BoltRadius",0.0);
  boltMat=ModelSupport::EvalDefMat(Control,keyName+"BoltMat",0);
  
  sealStep=Control.EvalDefVar<double>(keyName+"SealStep",0.0);
  sealThick=Control.EvalDefVar<double>(keyName+"SealThick",0.0);
  sealMat=ModelSupport::EvalDefMat(Control,keyName+"SealMat",0);
    
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}



void
InnerPort::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("InnerPort","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  const double outerBolt(boltStep+boltRadius+10.0*Geometry::zeroTol);

  if (windowMat)
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(window/2.0),Y);
      ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(window/2.0),Y);
    }
    
  // Seal constructed about origin:
  double SD(0.0);
  if (sealStep>Geometry::zeroTol)
    {
      SD+=sealStep;
      ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(SD+height/2.0),Z);

      SD+=sealThick/2.0;
      ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*sealThick/2.0,Y);
      ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*sealThick/2.0,Y);
      ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(SD+height/2.0),Z);
    }
  // This can be active with/without a seal
  if (nBolt)
    {
      SD+=outerBolt;
      ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*(SD+height/2.0),Z);
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
  
  std::string Out;
  const std::string boundaryStr=getRuleStr("Boundary");

  if (windowMat)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 3 -4 5 -6");
      System.addCell(MonteCarlo::Object(cellIndex++,windowMat,0.0,Out));
      addCell("Window",cellIndex-1);
      Out=ModelSupport::getComposite(SMap,buildIndex,
                                     "1 -2 3 -4 5 -6 (-101 : 102)");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      addCell("Void",cellIndex-1);

    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      addCell("Void",cellIndex-1);
    }
  
  if (sealStep>Geometry::zeroTol)
    {

      // Seal [4 segments]:
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -22 23 -13 25 -26");
      System.addCell(MonteCarlo::Object(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -22 14 -24 25 -26");
      System.addCell(MonteCarlo::Object(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -22 13 -14 -15 25");
      System.addCell(MonteCarlo::Object(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -22 13 -14 16 -26");
      System.addCell(MonteCarlo::Object(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);

      // surrounding seal [front/back]
      Out=ModelSupport::getComposite(SMap,buildIndex,
                                     "1 -21 23 -24 25 -26 (-13:14:-15:16) ");
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);

      Out=ModelSupport::getComposite(SMap,buildIndex,
                                     "22 -2 23 -24 25 -26 (-13:14:-15:16) ");
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);

      // Inner seal Mat:
      
      Out=ModelSupport::getComposite(SMap,buildIndex,
                                     "1 -2 13 -14 15 -16 (-3:4:-5:6)");
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);
      
       // Metal surrounding seal
      const int boltIndex((nBolt) ? buildIndex+30 : buildIndex+20);
      Out=ModelSupport::getComposite(SMap,buildIndex,boltIndex,
                                     " 1 -2 (-3M:4M:-5M:6M) " );
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,
                                       Out+boundaryStr));
      addCell("Outer",cellIndex-1);
    }
  else // No seal
    {
      const int boltIndex((nBolt) ? buildIndex+30 : buildIndex);
      Out=ModelSupport::getComposite(SMap,buildIndex,boltIndex,
                                     " 1 -2 (-3M:4M:-5M:6M) " );
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,
                                       Out+boundaryStr));
      addCell("Outer",cellIndex-1);
    }

  // Main container:
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 ");
  addOuterSurf(Out);    
    
  return;
}

void
InnerPort::createBolts(Simulation& System)
  /*!
    Create the bolts and the divided pattern in the cells
    \param System :: Simulation component
   */
{
  ELog::RegMethod RegA("InnerPort","createBolts");
  if (!nBolt) return;
  const std::string boundaryStr=getRuleStr("Boundary");
    
  std::string Out,OutComp,OutSide;

  const double NBDbl((nBolt>1) ? static_cast<double>(nBolt) : 1.0);
  // surf +2x
  const double SD((sealStep>Geometry::zeroTol) ?  
    sealStep+sealThick/2.0 : 0.0);
  // surf +3x
  const double outerBolt(boltStep+boltRadius+10.0*Geometry::zeroTol);
  
  const double VOffset(height/2.0+SD);
  const double HOffset(width/2.0+SD);
  const double VMid(height/2.0+SD+outerBolt/2.0);
  const double HMid(width/2.0+SD+outerBolt/2.0);
  const double VFull(height/2.0+SD+outerBolt);
  const double HFull(width/2.0+SD+outerBolt);
  
  const Geometry::Vec3D XStep(X*((2*SD+width)/NBDbl));
  const Geometry::Vec3D ZStep(Z*((2*SD+height)/NBDbl));

  int boltIndex(buildIndex+500);
  // only two sides of planes [vertical / horrizontal ]
  Geometry::Vec3D cutH=Origin-X*HOffset-Z*VOffset;
  Geometry::Vec3D cutV(cutH);
  int BI(boltIndex);
  for(size_t i=1;i<nBolt;i++)
    {
      cutH+=XStep;
      cutV+=ZStep;
      ModelSupport::buildPlane(SMap,BI+3,cutH,X);
      ModelSupport::buildPlane(SMap,BI+5,cutV,Z);
      BI+=10;
    }
  Geometry::Vec3D boltC;
  boltIndex=buildIndex+1000;
  for(size_t sideI=0;sideI<4;sideI++)
    {
      const double xSign((sideI<2) ? -1.0 : 1.0);
      const double zSign((sideI % 3) ? 1.0 : -1.0);

      boltC= (sideI % 2) ?
        Origin+X*(xSign*HOffset)+Z*(zSign*VMid) :
        Origin+X*(xSign*HMid)+Z*(zSign*VOffset);
      
      const Geometry::Vec3D& ActiveStep
        ((sideI % 2)  ? -XStep*xSign : -ZStep*xSign);
      BI=boltIndex;
      boltC+= ActiveStep/2.0;
      for(size_t i=0;i<nBolt;i++)
        {
          ModelSupport::buildCylinder(SMap,BI+7,boltC,Y,boltRadius);
          boltC+=ActiveStep;
          BI+=10;
        }
      boltIndex+=1000;
    }

  // Now create objects:
  const std::vector<int> cornerCut=
    {
      calcIntersect(Origin-X*HFull-Z*VFull),
      calcIntersect(Origin-X*HFull+Z*VFull),
      calcIntersect(Origin+X*HFull+Z*VFull),
      calcIntersect(Origin+X*HFull-Z*VFull)
    };

  const std::vector<std::string> surfSide
    ({"-23 33" , " 26 -36 "," 24 -34 ", " -25 35 "});
  const std::vector<int> leftSide({35,23,36,24});
  const std::vector<int> rightSide({36,24,35,23});
  const std::vector<int> planeSide({4,2,4,2});

  BI=buildIndex+1000;
  for(size_t sideI=0;sideI<4;sideI++)
    {
      OutSide=ModelSupport::getComposite(SMap,buildIndex,surfSide[sideI]);

      int leftIndex(buildIndex+leftSide[sideI]-1);
      int rightIndex;
      int PI((sideI>1) ?
             buildIndex+500+((static_cast<int>(nBolt)-2)*10) :
             buildIndex+500);   // plane index
      if (sideI>1) leftIndex*=-1;
      
      BI=buildIndex+1000*(static_cast<int>(sideI)+1);
      for(size_t i=0;i<nBolt;i++)
        {
          Out=ModelSupport::getComposite(SMap,buildIndex,BI,"1 -2 -7M");
          System.addCell(MonteCarlo::Object(cellIndex++,boltMat,0.0,Out));
          addCell("Bolt",cellIndex-1);
          
          rightIndex=(i!=nBolt-1) ?
            (PI+planeSide[sideI]) :
            buildIndex+rightSide[sideI]-1;
          if (sideI>1) rightIndex*=-1;

          Out=ModelSupport::getComposite(SMap,leftIndex,rightIndex," 1 -1M ");
          leftIndex=rightIndex;
          OutComp=ModelSupport::getComposite(SMap,buildIndex,BI," 1 -2 7M ");
          if (cornerCut[sideI])
            OutComp+= boundaryStr;
          
          System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,
                                           Out+OutComp+OutSide));
          addCell("Main",cellIndex-1);
          PI+= (sideI>1) ? -10 : 10;
          BI+=10;
        }
    }

  //  Out=ModelSupport::getComposite(SMap,leftIndex,rightIndex," 1 -1M ");  
  return;
}

bool
InnerPort::calcIntersect(const Geometry::Vec3D& Pt) const
  /*!
    Calculate if the Pt is within the boundary
    \param Pt :: Point to check
    \return true if outside the boundary
  */
{
  const HeadRule& boundaryHR=
    ExternalCut::getRule("Boundary");
  return (!boundaryHR.isValid(Pt));
}
  
void
InnerPort::addInnerCell(const int CN)
  /*!
    Add inner cell
    \param CN :: Cell Number
  */
{
  ELog::RegMethod RegA("InnerPort","addInnerCell");

  activeCells.insert(CN);
  return;
}
  
void
InnerPort::generateInsert()
  /*!
    Create the links for the object
  */
{
  ELog::RegMethod RegA("InnerPort","generateInsert");

  for(const int CN : activeCells)
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

  setConnect(0,Origin-Y*(length/2.0),-Y);
  setConnect(1,Origin+Y*(length/2.0),Y);
  setConnect(2,Origin-X*(width/2.0),-X);
  setConnect(3,Origin+X*(width/2.0),X);
  setConnect(4,Origin-Z*(height/2.0),-Z);
  setConnect(5,Origin+Z*(height/2.0),Z);

  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));
  setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  setLinkSurf(3,SMap.realSurf(buildIndex+3));
  setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  setLinkSurf(5,SMap.realSurf(buildIndex+6));
  
  return;
}

void
InnerPort::createAll(Simulation& System,
                     const attachSystem::FixedComp& beamFC,
                     const long int FIndex)
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
  generateInsert();       // Done here so that cells not invalid
  createSurfaces();
  
  createObjects(System);
  createBolts(System);
  createLinks();
  insertObjects(System);   


  return;
}
  
}  // NAMESPACE constructSystem
