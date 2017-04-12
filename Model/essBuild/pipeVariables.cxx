/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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


///\file pipeVariables.cxx

namespace setVariable
{

void
EssPipeVariables(FuncDataBase& Control)
  /*!
    Construct the moderator pipes variable table
    \param Control ::
   */
{
  ELog::RegMethod RegA("EssPipeVariables[F]","EssPipeVariables"); 
  
  const size_t nRad(4);
  const double invarPipeHeight(30.0);
  const double invarPipeLength(12.5);

  const double AlPPt3X(3.597);
  const double AlPPt3Y(11.0);
  const double connectPipeLength(2.0);

  const std::vector<double> R({1.1, 1.3, 1.5, 1.8});

  const std::vector<std::string> matAl
    ({"HPARA","Aluminium20K","Void","Aluminium"});

  const std::vector<double> temp({20.0,20.0,300.0,300.0});

  double signV(1.0);
  for(const std::string& MItem :
    {"TSupplyLeft","TReturnLeft","LSupplyLeft","LReturnLeft",
    "TSupplyRight","TReturnRight","LSupplyRight","LReturnRight"})

    {
      Control.addVariable(MItem+"AlNSegIn",3);
      Control.addVariable(MItem+"AlPPt0",Geometry::Vec3D(signV*1.9,0,0));
      Control.addVariable(MItem+"AlPPt1",Geometry::Vec3D(signV*1.9,2.0,0));
      Control.addVariable(MItem+"AlPPt2",Geometry::Vec3D(signV*AlPPt3X,3.0,0));
      Control.addVariable(MItem+"AlPPt3",Geometry::Vec3D(signV*AlPPt3X,AlPPt3Y,0));
      Control.addVariable(MItem+"AlNRadii",nRad);
      Control.addVariable(MItem+"AlActive0",3);
      Control.addVariable(MItem+"AlActive1",15);

      for (size_t i=0;i<nRad;i++)
        {
          const std::string strIndex = StrFunc::makeString(i);
          
          // TOP LEFT
          Control.addVariable(MItem+"AlRadius"+ strIndex,R[i]);
          Control.addVariable(MItem+"AlMat"+strIndex,matAl[i]); 
          Control.addVariable(MItem+"AlTemp"+strIndex,temp[i]);
        }
          
      signV*=-1.0;
    }
  return;
}

}
