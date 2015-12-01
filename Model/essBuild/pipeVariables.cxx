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

  Control.addVariable("TSupplyTopNSegIn",1);
  Control.addVariable("TSupplyTopPPt0",Geometry::Vec3D(2,0,0));
  Control.addVariable("TSupplyTopPPt1",Geometry::Vec3D(3,8,0));
  //  Control.addVariable("TSupplyTopPPt2",Geometry::Vec3D(4,60,0));

  Control.addVariable("TSupplyNRadii",5);
  Control.addVariable("TSupplyRadius0",1.7);
  Control.addVariable("TSupplyRadius1",1.8);
  Control.addVariable("TSupplyRadius2",2.3);
  Control.addVariable("TSupplyRadius3",2.5);
  Control.addVariable("TSupplyRadius4",2.7);
  Control.addVariable("TSupplyRadius5",2.9);
  Control.addVariable("TSupplyRadius6",3.5);
  Control.addVariable("TSupplyRadius7",3.7);

  Control.addVariable("TSupplyMat0","Steel71");
  
  Control.addVariable("TSupplyMat1","Aluminium");
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
  /*  Control.addVariable("TSupplyActive1",15);
  Control.addVariable("TSupplyActive2",63);
  Control.addVariable("TSupplyActive3",255);
  Control.addVariable("TSupplyActive4",127);
  */



  Control.addVariable("TConnectTopNSegIn",1);
  Control.addVariable("TConnectTopPPt0",Geometry::Vec3D(0,0,0));
  Control.addVariable("TConnectTopPPt1",Geometry::Vec3D(0,8,0));
  //  Control.addVariable("TConnectTopPPt2",Geometry::Vec3D(3,30,0));
  //  Control.addVariable("TConnectTopPPt2",Geometry::Vec3D(4,60,0));

  Control.addVariable("TConnectNRadii",4);
  Control.addVariable("TConnectRadius0",1.7);
  Control.addVariable("TConnectRadius1",1.8);
  Control.addVariable("TConnectRadius2",2.3);
  Control.addVariable("TConnectRadius3",2.5);
  Control.addVariable("TConnectRadius4",2.7);
  Control.addVariable("TConnectRadius5",2.9);
  Control.addVariable("TConnectRadius6",3.5);
  Control.addVariable("TConnectRadius7",3.7);

  Control.addVariable("TConnectMat0","Steel71");
  
  Control.addVariable("TConnectMat1","Aluminium");
  Control.addVariable("TConnectMat2","Void");
  Control.addVariable("TConnectMat3","Aluminium");
  Control.addVariable("TConnectMat4","Void");
  Control.addVariable("TConnectMat5","Aluminium");
  Control.addVariable("TConnectMat6","Void");
  Control.addVariable("TConnectMat7","Aluminium");

  Control.addVariable("TConnectTemp0",25.0);
  Control.addVariable("TConnectTemp1",25.0);

  Control.addVariable("TConnectActive0",15);




  return;
}

}
