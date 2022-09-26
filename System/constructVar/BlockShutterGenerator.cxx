/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/BlockShutterGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "BlockShutterGenerator.h"

namespace setVariable
{

BlockShutterGenerator::BlockShutterGenerator() :
  NSections(8),steelNumber(4),
  shutterFStep((201.0-179.2)/2.0),
  shutterLen(179.2),
  colletHGap(0.6),colletVGap(0.6),colletFGap(0.6),
  b4cThick(0.635),b4cSpace(0.2),
  steelOffset(0.5),steelAWidth(1.0),
  steelBWidth(3.0),
  colletMat("CastIron"),b4cMat("B4C"),
  steelMat("Stainless304")

  /*!
    Constructor and defaults
  */
{}

BlockShutterGenerator::~BlockShutterGenerator()
 /*!
   Destructor
 */
{}


void
BlockShutterGenerator::generateBox
(FuncDataBase& Control,
 const std::string& keyName,
 const double xStep,const double zStep,
 const double zAngle,const double xAngle,
 const double beamXSize,const double beamZSize,
 const double beamXAngle,const double beamZAngle) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param xStep :: start offset point
    \param zStep :: start offset point
    \param xAngle :: direction of main beam
    \param zAngle :: direction of main beam
    \param beamXSize :: start B4C spacer
    \param beamZSize :: start B4C spacer
    \param beamXAngle :: change (reduction is +ve) of X size
    \param beamZAngle :: change (reduction is +ve) of Z size
  */
{
  ELog::RegMethod RegA("BlockShutterGenerator","generatorBox");

  const std::string fullName(keyName+"Shutter");
  
  
  Control.addVariable(fullName+"ColletHGap",colletHGap);
  Control.addVariable(fullName+"ColletVGap",colletVGap);
  Control.addVariable(fullName+"ColletFGap",colletFGap);
  Control.addVariable(fullName+"ColletMat",colletMat);  //   surround material

  const std::string colName=fullName+"Insert";

  Control.addVariable(colName+"NB4C",NSections);
  Control.addVariable(colName+"NSteel",steelNumber);
								
  Control.addVariable(colName+"XAngle",xAngle);   
  Control.addVariable(colName+"XStep",xStep);     
  Control.addVariable(colName+"ZAngle",zAngle);   
  Control.addVariable(colName+"ZStep",zStep);     

  Control.addVariable(colName+"FStep",shutterFStep);
  Control.addVariable(colName+"Length",shutterLen);
  Control.addVariable(colName+"HGap",beamXSize);
  Control.addVariable(colName+"VGap",beamZSize);    
  Control.addVariable(colName+"HGapRAngle",beamXAngle);
  Control.addVariable(colName+"VGapRAngle",beamZAngle);

  Control.addVariable(colName+"B4CThick",b4cThick);
  Control.addVariable(colName+"B4CSpace",b4cSpace);
  Control.addVariable(colName+"SteelOffset",steelOffset);
  Control.addVariable(colName+"SteelAWidth",steelAWidth);
  Control.addVariable(colName+"SteelBWidth",steelBWidth);

  Control.addVariable(colName+"SteelMat",steelMat);
  Control.addVariable(colName+"B4cMat",b4cMat);  
  /*
  Control.addVariable(finalBlock+"CentX",0.0);    //   Inner blocks
  Control.addVariable(finalBlock+"CentZ",0.0);    //   Inner blocks
  Control.addVariable(finalBlock+"Len",3.4);      //   Inner blocks
  Control.addVariable(finalBlock+"VGap",4.0);     //   Height
  Control.addVariable(finalBlock+"HGap",4.0);     //   Hor. Gap (full)
  Control.addVariable(finalBlock+"Mat","Void");   //   Spacer
  */
  
  return;

}


}  // NAMESPACE setVariable
