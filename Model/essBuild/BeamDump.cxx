/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BeamDump.cxx
 *
 * Copyright (c) 2004-2016 by Konstantin Batkov
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

#include "BeamDump.h"

namespace essSystem
{

BeamDump::BeamDump(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamDump::BeamDump(const BeamDump& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  
  steelMat(A.steelMat),
  concMat(A.concMat),
  alMat(A.alMat),
  
  frontWallLength(A.frontWallLength),
  frontWallHeight(A.frontWallHeight),
  frontWallDepth(A.frontWallDepth),
  frontWallWidth(A.frontWallWidth),

  backWallLength(A.backWallLength),
  backWallDepth(A.backWallDepth),
  
  frontInnerWallDepth(A.frontInnerWallDepth),

  floorLength(A.floorLength),
  floorDepth(A.floorDepth),

  plate25Length(A.plate25Length),
  plate25Depth(A.plate25Depth),

  plate38Depth(A.plate38Depth),

  roofThick(A.roofThick),
  roofOverhangLength(A.roofOverhangLength),
  mainMat(A.mainMat),wallMat(A.wallMat)
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
      engActive=A.engActive;
      
      steelMat=A.steelMat;
      concMat=A.concMat;
      alMat=A.alMat;

      frontWallLength=A.frontWallLength;
      frontWallHeight=A.frontWallHeight;
      frontWallDepth=A.frontWallDepth;
      frontWallWidth=A.frontWallWidth;
      
      backWallLength=A.backWallLength;
      backWallDepth=A.backWallDepth;
      
      frontInnerWallDepth=A.frontInnerWallDepth;

      floorLength=A.floorLength;
      floorDepth=A.floorDepth;

      plate25Length=A.plate25Length;
      plate25Depth=A.plate25Depth;

      plate38Depth=A.plate38Depth;

      roofThick=A.roofThick;
      roofOverhangLength=A.roofOverhangLength;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

BeamDump*
BeamDump::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BeamDump(*this);
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
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcreteMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  frontWallLength=Control.EvalVar<double>(keyName+"FrontWallLength");
  frontWallHeight=Control.EvalVar<double>(keyName+"FrontWallHeight");
  frontWallDepth=Control.EvalVar<double>(keyName+"FrontWallDepth");
  frontWallWidth=Control.EvalVar<double>(keyName+"FrontWallWidth");

  backWallLength=Control.EvalVar<double>(keyName+"BackWallLength");
  backWallDepth=Control.EvalVar<double>(keyName+"BackWallDepth");

  frontInnerWallDepth=Control.EvalVar<double>(keyName+"FrontInnerWallDepth");

  floorLength=Control.EvalVar<double>(keyName+"FloorLength");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");

  plate25Length=Control.EvalVar<double>(keyName+"Plate25Length");
  plate25Depth=Control.EvalVar<double>(keyName+"Plate25Depth");

  plate38Depth=Control.EvalVar<double>(keyName+"Plate38Depth");

  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  roofOverhangLength=Control.EvalVar<double>(keyName+"RoofOverhangLength");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
BeamDump::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: object for origin
  */
{
  ELog::RegMethod RegA("BeamDump","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

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
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(frontWallLength),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(frontWallWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(frontWallWidth/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(frontWallDepth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(frontWallHeight),Z);

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

  std::string Out, Out1, Out2, Out3, Out4, Out5, Out6, Out7, Out8, Out9, Out10;
  // front wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // floor
  Out1=ModelSupport::getComposite(SMap,surfIndex," 2 -12 3 -4 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out1));

  Out3=ModelSupport::getComposite(SMap,surfIndex," 2 -21 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out3));

  // Floor - small steel plates
  Out4=ModelSupport::getComposite(SMap,surfIndex," 21 -22 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out4));

  Out5=ModelSupport::getComposite(SMap,surfIndex," 22 -12 3 -4 25 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out5));

  //  Floor - Al plate
  Out6=ModelSupport::getComposite(SMap,surfIndex," 2 -42 3 -4 35 -25 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out6));

  // Floor - void cell below Al plate
  Out7=ModelSupport::getComposite(SMap,surfIndex," 2 -42 3 -4 5 -35 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out7));

  // back wall
  Out2=ModelSupport::getComposite(SMap,surfIndex," 12 -42 3 -4 25 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out2));

  // Roof
  Out8=ModelSupport::getComposite(SMap,surfIndex," 51 -42 3 -4 6 -56 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out8));

  // void cell under overhead
  Out9=ModelSupport::getComposite(SMap,surfIndex," 51 -1 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out9));

  //  void cell inside shielding (vac pipe goes there)
  //Out10=ModelSupport::getComposite(SMap,surfIndex," 2 -12 3 -4 16 -6 ");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out10));


  // Out=ModelSupport::getComposite(SMap,surfIndex," 51 -42 3 -4 5 -56 ");
  addOuterSurf(Out);
  addOuterUnionSurf(Out1);
  addOuterUnionSurf(Out2);
  addOuterUnionSurf(Out3);
  addOuterUnionSurf(Out4);
  addOuterUnionSurf(Out5);
  addOuterUnionSurf(Out6);
  addOuterUnionSurf(Out7);
  addOuterUnionSurf(Out8);
  addOuterUnionSurf(Out9);
  //  addOuterUnionSurf(Out10);

  return;
}

  
void
BeamDump::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BeamDump","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  
  return;
}
  
  

  
void
BeamDump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,const long int& lp)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param lp :: link point
  */
{
  ELog::RegMethod RegA("BeamDump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // essSystem essSystem
