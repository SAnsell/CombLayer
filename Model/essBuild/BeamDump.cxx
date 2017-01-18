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

  wallThick(A.wallThick),
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

      wallThick=A.wallThick;
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

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

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
  //  ModelSupport::buildPlane(SMap,surfIndex+11,Origin+Y*(frontWallLength),Y);
  SMap.addMatch(surfIndex+11,SMap.realSurf(surfIndex+2));
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(frontWallLength+floorLength),Y);

  //  ModelSupport::buildPlane(SMap,surfIndex+13,Origin-X*(floorWidth/2.0),X);
  SMap.addMatch(surfIndex+13,SMap.realSurf(surfIndex+3));
  //  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(floorWidth/2.0),X);
  SMap.addMatch(surfIndex+14,SMap.realSurf(surfIndex+4));

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
  

  // back wall
  SMap.addMatch(surfIndex+41,SMap.realSurf(surfIndex+12));
  double y1=frontWallLength+floorLength+backWallLength;
  ModelSupport::buildPlane(SMap,surfIndex+42,Origin+Y*(y1),Y);

  SMap.addMatch(surfIndex+43,SMap.realSurf(surfIndex+3));
  SMap.addMatch(surfIndex+44,SMap.realSurf(surfIndex+4));

  ModelSupport::buildPlane(SMap,surfIndex+45,Origin-Z*(backWallDepth),Z);
  SMap.addMatch(surfIndex+46,SMap.realSurf(surfIndex+6));
  
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

  std::string Out, Out1, Out2, Out3, Out4, Out5;
  // front wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // floor
  Out1=ModelSupport::getComposite(SMap,surfIndex," 11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out1));

  Out3=ModelSupport::getComposite(SMap,surfIndex," 2 -21 13 -14 45 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out3));
  
  Out4=ModelSupport::getComposite(SMap,surfIndex," 21 -22 13 -14 45 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out4));

  Out5=ModelSupport::getComposite(SMap,surfIndex," 22 -41 13 -14 45 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out5));

  // back wall
  Out2=ModelSupport::getComposite(SMap,surfIndex," 41 -42 43 -44 45 -46 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out2));

  addOuterSurf(Out);
  addOuterUnionSurf(Out1);
  addOuterUnionSurf(Out2);
  addOuterUnionSurf(Out3);
  addOuterUnionSurf(Out4);
  addOuterUnionSurf(Out5);

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
