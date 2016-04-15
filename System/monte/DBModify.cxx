/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/DBModify.cxx
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
#include "DBModify.h"

namespace ModelSupport
{

void cloneESSMaterial()
  /*!
    Clone ESS materials if not using the ESS Database materials
  */
{
  ELog::RegMethod RegA("essDBMaterial[F]","cloneESSMaterial");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  
  DB.cloneMaterial("Tungsten_15.1g","Tungsten151");
  DB.cloneMaterial("Iron_10H2O","Iron10H2O");
  DB.cloneMaterial("Void","Helium");
  DB.cloneMaterial("Void","M2644");
  DB.cloneMaterial("Stainless304","SS316L");
  DB.cloneMaterial("Stainless304","SS316L785");

  return;
}
    
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

  // According to Fig. 7 in 10.1103/PhysRevB.91.180301, this cross section actually had 0.5% ortho-H,
  // therefore using this material is the same as adding 0.5 ortho-H
  MObj.setMaterial(101,"HPARA"," 1001.70c 1.0 ","hpara.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);
  
  MObj.setMaterial(102,"HORTHO"," 1004.70c 1.0 ","hortho.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);

  MObj.setMaterial(103, "LH05ortho", " 1001.70c 99.5 "
                   "1004.70c 0.5 ","hpara.10t hortho.10t", MLib);
  MObj.setDensity(-7.0e-2*(0.07/0.0689677));
  // 0.07/0.0689677 because CL for some reason changes the density. This correction works for 0.5% ortho only !!! todo !!!
  MDB.resetMaterial(MObj);

  // Generic light water
  // Simplified light water for  basic neutronic simulations
  MObj.setMaterial(110, "H2O", " 1001.70c 0.66666667 "
		   " 8016.70c 0.33333333 ", "lwtr.10t", MLib);
  MObj.setDensity(-1.0);
  MDB.resetMaterial(MObj);

  // Mix of water and 10% vol Be
  MObj.setMaterial(141, "H2OBe10",
		   " 04009.70c 0.12046753633 "
                   " 1001.70c 0.58635497578 "
                   " 8016.70c 0.29317748789 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009, 70, 'c', "h", "model");
  MObj.setDensity(-1.085);
  MDB.resetMaterial(MObj);

  // Mix of water and 4.7% vol Al as calculated by Marc (email from LZ 15.01.16) - checked by AT
  MObj.setMaterial(147, "H2OAl47",
		   " 1001.70c 0.647478328682 "
		   " 13027.70c 0.0287825069767 "
		   " 8016.70c 0.323739164341 ", "lwtr.10t al27.12t", MLib);
  MObj.setDensity(-1.0799);
  MDB.resetMaterial(MObj);

  // Natural Helium. Density -1.74E-4 g/cm3
  // Temperature: 300 K
  // Reference: Material Data Compilation 21 Sep 2015
  MObj.setMaterial(200, "Helium",
  		   " 2003.70c 1.34E-6 "
  		   " 2004.70c 0.999998660 ", "", MLib);
  MObj.setDensity(-1.74E-4);
  MDB.resetMaterial(MObj);

  MObj.setMaterial(1300, "Aluminium", " 13027.70c 1.0 ", "al27.12t", MLib);
  MObj.setDensity(-2.7);
  MDB.resetMaterial(MObj); 

  MObj.setMaterial(1301, "Aluminium20K", " 13027.70c 1.0 ", "al27.10t", MLib);
  MObj.setDensity(-2.73);
  MDB.resetMaterial(MObj); 

  // ESS Iron
  MObj.setMaterial(2600, "Iron",
		   " 26054.70c  0.058450000 "
		   " 26056.70c  0.917540000 "
		   " 26057.70c  0.021190000 "
		   " 26058.70c  0.002820000 ", "fe56.12t", MLib);
  //  MObj.setMXitem(6000, 70, 'c', "h", "06012");
  MObj.setDensity(-7.85);
  MDB.resetMaterial(MObj);

  // Homogeneous mixture of Iron and 10% volume H2O
  MObj.setMaterial(2610, "Iron10H2O",
                   " 01001.70c 0.077534884 "
                   " 08016.70c 0.038767442 "
                   " 26054.70c 0.051652129 "
                   " 26056.70c 0.810827964 "
                   " 26057.70c 0.018725554 "
                   " 26058.70c 0.002492027 ", "lwtr.10t fe56.12t", MLib);
  MObj.setDensity(-7.17);
  MDB.resetMaterial(MObj);


  // ESS  SS316L
  // Steel composition from ESS Design Update WP3
  MObj.setMaterial(2636, "SS316L",
		   " 06000.71c  0.001392603 "
		   " 14028.71c  0.007323064 "
		   " 14029.71c  0.000372017 "
		   " 14030.71c  0.000245523 "
		   " 15031.71c  0.000360008 "
		   " 16032.71c  0.000165168 "
		   " 16033.71c  0.000001304 "
		   " 16034.71c  0.000007390 "
		   " 16036.71c  0.000000017 "
		   " 24050.71c  0.007920331 "
		   " 24052.71c  0.152735704 "
		   " 24053.71c  0.017319003 "
		   " 24054.71c  0.004311066 "
		   " 25055.71c  0.018267327 "
		   " 26054.71c  0.038344779 "
		   " 26056.71c  0.601931034 "
		   " 26057.71c  0.013901213 "
		   " 26058.71c  0.001849996 "
		   " 27059.71c  0.000283816 "
		   " 28058.71c  0.080834464 "
		   " 28060.71c  0.031137291 "
		   " 28061.71c  0.001353516 "
		   " 28062.71c  0.004315603 "
		   " 28064.71c  0.001099057 "
		   " 42092.71c  0.002145890 "
		   " 42094.71c  0.001341000 "
		   " 42095.71c  0.002310064 "
		   " 42096.71c  0.002423388 "
		   " 42097.71c  0.001388944 "
		   " 42098.71c  0.003514494 "
		   " 42100.71c  0.001404926 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-7.76);
  MDB.resetMaterial(MObj);

  // Same ase 2636 but with Bilbao density of 7.93 g/cm3
  MObj.setMaterial(2637, "SS316L793",
		   " 06000.71c  0.001392603 "
		   " 14028.71c  0.007323064 "
		   " 14029.71c  0.000372017 "
		   " 14030.71c  0.000245523 "
		   " 15031.71c  0.000360008 "
		   " 16032.71c  0.000165168 "
		   " 16033.71c  0.000001304 "
		   " 16034.71c  0.000007390 "
		   " 16036.71c  0.000000017 "
		   " 24050.71c  0.007920331 "
		   " 24052.71c  0.152735704 "
		   " 24053.71c  0.017319003 "
		   " 24054.71c  0.004311066 "
		   " 25055.71c  0.018267327 "
		   " 26054.71c  0.038344779 "
		   " 26056.71c  0.601931034 "
		   " 26057.71c  0.013901213 "
		   " 26058.71c  0.001849996 "
		   " 27059.71c  0.000283816 "
		   " 28058.71c  0.080834464 "
		   " 28060.71c  0.031137291 "
		   " 28061.71c  0.001353516 "
		   " 28062.71c  0.004315603 "
		   " 28064.71c  0.001099057 "
		   " 42092.71c  0.002145890 "
		   " 42094.71c  0.001341000 "
		   " 42095.71c  0.002310064 "
		   " 42096.71c  0.002423388 "
		   " 42097.71c  0.001388944 "
		   " 42098.71c  0.003514494 "
		   " 42100.71c  0.001404926 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-7.93);
  MDB.resetMaterial(MObj);

  // Same ase 2636 but with a2t density of 7.85 g/cm3 and fe56.12t instead of fe56.14t (material 2636 in a2t200)
  MObj.setMaterial(2638, "SS316L785",
		   " 06000.71c  0.001392603 "
		   " 14028.71c  0.007323064 "
		   " 14029.71c  0.000372017 "
		   " 14030.71c  0.000245523 "
		   " 15031.71c  0.000360008 "
		   " 16032.71c  0.000165168 "
		   " 16033.71c  0.000001304 "
		   " 16034.71c  0.000007390 "
		   " 16036.71c  0.000000017 "
		   " 24050.71c  0.007920331 "
		   " 24052.71c  0.152735704 "
		   " 24053.71c  0.017319003 "
		   " 24054.71c  0.004311066 "
		   " 25055.71c  0.018267327 "
		   " 26054.71c  0.038344779 "
		   " 26056.71c  0.601931034 "
		   " 26057.71c  0.013901213 "
		   " 26058.71c  0.001849996 "
		   " 27059.71c  0.000283816 "
		   " 28058.71c  0.080834464 "
		   " 28060.71c  0.031137291 "
		   " 28061.71c  0.001353516 "
		   " 28062.71c  0.004315603 "
		   " 28064.71c  0.001099057 "
		   " 42092.71c  0.002145890 "
		   " 42094.71c  0.001341000 "
		   " 42095.71c  0.002310064 "
		   " 42096.71c  0.002423388 "
		   " 42097.71c  0.001388944 "
		   " 42098.71c  0.003514494 "
		   " 42100.71c  0.001404926 ", "fe56.12t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-7.85);
  MDB.resetMaterial(MObj);

  // ESS  20% vol SS316L 80% vol void
  MObj.setMaterial(2639, "SS316L20",
		   " 06000.71c  0.001392603 "
		   " 14028.71c  0.007323064 "
		   " 14029.71c  0.000372017 "
		   " 14030.71c  0.000245523 "
		   " 15031.71c  0.000360008 "
		   " 16032.71c  0.000165168 "
		   " 16033.71c  0.000001304 "
		   " 16034.71c  0.000007390 "
		   " 16036.71c  0.000000017 "
		   " 24050.71c  0.007920331 "
		   " 24052.71c  0.152735704 "
		   " 24053.71c  0.017319003 "
		   " 24054.71c  0.004311066 "
		   " 25055.71c  0.018267327 "
		   " 26054.71c  0.038344779 "
		   " 26056.71c  0.601931034 "
		   " 26057.71c  0.013901213 "
		   " 26058.71c  0.001849996 "
		   " 27059.71c  0.000283816 "
		   " 28058.71c  0.080834464 "
		   " 28060.71c  0.031137291 "
		   " 28061.71c  0.001353516 "
		   " 28062.71c  0.004315603 "
		   " 28064.71c  0.001099057 "
		   " 42092.71c  0.002145890 "
		   " 42094.71c  0.001341000 "
		   " 42095.71c  0.002310064 "
		   " 42096.71c  0.002423388 "
		   " 42097.71c  0.001388944 "
		   " 42098.71c  0.003514494 "
		   " 42100.71c  0.001404926 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-1.552); // 7.76*0.2
  MDB.resetMaterial(MObj);


// SS316L and void
// density -1.62E-2
// Reference: a2t200
MObj.setMaterial(2644, "M2644",
                 " 06000.70c  0.001392603 "
                 " 14028.70c  0.007323064 "
                 " 14029.70c  0.000372017 "
                 " 14030.70c  0.000245523 "
                 " 15031.70c  0.000360008 "
                 " 16032.70c  0.000165168 "
                 " 16033.70c  0.000001304 "
                 " 16034.70c  0.000007390 "
                 " 16036.70c  0.000000017 "
                 " 24050.70c  0.007920331 "
                 " 24052.70c  0.152735704 "
                 " 24053.70c  0.017319003 "
                 " 24054.70c  0.004311066 "
                 " 25055.70c  0.018267327 "
                 " 26054.70c  0.038344779 "
                 " 26056.70c  0.601931034 "
                 " 26057.70c  0.013901213 "
                 " 26058.70c  0.001849996 "
                 " 27059.70c  0.000283816 "
                 " 28058.70c  0.080834464 "
                 " 28060.70c  0.031137291 "
                 " 28061.70c  0.001353516 "
                 " 28062.70c  0.004315603 "
                 " 28064.70c  0.001099057 "
                 " 42092.70c  0.002145890 "
                 " 42094.70c  0.001341000 "
                 " 42095.70c  0.002310064 "
                 " 42096.70c  0.002423388 "
                 " 42097.70c  0.001388944 "
                 " 42098.70c  0.003514494 "
                 " 42100.70c  0.001404926 ", "FE56.12T", MLib);
MObj.setDensity(-1.62E-2);
MDB.resetMaterial(MObj);


// Invar36
// Reference: e-mail from Esben 30 Sep 2015
 ELog::EM << "Add MX/MT cards for Invar" << ELog::endCrit;
MObj.setMaterial(2660, "Invar36",
                 " 06000.70c  0.001000000 "
                 " 14028.70c 0.003227805 "
                 " 14029.70c 0.000163975 "
                 " 14030.70c 0.000108220 "
                 " 29063.70c 0.003457501 "
                 " 29065.70c 0.001542499 "
                 " 25055.70c 0.006000000 "
                 " 15031.70c 0.000250000 "
                 " 16032.70c 0.000237476 "
                 " 16033.70c 0.000001875 "
                 " 16034.70c 0.000010625 "
                 " 16036.70c 0.000000024 "
                 " 24050.70c 0.000217250 "
                 " 24052.70c 0.004189450 "
                 " 24053.70c 0.000475050 "
                 " 24054.70c 0.000118250 "
                 " 28058.70c 0.251884530 "
                 " 28060.70c 0.097025471 "
                 " 28061.70c 0.004217629 "
                 " 28062.70c 0.013447651 "
                 " 28064.70c 0.003424721 "
                 " 26054.70c 0.035654500 "
                 " 26056.70c 0.559699400 "
                 " 26057.70c 0.012925900 "
                 " 26058.70c  0.001720200 ", "", MLib);
 MObj.setDensity(-8.11);
 MDB.resetMaterial(MObj);


  // Beryllium
  // Reference: Material data compilation table
  MObj.setMaterial(4000, "Beryllium",
                 " 04009.70c  1.0 ", "BE.10T", MLib);
  MObj.setMXitem(4009, 70, 'c', "h", "model");
  MObj.setDensity(-1.85);
  MDB.resetMaterial(MObj);

  // Be + 5% H2O coolant - generated by mixtures.py 7 May 2015
  MObj.setMaterial(4005, "Be5H2O",
		   " 1001.70c 0.0272983770406 "
		   " 4009.70c 0.959052434439 "
		   " 8016.70c 0.0136491885203 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.8075);
  MDB.resetMaterial(MObj);

  // Be + 10% H2O coolant
  MObj.setMaterial(4010, "Be10H2O",
		   " 1001.70c 0.055140611 "
		   " 4009.70c 0.91728908 "
		   " 8016.70c 0.027570305 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.76);
  MDB.resetMaterial(MObj);

  // Be + 30% vol H2O coolant - generated by mixtures.py 12 Jan 2016
  MObj.setMaterial(4030, "Be30H2O",
		   " 1001.70c 0.171984352096 "
		   " 4009.70c 0.742023471856 "
		   " 8016.70c 0.0859921760479 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.595);
  MDB.resetMaterial(MObj);



  // Tungsten at 300 K
  MObj.setMaterial(7400, "Tungsten",
		   "74180.50c  0.001200000 "
		   "74182.70c  0.265000000 "
		   "74183.70c  0.143100000 "
		   "74184.70c  0.306400000 "
		   "74186.70c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-19.298); // density at 300 K according to the Material handbook
  MDB.resetMaterial(MObj);  

  // Tungsten at 600 K
  MObj.setMaterial(7401, "Tungsten600K",
		   "74180.50c  0.001200000 "
		   "74182.71c  0.265000000 "
		   "74183.71c  0.143100000 "
		   "74184.71c  0.306400000 "
		   "74186.71c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-19.298); // density at 300 K according to the Material handbook. YJL says at 600K we should use the same density
  MDB.resetMaterial(MObj);  


  // Tunsten as in a2t200 mat 7400 with average density 15.1 g/cm3
  // Tungsten bricks are arranged in square lattice.
  // The length X width of one brick is 3 cm X 1 cm and the gap between bricks is 0.2 cm.
  // Hence, the filling factor of the infinite square lattice is:
  // 3*1/(3+0.2)/(1+0.2) = 0.78125
  // Natural Tungsten density is 19.298 \approx 19.3 g/cm3 [Material book] at 300 K,
  // therefore the density of homogenised material is 15.0766 \approx 15.1 g/cm3 

  MObj.setMaterial(7451, "Tungsten151",
		   "74180.50c  0.001200000 "
		   "74182.71c  0.265000000 "
		   "74183.71c  0.143100000 "
		   "74184.71c  0.306400000 "
		   "74186.71c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-15.1);
  MDB.resetMaterial(MObj);

  // Bilbao M30001 - same ase 7400 but at 15.6 g/cm3
  MObj.setMaterial(7456, "Tungsten156",
		   "74180.50c  0.001200000 "
		   "74182.71c  0.265000000 "
		   "74183.71c  0.143100000 "
		   "74184.71c  0.306400000 "
		   "74186.71c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-15.6);
  MDB.resetMaterial(MObj);


  
  
  return;
}

} // NAMESPACE ModelSupport
