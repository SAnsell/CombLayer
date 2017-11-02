/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BeamDump.cxx
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

#include "BeamDump.h"

namespace essSystem
{

BeamDump::BeamDump(const std::string& Base,
		   const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Base+Key)),
  cellIndex(surfIndex+1),
  baseName(Base),active(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base name 
    \param Key :: Name for item in search
  */
{}

BeamDump::BeamDump(const BeamDump& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  baseName(A.baseName),active(A.active),
  engActive(A.engActive),steelMat(A.steelMat),
  concMat(A.concMat),alMat(A.alMat),waterMat(A.waterMat),
  airMat(A.airMat),cuMat(A.cuMat),graphiteMat(A.graphiteMat),
  frontWallLength(A.frontWallLength),
  frontWallHeight(A.frontWallHeight),
  frontWallDepth(A.frontWallDepth),
  frontWallWidth(A.frontWallWidth),
  frontWallHoleRad(A.frontWallHoleRad),
  backWallLength(A.backWallLength),
  backWallDepth(A.backWallDepth),
  frontInnerWallHeight(A.frontInnerWallHeight),
  frontInnerWallDepth(A.frontInnerWallDepth),
  frontInnerWallLength(A.frontInnerWallLength),
  frontInnerWallHoleRad(A.frontInnerWallHoleRad),
  backInnerWallLength(A.backInnerWallLength),
  backInnerWallGapLength(A.backInnerWallGapLength),
  sideInnerWallThick(A.sideInnerWallThick),
  sideWallThick(A.sideWallThick),floorLength(A.floorLength),
  floorDepth(A.floorDepth),plate25Length(A.plate25Length),
  plate25Depth(A.plate25Depth),plate38Depth(A.plate38Depth),
  roofThick(A.roofThick),
  roofOverhangLength(A.roofOverhangLength),
  innerRoofThick(A.innerRoofThick),
  vacPipeFrontInnerWallDist(A.vacPipeFrontInnerWallDist),
  vacPipeLength(A.vacPipeLength),vacPipeRad(A.vacPipeRad),
  vacPipeSideWallThick(A.vacPipeSideWallThick),
  vacPipeLidRmax(A.vacPipeLidRmax),
  vacPipeLid1Length(A.vacPipeLid1Length),
  vacPipeLid2Length(A.vacPipeLid2Length),
  vacPipeBaseLength(A.vacPipeBaseLength),
  vacPipeOuterConeOffset(A.vacPipeOuterConeOffset),
  vacPipeInnerConeTop(A.vacPipeInnerConeTop),
  wallThick(A.wallThick),waterPipeRad(A.waterPipeRad),
  waterPipeLength(A.waterPipeLength),
  waterPipeOffsetX(A.waterPipeOffsetX),
  waterPipeOffsetZ(A.waterPipeOffsetZ),
  waterPipeDist(A.waterPipeDist)
  /*!
    Copy constructor
    \param A :: BeamDump to copy
  */
{}

BeamDump&
BeamDump::operator=(const BeamDump& A)
  /*!
    Assignment operator
    \param A :: BeamDump to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      baseName=A.baseName;
      active=A.active;
      engActive=A.engActive;
      steelMat=A.steelMat;
      concMat=A.concMat;
      alMat=A.alMat;
      waterMat=A.waterMat;
      airMat=A.airMat;
      cuMat=A.cuMat;
      graphiteMat=A.graphiteMat;
      frontWallLength=A.frontWallLength;
      frontWallHeight=A.frontWallHeight;
      frontWallDepth=A.frontWallDepth;
      frontWallWidth=A.frontWallWidth;
      frontWallHoleRad=A.frontWallHoleRad;
      backWallLength=A.backWallLength;
      backWallDepth=A.backWallDepth;
      frontInnerWallHeight=A.frontInnerWallHeight;
      frontInnerWallDepth=A.frontInnerWallDepth;
      frontInnerWallLength=A.frontInnerWallLength;
      frontInnerWallHoleRad=A.frontInnerWallHoleRad;
      backInnerWallLength=A.backInnerWallLength;
      backInnerWallGapLength=A.backInnerWallGapLength;
      sideInnerWallThick=A.sideInnerWallThick;
      sideWallThick=A.sideWallThick;
      floorLength=A.floorLength;
      floorDepth=A.floorDepth;
      plate25Length=A.plate25Length;
      plate25Depth=A.plate25Depth;
      plate38Depth=A.plate38Depth;
      roofThick=A.roofThick;
      roofOverhangLength=A.roofOverhangLength;
      innerRoofThick=A.innerRoofThick;
      vacPipeFrontInnerWallDist=A.vacPipeFrontInnerWallDist;
      vacPipeLength=A.vacPipeLength;
      vacPipeRad=A.vacPipeRad;
      vacPipeSideWallThick=A.vacPipeSideWallThick;
      vacPipeLidRmax=A.vacPipeLidRmax;
      vacPipeLid1Length=A.vacPipeLid1Length;
      vacPipeLid2Length=A.vacPipeLid2Length;
      vacPipeBaseLength=A.vacPipeBaseLength;
      vacPipeOuterConeOffset=A.vacPipeOuterConeOffset;
      vacPipeInnerConeTop=A.vacPipeInnerConeTop;
      wallThick=A.wallThick;
      waterPipeRad=A.waterPipeRad;
      waterPipeLength=A.waterPipeLength;
      waterPipeOffsetX=A.waterPipeOffsetX;
      waterPipeOffsetZ=A.waterPipeOffsetZ;
      waterPipeDist=A.waterPipeDist;
    }
  return *this;
}

BeamDump::~BeamDump()
  /*!
    Destructor
  */
{}

void
BeamDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BeamDump","populate");

  FixedOffset::populate(Control);

  active=Control.EvalPair<int>(keyName,baseName,"Active");
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  steelMat=ModelSupport::EvalMat<int>(Control,baseName+"SteelMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcreteMat");
  alMat=ModelSupport::EvalMat<int>(Control,baseName+"AlMat");
  waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");
  airMat=ModelSupport::EvalMat<int>(Control,baseName+"AirMat");
  cuMat=ModelSupport::EvalMat<int>(Control,baseName+"CopperMat");
  graphiteMat=ModelSupport::EvalMat<int>(Control,baseName+"GraphiteMat");

  frontWallLength=Control.EvalVar<double>(keyName+"FrontWallLength");
  frontWallHeight=Control.EvalVar<double>(keyName+"FrontWallHeight");
  frontWallDepth=Control.EvalVar<double>(keyName+"FrontWallDepth");
  frontWallWidth=Control.EvalVar<double>(keyName+"FrontWallWidth");
  frontWallHoleRad=Control.EvalVar<double>(keyName+"FrontWallHoleRad");

  backWallLength=Control.EvalVar<double>(keyName+"BackWallLength");
  backWallDepth=Control.EvalVar<double>(keyName+"BackWallDepth");

  frontInnerWallHeight=Control.EvalVar<double>(keyName+"FrontInnerWallHeight");
  frontInnerWallDepth=Control.EvalVar<double>(keyName+"FrontInnerWallDepth");
  frontInnerWallLength=Control.EvalVar<double>(keyName+"FrontInnerWallLength");
  frontInnerWallHoleRad=Control.EvalVar<double>(keyName+"FrontInnerWallHoleRad");

  backInnerWallLength=Control.EvalVar<double>(keyName+"BackInnerWallLength");
  backInnerWallGapLength=Control.EvalVar<double>(keyName+"BackInnerWallGapLength");
  sideInnerWallThick=Control.EvalVar<double>(keyName+"SideInnerWallThick");
  sideWallThick=Control.EvalVar<double>(keyName+"SideWallThick");

  floorLength=Control.EvalVar<double>(keyName+"FloorLength");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");

  plate25Length=Control.EvalVar<double>(keyName+"Plate25Length");
  plate25Depth=Control.EvalVar<double>(keyName+"Plate25Depth");

  plate38Depth=Control.EvalVar<double>(keyName+"Plate38Depth");

  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  roofOverhangLength=Control.EvalVar<double>(keyName+"RoofOverhangLength");
  innerRoofThick=Control.EvalVar<double>(keyName+"InnerRoofThick");

  vacPipeFrontInnerWallDist=Control.EvalVar<double>(keyName+"VacPipeFrontInnerWallDist");
  vacPipeLength=Control.EvalVar<double>(keyName+"VacPipeLength");
  vacPipeRad=Control.EvalVar<double>(keyName+"VacPipeRad");
  vacPipeSideWallThick=Control.EvalVar<double>(keyName+"VacPipeSideWallThick");
  vacPipeLidRmax=Control.EvalVar<double>(keyName+"VacPipeLidRmax");
  vacPipeLid1Length=Control.EvalVar<double>(keyName+"VacPipeLid1Length");
  vacPipeLid2Length=Control.EvalVar<double>(keyName+"VacPipeLid2Length");
  vacPipeBaseLength=Control.EvalVar<double>(keyName+"VacPipeBaseLength");
  vacPipeOuterConeOffset=Control.EvalVar<double>(keyName+"VacPipeOuterConeOffset");
  vacPipeInnerConeTop=Control.EvalVar<double>(keyName+"VacPipeInnerConeTop");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  waterPipeRad=Control.EvalVar<double>(keyName+"WaterPipeRadius");
  waterPipeLength=Control.EvalVar<double>(keyName+"WaterPipeLength");
  waterPipeOffsetX=Control.EvalVar<double>(keyName+"WaterPipeOffsetX");
  waterPipeOffsetZ=Control.EvalVar<double>(keyName+"WaterPipeOffsetZ");
  waterPipeDist=Control.EvalVar<double>(keyName+"WaterPipeDist");

  return;
}

void
BeamDump::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BeamDump","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
BeamDump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamDump","createSurfaces");

  // Front wall
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*frontWallLength,Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(frontWallWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(frontWallWidth/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(frontWallDepth+frontInnerWallHeight+innerRoofThick),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(frontWallHeight-frontInnerWallHeight-innerRoofThick),Z);
  ModelSupport::buildCylinder(SMap,surfIndex+7,Origin,Y,frontWallHoleRad);

  // Floor
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(frontWallLength+floorLength),Y);

  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(frontInnerWallDepth+
						       floorDepth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,Origin-Z*(frontInnerWallDepth),Z);

  // Floor - small plates (25 and 26) under the floor
  ModelSupport::buildShiftedPlane(SMap, surfIndex+21,
				  SMap.realPtr<Geometry::Plane>(surfIndex+2),
				  plate25Length);
  ModelSupport::buildShiftedPlane(SMap, surfIndex+22,
				  SMap.realPtr<Geometry::Plane>(surfIndex+12),
				  -plate25Length);
  ModelSupport::buildPlane(SMap,surfIndex+25,Origin-Z*(backWallDepth),Z);

  // Al plate zmin
  ModelSupport::buildShiftedPlane(SMap, surfIndex+35,
				  SMap.realPtr<Geometry::Plane>(surfIndex+25),
				  -plate38Depth);

  // back wall
  double y1=frontWallLength+floorLength+backWallLength;
  ModelSupport::buildPlane(SMap,surfIndex+42,Origin+Y*(y1),Y);

  // top wall
  ModelSupport::buildShiftedPlane(SMap, surfIndex+51,
				  SMap.realPtr<Geometry::Plane>(surfIndex+1),
				  -roofOverhangLength);
  ModelSupport::buildShiftedPlane(SMap, surfIndex+56,
				  SMap.realPtr<Geometry::Plane>(surfIndex+6),
				  roofThick);

  // side walls - inner surfaces
  ModelSupport::buildPlane(SMap,surfIndex+63,Origin-X*sideInnerWallThick,X);
  ModelSupport::buildPlane(SMap,surfIndex+64,Origin+X*sideInnerWallThick,X);
  // side walls - outer surfaces
  ModelSupport::buildPlane(SMap,surfIndex+73,
			   Origin-X*(frontWallWidth/2.0+sideWallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+74,
			   Origin+X*(frontWallWidth/2.0+sideWallThick),X);

  // inner roof
  ModelSupport::buildShiftedPlane(SMap, surfIndex+76,
				  SMap.realPtr<Geometry::Plane>(surfIndex+6),
				  -innerRoofThick);

  // front inner wall
  ModelSupport::buildShiftedPlane(SMap, surfIndex+82,
				  SMap.realPtr<Geometry::Plane>(surfIndex+2),
				  frontInnerWallLength);
  ModelSupport::buildCylinder(SMap,surfIndex+87,Origin,Y,frontInnerWallHoleRad);

  // back inner wall
  ModelSupport::buildShiftedPlane(SMap, surfIndex+91,
				  SMap.realPtr<Geometry::Plane>(surfIndex+12),
				  -backInnerWallLength-backInnerWallGapLength);
  ModelSupport::buildShiftedPlane(SMap, surfIndex+92,
				  SMap.realPtr<Geometry::Plane>(surfIndex+12),
				  -backInnerWallGapLength);

  // Vacuum pipe
  ModelSupport::buildShiftedPlane(SMap, surfIndex+101,
				  SMap.realPtr<Geometry::Plane>(surfIndex+82),
				  vacPipeFrontInnerWallDist);
  const double pl102Y = frontWallLength + frontInnerWallLength + vacPipeFrontInnerWallDist + vacPipeLength;
  ModelSupport::buildPlane(SMap, surfIndex+102, Origin+Y*pl102Y, Y);
  ModelSupport::buildCylinder(SMap,surfIndex+107,Origin,Y,vacPipeRad);
  ModelSupport::buildCylinder(SMap,surfIndex+108,Origin,Y,
			      vacPipeRad+vacPipeSideWallThick);
  // Vacuum pipe lids
  ModelSupport::buildShiftedPlane(SMap, surfIndex+111,
				  SMap.realPtr<Geometry::Plane>(surfIndex+101),
				  vacPipeLid1Length);
  ModelSupport::buildShiftedPlane(SMap, surfIndex+112,
				  SMap.realPtr<Geometry::Plane>(surfIndex+102),
				  -vacPipeLid2Length);
  ModelSupport::buildCylinder(SMap,surfIndex+117,Origin,Y,vacPipeLidRmax);

  // vac pipe internal structure
  const double coneOpenAngle = 3.2;
  Geometry::Vec3D coneYdir(Y);
  Geometry::Quaternion::calcQRotDeg(coneOpenAngle,X).rotate(coneYdir);

  double coneYpos = pl102Y+vacPipeOuterConeOffset;

  ModelSupport::buildPlane(SMap,surfIndex+121,Origin+Y*coneYpos,Y);
  ModelSupport::buildCone(SMap,surfIndex+127,
  			  Origin+Y*(coneYpos)+Z*vacPipeRad,
			  coneYdir,
			  coneOpenAngle);

  // vac pipe internal structure: base
  ModelSupport::buildShiftedPlane(SMap, surfIndex+122,
				  SMap.realPtr<Geometry::Plane>(surfIndex+102),
				  -vacPipeBaseLength);

  coneYpos -= wallThick / tan(coneOpenAngle*M_PI/180);
  const double coneTop = pl102Y-vacPipeInnerConeTop;
  ModelSupport::buildPlane(SMap,surfIndex+131,Origin+Y*coneTop,Y);
  ModelSupport::buildCone(SMap,surfIndex+137,
  			  Origin+Y*(coneYpos)+Z*(vacPipeRad-wallThick),
			  coneYdir,
			  coneOpenAngle);
  const double coneTopOpenAngle = 11;
  ModelSupport::buildCone(SMap,surfIndex+138,
  			  Origin+Y*(coneTop)+Z*(vacPipeRad-wallThick*1.6),
			  coneYdir,
			  coneTopOpenAngle);

  // water cooling pipes
  Geometry::Vec3D pipeYdir(Y);
  Geometry::Quaternion::calcQRotDeg(coneOpenAngle*2.0,X).rotate(pipeYdir);
  ModelSupport::buildShiftedPlane(SMap, surfIndex+141,
				  SMap.realPtr<Geometry::Plane>(surfIndex+102),
				  -waterPipeLength);
  ModelSupport::buildCylinder(SMap,surfIndex+147,Origin-Z*(waterPipeOffsetZ) +
			      X*waterPipeOffsetX,pipeYdir,waterPipeRad);
  ModelSupport::buildCylinder(SMap,surfIndex+148,Origin-
			      Z*(waterPipeOffsetZ+waterPipeRad*2+waterPipeDist) +
			      X*waterPipeOffsetX,pipeYdir,waterPipeRad);




  return;
}

void
BeamDump::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamDump","createObjects");
  
  if (!active) return;

  std::string Out;
  // front wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // floor
  Out=ModelSupport::getComposite(SMap,surfIndex," 2 -12 3 -4 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 2 -21 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // Floor - small steel plates
  Out=ModelSupport::getComposite(SMap,surfIndex," 21 -22 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 22 -12 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  //  Floor - Al plate
  Out=ModelSupport::getComposite(SMap,surfIndex," 2 -42 3 -4 35 -25 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Floor - void cell below Al plate
  Out=ModelSupport::getComposite(SMap,surfIndex," 2 -42 3 -4 5 -35 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // back wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 12 -42 3 -4 25 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  // Roof
  Out=ModelSupport::getComposite(SMap,surfIndex," 51 -42 3 -4 6 -56 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  // Inner roof
  Out=ModelSupport::getComposite(SMap,surfIndex," 2 -12 3 -4 76 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // void cell under overhead
  Out=ModelSupport::getComposite(SMap,surfIndex," 51 -1 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // side walls
  //            inner
  Out=ModelSupport::getComposite(SMap,surfIndex, " 2 -12 3 -63 16 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 2 -12 64 -4 16 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  //            outer
  Out=ModelSupport::getComposite(SMap,surfIndex, " 51 -42 73 -3 5 -56 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 51 -42 4 -74 5 -56 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  // front inner wall
  Out=ModelSupport::getComposite(SMap,surfIndex, " 2 -82 63 -64 16 -76 87 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // proton hole in plates 22,28,29
  Out=ModelSupport::getComposite(SMap,surfIndex, " 2 -82 -87 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // back inner wall and gap
  Out=ModelSupport::getComposite(SMap,surfIndex, " 91 -92 63 -64 16 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 92 -12 63 -64 16 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // vac pipe
  Out=ModelSupport::getComposite(SMap,surfIndex, " 101 -131 -107 -137 -138 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // cone top
  Out=ModelSupport::getComposite(SMap,surfIndex, " -131 -137 138 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,graphiteMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex, " 131 -112 -137 ");// : -131 -137 138");
  System.addCell(MonteCarlo::Qhull(cellIndex++,graphiteMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 101 -112 -107 -127 137 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,graphiteMat,0.0,Out));
  // steel inside vac pipe cone and lid 2
  Out=ModelSupport::getComposite(SMap,surfIndex," 112 -102 -107 -127 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cuMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 101 -102 -107 127  122 (147:-141) (148:-141)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cuMat,0.0,Out));

  //water pipes
  Out=ModelSupport::getComposite(SMap,surfIndex," 141 -102 -147");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 141 -102 -148");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  // tiny cell
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 101 -102 -107 127 -122 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 101 -102 107 -108 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // vac pipe lids
  Out=ModelSupport::getComposite(SMap,surfIndex, " 101 -111 108 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 111 -112 108 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex, " 112 -102 108 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  //  void cell inside shielding (vac pipe goes there)
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 82 -91 63 -64 16 -76 (-101:102:117) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 51 -42 73 -74 5 -56 ");
  addOuterSurf(Out);

  return;
}


void
BeamDump::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BeamDump","createLinks");

  FixedComp::setConnect(0,Origin-Y*(roofOverhangLength),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+51));

  FixedComp::setConnect(1,Origin+Y*(frontWallLength+floorLength+backWallLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+42));

  FixedComp::setConnect(2,Origin-X*(frontWallWidth/2.0+sideWallThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+73));

  FixedComp::setConnect(3,Origin+X*(frontWallWidth/2.0+sideWallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+74));
  
  FixedComp::setConnect(4,Origin-Z*(frontWallDepth+frontInnerWallHeight+innerRoofThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(surfIndex+5));

  FixedComp::setConnect(5,Origin+
			Z*(frontWallHeight-frontInnerWallHeight-innerRoofThick+roofThick),-Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+56));

  return;
}




void
BeamDump::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BeamDump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem essSystem
