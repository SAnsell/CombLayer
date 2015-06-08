/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/essDBMaterial.cxx
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
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "essDBMaterial.h"

namespace ModelSupport
{

void addESSMaterial()
  /*!
     Initialize the database of materials
   */
{

  ELog::RegMethod RegA("essDBMaterial[F]","addESSMaterial");

  const std::string MLib="hlib=.70h pnlib=70u";
  ModelSupport::DBMaterial& MDB=ModelSupport::DBMaterial::Instance();

  MonteCarlo::Material MObj;
  // ESS materials

  MObj.setMaterial(1001,"HPARA"," 1001.70c 1.0 ","hpara.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);
  
  MObj.setMaterial(1002,"HORTHO"," 1004.70c 1.0 ","hortho.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);


  MObj.setMaterial(1003, "LH05ortho", " 1001.70c 99.5 1004.70c 0.5 ","hpara.10t hortho.10t", MLib);
  MObj.setDensity(-7.0e-2*(0.07/0.0689677)); // 0.07/0.0689677 because CL for some reason changes the density !!! todo !!!
  MDB.resetMaterial(MObj);

  // Generic light water
  // Simplified light water for  basic neutronic simulations
  MObj.setMaterial(110, "H2O", " 01001.70c 0.66666667 "
		                 " 08016.70c 0.33333333 ", "lwtr.10t", MLib);
  MObj.setDensity(-1.0);
  MDB.resetMaterial(MObj);



  MObj.setMaterial(1300, "Aluminium", " 13027.70c 1.0 ", "al27.12t", MLib);
  MObj.setDensity(-2.7);
  MDB.resetMaterial(MObj); 

  MObj.setMaterial(1301, "Aluminium20K", " 13027.70c 1.0 ", "al27.10t", MLib);
  MObj.setDensity(-2.73);
  MDB.resetMaterial(MObj); 

  
  
  return;
}

} // NAMESPACE ModelSupport
