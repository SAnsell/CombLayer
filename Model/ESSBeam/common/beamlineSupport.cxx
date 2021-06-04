/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/common/beamlineSupport.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"


namespace essBeamSystem
{

void
setBeamAxis(attachSystem::FixedOffset& beamAxis,
	    const FuncDataBase& Control,
	    const attachSystem::FixedGroup& GItem,
	    const int reverseZ)
  /*!
    Set the primary direction object
    \param beamAxis :: axis to rotate
    \param Control :: Database of variables
    \param GItem :: Guide Item to copy
    \param reverseZ :: Reverse axis [-1 Z is -ve / 0 no change / 1 Z is +ve]
  */
{
  ELog::RegMethod RegA("beamlineSupport","setBeamAxis");
  
  beamAxis.populate(Control);
  beamAxis.createUnitVector(GItem,0);
  beamAxis.setLinkCopy(0,GItem.getKey("Main"),1);
  beamAxis.setLinkCopy(1,GItem.getKey("Main"),2);
  beamAxis.setLinkCopy(2,GItem.getKey("Beam"),1);
  beamAxis.setLinkCopy(3,GItem.getKey("Beam"),2);

  // change to unsigned !!!
  beamAxis.linkShift(2); 
  beamAxis.linkShift(3);
  beamAxis.linkAngleRotate(2);
  beamAxis.linkAngleRotate(3);

  beamAxis.applyOffset();

  if (reverseZ)
    {
      const Geometry::Vec3D& ZVert(beamAxis.getZ());
      if ( (reverseZ>0 && ZVert.Z() < -Geometry::zeroTol) ||
	   (reverseZ<0 && ZVert.Z() > Geometry::zeroTol) )
	beamAxis.reverseZ();	
    }
  
  return;
}

} // NAMESPACE essSystem

