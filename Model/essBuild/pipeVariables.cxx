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
  
  const int nRad = 4;
  std::vector<double> R;
  R.push_back(1.0);
  R.push_back(1.2);
  R.push_back(1.5);
  R.push_back(1.8);

  std::vector<std::string> matAl;
  matAl.push_back("HPARA"); // email from LZ 27 Nov 2015
  matAl.push_back("Aluminium20K");
  matAl.push_back("Void");
  matAl.push_back("Aluminium");

  std::vector<std::string> matConnect;
  matConnect.push_back("HPARA"); // email from LZ 27 Nov 2015
  matConnect.push_back("SS316L");
  matConnect.push_back("Void");
  matConnect.push_back("SS316L");

  std::vector<std::string> matInvar;
  matInvar.push_back("HPARA"); // email from LZ 27 Nov 2015
  matInvar.push_back("Invar36");
  matInvar.push_back("Void");
  matInvar.push_back("Invar36");

  std::vector<double> temp;
  temp.push_back(20.0);
  temp.push_back(20.0);
  temp.push_back(300.0);
  temp.push_back(300.0);

  // top mod left

  Control.addVariable("TSupplyLeftAlNSegIn",3);
  Control.addVariable("TSupplyLeftAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyLeftAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyLeftAlPPt2",Geometry::Vec3D(3.286, 3.0, 0));
  Control.addVariable("TSupplyLeftAlPPt3",Geometry::Vec3D(3.286, 4.2, 0));
  Control.addVariable("TSupplyLeftAlNRadii",nRad);
  Control.addVariable("TSupplyLeftAlActive0",3);
  Control.addVariable("TSupplyLeftAlActive1",15);

  for (int i=0; i<nRad; i++)
    {
      std::string stri = StrFunc::makeString(i);
      Control.addVariable("TSupplyLeftAlRadius" + stri, R[i]);
      Control.addVariable("TSupplyLeftAlMat" + stri,  matAl[i]); 
      Control.addVariable("TSupplyLeftAlTemp" + stri,temp[i]);

      Control.addVariable("TSupplyLeftConnectRadius" + stri, R[i]);
      Control.addVariable("TSupplyLeftConnectMat" + stri, matConnect[i]);
      Control.addVariable("TSupplyLeftConnectTemp" + stri, temp[i]);

      Control.addVariable("TSupplyLeftInvarRadius" + stri, R[i]);
      Control.addVariable("TSupplyLeftInvarMat" + stri, matInvar[i]);
      Control.addVariable("TSupplyLeftInvarTemp" + stri, temp[i]);
  
      Control.addVariable("TReturnLeftAlRadius" + stri, R[i]);
      Control.addVariable("TReturnLeftAlMat" + stri,  matAl[i]); 
      Control.addVariable("TReturnLeftAlTemp" + stri,temp[i]);

      Control.addVariable("TReturnLeftConnectRadius" + stri, R[i]);
      Control.addVariable("TReturnLeftConnectMat" + stri, matConnect[i]);
      Control.addVariable("TReturnLeftConnectTemp" + stri, temp[i]);

      Control.addVariable("TReturnLeftInvarRadius" + stri, R[i]);
      Control.addVariable("TReturnLeftInvarMat" + stri, matInvar[i]);
      Control.addVariable("TReturnLeftInvarTemp" + stri, temp[i]);
    }


  Control.addVariable("TSupplyLeftConnectNSegIn",1);
  Control.addVariable("TSupplyLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TSupplyLeftConnectPPt1",Geometry::Vec3D(0, 2.5, 0));
  Control.addVariable("TSupplyLeftConnectNRadii", nRad);
  Control.addVariable("TSupplyLeftConnectActive0",15);


  Control.addVariable("TSupplyLeftInvarNSegIn",2);
  Control.addVariable("TSupplyLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TSupplyLeftInvarPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TSupplyLeftInvarPPt2",Geometry::Vec3D(0, 9.34, 400));
  Control.addVariable("TSupplyLeftInvarNRadii", nRad);
  Control.addVariable("TSupplyLeftInvarActive0",15);


  // TReturnLeft

  Control.Parse("TSupplyLeftAlNSegIn");
  Control.addVariable("TReturnLeftAlNSegIn");
  Control.addVariable("TReturnLeftAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("TReturnLeftAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("TReturnLeftAlPPt2",Geometry::Vec3D(-3.286, 3.0, 0));
  Control.addVariable("TReturnLeftAlPPt3",Geometry::Vec3D(-3.286, 4.2, 0));
  Control.addVariable("TReturnLeftAlNRadii", nRad);
  Control.addVariable("TReturnLeftAlActive0",3);
  Control.addVariable("TReturnLeftAlActive1",15);


  Control.Parse("TSupplyLeftConnectNSegIn");
  Control.addVariable("TReturnLeftConnectNSegIn");
  Control.addVariable("TReturnLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TReturnLeftConnectPPt1",Geometry::Vec3D(0, 2.5, 0));
  Control.addVariable("TReturnLeftConnectNRadii", nRad);
  Control.addVariable("TReturnLeftConnectActive0",15);

  Control.Parse("TSupplyLeftInvarNSegIn");
  Control.addVariable("TReturnLeftInvarNSegIn");
  Control.addVariable("TReturnLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TReturnLeftInvarPPt1",Geometry::Vec3D(0, 9.34, 0));
  Control.addVariable("TReturnLeftInvarPPt2",Geometry::Vec3D(0, 9.34, 400));
  Control.addVariable("TReturnLeftInvarNRadii", nRad);
  Control.addVariable("TReturnLeftInvarActive0",15);


  return;
}

}
