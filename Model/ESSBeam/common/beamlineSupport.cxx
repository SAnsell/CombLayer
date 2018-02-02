/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trex/beamlineSupport.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "debugMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "SecondTrack.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"

#include "beamlineSupport.h"

namespace essBeamSystem
{

void
setBeamAxis(attachSystem::FixedOffset& beamAxis,
	    const FuncDataBase& Control,
	    const attachSystem::FixedGroup& GItem,
	    const int reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Database of variables
    \param GItem :: Guide Item to copy
    \param reverseZ :: Reverse axis [-1 Z is -ve / 0 no change / 1 Z is +ve]
  */
{
  ELog::RegMethod RegA("beamlineSupport","setBeamAxis");
  
  beamAxis.populate(Control);
  beamAxis.createUnitVector(GItem);
  beamAxis.setLinkSignedCopy(0,GItem.getKey("Main"),1);
  beamAxis.setLinkSignedCopy(1,GItem.getKey("Main"),2);
  beamAxis.setLinkSignedCopy(2,GItem.getKey("Beam"),1);
  beamAxis.setLinkSignedCopy(3,GItem.getKey("Beam"),2);
  
  beamAxis.linkShift(3);
  beamAxis.linkShift(4);
  beamAxis.linkAngleRotate(3);
  beamAxis.linkAngleRotate(4);

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

