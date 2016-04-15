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
  ELog::RegMethod RegA("essVariables[F]","pipeVariables"); 
  
  const int nRad = 2;
  const double invarPipeHeight = 30;
  const double invarPipeLength = 12.5;
  const double AlPPt3X = 3.597;
  const double AlPPt3Y = 11.0;
  const double connectPipeLength = 2.0;

  std::vector<double> R;
  R.push_back(1.1);
  R.push_back(1.3);
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

  // TOP LEFT
  Control.addVariable("TSupplyLeftAlNSegIn",3);
  Control.addVariable("TSupplyLeftAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyLeftAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyLeftAlPPt2",Geometry::Vec3D(AlPPt3X, 3.0, 0));
  Control.addVariable("TSupplyLeftAlPPt3",Geometry::Vec3D(AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("TSupplyLeftAlNRadii",nRad);
  Control.addVariable("TSupplyLeftAlActive0",3);
  Control.addVariable("TSupplyLeftAlActive1",15);

  Control.addVariable("TSupplyLeftConnectNSegIn",1);
  Control.addVariable("TSupplyLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TSupplyLeftConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("TSupplyLeftConnectNRadii", nRad);
  Control.addVariable("TSupplyLeftConnectActive0",15);

  Control.addVariable("TSupplyLeftInvarNSegIn",2);
  Control.addVariable("TSupplyLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TSupplyLeftInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("TSupplyLeftInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("TSupplyLeftInvarNRadii", nRad);
  Control.addVariable("TSupplyLeftInvarActive0",15);

  Control.Parse("TSupplyLeftAlNSegIn");
  Control.addVariable("TReturnLeftAlNSegIn");
  Control.addVariable("TReturnLeftAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("TReturnLeftAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("TReturnLeftAlPPt2",Geometry::Vec3D(-AlPPt3X, 3.0, 0));
  Control.addVariable("TReturnLeftAlPPt3",Geometry::Vec3D(-AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("TReturnLeftAlNRadii", nRad);
  Control.addVariable("TReturnLeftAlActive0",3);
  Control.addVariable("TReturnLeftAlActive1",15);

  Control.Parse("TSupplyLeftConnectNSegIn");
  Control.addVariable("TReturnLeftConnectNSegIn");
  Control.addVariable("TReturnLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TReturnLeftConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("TReturnLeftConnectNRadii", nRad);
  Control.addVariable("TReturnLeftConnectActive0",15);

  Control.Parse("TSupplyLeftInvarNSegIn");
  Control.addVariable("TReturnLeftInvarNSegIn");
  Control.addVariable("TReturnLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TReturnLeftInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("TReturnLeftInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("TReturnLeftInvarNRadii", nRad);
  Control.addVariable("TReturnLeftInvarActive0",15);

  // TOP RIGHT
  Control.addVariable("TSupplyRightAlNSegIn",3);
  Control.addVariable("TSupplyRightAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("TSupplyRightAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("TSupplyRightAlPPt2",Geometry::Vec3D(AlPPt3X, 3.0, 0));
  Control.addVariable("TSupplyRightAlPPt3",Geometry::Vec3D(AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("TSupplyRightAlNRadii",nRad);
  Control.addVariable("TSupplyRightAlActive0",3);
  Control.addVariable("TSupplyRightAlActive1",15);

  Control.addVariable("TSupplyRightConnectNSegIn",1);
  Control.addVariable("TSupplyRightConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TSupplyRightConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("TSupplyRightConnectNRadii", nRad);
  Control.addVariable("TSupplyRightConnectActive0",15);

  Control.addVariable("TSupplyRightInvarNSegIn",2);
  Control.addVariable("TSupplyRightInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TSupplyRightInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("TSupplyRightInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("TSupplyRightInvarNRadii", nRad);
  Control.addVariable("TSupplyRightInvarActive0",15);

  Control.addVariable("TReturnRightAlNSegIn",3);
  Control.addVariable("TReturnRightAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("TReturnRightAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("TReturnRightAlPPt2",Geometry::Vec3D(-AlPPt3X, 3.0, 0));
  Control.addVariable("TReturnRightAlPPt3",Geometry::Vec3D(-AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("TReturnRightAlNRadii",nRad);
  Control.addVariable("TReturnRightAlActive0",3);
  Control.addVariable("TReturnRightAlActive1",15);

  Control.addVariable("TReturnRightConnectNSegIn",1);
  Control.addVariable("TReturnRightConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("TReturnRightConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("TReturnRightConnectNRadii", nRad);
  Control.addVariable("TReturnRightConnectActive0",15);

  Control.addVariable("TReturnRightInvarNSegIn",2);
  Control.addVariable("TReturnRightInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("TReturnRightInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("TReturnRightInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("TReturnRightInvarNRadii", nRad);
  Control.addVariable("TReturnRightInvarActive0",15);

  // LOW LEFT
  Control.addVariable("LSupplyLeftAlNSegIn",3);
  Control.addVariable("LSupplyLeftAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("LSupplyLeftAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("LSupplyLeftAlPPt2",Geometry::Vec3D(AlPPt3X, 3.0, 0));
  Control.addVariable("LSupplyLeftAlPPt3",Geometry::Vec3D(AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("LSupplyLeftAlNRadii",nRad);
  Control.addVariable("LSupplyLeftAlActive0",3);
  Control.addVariable("LSupplyLeftAlActive1",15);

  Control.addVariable("LSupplyLeftConnectNSegIn",1);
  Control.addVariable("LSupplyLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("LSupplyLeftConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("LSupplyLeftConnectNRadii", nRad);
  Control.addVariable("LSupplyLeftConnectActive0",15);

  Control.addVariable("LSupplyLeftInvarNSegIn",2);
  Control.addVariable("LSupplyLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("LSupplyLeftInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("LSupplyLeftInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("LSupplyLeftInvarNRadii", nRad);
  Control.addVariable("LSupplyLeftInvarActive0",15);

  Control.Parse("LSupplyLeftAlNSegIn");
  Control.addVariable("LReturnLeftAlNSegIn");
  Control.addVariable("LReturnLeftAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("LReturnLeftAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("LReturnLeftAlPPt2",Geometry::Vec3D(-AlPPt3X, 3.0, 0));
  Control.addVariable("LReturnLeftAlPPt3",Geometry::Vec3D(-AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("LReturnLeftAlNRadii", nRad);
  Control.addVariable("LReturnLeftAlActive0",3);
  Control.addVariable("LReturnLeftAlActive1",15);

  Control.Parse("LSupplyLeftConnectNSegIn");
  Control.addVariable("LReturnLeftConnectNSegIn");
  Control.addVariable("LReturnLeftConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("LReturnLeftConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("LReturnLeftConnectNRadii", nRad);
  Control.addVariable("LReturnLeftConnectActive0",15);

  Control.Parse("LSupplyLeftInvarNSegIn");
  Control.addVariable("LReturnLeftInvarNSegIn");
  Control.addVariable("LReturnLeftInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("LReturnLeftInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("LReturnLeftInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("LReturnLeftInvarNRadii", nRad);
  Control.addVariable("LReturnLeftInvarActive0",15);

  // LOW RIGHT
  Control.addVariable("LSupplyRightAlNSegIn",3);
  Control.addVariable("LSupplyRightAlPPt0",Geometry::Vec3D(1.9, 0, 0));
  Control.addVariable("LSupplyRightAlPPt1",Geometry::Vec3D(1.9, 2.0, 0));
  Control.addVariable("LSupplyRightAlPPt2",Geometry::Vec3D(AlPPt3X, 3.0, 0));
  Control.addVariable("LSupplyRightAlPPt3",Geometry::Vec3D(AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("LSupplyRightAlNRadii",nRad);
  Control.addVariable("LSupplyRightAlActive0",3);
  Control.addVariable("LSupplyRightAlActive1",15);

  Control.addVariable("LSupplyRightConnectNSegIn",1);
  Control.addVariable("LSupplyRightConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("LSupplyRightConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("LSupplyRightConnectNRadii", nRad);
  Control.addVariable("LSupplyRightConnectActive0",15);

  Control.addVariable("LSupplyRightInvarNSegIn",2);
  Control.addVariable("LSupplyRightInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("LSupplyRightInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("LSupplyRightInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("LSupplyRightInvarNRadii", nRad);
  Control.addVariable("LSupplyRightInvarActive0",15);

  Control.addVariable("LReturnRightAlNSegIn",3);
  Control.addVariable("LReturnRightAlPPt0",Geometry::Vec3D(-1.9, 0, 0));
  Control.addVariable("LReturnRightAlPPt1",Geometry::Vec3D(-1.9, 2.0, 0));
  Control.addVariable("LReturnRightAlPPt2",Geometry::Vec3D(-AlPPt3X, 3.0, 0));
  Control.addVariable("LReturnRightAlPPt3",Geometry::Vec3D(-AlPPt3X, AlPPt3Y, 0));
  Control.addVariable("LReturnRightAlNRadii",nRad);
  Control.addVariable("LReturnRightAlActive0",3);
  Control.addVariable("LReturnRightAlActive1",15);

  Control.addVariable("LReturnRightConnectNSegIn",1);
  Control.addVariable("LReturnRightConnectPPt0",Geometry::Vec3D(0, 0.0, 0));
  Control.addVariable("LReturnRightConnectPPt1",Geometry::Vec3D(0, connectPipeLength, 0));
  Control.addVariable("LReturnRightConnectNRadii", nRad);
  Control.addVariable("LReturnRightConnectActive0",15);

  Control.addVariable("LReturnRightInvarNSegIn",2);
  Control.addVariable("LReturnRightInvarPPt0",Geometry::Vec3D(0, 0,  0));
  Control.addVariable("LReturnRightInvarPPt1",Geometry::Vec3D(0, invarPipeLength, 0));
  Control.addVariable("LReturnRightInvarPPt2",Geometry::Vec3D(0, invarPipeLength, invarPipeHeight));
  Control.addVariable("LReturnRightInvarNRadii", nRad);
  Control.addVariable("LReturnRightInvarActive0",15);




  for (size_t i=0; i<nRad; i++)
    {
      std::string stri = StrFunc::makeString(i);

      // TOP LEFT
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

      // TOP RIGHT
      Control.addVariable("TSupplyRightAlRadius" + stri, R[i]);
      Control.addVariable("TSupplyRightAlMat" + stri,  matAl[i]); 
      Control.addVariable("TSupplyRightAlTemp" + stri,temp[i]);

      Control.addVariable("TSupplyRightConnectRadius" + stri, R[i]);
      Control.addVariable("TSupplyRightConnectMat" + stri, matConnect[i]);
      Control.addVariable("TSupplyRightConnectTemp" + stri, temp[i]);

      Control.addVariable("TSupplyRightInvarRadius" + stri, R[i]);
      Control.addVariable("TSupplyRightInvarMat" + stri, matInvar[i]);
      Control.addVariable("TSupplyRightInvarTemp" + stri, temp[i]);

      Control.addVariable("TReturnRightAlRadius" + stri, R[i]);
      Control.addVariable("TReturnRightAlMat" + stri,  matAl[i]); 
      Control.addVariable("TReturnRightAlTemp" + stri,temp[i]);

      Control.addVariable("TReturnRightConnectRadius" + stri, R[i]);
      Control.addVariable("TReturnRightConnectMat" + stri, matConnect[i]);
      Control.addVariable("TReturnRightConnectTemp" + stri, temp[i]);

      Control.addVariable("TReturnRightInvarRadius" + stri, R[i]);
      Control.addVariable("TReturnRightInvarMat" + stri, matInvar[i]);
      Control.addVariable("TReturnRightInvarTemp" + stri, temp[i]);


      // LOW LEFT
      Control.addVariable("LSupplyLeftAlRadius" + stri, R[i]);
      Control.addVariable("LSupplyLeftAlMat" + stri,  matAl[i]); 
      Control.addVariable("LSupplyLeftAlTemp" + stri,temp[i]);

      Control.addVariable("LSupplyLeftConnectRadius" + stri, R[i]);
      Control.addVariable("LSupplyLeftConnectMat" + stri, matConnect[i]);
      Control.addVariable("LSupplyLeftConnectTemp" + stri, temp[i]);

      Control.addVariable("LSupplyLeftInvarRadius" + stri, R[i]);
      Control.addVariable("LSupplyLeftInvarMat" + stri, matInvar[i]);
      Control.addVariable("LSupplyLeftInvarTemp" + stri, temp[i]);
  
      Control.addVariable("LReturnLeftAlRadius" + stri, R[i]);
      Control.addVariable("LReturnLeftAlMat" + stri,  matAl[i]); 
      Control.addVariable("LReturnLeftAlTemp" + stri,temp[i]);

      Control.addVariable("LReturnLeftConnectRadius" + stri, R[i]);
      Control.addVariable("LReturnLeftConnectMat" + stri, matConnect[i]);
      Control.addVariable("LReturnLeftConnectTemp" + stri, temp[i]);

      Control.addVariable("LReturnLeftInvarRadius" + stri, R[i]);
      Control.addVariable("LReturnLeftInvarMat" + stri, matInvar[i]);
      Control.addVariable("LReturnLeftInvarTemp" + stri, temp[i]);

      // LOW RIGHT
      Control.addVariable("LSupplyRightAlRadius" + stri, R[i]);
      Control.addVariable("LSupplyRightAlMat" + stri,  matAl[i]); 
      Control.addVariable("LSupplyRightAlTemp" + stri,temp[i]);

      Control.addVariable("LSupplyRightConnectRadius" + stri, R[i]);
      Control.addVariable("LSupplyRightConnectMat" + stri, matConnect[i]);
      Control.addVariable("LSupplyRightConnectTemp" + stri, temp[i]);

      Control.addVariable("LSupplyRightInvarRadius" + stri, R[i]);
      Control.addVariable("LSupplyRightInvarMat" + stri, matInvar[i]);
      Control.addVariable("LSupplyRightInvarTemp" + stri, temp[i]);

      Control.addVariable("LReturnRightAlRadius" + stri, R[i]);
      Control.addVariable("LReturnRightAlMat" + stri,  matAl[i]); 
      Control.addVariable("LReturnRightAlTemp" + stri,temp[i]);

      Control.addVariable("LReturnRightConnectRadius" + stri, R[i]);
      Control.addVariable("LReturnRightConnectMat" + stri, matConnect[i]);
      Control.addVariable("LReturnRightConnectTemp" + stri, temp[i]);

      Control.addVariable("LReturnRightInvarRadius" + stri, R[i]);
      Control.addVariable("LReturnRightInvarMat" + stri, matInvar[i]);
      Control.addVariable("LReturnRightInvarTemp" + stri, temp[i]);
    }


  return;
}

}
