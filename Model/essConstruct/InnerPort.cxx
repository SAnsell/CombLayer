/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essConstruct/InnerPort.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "InnerPort.h"

namespace essConstruct
{

InnerPort::InnerPort(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

InnerPort::InnerPort(const InnerPort& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
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
      attachSystem::FixedRotate::operator=(A);
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

  FixedRotate::populate(Control);

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

  HeadRule HR; 

  const HeadRule boundaryHR=getRule("Boundary");

  if (windowMat)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 3 -4 5 -6");
      makeCell("Window",System,cellIndex++,windowMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
                                     "1 -2 3 -4 5 -6 (-101 : 102)");
      makeCell("Void",System,cellIndex++,0,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
      makeCell("Void",System,cellIndex++,0,0.0,HR);
    }
  
  if (sealStep>Geometry::zeroTol)
    {

      // Seal [4 segments]:
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 23 -13 25 -26");
      makeCell("Seal",System,cellIndex++,sealMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 14 -24 25 -26");
      makeCell("Seal",System,cellIndex++,sealMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 13 -14 -15 25");
      makeCell("Seal",System,cellIndex++,sealMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 13 -14 16 -26");
      makeCell("Seal",System,cellIndex++,sealMat,0.0,HR);

      // surrounding seal [front/back]
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
                                     "1 -21 23 -24 25 -26 (-13:14:-15:16)");
      makeCell("Main",System,cellIndex++,mat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,
                                     "22 -2 23 -24 25 -26 (-13:14:-15:16) ");
      makeCell("Main",System,cellIndex++,mat,0.0,HR);

      // Inner seal Mat:
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
                                     "1 -2 13 -14 15 -16 (-3:4:-5:6)");
      makeCell("Main",System,cellIndex++,mat,0.0,HR);
      
       // Metal surrounding seal
      const int boltIndex((nBolt) ? buildIndex+30 : buildIndex+20);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,boltIndex,
                                     "1 -2 (-3M:4M:-5M:6M)");
      makeCell("Outer",System,cellIndex++,mat,0.0,HR*boundaryHR);
    }
  else // No seal
    {
      const int boltIndex((nBolt) ? buildIndex+30 : buildIndex);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,boltIndex,
                                     " 1 -2 (-3M:4M:-5M:6M) " );
      makeCell("Outer",System,cellIndex++,mat,0.0,HR*boundaryHR);
    }

  // Main container:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");
  addOuterSurf(HR);    
    
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

  HeadRule HR;
  const HeadRule boundaryHR=getRule("Boundary");
    
  HeadRule HRComp,HRSide;

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

  std::vector<int> boltCut;  // are bolts cut by boundaryHR
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
      const Geometry::Vec3D boltTest =
	boltC+X*(xSign*boltRadius)+Z*(zSign*boltRadius);
      boltCut.push_back(boundaryHR.isValid(boltTest));
			
      for(size_t i=0;i<nBolt;i++)
        {
          ModelSupport::buildCylinder(SMap,BI+7,boltC,Y,boltRadius);
          boltC+=ActiveStep;
          BI+=10;
        }
      boltIndex+=1000;
    }

  // Now create objects:
  // Check which cells will need boundary cut
  const std::vector<int> cornerCut=
    {
      boundaryHR.isValid(Origin-X*HFull-Z*VFull),
      boundaryHR.isValid(Origin-X*HFull+Z*VFull),
      boundaryHR.isValid(Origin+X*HFull+Z*VFull),
      boundaryHR.isValid(Origin+X*HFull-Z*VFull)
    };
  
  const std::vector<std::string> surfSide
    ({"-23 33" , "26 -36","24 -34", "-25 35"});
  const std::vector<int> leftSide({35,23,36,24});
  const std::vector<int> rightSide({36,24,35,23});
  const std::vector<int> planeSide({4,2,4,2});

  BI=buildIndex+1000;
  for(size_t sideI=0;sideI<4;sideI++)
    {
      HRSide=ModelSupport::getHeadRule(SMap,buildIndex,surfSide[sideI]);

      int leftIndex(buildIndex+leftSide[sideI]-1);
      int rightIndex;
      int PI((sideI>1) ?
             buildIndex+500+((static_cast<int>(nBolt)-2)*10) :
             buildIndex+500);   // plane index
      if (sideI>1) leftIndex*=-1;
      
      BI=buildIndex+1000*(static_cast<int>(sideI)+1);
      for(size_t i=0;i<nBolt;i++)
        {
          HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"1 -2 -7M");
	  if ((i==0 || i+1==nBolt) && !boltCut[sideI])
	    HR*=boundaryHR;	    
          makeCell("Bolt",System,cellIndex++,boltMat,0.0,HR);
          
          rightIndex=(i!=nBolt-1) ?
            (PI+planeSide[sideI]) :
            buildIndex+rightSide[sideI]-1;
          if (sideI>1) rightIndex*=-1;

          HR=ModelSupport::getHeadRule(SMap,leftIndex,rightIndex,"1 -1M");
          leftIndex=rightIndex;
          HRComp=ModelSupport::getHeadRule(SMap,buildIndex,BI,"1 -2 7M");

	  
	  if (!cornerCut[sideI])
	    HRComp *= boundaryHR;
          
          makeCell("Main",System,cellIndex++,mat,0.0,HR*HRComp*HRSide);
          PI+= (sideI>1) ? -10 : 10;
          BI+=10;
        }
    }

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
