/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Konstantin Batkov
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
pipeVariables(FuncDataBase& Control)
{
  
  // top mod return pipe

  Control.addVariable("TSupplyLeftAlTopNSegIn",3);
  Control.addVariable("TSupplyLeftAlTopPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyLeftAlTopPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyLeftAlTopPPt2",Geometry::Vec3D(3.286, 3.0, 0));
  Control.addVariable("TSupplyLeftAlTopPPt3",Geometry::Vec3D(3.286, 4.2, 0));

  Control.addVariable("TSupplyLeftAlNRadii",4);
  Control.addVariable("TSupplyLeftAlRadius0",1.0);
  Control.addVariable("TSupplyLeftAlRadius1",1.2);
  Control.addVariable("TSupplyLeftAlRadius2",1.5);
  Control.addVariable("TSupplyLeftAlRadius3",1.8);

  Control.addVariable("TSupplyLeftAlMat0","HPARA"); // email from LZ 27 Nov 2015
  
  Control.addVariable("TSupplyLeftAlMat1","Aluminium20K");
  Control.addVariable("TSupplyLeftAlMat2","Void");
  Control.addVariable("TSupplyLeftAlMat3","Aluminium");
  Control.addVariable("TSupplyLeftAlMat4","Void");
  Control.addVariable("TSupplyLeftAlMat5","Aluminium");
  Control.addVariable("TSupplyLeftAlMat6","Void");
  Control.addVariable("TSupplyLeftAlMat7","Aluminium");

  Control.addVariable("TSupplyLeftAlTemp0",25.0);
  Control.addVariable("TSupplyLeftAlTemp1",25.0);

  Control.addVariable("TSupplyLeftAlActive0",3);
  Control.addVariable("TSupplyLeftAlActive1",15);

  Control.addVariable("TSupplyLeftConnectTopNSegIn",1);
  Control.addVariable("TSupplyLeftConnectTopPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TSupplyLeftConnectTopPPt1",Geometry::Vec3D(0, 2.5, 0));

  Control.Parse("TSupplyLeftAlNRadii");
  Control.addVariable("TSupplyLeftConnectNRadii");
  Control.Parse("TSupplyLeftAlRadius0");
  Control.addVariable("TSupplyLeftConnectRadius0");
  Control.Parse("TSupplyLeftAlRadius1");
  Control.addVariable("TSupplyLeftConnectRadius1");
  Control.Parse("TSupplyLeftAlRadius2");
  Control.addVariable("TSupplyLeftConnectRadius2");
  Control.Parse("TSupplyLeftAlRadius3");
  Control.addVariable("TSupplyLeftConnectRadius3");

  Control.addVariable("TSupplyLeftConnectMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TSupplyLeftConnectMat1","SS316L");
  Control.addVariable("TSupplyLeftConnectMat2","Void");
  Control.addVariable("TSupplyLeftConnectMat3","SS316L");
  Control.addVariable("TSupplyLeftConnectMat4","Void");
  Control.addVariable("TSupplyLeftConnectMat5","SS316L");
  Control.addVariable("TSupplyLeftConnectMat6","Void");
  Control.addVariable("TSupplyLeftConnectMat7","SS316L");
  Control.addVariable("TSupplyLeftConnectTemp0",20.0);
  Control.addVariable("TSupplyLeftConnectTemp1",20.0);
  Control.addVariable("TSupplyLeftConnectActive0",15);

  // invar pipe

  Control.addVariable("TSupplyLeftInvarTopNSegIn",2);
  Control.addVariable("TSupplyLeftInvarTopPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TSupplyLeftInvarTopPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TSupplyLeftInvarTopPPt2",Geometry::Vec3D(0, 9.34, 400));

  Control.Parse("TSupplyLeftAlNRadii");
  Control.addVariable("TSupplyLeftInvarNRadii");
  Control.Parse("TSupplyLeftAlRadius0");
  Control.addVariable("TSupplyLeftInvarRadius0");
  Control.Parse("TSupplyLeftAlRadius1");
  Control.addVariable("TSupplyLeftInvarRadius1");
  Control.Parse("TSupplyLeftAlRadius2");
  Control.addVariable("TSupplyLeftInvarRadius2");
  Control.Parse("TSupplyLeftAlRadius3");
  Control.addVariable("TSupplyLeftInvarRadius3");

  Control.addVariable("TSupplyLeftInvarMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TSupplyLeftInvarMat1","Invar36");
  Control.addVariable("TSupplyLeftInvarMat2","Void");
  Control.addVariable("TSupplyLeftInvarMat3","Invar36");
  Control.addVariable("TSupplyLeftInvarMat4","Void");
  Control.addVariable("TSupplyLeftInvarMat5","Invar36");
  Control.addVariable("TSupplyLeftInvarMat6","Void");
  Control.addVariable("TSupplyLeftInvarMat7","Invar36");
  Control.addVariable("TSupplyLeftInvarTemp0",20.0);
  Control.addVariable("TSupplyLeftInvarTemp1",20.0);
  Control.addVariable("TSupplyLeftInvarActive0",15);

  return;
}

}
