/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/LinkSupport.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <deque>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "LinkSupport.h"

namespace attachSystem
{


  
int
getAttachPoint(const objectGroups& OGrp,
	       const std::string& FCName,
	       const std::string& linkName,
	       Geometry::Vec3D& Pt,
	       Geometry::Vec3D& YAxis)
  /*!
    Takes the linkName and the fixed object and converts
    this into the direction and point.
    - Note that link points are +1 offset and 
    \param OGrp :: Object groups
    \param FCName :: Name for the fixed object
    \param linkName :: Name/number for the link point
    \param Pt :: Link point [out]
    \param YAxis :: Y Out
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("LinkSupport","getAttachPoint");


  const attachSystem::FixedComp* FC=
    OGrp.getObject<attachSystem::FixedComp>(FCName);
  if (!FC) return 0;

  const long int index=FC->getSideIndex(linkName);
  Pt=FC->getLinkPt(index);
  YAxis=FC->getLinkAxis(index);
  return 1;
}

int
getAttachPointWithXYZ(const objectGroups& OGrp,
		      const std::string& FCName,
                      const std::string& linkName,
                      Geometry::Vec3D& Pt,
                      Geometry::Vec3D& XAxis,
                      Geometry::Vec3D& YAxis,
                      Geometry::Vec3D& ZAxis)
  /*!
    Takes the linkName and the fixed object and converts
    this into the direction and point.
    - Note that link points are +1 offset and 
    \param FCName :: Name for the fixed object
    \param linkName :: Name/number for the link point
    \param Pt :: Link point [out]
    \param XAxis :: X Out
    \param YAxis :: Y Out
    \param ZAxis :: Z Out
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("LinkSupport","getAttachPointWithXYZ");


  const FixedComp* FC=
    OGrp.getObject<attachSystem::FixedComp>(FCName);
  if (!FC) return 0;

  const long int index=FC->getSideIndex(linkName);
  // All these calls throw on error
  Pt=FC->getLinkPt(index);
  YAxis=FC->getLinkAxis(index);
  ZAxis=FC->getZ();

  FixedComp::computeZOffPlane(XAxis,YAxis,ZAxis);
  XAxis= (YAxis*ZAxis).unit();

  return 1;
}

size_t 
getPoint(const objectGroups& OGrp,
	 const std::vector<std::string>& StrItem,
	 const size_t index,
	 Geometry::Vec3D& Pt)
  /*!
    Get a vector based on a StrItem either using a
    a name unit or a value.
    \param StrItem :: List of strings
    \param index :: Place to start in list
    \param Pt :: Point found
    \retval  0 on failure
    \retval  1 basic point
    \retval  2 FixedComp+ axis
    \retval  3 itemed evector
   */
{
  ELog::RegMethod RegA("LinkSupport[F]","getPoint");

  const size_t NS(StrItem.size());
  // Simple Vec3D(a,b,c)
  if (NS>=index && StrFunc::convert(StrItem[index],Pt) )
    return 1;

  // Test of FixedPoint link
  if (NS >= index+2)
    {
      Geometry::Vec3D YAxis;  
      if (attachSystem::getAttachPoint
	  (OGrp,StrItem[index],StrItem[index+1],Pt,YAxis))
	return 2;
    }
  
  // Simple vector
  if (NS >= index+3 && StrFunc::convert(StrItem[index],Pt[0])
	   && StrFunc::convert(StrItem[index+1],Pt[1])
	   && StrFunc::convert(StrItem[index+2],Pt[2]) )
    return 3;

  return 0;
}

void
calcBoundaryLink(attachSystem::FixedComp& FC,const size_t linkIndex,
		 const HeadRule& boundary,
		 const Geometry::Vec3D& Origin,
		 const Geometry::Vec3D& Axis)
  /*!
    Calculates the intersecting surface and point 
    of a line with a headRule. The surfaces is the out
    going surface and the point is the intersection point
    along with the axis.  
    \param FC :: FixedComp to add link point to
    \param linkIndex :: link Index    
    \param boundary :: link Index
    \param Origin :: Origin of line
    \param Axis :: Axis of line
   */
{
  ELog::RegMethod RegA("LinkSupport[F]","calcBoundaryLink");

  double D;
  const int SN=boundary.trackSurf(Origin,Axis,D);
  if (SN)
    {
      FC.setLinkSurf(linkIndex,SN);
      FC.setConnect(linkIndex,Origin+Axis*D,Axis);
    }
  return;
}


  
}  // NAMESPACE attachSystem
