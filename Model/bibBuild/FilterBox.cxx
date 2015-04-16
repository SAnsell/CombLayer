/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuild/FilterBox.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "FilterBox.h"


namespace bibSystem
{

FilterBox::FilterBox(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  filterIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(filterIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}
FilterBox::FilterBox(const FilterBox& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  filterIndex(A.filterIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),width(A.width),
  height(A.height),length(A.length),angleA(A.angleA),
  angleB(A.angleB),wallThick(A.wallThick),wallGap(A.wallGap),
  beTemp(A.beTemp),beMat(A.beMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: FilterBox to copy
  */
{}

FilterBox&
FilterBox::operator=(const FilterBox& A)
  /*!
    Assignment operator
    \param A :: FilterBox to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      length=A.length;
      angleA=A.angleA;
      angleB=A.angleB;
      wallThick=A.wallThick;
      wallGap=A.wallGap;
      beTemp=A.beTemp;
      beMat=A.beMat;
      wallMat=A.wallMat;
    }
  return *this;
}

FilterBox::~FilterBox()
  /*!
    Destructor
  */
{}

void
FilterBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("FilterBox","populate");

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  angleA=Control.EvalVar<double>(keyName+"AngleA");;
  angleB=Control.EvalVar<double>(keyName+"AngleB");;  

  wallThick=Control.EvalVar<double>(keyName+"WallThick");;
  wallGap=Control.EvalVar<double>(keyName+"WallGap");;

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  beMat=ModelSupport::EvalMat<int>(Control,keyName+"BeMat");

  beTemp=Control.EvalVar<double>(keyName+"BeTemp");

  return;
}


void
FilterBox::createUnitVector(const attachSystem::FixedComp& FC,
			   const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Moderator side to attach to
  */
{
  ELog::RegMethod RegA("FilterBox","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getLinkPt(sideIndex); 

  applyShift(xStep,yStep,zStep);

  return;
}

void
FilterBox::createSurfaces()
  /*!
    Create the surface 
  */
{
  ELog::RegMethod RegA("FilterBox","createSurfaces");

  Geometry::Vec3D XRotA(X);
  Geometry::Vec3D XRotB(X);
  
  // a is an outside angle:
  Geometry::Quaternion::calcQRotDeg(-angleA,Z).rotate(XRotA);
  Geometry::Quaternion::calcQRotDeg(angleB,Z).rotate(XRotB);

  // NLayer loop
  // Outside-in system
  const double LayerV[]={-wallGap,-wallThick,0.0};
  double TThick(0.0);
  int fIndex(filterIndex);
  
  for(size_t i=0;i<3;i++)
    {
      ModelSupport::buildPlane(SMap,fIndex+1,
			       Origin-Y*TThick,Y);
      // wall inner
      ModelSupport::buildCylinder(SMap,fIndex+2,Origin,Z,
				  length+TThick); 
      ModelSupport::buildPlane(SMap,fIndex+3,
			       Origin-X*(width/2.0+TThick),XRotA);
      ModelSupport::buildPlane(SMap,fIndex+4,
			       Origin+X*(width/2.0+TThick),XRotB);
      ModelSupport::buildPlane(SMap,fIndex+5,
			       Origin-Z*(height/2.0+TThick),Z);
      ModelSupport::buildPlane(SMap,fIndex+6,
			       Origin+Z*(height/2.0+TThick),Z);
      TThick+=LayerV[i];
      fIndex+=10;
    }
  return; 
}

void
FilterBox::createObjects(Simulation& System)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
    \param CC :: Possible moderator clash
   */
{
  ELog::RegMethod RegA("FilterBox","createObjects");
  
  std::string Out;

  // FilterBox
  Out=ModelSupport::getComposite(SMap,filterIndex,"21 -22 23 -24 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,beMat,beTemp,Out));

  // Wrapper
  Out=ModelSupport::getComposite(SMap,filterIndex,
  	 "11 -12 13 -14 15 -16 (-21: 22 : -23 : 24 :-25 : 26)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,beTemp,Out));

  // Void
  Out=ModelSupport::getComposite(SMap,filterIndex,
  	 "1 -2 3 -4 5 -6 (-11: 12 : -13 : 14 :-15 : 16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,filterIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  return; 
}

void
FilterBox::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("FilterBox","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin,-Y); 
  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  // WHY HAVENT I WRITTEN AutoLinkSurf()
  FixedComp::setLinkSurf(0,-SMap.realSurf(filterIndex+1)); 
  FixedComp::setLinkSurf(1,SMap.realSurf(filterIndex+2));
  FixedComp::addLinkSurf(1,SMap.realSurf(filterIndex+1)); 
  FixedComp::setLinkSurf(2,-SMap.realSurf(filterIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(filterIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(filterIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(filterIndex+6));
  
  return;
}

void
FilterBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const size_t sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Side index
   */
{
  ELog::RegMethod RegA("FilterBox","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();;
  createObjects(System);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
