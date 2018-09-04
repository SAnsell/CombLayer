/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/DBModify.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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

void
processMaterialFile(const std::string& matFile)
  /*!
    Read materials from a file
    \param matFile
  */
{
  ELog::RegMethod RegA("DBModify[F]","processMaterialFile");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  DB.readFile(matFile);
  return;
}
  
void
cloneBasicMaterial()
  /*!
    Clone ESS materials if not using the ESS Database materials
  */
{
  ELog::RegMethod RegA("DBModify[F]","cloneBasicMaterial");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  ELog::EM<<"WARNING -- this is the basic materials build \n"
	  <<"       The results from simualation will be approximate \n"
	  <<ELog::endCrit;

  // original name to use
  DB.cloneMaterial("CastIron","Iron");
  DB.cloneMaterial("Aluminium","Aluminium20K");
  DB.cloneMaterial("Tungsten_15.1g","Tungsten151");
  DB.cloneMaterial("Iron_10H2O","Iron_10H2O");
  DB.cloneMaterial("Void","Helium");
  DB.cloneMaterial("Void","M2644");
  DB.cloneMaterial("Stainless304","SS316L");
  DB.cloneMaterial("Stainless304","SS316L_7.85g");

  // Al.20t -- actually available from Los Alamos-t2
  DB.removeThermal("Aluminium");
  DB.removeThermal("CH4inFoam");
  DB.removeThermal("CH4+Al22K");
  DB.removeThermal("CH4+Al26K");
  DB.removeThermal("CH4Al+Argon");
  DB.removeThermal("Aluminium6061");

  DB.removeThermal("Al2024");
  DB.removeThermal("AlFoam");
  DB.removeThermal("Al2214");
  DB.removeThermal("Boral5");
  DB.removeThermal("Boral5Degrade");
  DB.removeThermal("TS1Boral");
  DB.removeThermal("Alum5251");

  // silicon si.80t - si.84t (made by S. Ansell)
  DB.removeThermal("SiCrystal");
  DB.removeThermal("Silicon20K");
  DB.removeThermal("Silicon80K");
  DB.removeThermal("Silicon300K");


  // HYDROGENS:
  DB.overwriteMaterial("ParaH2","H2para19K");
  DB.cloneMaterial("H2para19K","ParaOrtho%0.5");
  DB.cloneMaterial("H2para19K","HPARA");
  
  return;
}

void
cloneESSMaterial()
  /*!
    Clone ESS materials if not using the ESS Database materials
  */
{
  ELog::RegMethod RegA("DBModify[F]","cloneESSMaterial");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  
  DB.cloneMaterial("Void","Helium");
  DB.cloneMaterial("Void","M2644");
  DB.cloneMaterial("Stainless304","SS316L");
  DB.cloneMaterial("Stainless304","SS316L_7.85g");

  DB.cloneMaterial("CastIron","Iron");
  DB.cloneMaterial("ParaH2","HPARA");
  DB.cloneMaterial("Aluminium","Aluminium20K");

  return;
}
    
void
addESSMaterial()
  /*!
     Initialize the database of materials
     this adds extra materials used by the ESS target division
   */
{

  ELog::RegMethod RegA("DBModify[F]","addESSMaterial");

  const std::string MLib="hlib=.70h pnlib=70u";
  ModelSupport::DBMaterial& MDB=ModelSupport::DBMaterial::Instance();

  MonteCarlo::Material MObj;
  // ESS materials
  
  // offset 1000 to avoid overlapping with the Stuart's numbering
  // (this makes problems - my materials are used instead of Stuarts
  // in case they have the same number)
  int imat=200;

  // According to Fig. 7 in 10.1103/PhysRevB.91.180301, this cross section actually had 0.5% ortho-H,
  // therefore using this material is the same as adding 0.5 ortho-H
  MObj.setMaterial(imat++/*101*/,"HPARA"," 1001.70c 1.0 ","hpara.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);
  
  MObj.setMaterial(imat++/*102*/,"HORTHO"," 1004.70c 1.0 ","hortho.10t", MLib);
  MObj.setDensity(-7.0e-2);
  MDB.resetMaterial(MObj);

  MObj.setMaterial(imat++/*103*/, "LH05ortho", " 1001.70c 99.5 "
                   "1004.70c 0.5 ","hpara.10t hortho.10t", MLib);
  MObj.setDensity(-7.0e-2*(0.07/0.0689677));
  // 0.07/0.0689677 because CL for some reason changes the density. This correction works for 0.5% ortho only !!! todo !!!
  MDB.resetMaterial(MObj);

  // HPARA + 5% vol Al
  // calculated with mixtures.py (commit b777e0f)
  MObj.setMaterial(imat++, "HPARAAl05",
		   " 1001.70c 0.928781449666 13027.70c 0.0712185503336 ", "hpara.10t al27.10t", MLib);
  MObj.setDensity(-0.203);
  MDB.resetMaterial(MObj);

  // Generic light water
  // Simplified light water for  basic neutronic simulations
  MObj.setMaterial(imat++/*110*/, "H2O", " 1001.70c 0.66666667 "
		   " 8016.70c 0.33333333 ", "lwtr.10t", MLib);
  MObj.setDensity(-1.0);
  MDB.resetMaterial(MObj);

  // Mix of water and 10% vol Be
  MObj.setMaterial(imat++/*141*/, "H2OBe10",
		   " 04009.70c 0.12046753633 "
                   " 1001.70c 0.58635497578 "
                   " 8016.70c 0.29317748789 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009, 70, 'c', "h", "model");
  MObj.setDensity(-1.085);
  MDB.resetMaterial(MObj);

  // Mix of water and 4.7% vol Al as calculated by Marc (email from LZ 15.01.16) - checked by AT
  MObj.setMaterial(imat++/*147*/, "H2OAl47",
		   " 1001.70c 0.647478328682 "
		   " 13027.70c 0.0287825069767 "
		   " 8016.70c 0.323739164341 ", "lwtr.10t al27.12t", MLib);
  MObj.setDensity(-1.0799);
  MDB.resetMaterial(MObj);

  // Natural Helium. Density -1.74E-4 g/cm3
  // Temperature: 300 K
  // Reference: Material Data Compilation 21 Sep 2015
  MObj.setMaterial(imat++/*200*/, "Helium",
  		   " 2003.70c 1.34E-6 "
  		   " 2004.70c 0.999998660 ", "", MLib);
  MObj.setDensity(-1.74E-4);
  MDB.resetMaterial(MObj);

  MObj.setMaterial(imat++/*1300*/, "Aluminium", " 13027.70c 1.0 ", "al27.12t", MLib);
  MObj.setDensity(-2.7);
  MDB.resetMaterial(MObj); 

  MObj.setMaterial(imat++/*1301*/, "Aluminium20K", " 13027.70c 1.0 ", "al27.10t", MLib);
  MObj.setDensity(-2.73);
  MDB.resetMaterial(MObj); 

  // ESS Iron
  MObj.setMaterial(imat++/*2600*/, "Iron",
		   " 26054.70c  0.058450000 "
		   " 26056.70c  0.917540000 "
		   " 26057.70c  0.021190000 "
		   " 26058.70c  0.002820000 ", "fe56.12t", MLib);
  //  MObj.setMXitem(6000, 70, 'c', "h", "06012");
  MObj.setDensity(-7.85);
  MDB.resetMaterial(MObj);

  // Homogeneous mixture of Iron and 10% volume H2O
  MObj.setMaterial(imat++/*2610*/, "Iron_10H2O",
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
  MObj.setMaterial(imat++/*2636*/, "SS316L",
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

  // Same as 2636 but with Bilbao density of 7.93 g/cm3
  MObj.setMaterial(imat++/*2637*/, "SS316L793",
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
  MObj.setMaterial(imat++/*2638*/, "SS316L_7.85g",
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
  MObj.setMaterial(imat++/*2639*/, "SS316L20",
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

  // Same ase 2636L785 but with 50%void (material 242 in TSM141108V3000)
  MObj.setMaterial(imat++/*2638*/, "SS316L3925",
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
  MObj.setDensity(-3.925);
  MDB.resetMaterial(MObj);


// SS316L and void
// density -1.62E-2
// Reference: a2t200
MObj.setMaterial(imat++/*2644*/, "M2644",
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
//\todo kbat ELog::EM << "Add MX/MT cards for Invar" << ELog::endCrit;
MObj.setMaterial(imat++/*2660*/, "Invar36",
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
  MObj.setMaterial(imat++/*4000*/, "Beryllium",
                 " 04009.70c  1.0 ", "BE.10T", MLib);
  MObj.setMXitem(4009, 70, 'c', "h", "model");
  MObj.setDensity(-1.85);
  MDB.resetMaterial(MObj);

  // Be + 5% H2O coolant - generated by mixtures.py 7 May 2015
  MObj.setMaterial(imat++/*4005*/, "Be5H2O",
		   " 1001.70c 0.0272983770406 "
		   " 4009.70c 0.959052434439 "
		   " 8016.70c 0.0136491885203 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.8075);
  MDB.resetMaterial(MObj);

  // Be + 10% H2O coolant
  MObj.setMaterial(imat++/*4010*/, "Be10H2O",
		   " 1001.70c 0.055140611 "
		   " 4009.70c 0.91728908 "
		   " 8016.70c 0.027570305 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.76);
  MDB.resetMaterial(MObj);

  // Be + 30% vol H2O coolant - generated by mixtures.py 12 Jan 2016
  MObj.setMaterial(imat++/*4030*/, "Be30H2O",
		   " 1001.70c 0.171984352096 "
		   " 4009.70c 0.742023471856 "
		   " 8016.70c 0.0859921760479 ", "lwtr.10t be.10t", MLib);
  MObj.setMXitem(4009,70,'c',"h","model");
  MObj.setDensity(-1.595);
  MDB.resetMaterial(MObj);

  // Tungsten at 300 K
  MObj.setMaterial(imat++/*7400*/, "Tungsten",
		   "74180.50c  0.001200000 "
		   "74182.70c  0.265000000 "
		   "74183.70c  0.143100000 "
		   "74184.70c  0.306400000 "
		   "74186.70c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-19.298); // density at 300 K according to the Material handbook
  MDB.resetMaterial(MObj);  

  // Tungsten at 600 K
  MObj.setMaterial(imat++/*7401*/, "Tungsten600K",
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

  MObj.setMaterial(imat++/*7451*/, "Tungsten_15.1g",
		   "74180.50c  0.001200000 "
		   "74182.71c  0.265000000 "
		   "74183.71c  0.143100000 "
		   "74184.71c  0.306400000 "
		   "74186.71c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-15.1);
  MDB.resetMaterial(MObj);

  // Bilbao M30001 - same ase 7400 but at 15.6 g/cm3
  MObj.setMaterial(imat++/*7456*/, "Tungsten156",
		   "74180.50c  0.001200000 "
		   "74182.71c  0.265000000 "
		   "74183.71c  0.143100000 "
		   "74184.71c  0.306400000 "
		   "74186.71c  0.284300000 ",
		   "",MLib);
  MObj.setDensity(-15.6);
  MDB.resetMaterial(MObj);

  
  MObj.setMaterial(imat++/*2660*/, "Invar36",
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

  // Regular concrete (Alan's mat #815)
  // Temperature: 600 K
  // Reference: TSM141108V2005CFG01
  MObj.setMaterial(imat++, "Concrete",
		   " 01001.70c     0.075923001 "
		   " 01002.70c     0.000011826 "
		   " 08016.70c     0.543531605 "
		   " 08017.70c     0.000207006 "
		   " 08018.03c     0.001115068 "
		   " 12024.70c     0.001407861 "
		   " 12025.70c     0.000178233 "
		   " 12026.70c     0.000196234 "
		   " 13027.70c     0.016224264 "
		   " 14028.70c     0.093658005 "
		   " 14029.70c     0.004757899 "
		   " 14030.70c     0.003140112 "
		   " 16032.70c     0.005470979 "
		   " 16033.70c     0.000043196 "
		   " 16034.70c     0.000244780 "
		   " 16036.70c     0.000000576 "
		   " 19039.70c     0.008592435 "
		   " 19040.70c     0.000001078 "
		   " 19041.70c     0.000620094 "
		   " 20040.70c     0.216567156 "
		   " 20042.70c     0.001445404 "
		   " 20043.70c     0.000301591 "
		   " 20044.70c     0.004660145 "
		   " 20046.70c     0.000008936 "
		   " 20048.70c     0.000417760 "
		   " 22046.70c     0.000149326 "
		   " 22047.70c     0.000134664 "
		   " 22048.70c     0.001334337 "
		   " 22049.70c     0.000097921 "
		   " 22050.70c     0.000093758 "
		   " 25055.70c     0.000871518 "
		   " 26054.70c     0.001059528 "
		   " 26056.70c     0.016632326 "
		   " 26057.70c     0.000384113 "
		   " 26058.70c     0.000051118 "
		   " 28058.70c     0.000317338 "
		   " 28060.70c     0.000122238 "
		   " 28061.70c     0.000005314 "
		   " 28062.70c     0.000016942 "
		   " 28064.70c     0.000004315 ", "lwtr.10t", MLib);
  MObj.setDensity(-2.35);
  MDB.resetMaterial(MObj);

  // Aluminium (tmp for matmesh7 only)
  MObj.setMaterial(imat++, "AluminiumBe", " 13027.70c 1.0 ", "al27.12t", MLib);
  MObj.setDensity(-2.7);
  MDB.resetMaterial(MObj);

  // SS316L and 10% vol water
  // Reference: mixtures.py 19 Jan 2017 git hash:14b5a90
  MObj.setMaterial(imat++, "SS316L_10H2O",
		   " 01001.70c 0.0782280522543 "
		   " 06000.71c 0.00122919206962 "
		   " 08016.70c 0.0391140261271 "
		   " 14028.71c 0.00646376045012 "
		   " 14029.71c 0.000328363752027 "
		   " 14030.71c 0.00021671282089 "
		   " 15031.71c 0.000317763913046 "
		   " 16032.71c 0.000145786843598 "
		   " 16033.71c 1.15098592979e-06 "
		   " 16034.71c 6.52284204076e-06 "
		   " 16036.71c 1.50051846675e-08 "
		   " 24050.71c 0.00699094289899 "
		   " 24052.71c 0.13481337905 "
		   " 24053.71c 0.0152867551925 "
		   " 24054.71c 0.00380519655552 "
		   " 25055.71c 0.0161238008833 "
		   " 26054.70c 0.0338453229371 "
		   " 26056.70c 0.531299195428 "
		   " 26057.70c 0.0122700157746 "
		   " 26058.70c 0.00163291362436 "
		   " 27059.71c 0.000250512440682 "
		   " 28058.71c 0.0713491799894 "
		   " 28060.71c 0.0274835765589 "
		   " 28061.71c 0.00119469161944 "
		   " 28062.71c 0.00380920117451 "
		   " 28064.71c 0.00097009136736 "
		   " 42092.71c 0.00189408680742 "
		   " 42094.71c 0.00118364427289 "
		   " 42095.71c 0.00203899628905 "
		   " 42096.71c 0.00213902261536 "
		   " 42097.71c 0.00122596242428 "
		   " 42098.71c 0.00310209596958 "
		   " 42100.71c 0.00124006906319 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-7.084);
  MDB.resetMaterial(MObj);

  // SS316L and 20% vol water
  // Reference: mixtures.py 1 Dec 2016 git hash:14b5a90
  MObj.setMaterial(imat++, "SS316L_20H2O",
		   " 01001.70c 0.153498348518 "
		   " 06000.71c 0.00107195960904 "
		   " 08016.70c 0.076749174259 "
		   " 14028.71c 0.00563694665486 "
		   " 14029.71c 0.000286361007319 "
		   " 14030.71c 0.000188991937465 "
		   " 15031.71c 0.00027711704982 "
		   " 16032.71c 0.000127138477158 "
		   " 16033.71c 1.0037572303e-06 "
		   " 16034.71c 5.68847080668e-06 "
		   " 16036.71c 1.30857921128e-08 "
		   " 24050.71c 0.00609669440768 "
		   " 24052.71c 0.117568686514 "
		   " 24053.71c 0.0133313454623 "
		   " 24054.71c 0.00331845373297 "
		   " 25055.71c 0.0140613202105 "
		   " 26054.70c 0.0295159886238 "
		   " 26056.70c 0.463337904539 "
		   " 26057.70c 0.0107004931432 "
		   " 26058.70c 0.00142403900385 "
		   " 27059.71c 0.000218468069075 "
		   " 28058.71c 0.062222528909 "
		   " 28060.71c 0.0239680068813 "
		   " 28061.71c 0.00104187229396 "
		   " 28062.71c 0.00332194609996 "
		   " 28064.71c 0.000846001848359 "
		   " 42092.71c 0.00165180414335 "
		   " 42094.71c 0.00103223807196 "
		   " 42095.71c 0.00177817748654 "
		   " 42096.71c 0.00186540891627 "
		   " 42097.71c 0.00106914308472 "
		   " 42098.71c 0.00270529046268 "
		   " 42100.71c 0.00108144526881 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-6.408);
  MDB.resetMaterial(MObj);

  // SS316L and 40% vol water
  // Reference: mixtures.py 1 Dec 2016; git hash: 14b5a90
  MObj.setMaterial(imat++, "SS316L_40H2O",
		   " 01001.70c 0.295812213697 "
		   " 06000.71c 0.000774679535654 "
		   " 08016.70c 0.147906106848 "
		   " 14028.71c 0.00407368634068 "
		   " 14029.71c 0.000206946241546 "
		   " 14030.71c 0.000136579946785 "
		   " 15031.71c 0.000200265854857 "
		   " 16032.71c 9.18799324322e-05 "
		   " 16033.71c 7.25391310009e-07 "
		   " 16034.71c 4.11092161117e-06 "
		   " 16036.71c 9.45678855073e-09 "
		   " 24050.71c 0.00440593503052 "
		   " 24052.71c 0.0849640739338 "
		   " 24053.71c 0.00963424407532 "
		   " 24054.71c 0.00239816703472 "
		   " 25055.71c 0.0101617793427 "
		   " 26054.70c 0.0213304980604 "
		   " 26056.70c 0.33484320651 "
		   " 26057.70c 0.0077329901141 "
		   " 26058.70c 0.00102911888186 "
		   " 27059.71c 0.000157881641136 "
		   " 28058.71c 0.0449667313918 "
		   " 28060.71c 0.0173211045312 "
		   " 28061.71c 0.000752936153649 "
		   " 28062.71c 0.0024006908847 "
		   " 28064.71c 0.000611385273777 "
		   " 42092.71c 0.00119371929313 "
		   " 42094.71c 0.000745973732149 "
		   " 42095.71c 0.00128504628157 "
		   " 42096.71c 0.00134808634661 "
		   " 42097.71c 0.000772644100989 "
		   " 42098.71c 0.00195504862475 "
		   " 42100.71c 0.00078153459479 ", "fe56.14t", MLib);
  MObj.setMXitem(6000, 71, 'c', "h", "06012");
  MObj.setDensity(-5.056);
  MDB.resetMaterial(MObj);

  MObj.setMaterial(imat++,"Silver","47107.70c 0.51839 "
                   "47109.70c 0.48161 ","",MLib); // IAEA table of nuclides
  MObj.setDensity(-10.49); // wiki
  MDB.resetMaterial(MObj);

  // Skanska concrete
  // Source: MATER.INP from MARS. Material "ESSConc"
  // (Composition received from Skanska)
  // The natural abundance data is from the 1997 report
  // of the IUPAC Subcommittee for Isotopic Abundance Measurements
  // by K.J.R. Rosman, P.D.P. Taylor Pure Appl. Chem. 1999, 71, 1593-1607.
  // See the Elements section in tables.xml. The weight fractions
  // calculated in this section were converted into atomic fractions
  // through MCNP table 40
  MObj.setMaterial(imat++,"SkanskaConcrete",
		   " 1001.70c  7.59312534134E-02 "
		   " 1002.70c  8.71640551643E-06 "
		   " 8016.70c  5.43526947592E-01 "
		   " 8017.70c  2.06950892017E-04 "
		   " 8018.03c  1.11647473471E-03 " // no ENDF/B7 for O18 -> JEFF
		   " 12024.70c 1.40785315301E-03 "
		   " 12025.70c 1.78230050329E-04 "
		   " 12026.70c 1.96234647556E-04 "
		   " 13027.70c 1.62241753736E-02 "
		   " 14028.70c 9.36645875617E-02 "
		   " 14029.70c 4.75605601141E-03 "
		   " 14030.70c 3.13523026267E-03 "
		   " 16032.70c 5.46727493842E-03 "
		   " 16033.70c 4.37717251109E-05 "
		   " 16034.70c 2.47073209942E-04 "
		   " 16036.70c 1.15370377360E-06 "
		   " 19039.70c 8.59239038130E-03 "
		   " 19040.70c 1.07825377081E-06 "
		   " 19041.70c 6.20088236070E-04 "
		   " 20040.70c 2.16565974405E-01 "
		   " 20042.70c 1.44539631664E-03 "
		   " 20043.70c 3.01589179418E-04 "
		   " 20044.70c 4.66011784421E-03 "
		   " 20046.70c 8.93555255277E-06 "
		   " 20048.70c 4.17755887748E-04 "
		   " 22046.70c 1.49322931163E-04 "
		   " 22047.70c 1.34665826718E-04 "
		   " 22048.70c 1.33432860539E-03 "
		   " 22049.70c 9.79193110454E-05 "
		   " 22050.70c 9.37599157158E-05 "
		   " 25055.70c 8.71513678950E-04 "
		   " 26054.70c 1.05952337437E-03 "
		   " 26056.70c 1.66322374761E-02 "
		   " 26057.70c 3.84109417430E-04 "
		   " 26058.70c 5.11182229355E-05 "
		   " 28058.70c 3.18268353245E-04 "
		   " 28060.70c 1.21673874326E-04 "
		   " 28061.70c 5.26856163138E-06 "
		   " 28062.70c 1.67366321839E-05 "
		   " 28064.70c 4.24408614207E-06", "lwtr.10t", MLib);
  MObj.setDensity(-2.35);
  MDB.resetMaterial(MObj);


  // Copper - natural composition from IAEA table of nuclides
  MObj.setMaterial(imat++,"Copper",
		   " 29063.70c 0.6915 "
		   " 29065.70c 0.3085 ","",MLib);
  MObj.setDensity(-8.94); // as in the outp file produced by MARS
  MDB.resetMaterial(MObj);

  // SS304L composition from ESS Design Update WP3
  // Reference: Material Data Compilation 29 Mar 2017
  MObj.setMaterial(imat++, "SS304L",
		   " 6000.70c      0.001366050 "
		   " 14028.70c      0.017958587 "
		   " 14029.70c      0.000912310 "
		   " 14030.70c      0.000602105 "
		   " 15031.70c      0.000794573 "
		   " 16032.70c      0.000486055 "
		   " 16033.70c      0.000003838 "
		   " 16034.70c      0.000021747 "
		   " 16036.70c      0.000000051 "
		   " 24050.70c      0.008683351 "
		   " 24052.70c      0.167449786 "
		   " 24053.70c      0.018987462 "
		   " 24054.70c      0.004726381 "
		   " 25055.70c      0.019910026 "
		   " 26054.70c      0.038864395 "
		   " 26056.70c      0.610087892 "
		   " 26057.70c      0.014089590 "
		   " 26058.70c      0.001875066 "
		   " 28058.70c      0.063434555 "
		   " 28060.70c      0.024434877 "
		   " 28061.70c      0.001062167 "
		   " 28062.70c      0.003386654 "
		   " 28064.70c      0.000862481 ", "fe56.12t", MLib);
  MObj.setMXitem(6000, 70, 'c', "h", "06012");
  MObj.setDensity(-7.85);
  MDB.resetMaterial(MObj);

  // Samarium Cobalt - Sm2Co17
  // rbfrend2-9100
  MObj.setMaterial(imat++,"Sm2Co17",
		   " 27059.70c  8.5   "
		   " 62147.70c  0.521 "
		   " 62149.70c  0.138 "
		   " 62150.70c  0.074 "
		   " 62152.70c  0.267 ","",MLib);
  MObj.setDensity(-8.4);
  MDB.resetMaterial(MObj);

  // Clay Till Lera
  // Natural isotopic composition from
  // Material Handbook version 20180822, page 178
  // [ESS-0028465]
  // The natural abundance data is from the 1997 report
  // of the IUPAC Subcommittee for Isotopic Abundance Measurements
  // by K.J.R. Rosman, P.D.P. Taylor Pure Appl. Chem. 1999, 71, 1593-1607.
  // See the Elements section in tables.xml. The weight fractions
  // calculated in this section were converted into atomic fractions
  // through MCNP table 40
  MObj.setMaterial(imat++, "ClayTillLera",
		   " 1001.70c  2.89426866629E-01 "
		   " 8016.70c  5.02562931167E-01 "
		   " 8017.70c  1.36236207426E-04 "
		   " 8018.03c  7.34987446678E-04 "  // no ENDF/B7 for O18 -> JEFF
		   " 12024.70c 3.44200455094E-03 "
		   " 12025.70c 4.35751933154E-04 "
		   " 12026.70c 4.79762856971E-04 "
		   " 13027.70c 2.72353121845E-02 "
		   " 14028.70c 8.26686973895E-02 "
		   " 14029.70c 4.19771444384E-03 "
		   " 14030.70c 2.76716427289E-03 "
		   " 19039.70c 9.71608474624E-04 "
		   " 19040.70c 1.21896294015E-07 "
		   " 19041.70c 7.01185155396E-05 "
		   " 20040.70c 1.13111187300E-02 "
		   " 20042.70c 7.54922774827E-05 "
		   " 20043.70c 1.57518628845E-05 "
		   " 20044.70c 2.43395462852E-04 "
		   " 20046.70c 4.66721689702E-07 "
		   " 20048.70c 2.18192461108E-05 "
		   " 22046.70c 4.61402799920E-03 "
		   " 22047.70c 4.16101444098E-03 "
		   " 22048.70c 4.12298171212E-02 "
		   " 22049.70c 3.02568296547E-03 "
		   " 22050.70c 2.89704909694E-03 "
		   " 25055.70c 2.37268078242E-03 "
		   " 26054.70c 1.19376730165E-05 "
		   " 26056.70c 1.87395926222E-04 "
		   " 26057.70c 4.32778909360E-06 "
		   " 26058.70c 5.75949379621E-07 "
		   " 37085.70c 1.05391642889E-02 "
		   " 37087.70c 4.06408386884E-03 "
		   " 56130.70c 1.00614985046E-07 "
		   " 56132.70c 9.58690110538E-08 "
		   " 56134.70c 2.29421225408E-06 "
		   " 56135.70c 6.25711426885E-06 "
		   " 56136.70c 7.45500306226E-06 "
		   " 56137.70c 1.06613948278E-05 "
		   " 56138.70c 6.80556198112E-05 ",
		   "lwtr.10t al27.12t fe56.12t zr/h.10t", MLib);
  MObj.setMXitem(6000, 70, 'c', "h", "06012");
  MObj.setDensity(-2.0);
  MDB.resetMaterial(MObj);

  // Reference: material StbTCABL in MARS
  // Cables about upstream and downstram walls at the top (in horizontal segments) of a stub
  // (Chemical composition is the same as for MARS 'CABL' but density is reduced by factor of 0.7)
  // Elements: H, C, Cl, Cu
  // mass fractions converted into atomic fractions by table 40
  MObj.setMaterial(imat++,"StbTCABL",
		   " 1001.70c  3.88760701228E-01 "
		   " 1002.70c  4.47126239451E-05 "
		   " 6012.70c  2.63615249316E-01 "
		   " 6013.70c  2.85119090550E-03 "
		   " 17035.70c 1.00520773169E-01 "
		   " 17037.70c 3.21273836065E-02 "
		   " 29063.70c 1.46695727581E-01 "
		   " 29065.70c 6.53842615705E-02 ","",MLib);
  MObj.setDensity(-2.0272);
  MDB.resetMaterial(MObj);

  return;
}

} // NAMESPACE ModelSupport
