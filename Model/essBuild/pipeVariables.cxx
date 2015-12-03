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
  
  // top mod left

  Control.addVariable("TSupplyLeftAlNSegIn",3);
  Control.addVariable("TSupplyLeftAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyLeftAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyLeftAlPPt2",Geometry::Vec3D(3.286, 3.0, 0));
  Control.addVariable("TSupplyLeftAlPPt3",Geometry::Vec3D(3.286, 4.2, 0));

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

  Control.addVariable("TSupplyLeftConnectNSegIn",1);
  Control.addVariable("TSupplyLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TSupplyLeftConnectPPt1",Geometry::Vec3D(0, 2.5, 0));

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


  Control.addVariable("TSupplyLeftInvarNSegIn",2);
  Control.addVariable("TSupplyLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TSupplyLeftInvarPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TSupplyLeftInvarPPt2",Geometry::Vec3D(0, 9.34, 400));
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


  // TReturnLeft

  Control.addVariable("TReturnLeftAlNSegIn",3);
  Control.addVariable("TReturnLeftAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("TReturnLeftAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("TReturnLeftAlPPt2",Geometry::Vec3D(-3.286, 3.0, 0));
  Control.addVariable("TReturnLeftAlPPt3",Geometry::Vec3D(-3.286, 4.2, 0));
  Control.addVariable("TReturnLeftAlNRadii",4);
  Control.addVariable("TReturnLeftAlRadius0",1.0);
  Control.addVariable("TReturnLeftAlRadius1",1.2);
  Control.addVariable("TReturnLeftAlRadius2",1.5);
  Control.addVariable("TReturnLeftAlRadius3",1.8);
  Control.addVariable("TReturnLeftAlMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TReturnLeftAlMat1","Aluminium20K");
  Control.addVariable("TReturnLeftAlMat2","Void");
  Control.addVariable("TReturnLeftAlMat3","Aluminium");
  Control.addVariable("TReturnLeftAlMat4","Void");
  Control.addVariable("TReturnLeftAlMat5","Aluminium");
  Control.addVariable("TReturnLeftAlMat6","Void");
  Control.addVariable("TReturnLeftAlMat7","Aluminium");
  Control.addVariable("TReturnLeftAlTemp0",20.0);
  Control.addVariable("TReturnLeftAlTemp1",20.0);
  Control.addVariable("TReturnLeftAlActive0",3);
  Control.addVariable("TReturnLeftAlActive1",15);


  Control.addVariable("TReturnLeftConnectNSegIn",1);
  Control.addVariable("TReturnLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TReturnLeftConnectPPt1",Geometry::Vec3D(0, 2.5, 0));
  Control.Parse("TReturnLeftAlNRadii");
  Control.addVariable("TReturnLeftConnectNRadii");
  Control.Parse("TReturnLeftAlRadius0");
  Control.addVariable("TReturnLeftConnectRadius0");
  Control.Parse("TReturnLeftAlRadius1");
  Control.addVariable("TReturnLeftConnectRadius1");
  Control.Parse("TReturnLeftAlRadius2");
  Control.addVariable("TReturnLeftConnectRadius2");
  Control.Parse("TReturnLeftAlRadius3");
  Control.addVariable("TReturnLeftConnectRadius3");
  Control.addVariable("TReturnLeftConnectMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TReturnLeftConnectMat1","SS316L");
  Control.addVariable("TReturnLeftConnectMat2","Void");
  Control.addVariable("TReturnLeftConnectMat3","SS316L");
  Control.addVariable("TReturnLeftConnectMat4","Void");
  Control.addVariable("TReturnLeftConnectMat5","SS316L");
  Control.addVariable("TReturnLeftConnectMat6","Void");
  Control.addVariable("TReturnLeftConnectMat7","SS316L");
  Control.addVariable("TReturnLeftConnectTemp0",20.0);
  Control.addVariable("TReturnLeftConnectTemp1",20.0);
  Control.addVariable("TReturnLeftConnectActive0",15);

  Control.addVariable("TReturnLeftInvarNSegIn",2);
  Control.addVariable("TReturnLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TReturnLeftInvarPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TReturnLeftInvarPPt2",Geometry::Vec3D(0, 9.34, 400));
  Control.Parse("TReturnLeftAlNRadii");
  Control.addVariable("TReturnLeftInvarNRadii");
  Control.Parse("TReturnLeftAlRadius0");
  Control.addVariable("TReturnLeftInvarRadius0");
  Control.Parse("TReturnLeftAlRadius1");
  Control.addVariable("TReturnLeftInvarRadius1");
  Control.Parse("TReturnLeftAlRadius2");
  Control.addVariable("TReturnLeftInvarRadius2");
  Control.Parse("TReturnLeftAlRadius3");
  Control.addVariable("TReturnLeftInvarRadius3");
  Control.addVariable("TReturnLeftInvarMat0","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("TReturnLeftInvarMat1","Invar36");
  Control.addVariable("TReturnLeftInvarMat2","Void");
  Control.addVariable("TReturnLeftInvarMat3","Invar36");
  Control.addVariable("TReturnLeftInvarMat4","Void");
  Control.addVariable("TReturnLeftInvarMat5","Invar36");
  Control.addVariable("TReturnLeftInvarMat6","Void");
  Control.addVariable("TReturnLeftInvarMat7","Invar36");
  Control.addVariable("TReturnLeftInvarTemp0",20.0);
  Control.addVariable("TReturnLeftInvarTemp1",20.0);
  Control.addVariable("TReturnLeftInvarActive0",15);


  return;
}

}
