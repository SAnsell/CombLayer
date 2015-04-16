/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/createDivide.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/bind.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Rules.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "SurInter.h"
#include "createDivide.h"

namespace ModelSupport
{

Geometry::Plane*
createDividerSurf(const int N,const Geometry::Vec3D& Z,
		  const Geometry::Plane* APn,const Geometry::Plane* BPn,
		  const Geometry::Plane* CPn,const Geometry::Plane* DPn)
  /*!
    Creates a line between two intersecting planes and build
    a surface with parallel to Z
    \param N :: New surface number
    \param Z :: Direction to ignore 
    \param APn :: First Plane [A-B pair]
    \param BPn :: Second Plane [A-B pair]
    \param CPn :: First Plane [C-D pair]
    \param DPn :: Second Plane [C-D pair]
    \return Divide Plane
  */
{
  ELog::RegMethod RegA("createDivide","createDividerSurf");

  if (!APn || !BPn || !CPn || !DPn)
    {
      ELog::EM<<"All surfaces not set"<<ELog::endCrit;
      if (APn) ELog::EM<<"APn Set: "<<*APn<<ELog::endCrit;
      if (BPn) ELog::EM<<"BPn Set: "<<*BPn<<ELog::endCrit;
      if (CPn) ELog::EM<<"CPn Set: "<<*CPn<<ELog::endCrit;
      if (DPn) ELog::EM<<"DPn Set: "<<*DPn<<ELog::endCrit;
      ELog::EM<<ELog::endErr;
      throw ColErr::ExBase(0,RegA.getFull());
    }
  // Create Null Plane
  const Geometry::Plane ZPlane(1,0,Geometry::Vec3D(0,0,0),Z);
  std::vector<Geometry::Vec3D> PtA=SurInter::makePoint(&ZPlane,APn,BPn);
  if (PtA.size()!=1)
    {
      ELog::EM<<"Failed to intersect "<<*APn<<" with "<<*BPn<<ELog::endErr;
      throw ColErr::ExBase(0,RegA.getFull());
    }
  const Geometry::Vec3D PA=PtA.front();
  PtA=SurInter::makePoint(&ZPlane,CPn,DPn);
  if (PtA.size()!=1)
    {
      ELog::EM<<"Failed to intersect "<<*CPn<<" with "<<*DPn<<ELog::endErr;
      throw ColErr::ExBase(1,RegA.getFull());
    }
  const Geometry::Vec3D PB=PtA.front();

  // Create Surface:
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N); 
  PX->setPlane(PA,PB,PB+Z);
  return PX;  
}
  

} // NAMESPACE ModelSupport
