/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/LinkSupport.cxx
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
#include "ContainedGroup.h"
#include "objectRegister.h"
#include "LinkSupport.h"

namespace attachSystem
{

long int
getLinkIndex(const std::string& linkName)
  /*!
    Convert a name front back etc into a standard link number
    \param linkName :: Linkname with a leading - if name reverse
    \return link number [-ve for beamFront/beamBack]
  */
{
  ELog::RegMethod RegA("LinkSupport","getLinkIndex");

  long int linkPt(0);
  if (!StrFunc::convert(linkName,linkPt))
    {
      if (linkName=="origin") 
	linkPt= 0;
      else if (linkName=="front") 
	linkPt= 1;
      else if (linkName=="front-") 
	linkPt= -1;
      else if (linkName=="back")
	linkPt= 2;
      else if (linkName=="back-")
	linkPt= -2;
      else if (linkName=="beamFront")
	linkPt=1001;
      else if (linkName=="beamBack")
	linkPt=1002;
      else if (linkName=="beamFront-")
	linkPt= -1001;
      else if (linkName=="beamBack-")
	linkPt= -1002;
      else
	{
	  throw ColErr::InContainerError<std::string>
	    (linkName,"linkName failure");
	}
    }
  return linkPt;
}

  
int
getAttachPoint(const std::string& FCName,
	       const std::string& linkName,
	       Geometry::Vec3D& Pt,Geometry::Vec3D& YAxis)
  /*!
    Takes the linkName and the fixed object and converts
    this into the direction and point.
    - Note that link points are +1 offset and 
    \param FCName :: Name for the fixed object
    \param linkName :: Name/number for the link point
    \param Pt :: Link point [out]
    \param YAxis :: Y Out
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("LinkSupport","getAttachPoint");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FixedComp* FC=
    OR.getObject<attachSystem::FixedComp>(FCName);
  if (!FC) return 0;

  const long int index=getLinkIndex(linkName);

  // All these calls throw on error
  if (index<0)
    {
      Pt=FC->getLinkPt(static_cast<size_t>(-index));
      YAxis=-FC->getLinkAxis(static_cast<size_t>(-index));
    }
  else if (index>0)
    {
      Pt=FC->getLinkPt(static_cast<size_t>(index));
      YAxis=-FC->getLinkAxis(static_cast<size_t>(index));
    }
  else
    {
      Pt=FC->getCentre();
      YAxis=-FC->getY();
    }
  return 1;
}

}  // NAMESPACE attachSystem
