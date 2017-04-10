/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Linac.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "AttachSupport.h"

#include "CellMap.h"
#include "BeamDump.h"
#include "FaradayCup.h"
#include "DTL.h"
#include "Linac.h"

namespace essSystem
{

Linac::Linac(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,12), attachSystem::CellMap(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1),
  beamDump(new BeamDump(Key,"BeamDump")),
  faradayCup(new FaradayCup(Key,"FaradayCup"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ELog::RegMethod RegA("Linac","Linac(const std::string&)");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(beamDump);
  OR.addObject(faradayCup);
}

Linac::Linac(const Linac& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
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
  tswMat(A.tswMat),
  tswNLayers(A.tswNLayers),
  beamDump(new BeamDump(*A.beamDump)),
  faradayCup(new FaradayCup(*A.faradayCup)),
  nDTL(A.nDTL),
  dtl(A.dtl)
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
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
      tswMat=A.tswMat;
      tswNLayers=A.tswNLayers;
      *beamDump=*A.beamDump;
      *faradayCup=*A.faradayCup;
      nDTL=A.nDTL;
      dtl=A.dtl;
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

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

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
  nAirLayers=Control.EvalDefVar<int>(keyName+"NAirLayers", 1);

  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  tswLength=Control.EvalVar<double>(keyName+"TSWLength");
  tswWidth=Control.EvalVar<double>(keyName+"TSWWidth");
  tswGap=Control.EvalVar<double>(keyName+"TSWGap");
  tswOffsetY=Control.EvalVar<double>(keyName+"TSWOffsetY");
  tswMat=ModelSupport::EvalMat<int>(Control,keyName+"TSWMat");
  tswNLayers=Control.EvalDefVar<int>(keyName+"TSWNLayers", 1);

  nDTL=Control.EvalDefVar<size_t>(keyName+"NDTLTanks", 5);

  return;
}

void
Linac::createUnitVector(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: sideIndex
  */
{
  ELog::RegMethod RegA("Linac","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
Linac::layerProcess(Simulation& System, const std::string& cellName,
		    const size_t& lpS, const size_t& lsS,
		    const int& nLayers, const int& mat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: TSW wall cell name
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
    \param nLayers :: number of layers to divide to
    \param mat :: material
  */
  {
    ELog::RegMethod RegA("Linac","layerProcess");
    if (nLayers>1)
      {
	const int pS = getLinkSurf(lpS);
	const int sS = getLinkSurf(lsS);

	const attachSystem::CellMap* CM = dynamic_cast<const attachSystem::CellMap*>(this);
	MonteCarlo::Object* wallObj(0);
	int wallCell(0);

	if (CM)
	  {
	    wallCell=CM->getCell(cellName);
	    wallObj=System.findQhull(wallCell);
	  }

	if (!wallObj)
	  throw ColErr::InContainerError<int>(wallCell,
					      "Cell '" + cellName + "' not found");

	double baseFrac = 1.0/nLayers;
	ModelSupport::surfDivide DA;
	for(int i=1;i<nLayers;i++)
	  {
	    DA.addFrac(baseFrac);
	    DA.addMaterial(mat);
	    baseFrac += 1.0/nLayers;
	  }
	DA.addMaterial(mat);

	DA.setCellN(wallCell);
	DA.setOutNum(cellIndex, surfIndex+10000);

	ModelSupport::mergeTemplate<Geometry::Plane,
				    Geometry::Plane> surroundRule;

	surroundRule.setSurfPair(SMap.realSurf(pS),
				 SMap.realSurf(sS));

	std::string OutA = getLinkString(lpS);
	std::string OutB = getLinkComplement(lsS);

	surroundRule.setInnerRule(OutA);
	surroundRule.setOuterRule(OutB);

	DA.addRule(&surroundRule);
	DA.activeDivideTemplate(System);

	cellIndex=DA.getCellNum();
      }
  }


void
Linac::createDTL(Simulation& System, const long int lp)
{
  /*!
    Create the DTL tanks
    \param lp :: link point for the origin of the 1st DTL tank
   */
  ELog::RegMethod RegA("Linac","createDTL");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nDTL; i++)
    {
      std::shared_ptr<DTL> d(new DTL(keyName,"DTL",i+1));
      OR.addObject(d);
      if (i==0)
      	{
      	  ELog::EM << "Why +1?" << ELog::endDiag;
      	  d->createAll(System, *this, lp+1);
      	} else
      	{
      	  ELog::EM << "Why +1?" << ELog::endDiag;
      	  d->createAll(System, *dtl[i-1],1+1);
      	}

      attachSystem::addToInsertControl(System,*this,*d); // works
      dtl.push_back(d);
    }
  ELog::EM << "Remove substruction of last DTL from FaradayCup" << ELog::endDiag;
  attachSystem::addToInsertLineCtrl(System,*dtl.back(),*faradayCup);
}

void
Linac::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Linac","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(widthRight),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(widthLeft),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(height),Z);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(length/2.0+wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length/2.0+wallThick),Y);

  ModelSupport::buildPlane(SMap,surfIndex+13,Origin-X*(widthRight+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(widthLeft+wallThick),X);
  // floor
  ModelSupport::buildPlane(SMap,surfIndex+23,Origin-X*(floorWidthRight),X);
  ModelSupport::buildPlane(SMap,surfIndex+24,Origin+X*(floorWidthLeft),X);

  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,Origin+Z*(height+roofThick),Z);

  // Temporary shielding walls
  double tswY(tswOffsetY);
  ModelSupport::buildPlane(SMap,surfIndex+101,Origin+Y*(tswY),Y);
  ModelSupport::buildPlane(SMap,surfIndex+103,Origin-X*(widthRight-tswLength),X);
  ModelSupport::buildPlane(SMap,surfIndex+104,Origin+X*(widthLeft-tswLength),X);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,surfIndex+102,Origin+Y*(tswY),Y);
  tswY += tswGap;
  ModelSupport::buildPlane(SMap,surfIndex+111,Origin+Y*(tswY),Y);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,surfIndex+112,Origin+Y*(tswY),Y);

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

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -101 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  setCell("airBefore", cellIndex-1);
  Out=ModelSupport::getComposite(SMap,surfIndex," 102 -111 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 112 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  setCell("airAfter", cellIndex-1);

  // side walls and roof
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 11 -12 13 -14 5 -16 (-1:2:-3:4:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  // wall bottom slab
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -24 15 -5 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -13 5 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 14 -24 5 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // temporary shielding walls
  // 1st wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 101 -102 3 -103 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,tswMat,0.0,Out));
  setCell("tsw1", cellIndex-1);
  Out=ModelSupport::getComposite(SMap,surfIndex," 101 -102 103 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // 2nd wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 111 -112 3 -104 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 111 -112 104 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,tswMat,0.0,Out));
  setCell("tsw2", cellIndex-1);

  // divide TSW walls
  layerProcess(System, "tsw1", 6, 7, tswNLayers, wallMat);
  layerProcess(System, "tsw2", 8, 9, tswNLayers, wallMat);

  // divide air before TSW
  layerProcess(System, "airBefore", 10, 6, nAirLayers, airMat);
  layerProcess(System, "airAfter", 9, 11, nAirLayers, airMat);

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -24 15 -16 ");
  addOuterSurf(Out);

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
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+11));

  FixedComp::setConnect(1,Origin+Y*(length/2.0+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));

  FixedComp::setConnect(2,Origin-X*(widthRight+wallThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+13));

  FixedComp::setConnect(3,Origin+X*(widthLeft+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+14));

  FixedComp::setConnect(4,Origin-Z*(depth+floorThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(surfIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height+roofThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+16));

  // TSW
  double tswY(tswOffsetY);
  FixedComp::setConnect(6,Origin+Y*(tswY),Y); //should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(6,SMap.realSurf(surfIndex+101));
  tswY += tswWidth;
  FixedComp::setConnect(7,Origin+Y*(tswY),Y);
  FixedComp::setLinkSurf(7,SMap.realSurf(surfIndex+102));
  tswY += tswGap;
  FixedComp::setConnect(8,Origin+Y*(tswY),Y); //should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(8,SMap.realSurf(surfIndex+111));
  tswY += tswWidth;
  FixedComp::setConnect(9,Origin+Y*(tswY),Y);
  FixedComp::setLinkSurf(9,SMap.realSurf(surfIndex+112));

  // walls
  FixedComp::setConnect(10,Origin-Y*(length/2.0),Y);
  FixedComp::setLinkSurf(10,SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(11,Origin+Y*(length/2.0),Y); // should be negative, but layerProcess needs positive
  FixedComp::setLinkSurf(11,SMap.realSurf(surfIndex+2));

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
  attachSystem::addToInsertControl(System,*this,*faradayCup);

  //  attachSystem::addToInsertControl(System,*beamDump,*faradayCup);

  createDTL(System, 10);

  return;
}

}  // essSystem essSystem
