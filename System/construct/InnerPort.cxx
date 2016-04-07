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
  
  sealStep=Control.EvalDefVar<double>(keyName+"SealStep",0.0);
  sealThick=Control.EvalDefVar<double>(keyName+"SealThick",0.0);
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

  const double outerBolt(boltStep+boltRadius+10.0*Geometry::zeroTol);
  
  // Seal constructed about origin:
  double SD(0.0);
  if (sealStep>Geometry::zeroTol)
    {
      SD+=sealStep;
      ModelSupport::buildPlane(SMap,portIndex+13,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+14,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+15,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,portIndex+16,Origin+Z*(SD+height/2.0),Z);

      SD+=sealThick/2.0;
      ModelSupport::buildPlane(SMap,portIndex+21,Origin-Y*sealThick/2.0,Y);
      ModelSupport::buildPlane(SMap,portIndex+22,Origin+Y*sealThick/2.0,Y);
      ModelSupport::buildPlane(SMap,portIndex+23,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+24,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+25,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,portIndex+26,Origin+Z*(SD+height/2.0),Z);
    }
  // This can be active with/without a seal
  if (nBolt)
    {
      SD+=outerBolt;
      ModelSupport::buildPlane(SMap,portIndex+33,Origin-X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+34,Origin+X*(SD+width/2.0),X);
      ModelSupport::buildPlane(SMap,portIndex+35,Origin-Z*(SD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,portIndex+36,Origin+Z*(SD+height/2.0),Z);
    }
  return;
}
  
void
InnerPort::createObjects(Simulation& System,
			 const std::string& boundary)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("InnerPort","createObjects");

  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,portIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  if (sealStep>Geometry::zeroTol)
    {

      // Seal [4 segments]:
      Out=ModelSupport::getComposite(SMap,portIndex,"21 -22 23 -13 25 -26");
      System.addCell(MonteCarlo::Qhull(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,portIndex,"21 -22 14 -24 25 -26");
      System.addCell(MonteCarlo::Qhull(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,portIndex,"21 -22 13 -14 -15 25");
      System.addCell(MonteCarlo::Qhull(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,portIndex,"21 -22 13 -14 16 -26");
      System.addCell(MonteCarlo::Qhull(cellIndex++,sealMat,0.0,Out));
      addCell("Seal",cellIndex-1);

      // surrounding seal [front/back]
      Out=ModelSupport::getComposite(SMap,portIndex,
                                     "1 -21 23 -24 25 -26 (-13:14:-15:16) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);

      Out=ModelSupport::getComposite(SMap,portIndex,
                                     "22 -2 23 -24 25 -26 (-13:14:-15:16) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);

      // Inner seal Mat:
      
      Out=ModelSupport::getComposite(SMap,portIndex,
                                     "1 -2 13 -14 15 -16 (-3:4:-5:6)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      addCell("Main",cellIndex-1);
      
       // Metal surrounding seal
      const int boltIndex((nBolt) ? portIndex+30 : portIndex+20);
      Out=ModelSupport::getComposite(SMap,portIndex,boltIndex,
                                     " 1 -2 (-3M:4M:-5M:6M) " );
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+boundary));
      addCell("Outer",cellIndex-1);
    }
  else // No seal
    {
      const int boltIndex((nBolt) ? portIndex+30 : portIndex);
      Out=ModelSupport::getComposite(SMap,portIndex,boltIndex,
                                     " 1 -2 (-3M:4M:-5M:6M) " );
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+boundary));
      addCell("Outer",cellIndex-1);
    }

  // Main container:
  Out=ModelSupport::getComposite(SMap,portIndex," 1 -2 ");
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
  
  std::string Out,OutComp,OutSide;

  const double SD((sealStep>Geometry::zeroTol) ? 
    sealStep+sealThick : 0.0);

  const double VOffset(height/2.0+SD);
  const double HOffset(width/2.0+SD);
  const Geometry::Vec3D XStep(X*(width/static_cast<double>(nBolt+1)));
  const Geometry::Vec3D ZStep(Z*(height/static_cast<double>(nBolt+1)));

  int boltIndex(portIndex+500);
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
      cutH+=XStep;
      cutV+=ZStep;
      BI+=10;
    }
  Geometry::Vec3D boltC(Origin-X*HOffset-Z*VOffset);
  boltIndex=portIndex+1000;
  for(size_t sideI=0;sideI<4;sideI++)
    {
      const double xSign((sideI<2) ? -1.0 : 1.0);
      const double zSign((sideI % 3) ? -1.0 : 1.0);
      const double dirSign((sideI % 2) ? -1.0 : 1.0);
      
      Geometry::Vec3D boltC(Origin+X*(xSign*HOffset)+Z*(zSign*VOffset));
      const Geometry::Vec3D& ActiveStep
        ((sideI % 2)  ? ZStep*dirSign : XStep*dirSign);
      
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
  // Now creat objects:
  // simple a - b :
  BI=portIndex+1000;
  Out=ModelSupport::getComposite(SMap,portIndex,BI,"1 -2 -7M");
  OutComp=ModelSupport::getComposite(SMap,portIndex,BI," 1 -2 7M ");
  OutSide=ModelSupport::getComposite(SMap,portIndex," -23 33 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out));
  addCell("Bolt",cellIndex-1);

  int leftIndex(portIndex+35-1);
  int rightIndex(portIndex+36-1);
  Out=ModelSupport::getComposite(SMap,leftIndex,rightIndex," 1 -1M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+OutComp+OutSide));
  addCell("Main",cellIndex-1);

  
  return;

  
  std::list<int> nearPlane({35,33,36,34});
  std::list<int> farPlane({36,34,35,33});
  // CREATE Objects:
  boltIndex=portIndex+500;
  for(size_t xIndex=0;xIndex<1;xIndex++)
    for(size_t zIndex=0;zIndex<1;zIndex++)
      {
        const int signV((xIndex) ? 1 : -1);
        
        int BI(boltIndex);
        // Note the -3 so that 3N etc works
        int layerI(signV*(nearPlane.front()+portIndex-3));
        for(size_t i=1;i<nBolt;i++)
          {
            Out=ModelSupport::getComposite(SMap,portIndex,BI,"1 -2 -7M");
            System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out));
            addCell("Bolt",cellIndex-1);
            Out=ModelSupport::getComposite(SMap,portIndex,BI,layerI,
                                           " 1 -2 7M  3N -3M ");
            System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
            addCell("Main",cellIndex-1);

            layerI=signV*BI;
            BI+=10;
          }
        
        Out=ModelSupport::getComposite(SMap,portIndex,BI,"1 -2 -7M");
        System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out));
        addCell("Bolt",cellIndex-1);

        ELog::EM<<"Added bolt"<<ELog::endDiag;
        // Note the -3 so that 3N etc works
        const int farI(signV*(farPlane.front()+portIndex-3));
        ELog::EM<<"Added bolt FAR:"<<farI<<ELog::endDiag;
        Out=ModelSupport::getComposite(SMap,portIndex,BI,"1 -2 7M");
        Out+=ModelSupport::getComposite(SMap,layerI,farI," 3 -3M ");
        ELog::EM<<"Added bolt OUT == "<<Out<<ELog::endDiag;
        System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
        addCell("Main",cellIndex-1);

        nearPlane.pop_front();
        farPlane.pop_front();

        boltIndex+=500;
      }
  

  
  return;
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
InnerPort::generateInsert(Simulation& System)
  /*!
    Create the links for the object
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("InnerPort","generateInsert");

  /*
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
  */
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

  setLinkSurf(0,-SMap.realSurf(portIndex+1));
  setLinkSurf(1,SMap.realSurf(portIndex+2));
  setLinkSurf(2,-SMap.realSurf(portIndex+3));
  setLinkSurf(3,SMap.realSurf(portIndex+3));
  setLinkSurf(4,-SMap.realSurf(portIndex+5));
  setLinkSurf(5,SMap.realSurf(portIndex+6));
  
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
  createObjects(System,boundary);
  createBolts(System);
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
