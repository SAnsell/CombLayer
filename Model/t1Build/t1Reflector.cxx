/***************************************************************************** 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1Reflector.cxx
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
#include <numeric>
#include <memory>

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
#include "LinkWrapper.h"
#include "LWOuter.h"
#include "LWInner.h"
#include "ReflectRods.h"
#include "t1Reflector.h"

namespace ts1System
{

t1Reflector::t1Reflector(const std::string& Key)  :
  attachSystem::FixedRotate(Key,11),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

t1Reflector::t1Reflector(const t1Reflector& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  xSize(A.xSize),ySize(A.ySize),
  ySizeColdCut(A.ySizeColdCut),zSize(A.zSize),cutLen(A.cutLen),
  defMat(A.defMat),Boxes(A.Boxes),Rods(A.Rods),
  Plates(A.Plates),baseZCut(A.baseZCut)
  /*!
    Copy constructor
    \param A :: t1Reflector to copy
  */
{}

t1Reflector&
t1Reflector::operator=(const t1Reflector& A)
  /*!
    Assignment operator
    \param A :: t1Reflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      xSize=A.xSize;
      ySize=A.ySize;
      ySizeColdCut=A.ySizeColdCut;
      zSize=A.zSize;
      cutLen=A.cutLen;
      defMat=A.defMat;
      Boxes=A.Boxes;
      Rods=A.Rods;
      Plates=A.Plates;
      baseZCut=A.baseZCut;
    }
  return *this;
}

t1Reflector::~t1Reflector() 
 /*!
   Destructor
 */
{}

void
t1Reflector::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("t1Reflector","populate");

  FixedRotate::populate(Control);

  xSize=Control.EvalVar<double>(keyName+"XSize");
  ySize=Control.EvalVar<double>(keyName+"YSize");
  ySizeColdCut=Control.EvalVar<double>(keyName+"YSizeColdCut");  
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutLen=Control.EvalVar<double>(keyName+"CutLen");

  // Material
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  // Box information
  baseZCut=Control.EvalVar<double>(keyName+"BaseZCut");

  return;
}
  

void
t1Reflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("t1Reflector","createSurface");

  // Outer layer:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*ySize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*ySize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*xSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*xSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zSize/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zSize/2.0,Z);
  // Corners:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin
			   -X*xSize/2.0-Y*(ySize/2.0-cutLen),-X-Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin
			   -X*xSize/2.0+Y*(ySize/2.0-cutLen),-X+Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin
			   +X*xSize/2.0-Y*(ySize/2.0-cutLen),X-Y);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin
			   +X*xSize/2.0+Y*(ySize/2.0-cutLen),X+Y);

  // Cold Moderators- Different Thickness:
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(ySize/2.0-ySizeColdCut),Y);

  return;
}


void
t1Reflector::createObjects(Simulation&)
/*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("t1Reflector","createObjects");
  
  HeadRule HR;
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6 -11 -12 -13 -14");
  addOuterSurf(HR);

  return;
}

void
t1Reflector::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("t1Reflector","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+6));
  
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+22));

  FixedComp::setConnect(0,Origin-Y*ySize/2.0,-Y);
  FixedComp::setConnect(2,Origin-X*xSize/2.0,-X);
  FixedComp::setConnect(4,Origin+Y*ySize/2.0,Y);
  FixedComp::setConnect(6,Origin+X*xSize/2.0,X);

  FixedComp::setConnect(1,Origin-X*xSize/2.0-Y*(ySize/2.0-cutLen),-X-Y);
  FixedComp::setConnect(3,Origin-X*xSize/2.0+Y*(ySize/2.0-cutLen),-X+Y);
  FixedComp::setConnect(5,Origin+X*xSize/2.0+Y*(ySize/2.0-cutLen),X+Y);
  FixedComp::setConnect(7,Origin+X*xSize/2.0-Y*(ySize/2.0-cutLen),X-Y);

  FixedComp::setConnect(8,Origin-Z*zSize/2.0,-Z);
  FixedComp::setConnect(9,Origin+Z*zSize/2.0,Z);

  FixedComp::setConnect(10,Origin+Y*(ySize/2.0-ySizeColdCut),Y);
  return;
}

HeadRule
t1Reflector::getHeadRule(const std::string& surfList) const
  /*!
    Exposes local version of getHeadRule
    \param surfList :: surface list
    \return HEadRule 
  */
{
  return ModelSupport::getHeadRule(SMap,buildIndex,surfList);
}

void
t1Reflector::createBoxes(Simulation& System,
			 const std::string& TName)
  /*!
    Create the reflector boxes
    \param System :: Simulation
    \param TName :: Target name
   */
{
  ELog::RegMethod RegA("t1Reflector","createBoxes");

  const objectGroups& OGrp=System;
  // ---------------- LEFT BASE --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxLBase")));
  
  Boxes[0]->addSurface(*this,"-9 -1 -2 -3 -4 -5");  // base
  Boxes[0]->addSurface(Origin-Z*baseZCut,Geometry::Vec3D(0,0,-1));  // base
  Boxes[0]->addSurface(Origin,Geometry::Vec3D(-1,0,0));  // base
  Boxes[0]->createAll(System,*this,0);


  // ---------------- RIGHT BASE --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxRBase")));
  Boxes[1]->addSurface(*this,"-9 -1 -8 -7 -6 -5"); 
  Boxes[1]->addSurface(*Boxes[0],"-7 8"); 
  Boxes[1]->createAll(System,*this,0);

  // ---------------- WATER CORNER --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxWater")));

  Boxes[2]->addSurface(*this,"-1 -8 -7"); 
  Boxes[2]->addSurface(OGrp,"WatSouthFlight",4);  // base
  Boxes[2]->addSurface(OGrp,"WaterMod",4);         // base
  Boxes[2]->addSurface(OGrp,"WatNorthFlight",3);  // base
  if (TName=="PVessel")
    Boxes[2]->addSurface(OGrp,"PVessel",11);  // press top  

//  Boxes[2]->addSurface(Origin+Z*7.10,Geometry::Vec3D(0,0,1));  //
  Boxes[2]->addSurface(Origin+Z*7.975,Geometry::Vec3D(0,0,1));  //                
  Boxes[2]->addSurface(OGrp,"WatNorthFlight",-6);  // roof
  Boxes[2]->createAll(System,*this,0);

  // ---------------- Merlin CORNER --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxMerlin")));

  Boxes[3]->addSurface(*this,"-6 -5 -4 -3"); 
  Boxes[3]->addSurface(OGrp,"WatNorthFlight",4);  
  Boxes[3]->addSurface(OGrp,"WaterMod",3);  
  Boxes[3]->addSurface(OGrp,"WatSouthFlight",3); 
  Boxes[3]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,1));  //
  Boxes[3]->addSurface(OGrp,"MerlinMod",-6);  //  What is this for?

  Boxes[3]->addExcludeObj(System,"MerlinMod");
  Boxes[3]->addExcludeObj(System,"MerlinFlight","outer");
  Boxes[3]->addExcludeObj(System,TName);
  Boxes[3]->createAll(System,*this,0);

  // ---------------- Methane CORNER --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxMethane")));

  Boxes[4]->addSurface(*this,"-3 -2 -1"); 
  Boxes[4]->addSurface(OGrp,"CH4FlightS",4);  // base
  Boxes[4]->addSurface(OGrp,"CH4Mod",4);  // base
  Boxes[4]->addSurface(OGrp,"CH4FlightN",3);  // base
  Boxes[4]->addSurface(*Boxes[0],7);

  Boxes[4]->addSurface(Origin-Z*(zStep*2.0),Geometry::Vec3D(0,0,-1));
  Boxes[4]->addExcludeObj(System,TName);
  Boxes[4]->addExcludeObj(System,"ProtonVoid");    
  Boxes[4]->createAll(System,*this,0);
  
  // ---------------- LH2 CORNER --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxLH2")));

  Boxes[5]->addSurface(*this,"-4 -11 -6 -7");  // sides
  Boxes[5]->addSurface(OGrp,"CH4FlightS",3);  // base
  Boxes[5]->addSurface(OGrp,"CH4Mod",3);  // base
  Boxes[5]->addSurface(OGrp,"CH4FlightN",4);  // base
  Boxes[5]->addSurface(*Boxes[0],7);
  Boxes[5]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,-1));  // roof    

									  
  Boxes[5]->addExcludeObj(System,"H2Mod");
  Boxes[5]->addExcludeObj(System,"H2Flight","outer");
  Boxes[5]->addExcludeObj(System,TName);

  Boxes[5]->createAll(System,*this,0);

  // ---------------- Merlin Wrapper --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxMerlinWrapper")));

  Boxes[6]->addSurface(OGrp,"MerlinMod","-1 -2 -3 -4 -5 -6");

  Boxes[6]->addExcludeObj(Boxes[3]->getLinkSurf(-27));
  Boxes[6]->addExcludeObj(Boxes[3]->getLinkSurf(-28));

  Boxes[6]->maskSection(0);
  Boxes[6]->maskSection(4);
  Boxes[6]->maskSection(5);

  Boxes[6]->addInsertCell(Boxes[3]->centralCell());
  Boxes[6]->createAll(System,*this,0);

  // ---------------- LH2 Wrapper --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxLH2Wrapper")));

  Boxes[7]->addSurface(OGrp,"H2Mod","-1 -2 -3 -4");
  Boxes[7]->addSurface(OGrp,"H2Flight","-5 -6");

  Boxes[7]->addExcludeObj(Boxes[5]->getLinkSurf(-27));
  Boxes[7]->addExcludeObj(Boxes[5]->getLinkSurf(-28));
  Boxes[7]->addExcludeObj(Boxes[5]->getLinkSurf(-29));
  Boxes[7]->addExcludeObj(Boxes[5]->getLinkSurf(-26));
  
  Boxes[7]->maskSection(0);
  Boxes[7]->maskSection(4);
  Boxes[7]->maskSection(5);

  Boxes[7]->addInsertCell(Boxes[5]->centralCell());
  Boxes[7]->createAll(System,*this,0);


  // Flightline wrapper for MERLIN:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxMerlinPlate")));
  Boxes[8]->addSurface(*Boxes[3],27);
  Boxes[8]->addSurface(OGrp,"MerlinFlight","-3 -4 -5 -6");
  Boxes[8]->addSurface(OGrp,"MerlinMod",1);
  Boxes[8]->maskSection(0);
  Boxes[8]->maskSection(2);
  Boxes[8]->maskSection(4);
  Boxes[8]->maskSection(5); 
  Boxes[8]->addInsertCell(Boxes[3]->centralCell());
  Boxes[8]->createAll(System,*this,0);
  // Flightline wrapper for LH2:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxLH2Plate")));
  Boxes[9]->addSurface(*Boxes[5],27);
  Boxes[9]->addSurface(OGrp,"H2Flight","-3 -4 -5 -6");
//  Boxes[9]->addSurface(OGrp,"H2Mod",-4);
  Boxes[9]->addSurface(OGrp,"H2Mod",1);    
  Boxes[9]->addExcludeObj(Boxes[5]->getLinkSurf(-21));
  Boxes[9]->maskSection(0);
  Boxes[9]->maskSection(1);
  Boxes[9]->maskSection(5); 
  Boxes[9]->addInsertCell(Boxes[5]->centralCell());
  Boxes[9]->createAll(System,*this,0);


  // ---------------- Top CORNER Hexagon --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxTopHex")));


  Boxes[10]->addSurface(*this,"-3 -2 -1 -8");
  Boxes[10]->addSurface(OGrp,"WaterMod",-2);  // masked  
  Boxes[10]->addSurface(OGrp,"WaterMod",-3);  // 
  Boxes[10]->addSurface(OGrp,"WatNorthFlight",-4);  // 
  Boxes[10]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,1));  
  Boxes[10]->addSurface(OGrp,"WaterMod",-6);  

  Boxes[10]->addExcludeObj(System,TName);
  Boxes[10]->addExcludeObj(System,"ProtonVoid");                     
  Boxes[10]->addExcludeObj(System,"WaterMod");
  Boxes[10]->addExcludeObj(Boxes[2]->getOuterSurf());
  Boxes[10]->addExcludeObj(System,"WatNorthFlight","outer");
  Boxes[10]->addExcludeObj(System,"WatSouthFlight","outer");
  Boxes[10]->maskSection(4);
  Boxes[10]->createAll(System,*this,0);

  // ---------------- Top CORNER Pentagon --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxTopPen")));

  Boxes[11]->addSurface(*this,"-8 -7 -6");
  Boxes[11]->addSurface(OGrp,"WaterMod",2);  // base
  Boxes[11]->addSurface(OGrp,"WatSouthFlight",-3);  // base
  Boxes[11]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,1));  //        
  Boxes[11]->addSurface(OGrp,"WaterMod",-6);  //     
    
  Boxes[11]->addExcludeObj(System,TName); 
  Boxes[11]->addExcludeObj(System,"ProtonVoid");             
  Boxes[11]->addExcludeObj(System,"WaterMod");
  Boxes[11]->addExcludeObj(Boxes[2]->getOuterSurf());
  Boxes[11]->addExcludeObj(System,"WatNorthFlight","outer");
  Boxes[11]->addExcludeObj(System,"WatSouthFlight","outer");

  Boxes[11]->maskSection(3);
  Boxes[11]->createAll(System,*this,0);

  
  // ---------------- Bottom Hexagon --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxBotHex")));

  Boxes[12]->addSurface(*this,"-7 -8 -1");
//  Boxes[12]->addSurface(*this,"6,7,10");  
  Boxes[12]->addSurface(OGrp,"CH4FlightS",-4);  // base
  Boxes[12]->addSurface(OGrp,"CH4Mod",2);  // base  
  Boxes[12]->addSurface(OGrp,"CH4FlightS",-3);  // base
  Boxes[12]->addSurface(*Boxes[0],7);  
  Boxes[12]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,-1));  //          

  Boxes[12]->addExcludeObj(System,TName);
  Boxes[12]->addExcludeObj(System,"ProtonVoid");
  Boxes[12]->addExcludeObj(System,"CH4FlightS","outer");

  Boxes[12]->maskSection(3);
  Boxes[12]->maskSection(4);
  Boxes[12]->maskSection(5); 
  Boxes[12]->createAll(System,*this,0);  
  // ---------------- Bottom Pentagon --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxBotPen")));

  Boxes[13]->addSurface(*this,"-4 -3");
  Boxes[13]->addSurface(OGrp,"CH4FlightN",-4);  // base
  Boxes[13]->addSurface(OGrp,"CH4Mod",1);  // base  
  Boxes[13]->addSurface(OGrp,"CH4FlightN",-3);  // base
  Boxes[13]->addSurface(*Boxes[0],7);  
  Boxes[13]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,-1));  //     
  Boxes[13]->addExcludeObj(System,TName);
  Boxes[13]->addExcludeObj(System,"ProtonVoid");
  Boxes[13]->addExcludeObj(System,"CH4FlightN","outer");
  Boxes[13]->addExcludeObj(System,"H2Mod");

//  Boxes[13]->maskSection(2);
  Boxes[13]->maskSection(3);
//  Boxes[13]->maskSection(4);
  Boxes[13]->createAll(System,*this,0);    

  // ---------------- Bottom Quad --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxBotQuad")));

  Boxes[14]->addSurface(OGrp,"CH4Mod","-1 -2 -3 -4");  
  Boxes[14]->addSurface(*Boxes[0],7);  
  Boxes[14]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,-1));  // 

  Boxes[14]->addExcludeObj(System,TName);
  Boxes[14]->addExcludeObj(System,"ProtonVoid");
  Boxes[14]->addExcludeObj(System,"CH4Mod");
  Boxes[14]->maskSection("0 1 2 3");
  Boxes[14]->createAll(System,*this,0);    

  // ---------------- Top --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxTop")));

  Boxes[15]->addSurface(*this,"-6 -5 -4 -3 -2 -1 -8 -7"); 
  Boxes[15]->addSurface(OGrp,"MerlinMod",6);  //               
  Boxes[15]->addSurface(*this,-10);  // roof
  Boxes[15]->createAll(System,*this,0);

  // ---------------- LH2 Void --------------------------------
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWInner("RBoxLH2Void")));

  Boxes[16]->addSurface(*this,"-4 -5 -6 11");  // sides
  Boxes[16]->addSurface(*Boxes[0],7);
  Boxes[16]->addSurface(Origin-Z*zStep*2.0,Geometry::Vec3D(0,0,-1));  // roof
  Boxes[16]->addExcludeObj(System,TName);

  Boxes[16]->createAll(System,*this,0);
  

  // Flightline wrapper for CH4 South:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxCH4South")));
  Boxes[17]->addSurface(OGrp,"CH4FlightS","-3 -4 -5 -6");
  Boxes[17]->addSurface(OGrp,"CH4Mod",2);
  Boxes[17]->addSurface(*Boxes[1],22); 
  Boxes[17]->addSurface(*Boxes[1],23);  
  Boxes[17]->addSurface(*Boxes[1],24); 
  Boxes[17]->maskSection(0);
  Boxes[17]->maskSection(1);  
  Boxes[17]->maskSection(4);
  Boxes[17]->maskSection(5);
  Boxes[17]->maskSection(6);
  Boxes[17]->maskSection(7);    
  Boxes[17]->addInsertCell(Boxes[12]->centralCell());
  Boxes[17]->addInsertCell(Boxes[13]->centralCell());

  Boxes[17]->createAll(System,*this,0);


  // Flightline wrapper for Water North:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxWatNorth")));
  Boxes[18]->addSurface(OGrp,"WatNorthFlight","-3");
  Boxes[18]->addSurface(*Boxes[10],26);   
  Boxes[18]->addSurface(OGrp,"WatNorthFlight","-5 -6");  
  Boxes[18]->addSurface(OGrp,"WaterMod",1);
  Boxes[18]->addSurface(*Boxes[0],22); 
  Boxes[18]->addSurface(*Boxes[0],23);  
  Boxes[18]->addSurface(*Boxes[0],24); 
  Boxes[18]->maskSection(0);
  Boxes[18]->maskSection(1);  
//  Boxes[18]->maskSection(2);
  Boxes[18]->maskSection(3);
  Boxes[18]->maskSection(4);
  Boxes[18]->maskSection(5);
  Boxes[18]->maskSection(6);
  Boxes[18]->maskSection(7);    
  Boxes[18]->addInsertCell(Boxes[10]->centralCell());
  Boxes[18]->addInsertCell(Boxes[11]->centralCell());

  Boxes[18]->createAll(System,*this,0);

  // Flightline wrapper for WaterSouth:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxWatSouth")));
  Boxes[19]->addSurface(OGrp,"WatSouthFlight","-4");
  Boxes[19]->addSurface(*Boxes[11],25);      
  Boxes[19]->addSurface(OGrp,"WatSouthFlight","-5 -6");   // can be combined?
  Boxes[19]->addSurface(OGrp,"WaterMod",2);
  Boxes[19]->addSurface(*Boxes[11],22); 
  Boxes[19]->addSurface(*Boxes[11],23);  
  Boxes[19]->maskSection(0);
  Boxes[19]->maskSection(1);  
//  Boxes[19]->maskSection(2);
  Boxes[19]->maskSection(3);
  Boxes[19]->maskSection(4);
  Boxes[19]->maskSection(5);
  Boxes[19]->maskSection(6);
  Boxes[19]->addInsertCell(Boxes[10]->centralCell());
  Boxes[19]->addInsertCell(Boxes[11]->centralCell());

  Boxes[19]->createAll(System,*this,0);

  // Flightline wrapper for CH4 North:
  Boxes.push_back
    (std::shared_ptr<constructSystem::LinkWrapper>
     (new constructSystem::LWOuter("RBoxCH4North")));
  Boxes[20]->addSurface(*Boxes[13],23);
  Boxes[20]->addSurface(*Boxes[13],25);  
  Boxes[20]->addSurface(OGrp,"CH4FlightN","-5 -6");
  Boxes[20]->addSurface(OGrp,"CH4Mod",1);
  Boxes[20]->addSurface(*Boxes[0],24); 

  Boxes[20]->addExcludeObj(System,"H2Mod");
  Boxes[20]->maskSection(0);
  Boxes[20]->maskSection(1);  
  Boxes[20]->maskSection(4);
  Boxes[20]->maskSection(5);
  
  Boxes[20]->addInsertCell(Boxes[12]->centralCell());
  Boxes[20]->addInsertCell(Boxes[13]->centralCell());

  Boxes[20]->createAll(System,*this,0);
 
  return;
}

void
t1Reflector::createRods(Simulation& System)
  /*!
    Add the rods as done AFTER pipework
    \param System :: Simulation fo raddin rods
  */
{
  ELog::RegMethod RegA("t1Rflector","createRods");

  //  const std::vector<size_t> active={0,1,2,4,5,15};
  //  const std::vector<size_t> active={5,6};
  const std::vector<size_t> active={0,1,2,4,5,15};
  for(const size_t i : active) 
    {
      // REFLECTOR RODS:
      Rods.push_back
	(std::shared_ptr<ReflectRods>(new ReflectRods("Rods",i)));
      Rods.back()->setObject(System.findObject(Boxes[i]->centralCell()));
      Rods.back()->createAll(System,*this,0);   // 0 not used
    }
  return;
}
		     
void
t1Reflector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("t1Reflector","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}
  
}  // NAMESPACE ts1System
