/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/TwinComp.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "Rules.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"

namespace attachSystem
{

TwinComp::TwinComp(const std::string& KN,const size_t NL) : 
  FixedComp(KN,NL),SecondTrack()
 /*!
    Constructor
    \param KN :: KeyName
    \param NL :: number of links
  */
{}

TwinComp::TwinComp(const TwinComp& A) : 
  FixedComp(A),SecondTrack(A)
  /*!
    Copy constructor
    \param A :: TwinComp to copy
  */
{}

TwinComp&
TwinComp::operator=(const TwinComp& A)
  /*!
    Assignment operator
    \param A :: TwinComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedComp::operator=(A);
      SecondTrack::operator=(A);
    }
  return *this;
}


void
TwinComp::createUnitVector(const FixedComp& FC,
			   const SecondTrack& SC)
  /*!
    Create the unit vectors
    \param FC :: FixedComp [Takes origin + directions]
    \param SC :: Beam component [Takes exit as enterance]
  */
{
  ELog::RegMethod RegA("TwinComp","createUnitVector(FC,ST)");

  FixedComp::createUnitVector(FC);
  SecondTrack::createUnitVector(SC);

  return;
}

void
TwinComp::createUnitVector(const TwinComp& TT)
  /*!
    Create the unit vectors : Uses the exit point
    of the TT to set origin:
    \param TT :: Relative to another TwinComp
  */
{
  ELog::RegMethod RegA("TwinComp","createUnitVector(TwinComp)");

  
  FixedComp::createUnitVector(TT);
  Origin=TT.getExit();

  SecondTrack::createUnitVector(static_cast<SecondTrack>(TT));
  return;
}
  

void
TwinComp::applyRotation(const Geometry::Vec3D& Axis,
			 const double Angle)
  /*!
    Apply a rotation to the basis set
    \param Axis :: rotation axis 
    \param Angle :: rotation angle
  */
{
  ELog::RegMethod RegA("TwinComp","applyRotation");

  FixedComp::applyRotation(Axis,Angle);
  SecondTrack::applyRotation(Axis,Angle);
  return;
}
  
void
TwinComp::setBeamExit(const int SN,const Geometry::Vec3D& Start,
		     const Geometry::Vec3D& DirectY)
  /*!
    Use a line intersect to get the intersection diection
    \param Start :: Start point of line
    \param SN :: Surface to interect
    \param DirectY :: direction
   */
{
  ELog::RegMethod RegA("TwinComp","setBeamExit");

  // Get Beamline intersection point [exact]
  MonteCarlo::LineIntersectVisit AxisLine(Start,DirectY);
  const Geometry::Quadratic* OutPlane=
    SMap.realPtr<const Geometry::Quadratic>(SN);
  SecondTrack::setBeamExit(AxisLine.getPoint(OutPlane,Start),DirectY);
  return;
}

void
TwinComp::report() const
  /*!
    Report all the position to the log stream
   */
{
  // NO ELog -- so reporting can 
  const masterRotate& MR=masterRotate::Instance();
  ELog::EM<<"------------------------------------"<<ELog::endDebug;
  ELog::EM<<"Origin =="<<MR.calcRotate(Origin)<<ELog::endTrace;
  ELog::EM<<"X      =="<<MR.calcAxisRotate(X)<<ELog::endTrace;
  ELog::EM<<"Y      =="<<MR.calcAxisRotate(Y)<<ELog::endTrace;
  ELog::EM<<"Z      =="<<MR.calcAxisRotate(Z)<<ELog::endTrace;

  ELog::EM<<"BEnter =="<<MR.calcRotate(bEnter)<<ELog::endTrace;
  ELog::EM<<"BExit  =="<<MR.calcRotate(bExit)<<ELog::endTrace;
  ELog::EM<<"BX     =="<<MR.calcAxisRotate(bX)<<ELog::endTrace;
  ELog::EM<<"BY     =="<<MR.calcAxisRotate(bY)<<ELog::endTrace;
  ELog::EM<<"BZ     =="<<MR.calcAxisRotate(bZ)<<ELog::endTrace;
  ELog::EM<<"------------------------------------"<<ELog::endTrace;

  return;
  
}
  
}  // NAMESPACE attachSystem
