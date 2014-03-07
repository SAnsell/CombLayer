/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   d4cModel/BellJar.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>

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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BellJar.h"

namespace d4cSystem
{

BellJar::BellJar(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  bellIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bellIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BellJar::BellJar(const BellJar& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  bellIndex(A.bellIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),radius(A.radius),
  wallThick(A.wallThick),height(A.height),wallMat(A.wallMat),
  colRadius(A.colRadius),colWidth(A.colWidth),
  colFront(A.colFront),colBack(A.colBack),colMat(A.colMat),
  colAngle(A.colAngle),innerVoid(A.innerVoid),
  midVoid(A.midVoid)
  /*!
    Copy constructor
    \param A :: BellJar to copy
  */
{}

BellJar&
BellJar::operator=(const BellJar& A)
  /*!
    Assignment operator
    \param A :: BellJar to copy
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
      radius=A.radius;
      wallThick=A.wallThick;
      height=A.height;
      wallMat=A.wallMat;
      colRadius=A.colRadius;
      colWidth=A.colWidth;
      colFront=A.colFront;
      colBack=A.colBack;
      colMat=A.colMat;
      colAngle=A.colAngle;
      innerVoid=A.innerVoid;
      midVoid=A.midVoid;
    }
  return *this;
}


BellJar::~BellJar()
  /*!
    Destructor
   */
{}

void
BellJar::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BellJar","populate");


    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  height=Control.EvalVar<double>(keyName+"Height");

  colRadius=Control.EvalVar<double>(keyName+"ColRadius");
  colWidth=Control.EvalVar<double>(keyName+"ColWidth");
  colFront=Control.EvalVar<double>(keyName+"ColFront");
  colBack=Control.EvalVar<double>(keyName+"ColBack");
  colMat=ModelSupport::EvalMat<int>(Control,keyName+"ColMat");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  int NL(1);
  std::string KN=keyName+StrFunc::makeString("ColAngle",NL);
  while(Control.hasVariable(KN))
    {
      colAngle.push_back(Control.EvalVar<double>(KN)*M_PI/180.0);
      KN=keyName+StrFunc::makeString("ColAngle",++NL);
    }

  return;
}

void
BellJar::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("BellJar","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
BellJar::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("BellJar","createSurfaces");

  ModelSupport::buildCylinder(SMap,bellIndex+7,Origin,Z,radius);  
  ModelSupport::buildCylinder(SMap,bellIndex+17,Origin,Z,radius+wallThick);  
  // Inner void
  if ((colRadius-colFront)>0.0 && !colAngle.empty())
    ModelSupport::buildCylinder(SMap,bellIndex+27,Origin,Z,
				colRadius-colFront);  
  
  ModelSupport::buildPlane(SMap,bellIndex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,bellIndex+6,Origin+Z*height/2.0,Z);  

  ModelSupport::buildPlane(SMap,bellIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,bellIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);  

  int SI(bellIndex+100);

  for(size_t i=0;i<colAngle.size();i++)
    {
      const Geometry::Vec3D axis(Y*cos(colAngle[i])+X*sin(colAngle[i]));
      const Geometry::Vec3D alt(Y*sin(-colAngle[i])+X*cos(colAngle[i]));
      const Geometry::Vec3D Cent=Origin+axis*colRadius;
      
      ModelSupport::buildPlane(SMap,SI+3,Cent-axis*colFront,
			       Cent-(alt*colWidth),Cent-(alt*colWidth)+Z,alt);
      ModelSupport::buildPlane(SMap,SI+4,Cent-axis*colFront,
			       Cent+(alt*colWidth),Cent+(alt*colWidth)+Z,alt);
      ModelSupport::buildPlane(SMap,SI+8,Cent+axis*colBack,
			       Cent-alt*colWidth,Cent-alt*colWidth+Z,alt);
      ModelSupport::buildPlane(SMap,SI+9,Cent+axis*colBack,
			       Cent+alt*colWidth,Cent+alt*colWidth+Z,alt);
      SI+=10;
    }
  return; 
}

void
BellJar::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BellJar","createObjects");

  std::string Out;
  // First make inner/outer void/wall and top/base
  
  // Make general 
  Out=ModelSupport::getComposite(SMap,bellIndex," 15 -16 -17 ");
  addOuterSurf(Out);

  // Outer Wall
  Out=ModelSupport::getSetComposite(SMap,bellIndex," 15 -16 -17 (7:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Assuming a mid void boundary:
  if ((colRadius-colFront)>0.0 && !colAngle.empty())
    {
      Out=ModelSupport::getSetComposite(SMap,bellIndex," 5 -6 -27");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0.0,0.0,Out));
      innerVoid=cellIndex-1;

      Out=ModelSupport::getSetComposite(SMap,bellIndex," 5 -6 -7 27");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0.0,0.0,Out));
      midVoid=cellIndex-1;

      // Now add collimator blades
      
      MonteCarlo::Qhull* voidObj=System.findQhull(midVoid);
      if (!voidObj)
	throw ColErr::InContainerError<int>(midVoid,
					    "midVoid in System:Objects");
      int SI(bellIndex+100);
      for(size_t i=0;i<colAngle.size();i++)
	{
	  Out=ModelSupport::getComposite(SMap,SI,bellIndex,
					 " 3 -4 8 -9 5M -6M");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,colMat,0.0,Out));
	  Out=ModelSupport::getComposite(SMap,SI," (-3:4:-8:9) ");
	  voidObj->addSurfString(Out);
	  SI+=10;
	}
    }
  else
    {
      Out=ModelSupport::getSetComposite(SMap,bellIndex," 5 -6 -7");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0.0,0.0,Out));
      innerVoid=cellIndex-1;
      midVoid=0;
    }

  return; 
}

void
BellJar::createLinks()
  /*!
    Creates a full attachment set
  */
{
  
  return;
}


void
BellJar::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to add
   */
{
  ELog::RegMethod RegA("BellJar","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
