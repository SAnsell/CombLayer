/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomOpenStack.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ZoomOpenStack.h"

namespace zoomSystem
{

ZoomOpenStack::ZoomOpenStack(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,2),
  stackIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(stackIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ZoomOpenStack::ZoomOpenStack(const ZoomOpenStack& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  stackIndex(A.stackIndex),cellIndex(A.cellIndex),
  nItem(A.nItem),posIndex(A.posIndex),width(A.width),
  height(A.height),length(A.length),wallThick(A.wallThick),
  windowThick(A.windowThick),wallMat(A.wallMat),
  windowMat(A.windowMat),voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: ZoomOpenStack to copy
  */
{}

ZoomOpenStack&
ZoomOpenStack::operator=(const ZoomOpenStack& A)
  /*!
    Assignment operator
    \param A :: ZoomOpenStack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      nItem=A.nItem;
      posIndex=A.posIndex;
      width=A.width;
      height=A.height;
      length=A.length;
      wallThick=A.wallThick;
      windowThick=A.windowThick;
      wallMat=A.wallMat;
      windowMat=A.windowMat;
      voidCell=A.voidCell;
    }
  return *this;
}


ZoomOpenStack::~ZoomOpenStack() 
 /*!
   Destructor
 */
{}

void
ZoomOpenStack::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase object
 */
{
  ELog::RegMethod RegA("ZoomOpenStack","populate");
  
  // Master values

  nItem=Control.EvalVar<size_t>(keyName+"NItem");
  posIndex=Control.EvalVar<size_t>(keyName+"Index");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  windowThick=Control.EvalVar<double>(keyName+"WindowThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  windowMat=Control.EvalVar<int>(keyName+"WindowMat");


  guideOffset=Control.EvalVar<double>(keyName+"GuideOffset");
  guideSep=Control.EvalVar<double>(keyName+"GuideSep");
  guideHeight=Control.EvalVar<double>(keyName+"GuideHeight");
  guideWidth=Control.EvalVar<double>(keyName+"GuideWidth");
  guideThick=Control.EvalVar<double>(keyName+"GuideThick");
  guideMat=ModelSupport::EvalMat<int>(Control,keyName+"GuideMat");
  
  return;
}
  
void
ZoomOpenStack::createUnitVector(const attachSystem::TwinComp& TT)
  /*!
    Create the unit vectors- Y Down the beamline
    \param TT :: Twin item 
  */
{
  ELog::RegMethod RegA("ZoomOpenStack","createUnitVector");
  TwinComp::createUnitVector(TT);
  
  Origin=TT.getBeamStart();
  applyShift(xStep,yStep,zStep);  
  bEnter=TT.getBeamStart();
  bExit=bEnter+Y*length;
  
  return;
}

void
ZoomOpenStack::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomOpenStack","createSurface");

  // Create Outer surfaces
  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,stackIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,stackIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,stackIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,stackIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,stackIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,stackIndex+6,Origin+Z*height/2.0,Z);

  // walls
  ModelSupport::buildPlane(SMap,stackIndex+11,
			   Origin-Y*windowThick,Y);
  ModelSupport::buildPlane(SMap,stackIndex+12,
			   Origin+Y*(length+windowThick),Y);
  ModelSupport::buildPlane(SMap,stackIndex+13,
			   Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,stackIndex+14,
			   Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,stackIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,stackIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);

  Geometry::Vec3D UnitO=bEnter-bX*(guideSep*
				   static_cast<double>(posIndex));
  // Stack cut position
  ModelSupport::buildPlane(SMap,stackIndex+101,
			   Origin+Y*guideOffset,Y);
  ModelSupport::buildPlane(SMap,stackIndex+102,
			   Origin+Y*(length-guideOffset),Y);

  int SI(stackIndex+100);
  for(size_t i=0;i<nItem;i++)
    {
      ModelSupport::buildPlane(SMap,SI+3,
			       UnitO-bX*(guideWidth/2.0),bX);
      ModelSupport::buildPlane(SMap,SI+4,
			       UnitO+bX*(guideWidth/2.0),bX);
      ModelSupport::buildPlane(SMap,SI+13,
			       UnitO-bX*(guideWidth/2.0+guideThick),bX);
      ModelSupport::buildPlane(SMap,SI+14,
			       UnitO+bX*(guideWidth/2.0+guideThick),bX);

      ModelSupport::buildPlane(SMap,SI+5,
			       UnitO-bZ*(guideHeight/2.0),bZ);
      ModelSupport::buildPlane(SMap,SI+6,
			       UnitO+bX*(guideHeight/2.0),bZ);
      ModelSupport::buildPlane(SMap,SI+15,
			       UnitO-bZ*(guideHeight/2.0+guideThick),bZ);
      ModelSupport::buildPlane(SMap,SI+16,
			       UnitO+bZ*(guideHeight/2.0+guideThick),bZ);
      SI+=100;
      UnitO+=bX*guideSep;
    }
  
  return;
}

void
ZoomOpenStack::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZoomOpenStack","createObjects");
  
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,stackIndex,"11 -12 13 -14 15 -16");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,stackIndex,"11 -12 13 -14 15 -16 "
                              " (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));


  int SI(stackIndex+100);
  HeadRule MidVoid;
  for(size_t i=0;i<nItem;i++)
    {
      // inner void
      Out=ModelSupport::getComposite(SMap,SI,stackIndex,
				     "101M -102M 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      Out=ModelSupport::getComposite(SMap,SI,stackIndex,
				     "101M -102M (-3:4:-5:6) 13 -14 15 -16 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,guideMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,SI,stackIndex,
				     "101M -102M 13 -14 15 -16 ");
      MidVoid.addIntersection(Out);
    }

    // void for units
  Out=ModelSupport::getComposite(SMap,stackIndex,"1 -2 3 -4 5 -6 ");
  MidVoid.makeComplement();
  Out+=MidVoid.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

void
ZoomOpenStack::createAll(Simulation& System,const int vCell,
			 const attachSystem::TwinComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param vCell :: Void cell to build object in
    \param FC :: Beamline track to place object within
  */
{
  ELog::RegMethod RegA("ZoomOpenStack","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  addInsertCell(vCell);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE moderatorSystem
