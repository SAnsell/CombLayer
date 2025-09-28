/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/PowerFilter.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Quaternion.h"
#include "Line.h"
#include "BeamAxis.h"

#include "PowerFilter.h"

namespace xraySystem
{

PowerFilter::PowerFilter(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  constructSystem::BeamAxis()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PowerFilter::PowerFilter(const PowerFilter& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  constructSystem::BeamAxis(A),
  maxLength(A.maxLength),width(A.width),height(A.height),
  baseLength(A.baseLength),
  wedgeAngle(A.wedgeAngle),
  mat(A.mat),
  holderMat(A.holderMat)
  /*!
    Copy constructor
    \param A :: PowerFilter to copy
  */
{}

PowerFilter&
PowerFilter::operator=(const PowerFilter& A)
  /*!
    Assignment operator
    \param A :: PowerFilter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      constructSystem::BeamAxis::operator=(A);
      maxLength=A.maxLength;
      baseLength=A.baseLength;
      wedgeAngle=A.wedgeAngle;
      width=A.width;
      height=A.height;
      mat=A.mat;
      holderMat=A.holderMat;
    }
  return *this;
}

PowerFilter*
PowerFilter::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PowerFilter(*this);
}

PowerFilter::~PowerFilter()
  /*!
    Destructor
  */
{}

void
PowerFilter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PowerFilter","populate");

  FixedRotate::populate(Control);

  maxLength=Control.EvalVar<double>(keyName+"MaxLength");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  wedgeAngle=Control.EvalVar<double>(keyName+"WedgeAngle");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  baseHeight=Control.EvalVar<double>(keyName+"BaseHeight");
  filterZOffset=Control.EvalVar<double>(keyName+"FilterZOffset");
  filterGap=Control.EvalVar<double>(keyName+"FilterGap");
  holderWidth=Control.EvalVar<double>(keyName+"HolderWidth");
  holderLength=Control.EvalVar<double>(keyName+"HolderLength");
  holderHeight=Control.EvalVar<double>(keyName+"HolderHeight");
  holderDepth=Control.EvalVar<double>(keyName+"HolderDepth");
  holderGapWidth=Control.EvalVar<double>(keyName+"HolderGapWidth");
  holderGapHeight=Control.EvalVar<double>(keyName+"HolderGapHeight");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  holderMat=ModelSupport::EvalMat<int>(Control,keyName+"HolderMat");

  return;
}

void
PowerFilter::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PowerFilter","createSurfaces");

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);

  const double totalHeight = height+baseHeight;
  // thickness of the blade in the middle
  const double b1 = maxLength - totalHeight*tan(wedgeAngle*M_PI/180.0)/2.0;

  const double yshift = filterGap/2.0+b1;
  const double dz = filterZOffset/2.0;

  // upstream filter
  /// blade
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(yshift),Y);
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*(yshift+baseLength),Y);

  Geometry::Vec3D v(Y);
  Geometry::Quaternion::calcQRotDeg(-wedgeAngle,X).rotate(v);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(maxLength-yshift)+Z*(totalHeight/2.0+dz),v);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(totalHeight/2.0-dz),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(dz+totalHeight/2.0-baseHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,Origin+Z*(dz+totalHeight/2.0),Z);

  /// holder
  ModelSupport::buildShiftedPlane(SMap,buildIndex+121,buildIndex+111,Y,-holderLength);
  ModelSupport::buildPlane(SMap,buildIndex+123,Origin-X*(holderWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+124,Origin+X*(holderWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+125,Origin-Z*(holderDepth-dz),Z);
  ModelSupport::buildPlane(SMap,buildIndex+126,Origin+Z*(holderHeight+dz),Z);

  const double R=holderGapWidth/2.0;
  ModelSupport::buildPlane(SMap,buildIndex+143,Origin-X*(R),X);
  ModelSupport::buildPlane(SMap,buildIndex+144,Origin+X*(R),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+135,buildIndex+126,Y,-holderGapHeight+R);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin+Z*(holderHeight-holderGapHeight+R+dz),Y,R);

  // downstream filter
  /// blade
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin+Y*(yshift),Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin+Y*(yshift+baseLength),Y);

  Geometry::Vec3D w(Y);
  Geometry::Quaternion::calcQRotDeg(-wedgeAngle,X).rotate(w);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(-maxLength+yshift)-Z*(totalHeight/2.0+dz),w);

  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(totalHeight/2.0+dz),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin-Z*(totalHeight/2.0-baseHeight+dz),Z);
  ModelSupport::buildPlane(SMap,buildIndex+216,Origin+Z*(totalHeight/2.0-dz),Z);

  /// holder
  ModelSupport::buildShiftedPlane(SMap,buildIndex+221,buildIndex+211,Y,holderLength);
  ModelSupport::buildPlane(SMap,buildIndex+225,Origin-Z*(holderHeight+dz),Z);
  ModelSupport::buildPlane(SMap,buildIndex+226,Origin+Z*(holderDepth-dz),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+235,buildIndex+226,Y,-holderGapHeight+R);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin+Z*(holderDepth-holderGapHeight+R-dz),Y,R);


  return;
}

void
PowerFilter::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PowerFilter","createObjects");

  const HeadRule lrHR = ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");

  HeadRule HR;
  // upstream filter
  /// blade
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -101 106 -116 ");
  makeCell("Base",System,cellIndex++,mat,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -101 105 -106 ");
  makeCell("VoidBeforeBlade",System,cellIndex++,0,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 105 -116 ");
  makeCell("Blade",System,cellIndex++,mat,0.0,HR*lrHR);

  /// holder
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -111 123 -143 135 -126");
  makeCell("UpstreamHolder",System,cellIndex++,holderMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -111 144 -124 135 -126");
  makeCell("UpstreamHolder",System,cellIndex++,holderMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -111 123 -124 125 -135 107");
  makeCell("UpstreamHolder",System,cellIndex++,holderMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -111 143 -144 135 -126 ");
  makeCell("UpstreamHolderGapRec",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -111 -135 -107 ");
  makeCell("UpstreamHolderGapCyl",System,cellIndex++,0,0.0,HR);


  // downstream filter
  /// blade
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -211 205 -206 ");
  makeCell("Base",System,cellIndex++,mat,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -211 206 -216 ");
  makeCell("VoidBeforeBlade",System,cellIndex++,0,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -201 205 -216 ");
  makeCell("Blade",System,cellIndex++,mat,0.0,HR*lrHR);

  /// holder
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 211 -221 123 -143 235 -226");
  makeCell("DownstreamHolder",System,cellIndex++,holderMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 211 -221 144 -124 235 -226");
  makeCell("DownstreamHolder",System,cellIndex++,holderMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 211 -221 123 -124 225 -235 207");
  makeCell("DownstreamHolder",System,cellIndex++,holderMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 211 -221 143 -144 235 -226 ");
  makeCell("DownstreamHolderGapRec",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 211 -221 -235 -207 ");
  makeCell("DownstreamHolderGapCyl",System,cellIndex++,0,0.0,HR);

  // aux voids
  /// front
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -102 123 -124 125 -135 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -102 3 -4 135 -105 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -102 4 -124 135 -116 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -102 123 -3 135 -116 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 111 -102 123 -124 116 -126 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  /// back
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -211 123 -124 225 -205 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -211 4 -124 205 -216 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -211 123 -3 205 -216 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -211 123 -124 216 -226 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  //
  bool A = holderDepth - holderHeight - filterZOffset > 0.0;
  const int nzmin = A ? 125 : 225;
  const int nzmax = A ? 226 : 126;
  HeadRule HRzmin = ModelSupport::getHeadRule(SMap,buildIndex,std::to_string(nzmin));
  HeadRule HRzmax = ModelSupport::getHeadRule(SMap,buildIndex,std::to_string(-nzmax));

  // void between the two blades
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 102 -202 123 -124 125 -226 ");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  if (A) { // void below downstream blade
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"  202 -221 123 -124 125 -225 ");
    makeCell("Void",System,cellIndex++,0,0.0,HR);
  } else { // void below upstream blade
    HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -202 123 -124 225 -125 ");
    makeCell("Void",System,cellIndex++,0,0.0,HR);
  }

  if (A) { // void above upstream blade
    HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -102 123 -124 126 -226 ");//
    makeCell("Void",System,cellIndex++,0,0.0,HR);
  } else { //void above downstream blade
    HR=ModelSupport::getHeadRule(SMap,buildIndex," 102 -221 123 -124 226 -126");
    makeCell("Void",System,cellIndex++,0,0.0,HR);
  }

  /// two blades
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 121 -221 123 -124 ");
  addOuterSurf(HR*HRzmin*HRzmax);


  return;
}


void
PowerFilter::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("PowerFilter","createLinks");

  // same definition as createSurfaces
  const double totalHeight = height+baseHeight;
  const double b1 = maxLength - totalHeight*tan(wedgeAngle*M_PI/180.0)/2.0;
  const double dz = filterZOffset/2.0;
  const double yshift = filterGap/2.0+b1;

  FixedComp::setConnect(0,Origin-Y*(yshift+baseLength-holderLength),-Y);
  FixedComp::setNamedLinkSurf(0,"Back",-SMap.realSurf(buildIndex+121));

  FixedComp::setConnect(1,Origin+Y*(yshift+baseLength+holderLength),Y);
  FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+221));

  FixedComp::setConnect(2,Origin-X*(holderWidth/2.0),-X);
  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+123));

  FixedComp::setConnect(3,Origin+X*(holderWidth/2.0),X);
  FixedComp::setNamedLinkSurf(2,"Right",SMap.realSurf(buildIndex+124));

  // same code as in buildObjects
  bool A = holderDepth - holderHeight - filterZOffset > 0.0;
  const int nzmin = A ? 125 : 225;
  const int nzmax = A ? 226 : 126;

  const double zmin = A ? holderDepth-dz : holderHeight+dz;
  const double zmax = A ? holderDepth-dz : holderHeight+dz;

  FixedComp::setConnect(4,Origin-Z*(zmin),-Z);
  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+nzmin));

  FixedComp::setConnect(5,Origin+Z*(zmax),Z);
  FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+nzmax));

  return;
}

void
PowerFilter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PowerFilter","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
