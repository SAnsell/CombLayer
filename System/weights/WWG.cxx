/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWGconstruct.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "WeightMesh.h"
#include "WWG.h"

namespace WeightSystem
{

WWG::WWG() :
  EBin({1e8})
  /*!
    Constructor
  */
{}

void
WWG::setEnergyBin(const std::vector<double>& EB)
  /*!
    Set the energy bins
  */
{
  EBin=EB;
  return;
}
  
  
void
WWG::write(std::ostream& OX) const
  /*!
    Wreit to the MCNP file
   */
{
  ELog::RegMethod RegA("WWG","write");

  Grid.write(OX);
  return;
}
  
void
WWG::writeWWINP(const std::string& FName) const
  /*!
    Write out separate WWINP file
    \param FNAme :: Output filename
  */
{
  std::ofstream OX;
  OX.open(FName.c_str());

  Grid.writeWWINP(OX,EBin.size());
  size_t itemCnt=0;
  for(const double& E : EBin)
    StrFunc::writeLine(OX,E,itemCnt,6);
  OX.close();
		       
  return;
}  
  
}  // NAMESPACE WeightSystem
