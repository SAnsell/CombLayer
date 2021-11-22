/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Jaws.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "mergeTemplate.h"

#include "Jaws.h"

namespace constructSystem
{

Jaws::Jaws(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Jaws::Jaws(const Jaws& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  zOpen(A.zOpen),
  zThick(A.zThick),zCross(A.zCross),zLen(A.zLen),
  xOpen(A.xOpen),xThick(A.xThick),xCross(A.xCross),
  xLen(A.xLen),jawGap(A.jawGap),XHeight(A.XHeight),
  YHeight(A.YHeight),ZHeight(A.ZHeight),wallThick(A.wallThick),
  zJawMat(A.zJawMat),xJawMat(A.xJawMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Jaws to copy
  */
{}

Jaws&
Jaws::operator=(const Jaws& A)
  /*!
    Assignment operator
    \param A :: Jaws to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      zOpen=A.zOpen;
      zThick=A.zThick;
      zCross=A.zCross;
      zLen=A.zLen;
      xOpen=A.xOpen;
      xThick=A.xThick;
      xCross=A.xCross;
      xLen=A.xLen;
      jawGap=A.jawGap;
      XHeight=A.XHeight;
      YHeight=A.YHeight;
      ZHeight=A.ZHeight;
      linerThick=A.linerThick;
      wallThick=A.wallThick;
      zJawMat=A.zJawMat;
      xJawMat=A.xJawMat;
      linerMat=A.linerMat;
      wallMat=A.wallMat;
    }
  return *this;
}


Jaws::~Jaws() 
  /*!
    Destructor
  */
{}


void
Jaws::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Jaws","populate");

  FixedRotate::populate(Control);
  
  zOpen=Control.EvalVar<double>(keyName+"ZOpen");
  zThick=Control.EvalVar<double>(keyName+"ZThick");
  zCross=Control.EvalVar<double>(keyName+"ZCross");
  zLen=Control.EvalVar<double>(keyName+"ZLen");

  xOpen=Control.EvalVar<double>(keyName+"XOpen");
  xThick=Control.EvalVar<double>(keyName+"XThick");
  xCross=Control.EvalVar<double>(keyName+"XCross");
  xLen=Control.EvalVar<double>(keyName+"XLen");

  jawGap=Control.EvalVar<double>(keyName+"Gap");

  XHeight=Control.EvalVar<double>(keyName+"XHeight");
  YHeight=Control.EvalVar<double>(keyName+"YHeight");
  ZHeight=Control.EvalVar<double>(keyName+"ZHeight");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  linerThick=Control.EvalDefVar<double>(keyName+"LinerThick",0.0);

  zJawMat=ModelSupport::EvalMat<int>(Control,keyName+"zJawMat");
  xJawMat=ModelSupport::EvalMat<int>(Control,keyName+"xJawMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  linerMat=ModelSupport::EvalDefMat(Control,keyName+"LinerMat",0);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",0);
  if (nLayers)
    {
      ModelSupport::populateDivide(Control,nLayers,keyName+"Mat",
				   xJawMat,jawXMatVec);
      ModelSupport::populateDivide(Control,nLayers,keyName+"Mat",
				   zJawMat,jawZMatVec);
      ModelSupport::populateDivideLen(Control,nLayers,keyName+"Len",
				      xLen,jawXFrac);
      ModelSupport::populateDivideLen(Control,nLayers,keyName+"Len",
				      zLen,jawZFrac);
    }
  
  return;
}

void
Jaws::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Jaws","createSurfaces");

  // Primary outside layers:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(YHeight/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(YHeight/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(ZHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(ZHeight/2.0),Z);

  // Walls 
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wallThick+XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wallThick+ZHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wallThick+ZHeight/2.0),Z);

  // X JAW : [+ve Y side]
  
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(xThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+153,Origin-X*(xLen+xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+154,Origin+X*(xLen+xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(xCross/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(xCross/2.0),Z);

  // Z JAW : [-ve Y Side]
  
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*(zThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin-Y*(jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(zCross/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(zCross/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+255,Origin-Z*(zLen+zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+256,Origin+Z*(zLen+zOpen/2.0),Z);

  // Liner::
  ModelSupport::buildPlane(SMap,buildIndex+1103,
			   Origin-X*(xOpen/2.0-linerThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+1104,
			   Origin+X*(xOpen/2.0-linerThick),X);
  
  ModelSupport::buildPlane(SMap,buildIndex+1205,
			   Origin-Z*(zOpen/2.0-linerThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1206,
			   Origin+Z*(zOpen/2.0-linerThick),Z);

  
  return;
}

void
Jaws::createObjects(Simulation& System)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Jaws","createObjects");

  HeadRule CutRule;
  std::string Out;
  
  // X Jaw A:
  if (linerThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 101 -102 103 -1103 105 -106 ");
      System.addCell(MonteCarlo::Object(cellIndex++,linerMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 101 -102 -104 1104 105 -106 ");
      System.addCell(MonteCarlo::Object(cellIndex++,linerMat,0.0,Out));
    }
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -103 153 105 -106 ");
  System.addCell(MonteCarlo::Object(cellIndex++,xJawMat,0.0,Out));
  addCell("xJaw",cellIndex-1);
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 1103 -1104 105 -106");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 104 -154 105 -106 ");
  System.addCell(MonteCarlo::Object(cellIndex++,xJawMat,0.0,Out));
  addCell("xJaw",cellIndex-1);

  
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 153 -154 105 -106 ");
  CutRule.procString(Out);

  // Z Jaw A:
  if (linerThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 201 -202 205 -1205 203 -204 ");
      System.addCell(MonteCarlo::Object(cellIndex++,linerMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 201 -202 -206 1206 203 -204 ");
      System.addCell(MonteCarlo::Object(cellIndex++,linerMat,0.0,Out));
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 -205 255 203 -204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,zJawMat,0.0,Out));
  addCell("zJaw",cellIndex-1);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 1205 -1206 203 -204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 206 -256 203 -204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,zJawMat,0.0,Out));
  addCell("zJaw",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -204 255 -256 ");
  CutRule.addUnion(Out);
  CutRule.makeComplement();

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  Out+=getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+CutRule.display()));

  if (wallThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 13 -14 15 -16 (-3 : 4 : -5: 6)");
      System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
    }

  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 15 -16 ");
  addOuterSurf(Out);      

  return;
}

void
Jaws::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Jaws","createLinks");

  FixedComp::setConnect(0,Origin-Y*(YHeight/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(YHeight/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(wallThick+XHeight/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
 
  FixedComp::setConnect(3,Origin+X*(wallThick+XHeight/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,Origin-Z*(wallThick+ZHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(wallThick+ZHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void
Jaws::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    Currently two layer fields
      -- X
      -- Z
    \param System :: Simulation to work on
  */

{
  ELog::RegMethod RegA("Jaws","LayerProcess");
  
  if (nLayers>1)
    {
      // X LAYER:
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(jawXFrac[i-1]);
	  DA.addMaterial(jawXMatVec[i-1]);
	}
      DA.addMaterial(jawXMatVec.back());

	  
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      

      surroundRule.setSurfPair(SMap.realSurf(buildIndex+101),
			       SMap.realSurf(buildIndex+102));
      
      OutA=ModelSupport::getComposite(SMap,buildIndex," 101 ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," -102 ");
      
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      
      DA.addRule(&surroundRule);
      
      for(size_t jawNumber=0;jawNumber<2;jawNumber++)
	{
	  const std::string cellName("xJaw");
	  const int cellNumber(getCell(cellName,jawNumber));
	  DA.setCellN(cellNumber);
	  DA.setOutNum(cellIndex,buildIndex+1001+
		       static_cast<int>(jawNumber)*100);
	  DA.activeDivideTemplate(System);
	  cellIndex=DA.getCellNum();
	  // remove from cellMap:
	  removeCell(cellName,jawNumber);   
	  setCells(cellName,cellNumber,cellIndex-1);
	}
    }

  // Z LAYER:
  if (nLayers>1)
    {

      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(jawZFrac[i-1]);
	  DA.addMaterial(jawZMatVec[i-1]);
	}
      DA.addMaterial(jawZMatVec.back());

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      

      surroundRule.setSurfPair(SMap.realSurf(buildIndex+201),
			       SMap.realSurf(buildIndex+202));
      
      OutA=ModelSupport::getComposite(SMap,buildIndex," 201 ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," -202 ");
      
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      
      DA.addRule(&surroundRule);
      
      for(size_t jawNumber=0;jawNumber<2;jawNumber++)
	{
	  const std::string cellName("zJaw");
	  const int cellNumber(getCell(cellName,jawNumber));
	  DA.setCellN(cellNumber);
	  DA.setOutNum(cellIndex,buildIndex+2001+
		       static_cast<int>(jawNumber)*100);
	  DA.activeDivideTemplate(System);
	  cellIndex=DA.getCellNum();
	  // remove from cellMap:
	  removeCell(cellName,jawNumber);   
	  setCells(cellName,cellNumber,cellIndex-1);
	}
    }
  return;
}
  
void
Jaws::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,
		const long int FIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Jaws","createAll(FC)");
  
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  layerProcess(System);
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
