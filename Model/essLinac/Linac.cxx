/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Linac.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "FixedRotate.h"
#include "surfDBase.h"
#include "surfDivide.h"
#include "mergeTemplate.h"

#include "AttachSupport.h"

#include "CellMap.h"
#include "BeamDump.h"
#include "FaradayCup.h"
#include "Linac.h"

namespace essSystem
{

Linac::Linac(const std::string& Key)  :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  beamDump(new BeamDump(Key,"BeamDump")),
  faradayCup(new FaradayCup(Key+"FaradayCup"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ELog::RegMethod RegA("Linac","Linac(const std::string&)");
  
  ModelSupport::objectRegister& OR =
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(beamDump);
  OR.addObject(faradayCup);
}

Linac::Linac(const Linac& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  engActive(A.engActive),
  length(A.length),widthLeft(A.widthLeft),
  widthRight(A.widthRight),
  height(A.height),
  depth(A.depth),
  wallThick(A.wallThick),
  roofThick(A.roofThick),
  floorThick(A.floorThick),
  floorWidthLeft(A.floorWidthLeft),
  floorWidthRight(A.floorWidthRight),
  nAirLayers(A.nAirLayers),
  airMat(A.airMat),wallMat(A.wallMat),
  tswLength(A.tswLength),
  tswWidth(A.tswWidth),
  tswGap(A.tswGap),
  tswOffsetY(A.tswOffsetY),
  tswNLayers(A.tswNLayers),
  beamDump(new BeamDump(*A.beamDump)),
  faradayCup(new FaradayCup(*A.faradayCup))
  /*!
    Copy constructor
    \param A :: Linac to copy
  */
{}

Linac&
Linac::operator=(const Linac& A)
  /*!
    Assignment operator
    \param A :: Linac to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      engActive=A.engActive;
      length=A.length;
      widthLeft=A.widthLeft;
      widthRight=A.widthRight;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      floorWidthLeft=A.floorWidthLeft;
      floorWidthRight=A.floorWidthRight;
      nAirLayers=A.nAirLayers;
      airMat=A.airMat;
      wallMat=A.wallMat;
      tswLength=A.tswLength;
      tswWidth=A.tswWidth;
      tswGap=A.tswGap;
      tswOffsetY=A.tswOffsetY;
      tswNLayers=A.tswNLayers;
      *beamDump=*A.beamDump;
      *faradayCup=*A.faradayCup;
    }
  return *this;
}

Linac::~Linac()
  /*!
    Destructor
  */
{}

void
Linac::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Linac","populate");

  FixedRotate::populate(Control);

  // This is to be replaces with -tEng IParam
  engActive=Control.EvalTail<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  widthLeft=Control.EvalVar<double>(keyName+"WidthLeft");
  widthRight=Control.EvalVar<double>(keyName+"WidthRight");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  floorWidthLeft=Control.EvalVar<double>(keyName+"FloorWidthLeft");
  floorWidthRight=Control.EvalVar<double>(keyName+"FloorWidthRight");
  nAirLayers=Control.EvalDefVar<size_t>(keyName+"NAirLayers", 1);

  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  tswLength=Control.EvalVar<double>(keyName+"TSWLength");
  tswWidth=Control.EvalVar<double>(keyName+"TSWWidth");
  tswGap=Control.EvalVar<double>(keyName+"TSWGap");
  tswOffsetY=Control.EvalVar<double>(keyName+"TSWOffsetY");
  tswNLayers=Control.EvalDefVar<size_t>(keyName+"TSWNLayers", 1);

  return;
}

void
Linac::layerProcess(Simulation& System, const std::string& cellName,
		    const long int linkPrimSurf,const long int linkSndSurf,
		    const size_t nLayers, const int mat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: TSW wall cell name
    \param linkPrimSurf :: link pont of primary surface
    \param linkSndSurf :: link point of secondary surface
    \param nLayers :: number of layers to divide to
    \param mat :: material
  */
{
  ELog::RegMethod RegA("Linac","layerProcess");
  if (nLayers>1 && linkPrimSurf && linkSndSurf)
    {
      const int pS = getLinkSurf(linkPrimSurf);
      const int sS = getLinkSurf(linkSndSurf);
      
      const int wallCell=getCell(cellName);
      
      double baseFrac = 1.0/static_cast<double>(nLayers);
      ModelSupport::surfDivide DA;
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(baseFrac);
	  DA.addMaterial(mat);
	  baseFrac += 1.0/static_cast<double>(nLayers);
	}
      DA.addMaterial(mat);
      
      DA.setCellN(wallCell);
      DA.setOutNum(cellIndex, buildIndex+10000);
      
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      
      surroundRule.setSurfPair(SMap.realSurf(pS),
			       SMap.realSurf(sS));
      
      const HeadRule HRA = getFullRule(linkPrimSurf);
      const HeadRule HRB = getFullRule(-linkSndSurf);
      
      surroundRule.setInnerRule(HRA);
      surroundRule.setOuterRule(HRB);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      
      cellIndex=DA.getCellNum();
    }
  return;
}


void
Linac::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Linac","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(widthRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(widthLeft),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length/2.0+wallThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(widthRight+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(widthLeft+wallThick),X);
  // floor
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(floorWidthRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(floorWidthLeft),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);

  // Temporary shielding walls
  double tswY(tswOffsetY);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(tswY),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(widthRight-tswLength),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(widthLeft-tswLength),X);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(tswY),Y);
  tswY += tswGap;
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin+Y*(tswY),Y);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,buildIndex+112,Origin+Y*(tswY),Y);

  return;
}

void
Linac::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Linac","createObjects");

  HeadRule HR;
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -101 3 -4 5 -6");
  makeCell("airBefore",System,cellIndex++,airMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -111 3 -4 5 -6");
  makeCell("midAir",System,cellIndex++,airMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"112 -2 3 -4 5 -6");
  makeCell("airAfter",System,cellIndex++,airMat,0.0,HR);


  // side walls and roof
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"11 -12 13 -14 5 -16 (-1:2:-3:4:6)");
  makeCell("Walls",System,cellIndex++,wallMat,0.0,HR);
  // wall bottom slab
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 23 -24 15 -5");
  makeCell("Base",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 23 -13 5 -16");
  makeCell("Roof",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 14 -24 5 -16");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  // temporary shielding walls
  // 1st wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 3 -103 5 -6");
  makeCell("tsw1",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -4 5 -6");
  makeCell("tsw1Gap",System,cellIndex++,airMat,0.0,HR);

  // 2nd wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"111 -112 3 -104 5 -6");
  makeCell("tsw2Gap",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"111 -112 104 -4 5 -6");
  makeCell("tsw2",System,cellIndex++,wallMat,0.0,HR);


  // divide TSW walls
  layerProcess(System,"tsw1", 7, 8, tswNLayers, wallMat);
  layerProcess(System,"tsw2", 9, 10, tswNLayers, wallMat);

  // divide air before TSW
  layerProcess(System,"airBefore", 11, 7, nAirLayers, airMat);
  layerProcess(System,"airAfter", 10, 12, nAirLayers, airMat);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 23 -24 15 -16");
  addOuterSurf(HR);

  return; 
}


void
Linac::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Linac","createLinks");

  // outer links
  FixedComp::setConnect(0,Origin-Y*(length/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(1,Origin+Y*(length/2.0+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(2,Origin-X*(widthRight+wallThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,Origin+X*(widthLeft+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,Origin-Z*(depth+floorThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height+roofThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  // TSW
  double tswY(tswOffsetY);
  FixedComp::setConnect(6,Origin+Y*(tswY),Y); //should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+101));
  tswY += tswWidth;
  FixedComp::setConnect(7,Origin+Y*(tswY),Y);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+102));
  tswY += tswGap;
  FixedComp::setConnect(8,Origin+Y*(tswY),Y); //should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+111));
  tswY += tswWidth;
  FixedComp::setConnect(9,Origin+Y*(tswY),Y);
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+112));

  // walls
  FixedComp::setConnect(10,Origin-Y*(length/2.0),Y);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(11,Origin+Y*(length/2.0),Y); // should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(11,SMap.realSurf(buildIndex+2));

  return;
}




void
Linac::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Linac","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);

  beamDump->createAll(System,*this,0);
  attachSystem::addToInsertLineCtrl(System,*this,*beamDump);

  faradayCup->createAll(System,*this,0);
  attachSystem::addToInsertForced(System,*this,*faradayCup);

  return;
}

}  // essSystem essSystem
