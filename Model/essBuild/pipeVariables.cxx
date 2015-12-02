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

  Control.addVariable("TSupplyTopNSegIn",3);
  Control.addVariable("TSupplyTopPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyTopPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyTopPPt2",Geometry::Vec3D(3.286, 3.0, 0));
  Control.addVariable("TSupplyTopPPt3",Geometry::Vec3D(3.186, 4.2, 0));

  Control.addVariable("TSupplyNRadii",4);
  Control.addVariable("TSupplyRadius0",1.0);
  Control.addVariable("TSupplyRadius1",1.2);
  Control.addVariable("TSupplyRadius2",1.5);
  Control.addVariable("TSupplyRadius3",1.8);

  Control.addVariable("TSupplyMat0","HPARA"); // email from LZ 27 Nov 2015
  
  Control.addVariable("TSupplyMat1","Aluminium20K");
  Control.addVariable("TSupplyMat2","Void");
  Control.addVariable("TSupplyMat3","Aluminium");
  Control.addVariable("TSupplyMat4","Void");
  Control.addVariable("TSupplyMat5","Aluminium");
  Control.addVariable("TSupplyMat6","Void");
  Control.addVariable("TSupplyMat7","Aluminium");

  Control.addVariable("TSupplyTemp0",25.0);
  Control.addVariable("TSupplyTemp1",25.0);

  Control.addVariable("TSupplyActive0",3);
  Control.addVariable("TSupplyActive1",15);

  Control.addVariable("TConnectTopNSegIn",1);
  Control.addVariable("TConnectTopPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TConnectTopPPt1",Geometry::Vec3D(0, 2.5, 0));

  Control.Parse("TSupplyNRadii");
  Control.addVariable("TConnectNRadii");
  Control.Parse("TSupplyRadius0");
  Control.addVariable("TConnectRadius0");
  Control.Parse("TSupplyRadius1");
  Control.addVariable("TConnectRadius1");
  Control.Parse("TSupplyRadius2");
  Control.addVariable("TConnectRadius2");
  Control.Parse("TSupplyRadius3");
  Control.addVariable("TConnectRadius3");

  Control.addVariable("TConnectMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TConnectMat1","SS316L");
  Control.addVariable("TConnectMat2","Void");
  Control.addVariable("TConnectMat3","SS316L");
  Control.addVariable("TConnectMat4","Void");
  Control.addVariable("TConnectMat5","SS316L");
  Control.addVariable("TConnectMat6","Void");
  Control.addVariable("TConnectMat7","SS316L");
  Control.addVariable("TConnectTemp0",20.0);
  Control.addVariable("TConnectTemp1",20.0);
  Control.addVariable("TConnectActive0",15);

  // invar pipe

  Control.addVariable("TInvarTopNSegIn",2);
  Control.addVariable("TInvarTopPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TInvarTopPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TInvarTopPPt2",Geometry::Vec3D(0, 9.34, 400));

  Control.Parse("TSupplyNRadii");
  Control.addVariable("TInvarNRadii");
  Control.Parse("TSupplyRadius0");
  Control.addVariable("TInvarRadius0");
  Control.Parse("TSupplyRadius1");
  Control.addVariable("TInvarRadius1");
  Control.Parse("TSupplyRadius2");
  Control.addVariable("TInvarRadius2");
  Control.Parse("TSupplyRadius3");
  Control.addVariable("TInvarRadius3");

  Control.addVariable("TInvarMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TInvarMat1","Invar36");
  Control.addVariable("TInvarMat2","Void");
  Control.addVariable("TInvarMat3","Invar36");
  Control.addVariable("TInvarMat4","Void");
  Control.addVariable("TInvarMat5","Invar36");
  Control.addVariable("TInvarMat6","Void");
  Control.addVariable("TInvarMat7","Invar36");
  Control.addVariable("TInvarTemp0",20.0);
  Control.addVariable("TInvarTemp1",20.0);
  Control.addVariable("TInvarActive0",15);

  return;
}

}
