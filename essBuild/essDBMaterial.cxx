/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/essDBMaterial.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

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
  ELog::EM<<"ESS Materials NOT LOADED "<<ELog::endWarn;

  return;
  const std::string MLib="hlib=.70h pnlib=70u";
  ModelSupport::DBMaterial& MDB=
    ModelSupport::DBMaterial::Instance();

  MonteCarlo::Material MObj;
  MObj.setMaterial(75,"EssH2"," 1001.70c 1.000000000 ","para.19t",
		   MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);

  //M 0115 #76
  MObj.setMaterial(76,"EssH2O","1001.70c  0.666562842 "
		   " 1002.70c  0.000103824 8016.70c  0.332540192 "
		   " 8017.70c  0.000126332 8018.70c  0.000666810 ",
		   "",MLib);
  MObj.setDensity(-1.0);
  MDB.resetMaterial(MObj);
 
  // M0200 #77
  MObj.setMaterial(77,"EssHe"," 2003.70c  0.00000134 "
		   " 2004.70c  0.99999866 " ,
		   "",MLib);
  MObj.setDensity(-1.74e-4);
  MDB.resetMaterial(MObj);
  
  // M0201 #78 [Copy]
  // M0202 #79 [Copy]

  // M0400 #80 Be
  MObj.setMaterial(80,"EssM0400","4009.70c  1.0 ","",MLib);
  MObj.setDensity(-1.85);
  MDB.resetMaterial(MObj); 

  // M1360 #81
  MObj.setMaterial(81,"EssM1360"," 12024.70c  0.008812087 "
		   "12025.70c 0.001115595 12026.70c  0.001228270 "
		   "13027.70c 0.977848644 14028.70c  0.005342094 "
		   "14029.70c 0.000271383 14030.70c  0.000179107 "
		   "22046.70c 0.000035050 22047.70c  0.000031608 "
		   "22048.70c  0.000313196 22049.70c  0.000022984 "
		   "22050.70c  0.000022007 24050.70c  0.000044183 "
		   "24052.70c  0.000852028 24053.70c  0.000096613 "
		   "24054.70c  0.000024049 25055.70c  0.000370161 "
		   "26054.70c  0.000099328 26056.70c  0.001559232 "
		   "26057.70c  0.000036009 26058.70c  0.000004792 "
		   "29063.70c  0.000811409 29065.70c  0.000361995 "
		   "30000.70c  0.000518176 ","",MLib);
  MObj.setDensity(-2.70);
  MDB.resetMaterial(MObj);
  
  // M1361 #82
  MObj.setMaterial(82,"M1361",
		   "12024.70c 0.008812087 12025.70c 0.001115595 "
		   "12026.70c 0.001228270 13027.70c 0.977848644 "
		   "14028.70c 0.005342094 14029.70c 0.000271383 "
		   "14030.70c 0.000179107 2046.70c 0.000035050 "
		   "22047.70c 0.000031608 2048.70c 0.000313196 "
		   "22049.70c 0.000022984 2050.70c 0.000022007 "
		   "24050.70c 0.000044183 4052.70c 0.000852028 "
		   "24053.70c 0.000096613 4054.70c 0.000024049 "
		   "25055.70c 0.000370161 6054.70c 0.000099328 "
		   "26056.70c 0.001559232 6057.70c 0.000036009 "
		   "26058.70c 0.000004792 9063.70c 0.000811409 "
		   "29065.70c 0.000361995 0000.70c 0.000518176 ",
		   "",MLib);
  MObj.setDensity(-2.73);
  MDB.resetMaterial(MObj);
  // M2600 #83 
  MObj.setMaterial(83,"M2600","26054.70c  0.058450000 "
		   "26056.70c 0.917540000 26057.70c 0.021190000 "
		   "26058.70c 0.002820000 ","",MLib);
  MObj.setDensity(-7.85);
  MDB.resetMaterial(MObj);

  //M2616 #84   
  MObj.setMaterial(84,"M2616",
		   "06000.71c  0.001392603 14028.71c  0.007323064 "
		   "14029.71c  0.000372017 14030.71c  0.000245523 "
		   "15031.71c  0.000360008 16032.71c  0.000165168 "
		   "16033.71c  0.000001304 16034.71c  0.000007390 "
		   "16036.71c  0.000000017 24050.71c  0.007920331 "
		   "24052.71c  0.152735704 24053.71c  0.017319003 "
		   "24054.71c  0.004311066 25055.71c  0.018267327 "
		   "26054.71c  0.038344779 26056.71c  0.601931034 " 
		   "26057.71c  0.013901213 26058.71c  0.001849996 "
		   "27059.71c  0.000283816 28058.71c  0.080834464 "
		   "28060.71c  0.031137291 28061.71c  0.001353516 "
		   "28062.71c  0.004315603 28064.71c  0.001099057 ",
		   "",MLib);
  MObj.setDensity(-7.76);
  MDB.resetMaterial(MObj);  


  //M74001 #85    (ess tungsten)
  MObj.setMaterial(85,"M74001",
		   "74182.70c  0.265000000 "
		   "74183.70c  0.143100000 "
		   "74184.70c  0.306400000 "
		   "74186.70c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-19.20);
  MDB.resetMaterial(MObj);  

  
  return;
}

} // NAMESPACE ModelSupport
