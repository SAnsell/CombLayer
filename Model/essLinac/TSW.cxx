/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/TSW.cxx
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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

#include "TSW.h"

namespace essSystem
{

  TSW::TSW(const std::string& baseKey,const std::string& extraKey,
	   const size_t& index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(baseKey+extraKey+std::to_string(index),6),
  attachSystem::CellMap(),
  baseName(baseKey),
  Index(index)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TSW::TSW(const TSW& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  Index(A.Index),
  width(A.width),nLayers(A.nLayers),
  wallMat(A.wallMat),
  airMat(A.airMat),
  doorMat(A.doorMat),
  doorGap(A.doorGap),
  doorOffset(A.doorOffset),
  doorHeightHigh(A.doorHeightHigh),
  doorWidthHigh(A.doorWidthHigh),
  doorThickWide(A.doorThickWide),
  doorThickHigh(A.doorThickHigh),
  doorHeightLow(A.doorHeightLow),
  doorWidthLow(A.doorWidthLow),
  doorLowGapWidth(A.doorLowGapWidth),
  doorLowGapHeight(A.doorLowGapHeight),
  hole1StepY(A.hole1StepY),
  hole1StepZ(A.hole1StepZ),
  hole1Width(A.hole1Width),
  hole1Height(A.hole1Height),
  hole2StepY(A.hole2StepY),
  hole2StepZ(A.hole2StepZ),
  hole2Radius(A.hole2Radius),
  hole3StepY(A.hole3StepY),
  hole3StepZ(A.hole3StepZ),
  hole3Radius(A.hole3Radius),
  hole4StepY(A.hole4StepY),
  hole4StepZ(A.hole4StepZ),
  hole4Width(A.hole4Width),
  hole4Height(A.hole4Height),
  hole5StepY(A.hole5StepY),
  hole5StepZ(A.hole5StepZ),
  hole5Radius(A.hole5Radius),
  hole6StepY(A.hole6StepY),
  hole6StepZ(A.hole6StepZ),
  hole6Radius(A.hole6Radius),
  hole7StepZ(A.hole7StepZ),
  hole7Width(A.hole7Width),
  hole7Height(A.hole7Height),
  hole8StepZ(A.hole8StepZ),
  hole8Width(A.hole8Width),
  hole8Height(A.hole8Height)
/*!
    Copy constructor
    \param A :: TSW to copy
  */
{}

TSW&
TSW::operator=(const TSW& A)
  /*!
    Assignment operator
    \param A :: TSW to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      nLayers=A.nLayers;
      wallMat=A.wallMat;
      airMat=A.airMat;
      doorMat=A.doorMat;
      doorGap=A.doorGap;
      doorOffset=A.doorOffset;
      doorHeightHigh=A.doorHeightHigh;
      doorWidthHigh=A.doorWidthHigh;
      doorThickWide=A.doorThickWide;
      doorThickHigh=A.doorThickHigh;
      doorHeightLow=A.doorHeightLow;
      doorWidthLow=A.doorWidthLow;
      doorLowGapWidth=A.doorLowGapWidth;
      doorLowGapHeight=A.doorLowGapHeight;
      hole1StepY=A.hole1StepY;
      hole1StepZ=A.hole1StepZ;
      hole1Width=A.hole1Width;
      hole1Height=A.hole1Height;
      hole2StepY=A.hole2StepY;
      hole2StepZ=A.hole2StepZ;
      hole2Radius=A.hole2Radius;
      hole3StepY=A.hole3StepY;
      hole3StepZ=A.hole3StepZ;
      hole3Radius=A.hole3Radius;
      hole4StepY=A.hole4StepY;
      hole4StepZ=A.hole4StepZ;
      hole4Width=A.hole4Width;
      hole4Height=A.hole4Height;
      hole5StepY=A.hole5StepY;
      hole5StepZ=A.hole5StepZ;
      hole5Radius=A.hole5Radius;
      hole6StepY=A.hole6StepY;
      hole6StepZ=A.hole6StepZ;
      hole6Radius=A.hole6Radius;
      hole7StepZ=A.hole7StepZ;
      hole7Width=A.hole7Width;
      hole7Height=A.hole7Height;
      hole8StepZ=A.hole8StepZ;
      hole8Width=A.hole8Width;
      hole8Height=A.hole8Height;
    }
  return *this;
}

TSW*
TSW::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TSW(*this);
}

TSW::~TSW()
  /*!
    Destructor
  */
{}

void
TSW::layerProcess(Simulation& System, const std::string& cellName,
		    const long int& lpS, const long int& lsS,
		    const size_t& N, const int& mat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: TSW wall cell name
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
    \param N :: number of layers to divide to
    \param mat :: material
  */
{
    ELog::RegMethod RegA("TSW","layerProcess");

    if (N<=1)
      return;

    const long int pS(getLinkSurf(lpS));
    const long int sS(getLinkSurf(lsS));

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

    double baseFrac = 1.0/N;
    ModelSupport::surfDivide DA;
    for(size_t i=1;i<N;i++)
      {
	DA.addFrac(baseFrac);
	DA.addMaterial(mat);
	baseFrac += 1.0/N;
      }
    DA.addMaterial(mat);

    DA.setCellN(wallCell);
    DA.setOutNum(cellIndex, buildIndex+10000);

    ModelSupport::mergeTemplate<Geometry::Plane,
				Geometry::Plane> surroundRule;

    surroundRule.setSurfPair(SMap.realSurf(static_cast<int>(pS)),
			     SMap.realSurf(static_cast<int>(sS)));

    std::string OutA = getLinkString(lpS);
    std::string OutB = getLinkString(-lsS);

    surroundRule.setInnerRule(OutA);
    surroundRule.setOuterRule(OutB);

    DA.addRule(&surroundRule);
    DA.activeDivideTemplate(System);

    cellIndex=DA.getCellNum();

    return;
}

void
TSW::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TSW","populate");

  FixedOffset::populate(Control);
  width=Control.EvalVar<double>(keyName+"Width");
  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",1);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  airMat=ModelSupport::EvalMat<int>(Control,baseName+"AirMat");
  doorMat=ModelSupport::EvalMat<int>(Control,keyName+"DoorMat");
  doorGap=Control.EvalVar<double>(keyName+"DoorGap");
  doorOffset=Control.EvalVar<double>(keyName+"DoorOffset");

  doorHeightHigh=Control.EvalVar<double>(keyName+"DoorHeightHigh");
  doorWidthHigh=Control.EvalVar<double>(keyName+"DoorWidthHigh");
  doorThickWide=Control.EvalVar<double>(keyName+"DoorThickWide");
  doorThickHigh=Control.EvalVar<double>(keyName+"DoorThickHigh");
  doorHeightLow=Control.EvalVar<double>(keyName+"DoorHeightLow");
  doorWidthLow=Control.EvalVar<double>(keyName+"DoorWidthLow");
  doorLowGapWidth=Control.EvalVar<double>(keyName+"DoorLowGapWidth");
  doorLowGapHeight=Control.EvalVar<double>(keyName+"DoorLowGapHeight");

  hole1StepY=Control.EvalVar<double>(keyName+"Hole1StepY");
  hole1StepZ=Control.EvalVar<double>(keyName+"Hole1StepZ");
  hole1Width=Control.EvalVar<double>(keyName+"Hole1Width");
  hole1Height=Control.EvalVar<double>(keyName+"Hole1Height");

  hole2StepY=Control.EvalVar<double>(keyName+"Hole2StepY");
  hole2StepZ=Control.EvalVar<double>(keyName+"Hole2StepZ");
  hole2Radius=Control.EvalVar<double>(keyName+"Hole2Radius");

  hole3StepY=Control.EvalVar<double>(keyName+"Hole3StepY");
  hole3StepZ=Control.EvalVar<double>(keyName+"Hole3StepZ");
  hole3Radius=Control.EvalVar<double>(keyName+"Hole3Radius");

  hole4StepY=Control.EvalVar<double>(keyName+"Hole4StepY");
  hole4StepZ=Control.EvalVar<double>(keyName+"Hole4StepZ");
  hole4Width=Control.EvalVar<double>(keyName+"Hole4Width");
  hole4Height=Control.EvalVar<double>(keyName+"Hole4Height");

  hole5StepY=Control.EvalVar<double>(keyName+"Hole5StepY");
  hole5StepZ=Control.EvalVar<double>(keyName+"Hole5StepZ");
  hole5Radius=Control.EvalVar<double>(keyName+"Hole5Radius");

  hole6StepY=Control.EvalVar<double>(keyName+"Hole6StepY");
  hole6StepZ=Control.EvalVar<double>(keyName+"Hole6StepZ");
  hole6Radius=Control.EvalVar<double>(keyName+"Hole6Radius");
  hole7StepZ=Control.EvalVar<double>(keyName+"Hole7StepZ");
  hole7Width=Control.EvalVar<double>(keyName+"Hole7Width");
  hole7Height=Control.EvalVar<double>(keyName+"Hole7Height");
  hole8StepZ=Control.EvalVar<double>(keyName+"Hole8StepZ");
  hole8Width=Control.EvalVar<double>(keyName+"Hole8Width");
  hole8Height=Control.EvalVar<double>(keyName+"Hole8Height");

  return;
}

void
TSW::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TSW","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
TSW::createSurfaces(const attachSystem::FixedComp& FC,
		    const long int wall1,
		    const long int wall2,
		    const long int floor)
  /*!
    Create All the surfaces
    \param FC :: Central origin
    \param wall1 :: link point for origin
    \param wall1 :: link point for the opposite wall
    \param floor :: link point for floor
  */
{
  ELog::RegMethod RegA("TSW","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+X*(width),X);

  // door
  //       thick region
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(doorWidthHigh/2.0-doorOffset),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin-Y*(doorWidthHigh/2.0+doorOffset),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin+X*(doorThickWide),X);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(FC.getLinkPt(floor).Z()+doorHeightHigh),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+111,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+101),
                                  -doorGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+112,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+102),
                                  doorGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+113,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+103),
                                  doorGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+116,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+106),
                                  doorGap);

  //       thin region
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin+Y*(doorWidthLow/2.0-doorOffset),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin-Y*(doorWidthLow/2.0+doorOffset),-Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+211,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+201),
                                  -doorGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+212,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+202),
                                  doorGap);

  //        low region
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin+X*(doorThickHigh),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+213,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+203),
                                  doorGap);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(FC.getLinkPt(floor).Z()+doorHeightLow),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+216,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+206),
                                  doorGap);

  // Hole 1 (rectangular penetration above the door)
  ModelSupport::buildPlane(SMap,buildIndex+301,
			   Origin-Y*(hole1StepY-hole1Width/2.0),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,
			   Origin-Y*(hole1StepY+hole1Width/2.0),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+305,
			   Origin+Z*(hole1StepZ-hole1Height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,
			   Origin+Z*(hole1StepZ+hole1Height/2.0),Z);

  // Hole 2 (circular penetration below Hole 1)
  ModelSupport::buildCylinder(SMap,buildIndex+407,
			      Origin-Y*hole2StepY+Z*hole2StepZ,
			      X,
			      hole2Radius);

  // Hole 3 (circular penetration left of Hole 1)
  ModelSupport::buildCylinder(SMap,buildIndex+507,
			      Origin-Y*hole3StepY+Z*hole3StepZ,
			      X,
			      hole3Radius);

  // Hole 4 (rectangular penetration left to the door)
  ModelSupport::buildPlane(SMap,buildIndex+601,
			   Origin-Y*(hole4StepY-hole4Width/2.0),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+602,
			   Origin-Y*(hole4StepY+hole4Width/2.0),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+605,
			   Origin+Z*(hole4StepZ-hole4Height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+606,
			   Origin+Z*(hole4StepZ+hole4Height/2.0),Z);

  // Hole 5 (circular penetration left of Hole 4)
  ModelSupport::buildCylinder(SMap,buildIndex+707,
			      Origin-Y*hole5StepY+Z*hole5StepZ,
			      X,
			      hole5Radius);

  // Hole 6 (circular penetration above Hole 4)
  ModelSupport::buildCylinder(SMap,buildIndex+807,
			      Origin-Y*hole6StepY+Z*hole6StepZ,
			      X,
			      hole6Radius);
  // Hole 7 (lower rectangular penetration attached to the left linac tunnel wall)
  ModelSupport::buildPlane(SMap,buildIndex+902,Origin-Y*(hole7Width),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+905,
			   Origin+Z*(hole7StepZ-hole7Height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+906,
			   Origin+Z*(hole7StepZ+hole7Height/2.0),Z);

  // Hole 8 (upper rectangular penetration attached to the left linac tunnel wall)
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin-Y*(hole8Width),-Y);
  ModelSupport::buildPlane(SMap,buildIndex+1005,
			   Origin+Z*(hole8StepZ-hole8Height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,
			   Origin+Z*(hole8StepZ+hole8Height/2.0),Z);

  // Gap between the door and threshold
  ModelSupport::buildShiftedPlane(SMap,buildIndex+1103,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+101),
                                  (doorWidthHigh-doorLowGapWidth)/2.0);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+1104,
                                  SMap.realPtr<Geometry::Plane>(buildIndex+102),
                                  -(doorWidthHigh-doorLowGapWidth)/2.0);
  ModelSupport::buildPlane(SMap,buildIndex+1105,
			   Origin+Z*(FC.getLinkPt(floor).Z()+doorLowGapHeight),Z);

  return;
}

void
TSW::createObjects(Simulation& System,const attachSystem::FixedComp& FC,
		   const long int wall1,const long int wall2,
		   const long int floor,const long int roof)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param wall2 :: link point of the Linac wall origin
    \param wall2 :: link point of the opposite Linac wall
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("TSW","createObjects");

  const std::string tb(FC.getLinkString(floor) + FC.getLinkString(roof));
  const std::string side(ModelSupport::getComposite(SMap,buildIndex," 1 -2 "));
  const std::string common(side+tb);

  std::string Out;
  // door
  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex,
			       " 1 -103 101 -102 -106 (-1103:1104:1105) ");
  System.addCell(MonteCarlo::Object(cellIndex++,doorMat,0.0,Out));
  // lower gap
  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex,
			       " 1 -203 1103 -1104 -1105 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));

  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex," 103 -203 201 -202 -106 (-1103:1104:1105) ");
  System.addCell(MonteCarlo::Object(cellIndex++,doorMat,0.0,Out));

  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex," 203 -2 201 -202 -206 ");
  System.addCell(MonteCarlo::Object(cellIndex++,doorMat,0.0,Out));

  // splitting the wall near the door to simplify the wall cell
  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex," 111 -211 113 -2 -116 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out = FC.getLinkString(floor) +
    ModelSupport::getComposite(SMap,buildIndex," 212 -112 113 -2 -116 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out = ModelSupport::getComposite(SMap,buildIndex,
				   " 211 -212 213 -2 216 -116 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  if (doorGap>Geometry::zeroTol)
    {
      Out = FC.getLinkString(floor) +
	ModelSupport::getComposite(SMap,buildIndex,
				   " 1 -103 111 -112 -116 (-101:102:106) ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));

      Out = FC.getLinkString(floor) +
	ModelSupport::getComposite(SMap,buildIndex,
				   " 103 -113 111 -112 -116 (-211:212:116) ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));

      Out = FC.getLinkString(floor) +
	ModelSupport::getComposite(SMap,buildIndex,
				   " 203 -213 211 -212 216 -116 ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));

      Out = FC.getLinkString(floor) +
	ModelSupport::getComposite(SMap,buildIndex,
				   " 103 -203 211 -212 -116 (-201:202:106) ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));

      Out = FC.getLinkString(floor) +
	ModelSupport::getComposite(SMap,buildIndex,
				   " 203 -2 211 -212 -216 (-201:202:206) ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));
    }

  // Hole 1
  Out = ModelSupport::getComposite(SMap,buildIndex," 301 -302 305 -306 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 2
  Out = ModelSupport::getComposite(SMap,buildIndex," -407 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 3
  Out = ModelSupport::getComposite(SMap,buildIndex," -507 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 4 [rectangular]
  Out = ModelSupport::getComposite(SMap,buildIndex," 601 -602 605 -606 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 5
  Out = ModelSupport::getComposite(SMap,buildIndex," -707 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 6
  Out = ModelSupport::getComposite(SMap,buildIndex," -807 ");
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 7
  Out = ModelSupport::getComposite(SMap,buildIndex," -902 905 -906 ");
  Out += FC.getLinkString(wall1);
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // Hole 8
  Out = ModelSupport::getComposite(SMap,buildIndex," -1002 1005 -1006 ");
  Out += FC.getLinkString(wall1);
  System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out+side));

  // the wall
  Out = common+FC.getLinkString(wall1) + FC.getLinkString(wall2) +
    ModelSupport::getComposite(SMap,buildIndex,
			       " (-111:112:116) "
			       " (-301:302:-305:306) 407 507 "
			       " (-601:602:-605:606) 707 807 "
			       " (902:-905:906) (1002:-1005:1006) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  setCell("wall", cellIndex-1);
  //  layerProcess(System, "wall", 3, 7, nLayers, wallMat);
  layerProcess(System, "wall", 3, -4, nLayers, wallMat);

  //  const double linacWidth(FC.getLinkDistance(wall1, wall2));

  Out=common+FC.getLinkString(wall1) + FC.getLinkString(wall2);
  addOuterSurf(Out);

  return;
}


void
TSW::createLinks(const attachSystem::FixedComp& FC,
		 const long int wall1,const long int wall2,
		 const long int floor,const long int roof)
/*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TSW","createLinks");

  FixedComp::setLinkSignedCopy(0,FC,-wall1);
  FixedComp::setLinkSignedCopy(1,FC,-wall2); // wrong


  FixedComp::setConnect(2,Origin,-X); // wrong
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(3,Origin+X*(width),X); // same as 0
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+2));

  FixedComp::setLinkSignedCopy(4,FC,-floor); // as 0 but from the other side of the whall
  FixedComp::setLinkSignedCopy(5,FC,-roof);

  ELog::EM << "Check the links [now they are wrong!]" << ELog::endCrit;

  for (int i=0; i<=5; i++)
    ELog::EM << i << ":\t" << getLinkSurf(i+1) << " " << getLinkPt(i+1) << ELog::endDiag;

  return;
}




void
TSW::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int wall1,
	       const long int wall2,
	       const long int floor,
	       const long int roof)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param wall1 :: link point for origin
    \param wall2 :: link point for the opposite wall
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("TSW","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,-wall1);
  createSurfaces(FC,wall1,wall2,floor);
  createLinks(FC,wall1,wall2,floor,roof);
  createObjects(System,FC,wall1,wall2,floor,roof);
  insertObjects(System);

  return;
}

}  // essSystem
