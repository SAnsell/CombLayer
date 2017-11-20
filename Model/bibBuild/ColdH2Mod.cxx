/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/ColdH2Mod.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "ColdH2Mod.h"


namespace bibSystem
{

ColdH2Mod::ColdH2Mod(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  coldIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(coldIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


ColdH2Mod::ColdH2Mod(const ColdH2Mod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  coldIndex(A.coldIndex),cellIndex(A.cellIndex),
  width(A.width),
  height(A.height),depth(A.depth),wallThick(A.wallThick),
  sideGap(A.sideGap),frontGap(A.frontGap),backGap(A.backGap),
  vertGap(A.vertGap),modMat(A.modMat),waterMat(A.waterMat),
  wallMat(A.wallMat),premThick(A.premThick),wallPremThick(A.wallPremThick),
  premGap(A.premGap),modTemp(A.modTemp),waterTemp(A.waterTemp)
  /*!
    Copy constructor
    \param A :: ColdH2Mod to copy
  */
{}

ColdH2Mod&
ColdH2Mod::operator=(const ColdH2Mod& A)
  /*!
    Assignment operator
    \param A :: ColdH2Mod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      sideGap=A.sideGap;
      frontGap=A.frontGap;
      backGap=A.backGap;
      vertGap=A.vertGap;
      modMat=A.modMat;
      waterMat=A.waterMat;
      wallMat=A.wallMat;
      premThick=A.premThick;
      wallPremThick=A.wallPremThick;
      premGap=A.premGap;
      modTemp=A.modTemp;
      waterTemp=A.waterTemp;
    }
  return *this;
}

ColdH2Mod::~ColdH2Mod()
  /*!
    Destructor
  */
{}

void
ColdH2Mod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ColdH2Mod","populate");

  FixedOffset::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  sideGap=Control.EvalVar<double>(keyName+"SideGap");
  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  backGap=Control.EvalVar<double>(keyName+"BackGap");
  vertGap=Control.EvalVar<double>(keyName+"VertGap");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");  
    
  premThick=Control.EvalVar<double>(keyName+"PremThick");
  wallPremThick=Control.EvalVar<double>(keyName+"WallPremThick");
  premGap=Control.EvalVar<double>(keyName+"PremGap");

  waterTemp=Control.EvalVar<double>(keyName+"WaterTemp");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  return;
}


void
ColdH2Mod::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ColdH2Mod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
ColdH2Mod::createSurfaces(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create planes for the silicon and Polyethene layers
    \param FC :: FixedComponent for front surface
    \param sideIndex :: Index on front surface
  */
{
  ELog::RegMethod RegA("ColdH2Mod","createSurfaces");

  ModelSupport::buildPlane(SMap,coldIndex+1,Origin-Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,coldIndex+2,Origin+Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,coldIndex+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,coldIndex+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,coldIndex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,coldIndex+6,Origin+Z*height/2.0,Z);  

  ModelSupport::buildPlane(SMap,coldIndex+11,Origin-Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+12,Origin+Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+13,Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,coldIndex+14,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,coldIndex+15,Origin-Z*(height/2.0+wallThick),Z); 
  ModelSupport::buildPlane(SMap,coldIndex+16,Origin+Z*(height/2.0+wallThick),Z); 

  // This can be done in a loop but left for extension later.

  double D(wallThick+premGap);
  ModelSupport::buildPlane(SMap,coldIndex+201,
			   Origin-Y*(depth/2.0+D),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+202,
			   Origin+Y*(depth/2.0+D),Y);
  ModelSupport::buildPlane(SMap,coldIndex+203,
			   Origin-X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+204,
			   Origin+X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+205,
			   Origin-Z*(height/2.0+D),Z);  
  ModelSupport::buildPlane(SMap,coldIndex+206,
			   Origin+Z*(height/2.0+D),Z);  

  D+=wallPremThick;
  ModelSupport::buildPlane(SMap,coldIndex+211,
			   Origin-Y*(depth/2.0+D),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+213,
			   Origin-X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+214,
			   Origin+X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+215,
			   Origin-Z*(height/2.0+D),Z);  
  ModelSupport::buildPlane(SMap,coldIndex+216,
			   Origin+Z*(height/2.0+D),Z);  

  D+=premThick;
  ModelSupport::buildPlane(SMap,coldIndex+221,
			   Origin-Y*(depth/2.0+D),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+223,
			   Origin-X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+224,
			   Origin+X*(width/2.0+D),X);  
  ModelSupport::buildPlane(SMap,coldIndex+225,
			   Origin-Z*(height/2.0+D),Z);  
  ModelSupport::buildPlane(SMap,coldIndex+226,
			   Origin+Z*(height/2.0+D),Z);  
  
  D+=wallPremThick;
  ModelSupport::buildPlane(SMap,coldIndex+231,
			   Origin-Y*(depth/2.0+D),Y);
  ModelSupport::buildPlane(SMap,coldIndex+233,
			   Origin-X*(width/2.0+D),X);
  ModelSupport::buildPlane(SMap,coldIndex+234,
			   Origin+X*(width/2.0+D),X);
  ModelSupport::buildPlane(SMap,coldIndex+235,
			   Origin-Z*(height/2.0+D),Z);
  ModelSupport::buildPlane(SMap,coldIndex+236,
			   Origin+Z*(height/2.0+D),Z); 
  

  SMap.addMatch(coldIndex+21,FC.getLinkSurf(sideIndex)); // all links point out
  ModelSupport::buildPlane(SMap,coldIndex+22,Origin+
			   Y*(depth/2.0+wallThick+backGap),Y);  
  ModelSupport::buildPlane(SMap,coldIndex+23,Origin-
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,coldIndex+24,Origin+
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,coldIndex+25,Origin-
			   Z*(height/2.0+wallThick+vertGap),Z); 
  ModelSupport::buildPlane(SMap,coldIndex+26,Origin+
			   Z*(height/2.0+wallThick+vertGap),Z); 

  return; 
}

void
ColdH2Mod::createObjects(Simulation& System)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("ColdH2Mod","createObjects");
  
  std::string Out;

  // Water
  Out=ModelSupport::getComposite(SMap,coldIndex,"1 -2 3 -4 5 -6");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Wall of water moderator
  Out=ModelSupport::getComposite(SMap,coldIndex,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));

  // Gap between mod and premod
  Out=ModelSupport::getComposite(SMap,coldIndex,
				 "(-11:12:-13:14:-15:16) -202 201 203 -204 205 -206 ");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  
  // Water premoderator
  Out=ModelSupport::getComposite(SMap,coldIndex,
				 "(-211:-213:214:-215:216) -202 221 223 -224 225 -226 ");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));
  
  // Wall 1 of Water premoderator
  Out=ModelSupport::getComposite(SMap,coldIndex,
				 "(-201:-203:204:-205:206) -202 211 213 -214 215 -216 ");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,waterTemp,Out));
  
  // Wall 2 of Water premoderator
  Out=ModelSupport::getComposite(SMap,coldIndex,
				 "(-221:-223:224:-225:226) -202 231 233 -234 235 -236 ");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,waterTemp,Out));

  // Box
  Out=ModelSupport::getComposite(SMap,coldIndex,
	     	 "21 -22 23 -24 25 -26 (-231:202:234:-233:-235:236)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,coldIndex,"21 -22 23 -24 25 -26" );
  addOuterSurf(Out);

  return; 
}

void
ColdH2Mod::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("ColdH2Mod","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin-Y*(depth/2.0+wallThick+frontGap),-Y);  
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+wallThick+backGap),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick+sideGap),-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick+sideGap),X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick+vertGap),-Z);  
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick+vertGap),Z);  

  FixedComp::setLinkSurf(0,-SMap.realSurf(coldIndex+21));
  FixedComp::setLinkSurf(1,SMap.realSurf(coldIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(coldIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(coldIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(coldIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(coldIndex+26));
  
  return;
}

void
ColdH2Mod::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const size_t orgIndex,
		     const size_t sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param orgIndex :: Origin point
    \param sideIndex :: Side index
   */
{
  ELog::RegMethod RegA("ColdH2Mod","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,orgIndex+1);
  createSurfaces(FC,sideIndex+1);
  createObjects(System);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
