/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/FrontEndBuilding.cxx
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#include "groupRange.h"
#include "objectGroups.h"
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
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "CellMap.h"
#include "SurfMap.h"

#include "FrontEndBuilding.h"

namespace essSystem
{

FrontEndBuilding::FrontEndBuilding(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,12),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FrontEndBuilding::FrontEndBuilding(const FrontEndBuilding& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),
  widthLeft(A.widthLeft),
  widthRight(A.widthRight),
  wallThick(A.wallThick),
  shieldWall1Offset(A.shieldWall1Offset),
  shieldWall1Thick(A.shieldWall1Thick),
  shieldWall1Length(A.shieldWall1Length),
  shieldWall2Offset(A.shieldWall2Offset),
  shieldWall2Thick(A.shieldWall2Thick),
  shieldWall2Length(A.shieldWall2Length),
  shieldWall3Offset(A.shieldWall3Offset),
  shieldWall3Thick(A.shieldWall3Thick),
  shieldWall3Length(A.shieldWall3Length),
  dropHatchLength(A.dropHatchLength),
  dropHatchWidth(A.dropHatchWidth),
  dropHatchWallThick(A.dropHatchWallThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  gapMat(A.gapMat),
  gapALength(A.gapALength),
  gapAHeightLow(A.gapAHeightLow),
  gapAHeightTop(A.gapAHeightTop),
  gapADist(A.gapADist),
  gapBLength(A.gapBLength),
  gapBHeightLow(A.gapBHeightLow),
  gapBHeightTop(A.gapBHeightTop),
  gapBDist(A.gapBDist),
  gapABDist(A.gapABDist)
  /*!
    Copy constructor
    \param A :: FrontEndBuilding to copy
  */
{}

FrontEndBuilding&
FrontEndBuilding::operator=(const FrontEndBuilding& A)
  /*!
    Assignment operator
    \param A :: FrontEndBuilding to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      widthLeft=A.widthLeft;
      widthRight=A.widthRight;
      wallThick=A.wallThick;
      shieldWall1Offset=A.shieldWall1Offset;
      shieldWall1Thick=A.shieldWall1Thick;
      shieldWall1Length=A.shieldWall1Length;
      shieldWall2Offset=A.shieldWall2Offset;
      shieldWall2Thick=A.shieldWall2Thick;
      shieldWall2Length=A.shieldWall2Length;
      shieldWall3Offset=A.shieldWall3Offset;
      shieldWall3Thick=A.shieldWall3Thick;
      shieldWall3Length=A.shieldWall3Length;
      dropHatchLength=A.dropHatchLength;
      dropHatchWidth=A.dropHatchWidth;
      dropHatchWallThick=A.dropHatchWallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      gapMat=A.gapMat;
      gapALength=A.gapALength;
      gapAHeightLow=A.gapAHeightLow;
      gapAHeightTop=A.gapAHeightTop;
      gapADist=A.gapADist;
      gapBLength=A.gapBLength;
      gapBHeightLow=A.gapBHeightLow;
      gapBHeightTop=A.gapBHeightTop;
      gapBDist=A.gapBDist;
      gapABDist=A.gapABDist;
    }
  return *this;
}

FrontEndBuilding*
FrontEndBuilding::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FrontEndBuilding(*this);
}

FrontEndBuilding::~FrontEndBuilding()
  /*!
    Destructor
  */
{}

void
FrontEndBuilding::layerProcess(Simulation& System, const std::string& cellName,
			       const long int& lpS, const long int& lsS,
			       const std::vector<double>& frac,
			       const std::vector<int>& fracMat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: shielding wall cell name
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
    \param frac :: vector of fractions
    \param fracMat :: vector of materials
  */
{
    ELog::RegMethod RegA("FrontEndBuilding","layerProcess");

    const size_t N(frac.size()+1);

    if (N!=fracMat.size())
      throw ColErr::SizeError<size_t>(N,fracMat.size(),
				   "size of fracMat must be equal to the size of frac+1");

    if (N<=1)
      return;

    const int pS(getLinkSurf(lpS));
    const int sS(getLinkSurf(lsS));

    const attachSystem::CellMap* CM = dynamic_cast<const attachSystem::CellMap*>(this);
    MonteCarlo::Object* wallObj(0);
    int wallCell(0);

    if (CM)
      {
	wallCell=CM->getCell(cellName);
	wallObj=System.findObject(wallCell);
      }

    if (!wallObj)
      throw ColErr::InContainerError<int>(wallCell,
					  "Cell '" + cellName + "' not found");

    double baseFrac = frac[0];
    ModelSupport::surfDivide DA;
    for(size_t i=1;i<N;i++)
      {
	DA.addFrac(baseFrac);
	DA.addMaterial(fracMat[i-1]);
	baseFrac += frac[i];
      }
    DA.addMaterial(fracMat.back());

    DA.setCellN(wallCell);
    DA.setOutNum(cellIndex, surfIndex+10000);

    ModelSupport::mergeTemplate<Geometry::Plane,
				Geometry::Plane> surroundRule;

    surroundRule.setSurfPair(SMap.realSurf(pS),SMap.realSurf(sS));

    std::string OutA(getLinkString(lpS));
    std::string OutB(getLinkString(-lsS));

    surroundRule.setInnerRule(OutA);
    surroundRule.setOuterRule(OutB);

    DA.addRule(&surroundRule);
    DA.activeDivideTemplate(System);

    cellIndex=DA.getCellNum();

    return;
}

void
FrontEndBuilding::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  widthLeft=Control.EvalVar<double>(keyName+"WidthLeft");
  widthRight=Control.EvalVar<double>(keyName+"WidthRight");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  shieldWall1Offset=Control.EvalVar<double>(keyName+"ShieldWall1Offset");
  shieldWall1Thick=Control.EvalVar<double>(keyName+"ShieldWall1Thick");
  shieldWall1Length=Control.EvalVar<double>(keyName+"ShieldWall1Length");
  shieldWall2Offset=Control.EvalVar<double>(keyName+"ShieldWall2Offset");
  shieldWall2Thick=Control.EvalVar<double>(keyName+"ShieldWall2Thick");
  shieldWall2Length=Control.EvalVar<double>(keyName+"ShieldWall2Length");
  shieldWall3Offset=Control.EvalVar<double>(keyName+"ShieldWall3Offset");
  shieldWall3Thick=Control.EvalVar<double>(keyName+"ShieldWall3Thick");
  shieldWall3Length=Control.EvalVar<double>(keyName+"ShieldWall3Length");
  dropHatchLength=Control.EvalVar<double>(keyName+"DropHatchLength");
  dropHatchWidth=Control.EvalVar<double>(keyName+"DropHatchWidth");
  dropHatchWallThick=Control.EvalVar<double>(keyName+"DropHatchWallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  gapMat=ModelSupport::EvalMat<int>(Control,keyName+"GapMat");
  gapALength=Control.EvalVar<double>(keyName+"GapALength");
  gapAHeightLow=Control.EvalVar<double>(keyName+"GapAHeightLow");
  gapAHeightTop=Control.EvalVar<double>(keyName+"GapAHeightTop");
  gapADist=Control.EvalVar<double>(keyName+"GapADist");
  gapBLength=Control.EvalVar<double>(keyName+"GapBLength");
  gapBHeightLow=Control.EvalVar<double>(keyName+"GapBHeightLow");
  gapBHeightTop=Control.EvalVar<double>(keyName+"GapBHeightTop");
  gapBDist=Control.EvalVar<double>(keyName+"GapBDist");
  gapABDist=Control.EvalVar<double>(keyName+"GapABDist");

  return;
}

void
FrontEndBuilding::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
FrontEndBuilding::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin+X*(widthLeft),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin-X*(widthRight),X);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(wallThick),Y);
  SurfMap::addSurf("farDivide",-SMap.realSurf(surfIndex+11));
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length+wallThick),Y);

  ModelSupport::buildPlane(SMap,surfIndex+13,Origin+X*(widthLeft+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin-X*(widthRight+wallThick),X);

  // shield wall 1
  ModelSupport::buildPlane(SMap,surfIndex+102,Origin+Y*(shieldWall1Length),Y);
  ModelSupport::buildPlane(SMap,surfIndex+103,Origin-X*(shieldWall1Offset),X);
  ModelSupport::buildPlane(SMap,surfIndex+104,
  			   Origin-X*(shieldWall1Offset+shieldWall1Thick),X);

  // shield wall 2
  ModelSupport::buildPlane(SMap,surfIndex+201,Origin+Y*(shieldWall2Offset),Y);
  ModelSupport::buildPlane(SMap,surfIndex+202,Origin+Y*(shieldWall2Offset+shieldWall2Thick),Y);
  ModelSupport::buildShiftedPlane(SMap,surfIndex+204,
				  SMap.realPtr<Geometry::Plane>(surfIndex+104),
				  -shieldWall2Length);
  ModelSupport::buildShiftedPlane(SMap,surfIndex+213,
				  SMap.realPtr<Geometry::Plane>(surfIndex+204),
				  shieldWall2Thick);

  // Drop Hatch
  ModelSupport::buildPlane(SMap,surfIndex+301,
			   Origin+Y*(length-dropHatchLength)/2.0,Y);
  ModelSupport::buildPlane(SMap,surfIndex+302,
			   Origin+Y*(length+dropHatchLength)/2.0,Y);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+303,
				  SMap.realPtr<Geometry::Plane>(surfIndex+13),
				  dropHatchWidth);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+311,
				  SMap.realPtr<Geometry::Plane>(surfIndex+301),
				  -dropHatchWallThick);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+312,
				  SMap.realPtr<Geometry::Plane>(surfIndex+302),
				  dropHatchWallThick);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+313,
				  SMap.realPtr<Geometry::Plane>(surfIndex+303),
				  dropHatchWallThick);

  // shield wall 3
  ModelSupport::buildShiftedPlane(SMap,surfIndex+402,
				  SMap.realPtr<Geometry::Plane>(surfIndex+2),
				  -shieldWall3Length);
  ModelSupport::buildPlane(SMap,surfIndex+403,Origin-X*(shieldWall3Offset),X);
  ModelSupport::buildPlane(SMap,surfIndex+404,
			   Origin-X*(shieldWall3Offset+shieldWall3Thick),X);

  return;
}

void
FrontEndBuilding::createObjects(Simulation& System,
				const attachSystem::FixedComp &FC,
				const long int floorIndexLow,
				const long int floorIndexTop,
				const long int roofIndexLow,
				const long int roofIndexTop)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: central origin
    \param floorIndexLow :: bottom floor lp
    \param floorIndexTop :: top floor lp
    \param roofIndexLow :: bottom roof lp
    \param roofIndexTop :: top roof lp
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createObjects");

  std::string Out;
  const std::string airTB(FC.getLinkString(floorIndexTop) +
			  FC.getLinkString(roofIndexLow));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -3 103 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -102 -103 104 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+airTB));
  setCell("ShieldingWall1",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,surfIndex," 102 -2 -103 104 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 201 -202 -104 204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 202 -102 -213 204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 102 -2 -213 204 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 201 -202 -204 4 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -201 -104 4 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 202 -2 -204 4 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));


  Out=ModelSupport::getComposite(SMap,surfIndex," 202 -402 -104 213 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 402 -2 -404 213 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 402 -2 -403 404 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+airTB));

  Out=ModelSupport::getComposite(SMap,surfIndex," 402 -2 403 -104 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out+airTB));


  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -3 4 ")+airTB;

  HeadRule HR;
  HR.procString(Out);
  HR.makeComplement();

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 -13 14 ") +
    FC.getLinkString(-floorIndexLow) + FC.getLinkString(-roofIndexTop);
  addOuterSurf(Out);

  // gap in the wall for the dropHatch
  Out += ModelSupport::getComposite(SMap,surfIndex," (-301:302:-3:") +
    FC.getLinkString(-floorIndexTop)+":"+FC.getLinkString(-roofIndexLow)+")";
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+HR.display()));

  Out=ModelSupport::getComposite(SMap,surfIndex," 301 -302 -303 3 ")+airTB;
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out));

  HR.procString(Out);
  HR.makeComplement();

  Out=ModelSupport::getComposite(SMap,surfIndex," 311 -312 -313 13 ") +
  FC.getLinkString(-floorIndexLow) + FC.getLinkString(-roofIndexTop);
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+HR.display()));

  addOuterUnionSurf(Out);

  // Penetrations A and B in ShieldingWall1
  std::vector<int> fracMat;
  for (int i=0; i<4; i++)
    fracMat.push_back(wallMat);

  std::vector<double> frac;
  frac.push_back(gapBLength/shieldWall1Length);
  frac.push_back(gapABDist/shieldWall1Length);
  frac.push_back(gapALength/shieldWall1Length);

  layerProcess(System, "ShieldingWall1", -7, 8, frac,fracMat);
  setCell("PenetrationA", cellIndex-2);
  setCell("PenetrationB", cellIndex-4);

  fracMat.clear();
  for (int i=0; i<4; i++)
    fracMat.push_back(i%2 ? wallMat : gapMat);

  const double totalHeight(getLinkDistance(11,12));

  frac.clear();
  frac.push_back(gapBHeightLow/totalHeight);
  frac.push_back(gapBDist/totalHeight);
  frac.push_back(gapBHeightTop/totalHeight);

  layerProcess(System, "PenetrationB", 11, -12, frac,fracMat);

  frac.clear();
  frac.push_back(gapAHeightLow/totalHeight);
  frac.push_back(gapADist/totalHeight);
  frac.push_back(gapAHeightTop/totalHeight);

  layerProcess(System, "PenetrationA", 11, -12, frac,fracMat);

  return;
}


void
FrontEndBuilding::createLinks(const attachSystem::FixedComp& FC,
			      const long int floorIndexLow,
			      const long int floorIndexTop,
			      const long int roofIndexLow,
			      const long int roofIndexTop)
  /*!
    Create all the linkes
    \param FC :: central origin
    \param floorIndexLow :: bottom floor lp
    \param floorIndexTop :: top floor lp
    \param roofIndexLow :: bottom roof lp
    \param roofIndexTop :: top roof lp
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));

  FixedComp::setConnect(2,Origin+
			X*(widthLeft+wallThick+dropHatchWidth+dropHatchWallThick)+
			Y*((length+wallThick)/2.0),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+313));

  FixedComp::setConnect(3,Origin-X*(widthRight+wallThick)+
			Y*((length+wallThick)/2.0),-X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(surfIndex+14));

  FixedComp::setConnect(4,Origin+Z*FC.getLinkPt(floorIndexLow).Z()+
			Y*((length+wallThick)/2.0),-Z);
  FixedComp::setLinkSurf(4,FC.getLinkSurf(floorIndexLow));

  FixedComp::setConnect(5,Origin+Z*FC.getLinkPt(roofIndexTop).Z()+
			Y*((length+wallThick)/2.0),Z);
  FixedComp::setLinkSurf(5,FC.getLinkSurf(roofIndexTop));

  // ShieldingWall1
  FixedComp::setConnect(6,Origin-X*(shieldWall1Offset+shieldWall1Thick/2.0),-Y);
  FixedComp::setLinkSurf(6,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(7,Origin-X*(shieldWall1Offset+shieldWall1Thick/2.0)+
   			Y*(shieldWall1Length),Y);
  FixedComp::setLinkSurf(7,SMap.realSurf(surfIndex+102));

  FixedComp::setConnect(8,Origin-X*(shieldWall1Offset)+
  			Y*(shieldWall1Length/2.0),-X);
  FixedComp::setLinkSurf(8,-SMap.realSurf(surfIndex+103));

  FixedComp::setConnect(9,Origin-X*(shieldWall1Offset+shieldWall1Thick)+
  			Y*(shieldWall1Length/2.0),X);
  FixedComp::setLinkSurf(9,SMap.realSurf(surfIndex+104));

  FixedComp::setConnect(10,Origin+Z*FC.getLinkPt(floorIndexTop).Z()+
			Y*((length+wallThick)/2.0),-Z);
  FixedComp::setLinkSurf(10,FC.getLinkSurf(floorIndexTop));

  FixedComp::setConnect(11,Origin+Z*FC.getLinkPt(roofIndexLow).Z()+
			Y*((length+wallThick)/2.0),Z);
  FixedComp::setLinkSurf(11,FC.getLinkSurf(roofIndexLow));

  // size_t N = NConnect();
  // for (size_t i=0; i<=N; i++)
  //   {
  //     ELog::EM << keyName << ": " << i << "\t" << getLinkPt(i) << "\t" << getLinkString(i) << ELog::endDiag;
  //   }

  return;
}




void
FrontEndBuilding::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex,
			    const long int floorIndexLow,
			    const long int floorIndexTop,
			    const long int roofIndexLow,
			    const long int roofIndexTop)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
    \param floorIndexLow :: bottom floor lp
    \param floorIndexTop :: top floor lp
    \param roofIndexLow :: bottom roof lp
    \param roofIndexTop :: top roof lp
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks(FC,floorIndexLow,floorIndexTop,roofIndexLow,roofIndexTop);
  createObjects(System,FC,floorIndexLow,floorIndexTop,roofIndexLow,roofIndexTop);
  insertObjects(System);

  return;
}

}  // essSystem
