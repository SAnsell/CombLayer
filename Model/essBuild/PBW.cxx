/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PBW.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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

#include "PBW.h"

namespace essSystem
{

PBW::PBW(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PBW::PBW(const PBW& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  shieldNSegments(A.shieldNSegments),
  shieldSegmentLength(A.shieldSegmentLength),
  shieldSegmentRad(A.shieldSegmentRad),
  plugLength(A.plugLength),plugWidth1(A.plugWidth1),
  plugWidth2(A.plugWidth2),
  plugHeight(A.plugHeight),
  plugDepth(A.plugDepth),
  plugMat(A.plugMat),
  plugVoidLength(A.plugVoidLength),
  plugVoidWidth(A.plugVoidWidth),
  plugVoidDepth(A.plugVoidDepth),
  plugVoidHeight(A.plugVoidHeight),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: PBW to copy
  */
{}

PBW&
PBW::operator=(const PBW& A)
  /*!
    Assignment operator
    \param A :: PBW to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      shieldNSegments=A.shieldNSegments;
      shieldSegmentLength=A.shieldSegmentLength;
      shieldSegmentRad=A.shieldSegmentRad;
      plugLength=A.plugLength;
      plugWidth1=A.plugWidth1;
      plugWidth2=A.plugWidth2;
      plugHeight=A.plugHeight;
      plugDepth=A.plugDepth;
      plugMat=A.plugMat;
      plugVoidLength=A.plugVoidLength;
      plugVoidWidth=A.plugVoidWidth;
      plugVoidDepth=A.plugVoidDepth;
      plugVoidHeight=A.plugVoidHeight;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

PBW*
PBW::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PBW(*this);
}

PBW::~PBW()
  /*!
    Destructor
  */
{}

void
PBW::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PBW","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  shieldNSegments=Control.EvalVar<size_t>(keyName+"ShieldNSegments");

  plugLength=Control.EvalVar<double>(keyName+"PlugLength");
  plugWidth1=Control.EvalVar<double>(keyName+"PlugWidth1");
  plugWidth2=Control.EvalVar<double>(keyName+"PlugWidth2");
  plugHeight=Control.EvalVar<double>(keyName+"PlugHeight");
  plugDepth=Control.EvalVar<double>(keyName+"PlugDepth");
  plugMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugMat");
  plugVoidLength=Control.EvalVar<double>(keyName+"PlugVoidLength");
  plugVoidWidth=Control.EvalVar<double>(keyName+"PlugVoidWidth");
  plugVoidDepth=Control.EvalVar<double>(keyName+"PlugVoidDepth");
  plugVoidHeight=Control.EvalVar<double>(keyName+"PlugVoidHeight");
  wallThick=1;//Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  double l,r;
  for (size_t i=0; i<shieldNSegments; i++)
    {
      l = Control.EvalVar<double>(keyName+"ShieldSegmentLength"+std::to_string(i+1));
      shieldSegmentLength.push_back(l);
      r = Control.EvalVar<double>(keyName+"ShieldSegmentRadius"+std::to_string(i+1));
      shieldSegmentLength.push_back(r);
    }

  return;
}

void
PBW::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: object for origin
  */
{
  ELog::RegMethod RegA("PBW","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
PBW::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PBW","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(plugLength/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(plugLength/2.0),Y);

  const double alpha = atan((plugWidth1-plugWidth2)/2.0/plugLength)*180/M_PI;

  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+3,
				  Origin-X*(plugWidth1/2.0)+Y*(plugLength/2.0),
				  X,Z,alpha);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+4,
				  Origin+X*(plugWidth1/2.0)+Y*(plugLength/2.0),
				  X,Z,-alpha);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(plugDepth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(plugHeight),Z);

  return;
}

void
PBW::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PBW","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  addOuterSurf(Out);

  return;
}


void
PBW::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PBW","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  return;
}




void
PBW::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,const long int& lp)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param lp :: link point
  */
{
  ELog::RegMethod RegA("PBW","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);

  return;
}

}  // essSystem essSystem
