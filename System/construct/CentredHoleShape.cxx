/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/CentredHoleShape.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "HoleShape.h"
#include "CentredHoleShape.h"

namespace constructSystem
{

CentredHoleShape::CentredHoleShape(const std::string& Key) :
  HoleShape(Key),
  angleOffset(0),radialStep(0.0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

CentredHoleShape::CentredHoleShape(const CentredHoleShape& A) : 
  HoleShape(A),
  masterAngle(A.masterAngle),angleCentre(A.angleCentre),
  angleOffset(A.angleOffset),radialStep(A.radialStep),
  rotCentre(A.rotCentre),rotAngle(A.rotAngle)
  /*!
    Copy constructor
    \param A :: CentredHoleShape to copy
  */
{}

CentredHoleShape&
CentredHoleShape::operator=(const CentredHoleShape& A)
  /*!
    Assignment operator
    \param A :: CentredHoleShape to copy
    \return *this
  */
{
  if (this!=&A)
    {
      HoleShape::operator=(A);
      masterAngle=A.masterAngle;
      angleCentre=A.angleCentre;
      angleOffset=A.angleOffset;
      radialStep=A.radialStep;
      rotCentre=A.rotCentre;
      rotAngle=A.rotAngle;
    }
  return *this;
}

void
CentredHoleShape::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CentredHoleShape","populate");

  HoleShape::populate(Control);

  radialStep=Control.EvalDefVar<double>(keyName+"RadialStep",0.0);
  angleCentre=Control.EvalDefVar<double>(keyName+"AngleCentre",0.0);
  angleOffset=Control.EvalDefVar<double>(keyName+"AngleOffset",0.0);

  masterAngle=
    Control.EvalDefVar<double>(keyName+"MasterAngle",masterAngle);
  rotAngle=masterAngle+angleCentre;
      
  return;
}

void
CentredHoleShape::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors:
    LC gives the origin for the wheel
    \param FC :: Rotational origin of wheel
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("CentredHoleShape","createUnitVector");

  // we apply main hole rotation/ displacement
  // then convert from Centre to hole (if required)

  FixedRotate::createUnitVector(FC,sideIndex);


  rotCentre=Origin;
  
  // Now need to move origin to centre of shape

  const Geometry::Quaternion Qy=
    Geometry::Quaternion::calcQRotDeg(rotAngle,Y);
  Qy.rotate(X);
  Qy.rotate(Z);
  FixedComp::applyShift(0,0,radialStep);

  // But now apply angleCentre rotation for the shape itself

  const Geometry::Quaternion QyPrime=
    Geometry::Quaternion::calcQRotDeg(angleOffset,Y);
  QyPrime.rotate(X);
  QyPrime.rotate(Z);
  return;
}

void
CentredHoleShape::setMasterAngle(const double MA)
  /*!
    Given the main angle for the system
    \param MA :: master angle for the wheel
  */
{
  ELog::RegMethod RegA("CentredHoleShape","setMasterAngle");

  masterAngle=MA;
  rotAngle=masterAngle+angleOffset;
  return;
}

  
}  // NAMESPACE constructSystem
