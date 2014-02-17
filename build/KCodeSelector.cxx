/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/KCodeSelector.cxx
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
#include <boost/array.hpp>

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
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "KCode.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "shutterBlock.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "inputParam.h"
#include "BulkShield.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "Convex.h"
#include "Torpedo.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "ColBox.h"
#include "Collimator.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "InnerWall.h"
#include "Hutch.h"
#include "KGroup.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "KCodeSelector.h"


namespace SDef
{

void 
kcodeSelection(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    Build the source based on the input parameter table
    \param System :: Simulation to use
    \param IParam :: Input parameter
    \todo Finish...
  */
{
  ELog::RegMethod RegA("KCodeSelector","kcodeSelection");

  // NOT FINISHED CODE:
  const FuncDataBase& Control=System.getDataBase();
  SDef::KCode& KCard=System.getPC().getKCodeCard();

  return;
}

} // NAMESPACE SDef
