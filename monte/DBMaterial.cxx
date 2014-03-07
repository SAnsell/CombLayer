/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/DBMaterial.cxx
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
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"

namespace ModelSupport
{

DBMaterial::DBMaterial() 
  /*!
    Constructor
  */
{
  initMaterial();
  initMXUnits();
}

DBMaterial&
DBMaterial::Instance() 
  /*!
    Singleton constructor
    \return DBMaterial object
  */
{
  static DBMaterial DObj;
  return DObj;
}

void
DBMaterial::checkNameIndex(const int MIndex,const std::string& MName) const
  /*!
    Check the name and index is not already in the system
    throw on error
    \param MIndex :: Index Number
    \param MName :: Material name
  */
{
  if ( MStore.find(MIndex)!=MStore.end() )
    throw ColErr::InContainerError<int>(MIndex,"MIndex");
  if ( IndexMap.find(MName)!=IndexMap.end() )
    throw ColErr::InContainerError<std::string>(MName,"MName");
  return;
}

int
DBMaterial::getFreeNumber() const
  /*!
    Get the first available free number
    \return unallocated material number
   */
{
  ELog::RegMethod RegA("DBMaterial","getFreeNumber");
  int FNum(static_cast<int>(MStore.size()));  
  FNum++;               // Avoid possible zero
  while(MStore.find(FNum)!=MStore.end())
    {
      FNum++;
    }
  return FNum;
  
}

void
DBMaterial::initMaterial()
  /*!
     Initialize the database of materials
   */
{
  ELog::RegMethod RegA("DBMaterial","initMaterial");

  const std::string MLib="hlib=.70h pnlib=70u";

  MonteCarlo::Material MObj;
  // TWO ULTRA SPECIAL MATERIALS!!!
  MObj.setMaterial(-1,"InValid","00000.00c 1.0","",MLib); 
  setMaterial(MObj);
  MObj.setMaterial(0,"Void","00000.00c 1.0","",MLib);
  setMaterial(MObj);


  MObj.setMaterial(1,"MachineTugnsten",
		   "26000.55c 0.001773 28000.50c 0.003935 74182.24c 0.0165487 "
		   "74183.24c 0.0089819772 74184.24c 0.0192722  74186.24c "
		   " 0.018014274","",MLib);
  setMaterial(MObj);
  MObj.setMaterial(3,"Stainless304",
		   "6000.70c 3.18640e-4 14028.24c 1.70336e-3 "
		   "15031.24c 6.95038e-5 16032.60c 4.47520e-5 "
		   "24000.50c 1.74813e-2 25055.60c 1.74159e-3 "
		   "26054.24c 3.380e-3 26056.24c 5.3455e-2 "
		   "26057.24c 1.282177e-3 28000.50c 8.15128e-3",
		   "",MLib);
  setMaterial(MObj);
  // Material #5: Aluminum 5083
  MObj.setMaterial(5,"Aluminium",
		   "13027.24c 0.054381 12024.70c 0.002070 12025.70c "
		   "0.0002621 12026.70c 0.0002885 14028.24c 0.000217 "
		   "22048.70c 0.000087 24052.24c 0.000078 24053.24c 0.000010 "
		   "25055.60c 0.000408 26056.24c 0.000214 26057.24c 0.000005 "
		   "29063.24c 0.00004  29065.24c 0.00002 "
		   "30000.70c 0.000145 ","al.20t",MLib);
  setMaterial(MObj);

  // Material #4 : Methane + 10% Al.
  MObj.setMaterial(4,"CH4inFoam",
		   "6000.70c 0.0167364 1001.70c 0.066945 13027.24c 0.0060185",
		   "smeth.26t al.20t",MLib);
  setMaterial(MObj);
  // Material #6: Gadolinium
  // Total atom density 0.03044578 - 7.95 grams per cc
  MObj.setMaterial(6,"Gadolinium",
		   "64152.55c 6.089e-6 64154.55c 6.63718e-4 "
		   "64155.55c 4.50597e-3 64156.55c 6.23225e-3 "
		   "64157.55c 4.764764e-3 64158.55c 7.56273e-3 "
		   "64160.55c 6.65545e-3 ","",MLib);
  setMaterial(MObj);
  //  Material #7: Cadmium
  //  Total atom density 0.04741207 - 8.85 grams per cc
  MObj.setMaterial(7,"Cadmium","48000.42c 0.04741207","",MLib);
  setMaterial(MObj);

  //  Material #8: Inconel
  // Total atom density 8.3309e-02 - 7.8246g/cc
  MObj.setMaterial(8,"Inconnel",
		   "28000.50c 4.2821e-02  24000.50c 1.6328e-02 "
		   "26000.55c 2.4160e-02","",MLib);
  setMaterial(MObj);
  // Material #9: helium at atmospheric pressure
  // Total atom density 2.45e-05
  MObj.setMaterial(9,"helium","2004.50c 2.45e-5","",MLib);
  setMaterial(MObj);

  // Material #10: Beryllium + heavy water
  // Total atom density 0.1187475
  MObj.setMaterial(10,"BeInD2O",
		   "4009.80c 0.0988968 1001.70c 3.31e-5 1002.24c 0.0132007 "
		   "8016.24c 0.0066169","hwtr.01t be.61t",MLib);
  setMaterial(MObj);

  // Material #11: Light water
  // Total atom density 0.10028340 - 1.0 grams per cc
  MObj.setMaterial(11,"H2O",
		   "1001.70c 0.06684557 1002.24c 0.00001003 "
		   "8016.24c 0.03342780","lwtr.01t",MLib);
  setMaterial(MObj);

  // Material #15 ZrH2 number density == 0.1087 on a density of 5.61g/cc
  MObj.setMaterial(15,"ZrH2","1001.70c 2.3316e-3 "
		   "40000.60c 0.10636","h/zr.01t zr/h.01t",MLib);
  setMaterial(MObj);

  // Material #16: Liquid para-hydrogen (original)
  //Total atom density 0.041957 -  20 K; 0.07021 grams per cc
  MObj.setMaterial(16,"paraH2-org","1001.70c 0.041957","parah.20t",MLib);
  setMaterial(MObj);

  // Material #17 Magnox
  // Total atom density 0.04424 - 1.74 gm/cc
  MObj.setMaterial(17,"Magnox","12000.60c 0.03915 13027.24c "
		   "3.32e-3 4009.80c 1.77e-3","",MLib);
  setMaterial(MObj);

  //Material #22  Cooled Be9 [nat 100%]
  MObj.setMaterial(22,"Be77K","4009.80c 0.0988968","be.77t",MLib);
  setMaterial(MObj);

  // Material #23 Lead  11340kg m-3 [0.03296371]
  //204Pb 1.4 206Pb  24.1 207Pb 22.1  208Pb 	52.4 
  MObj.setMaterial(23,"Lead","82206.24c 0.00794248 "
		   "82207.24c 0.00728335 82208.25c 0.0172691","",MLib);
  setMaterial(MObj);

  // Material #24 Lead  11340kg m-3 [0.03296371] + Sb at 1.2%
  //204Pb 1.4 206Pb  24.1 207Pb 22.1  208Pb 	52.4 
  MObj.setMaterial(24,"Lead+Sb",
		   "51121.70c 0.000227377 51123.70c 0.000170066 "
		   "82204.24c 0.000458119 82206.24c 0.00788619 "
		   "82207.24c 0.00723174 82208.25c 0.0171467 ",
		   "",MLib);
  setMaterial(MObj);

  // Material #25: Liquid para-hydrogen (Granada)
  // Total atom density 0.041957 -  20 K; 0.07021 grams per cc
  MObj.setMaterial(25,"ParaH2","1001.70c 0.041957","ph215.00t",MLib);
  setMaterial(MObj);

  // Material #26 is solid methane (90v/o) and Aluminum (10v/o) 22K
  MObj.setMaterial(26,"CH4+Al22K","6012.60c -0.3439 "
		   "1001.70c -0.1145 13027.24c -0.2698",
		   "smeth.22t al.20t",MLib);
  setMaterial(MObj);

  //Material #27 is solid methane (90v/o) and Aluminum (10v/o) 30K
  MObj.setMaterial(27,"CH4+Al30K","6012.60c -0.3439 1001.70c -0.1145 "
		   "13027.24c -0.2698","smeth.30t al.20t",MLib);
  setMaterial(MObj);

  // Material #28 is solid methane (90v/o) and Aluminum (10v/o) 26K
  MObj.setMaterial(28,"CH4+Al26K",
		   "6012.60c  -0.3439 1001.70c -0.1145 13027.24c -0.2698",
		   "smeth.01t al.20t",MLib);
  setMaterial(MObj);

  // Material #29 is liquid methane at 100K (based on kernel T)
  MObj.setMaterial(29,"CH4Liq",
		   "1001.70c 0.066183 6000.70c 0.016546",
		   "lmeth.01t",MLib);
  setMaterial(MObj);

  // Material #30 Solid ICE at 26K Total atom dens 0.10028340
  MObj.setMaterial(30,"Ice26K",
		   "1001.70c 0.06684557 1002.24c 0.00001003 "
		   "8016.24c 0.03342780",
		   "ice26.00t",MLib);
  setMaterial(MObj);

  // Material #31 Heavy water
  // Total atom density 0.09925325 -
  // (99.75 a% D2O & 0.25 a% H2O) 1.1 grams per cc
  MObj.setMaterial(31,"D2O",
		   "1001.70c 0.00016542 1002.24c 0.06600341 "
		   "8016.24c 0.03308442","hwtr.01t",MLib);
  setMaterial(MObj);

  // Material #32 Tantalum (old version)
  // Total atom density 0.05524655 - 16.60 grams per cc
  MObj.setMaterial(32,"Tantalum","73181.50c 0.05524655","",MLib);
  setMaterial(MObj);

  // Material #33 Nickel
  // Total atom density 0.091321 - 8.9 grams per cc 
  MObj.setMaterial(33,"Nickel",
		   "28058.24c 0.062169 28060.24c 0.02394719 "
		   "28061.24c 0.00104097 28062.24c 0.00331906 "
		   "28064.24c 0.00084527","",MLib);
  setMaterial(MObj);


  // Material #37: Beryllium solid at RT
  // Total atom density 0.1187475
  MObj.setMaterial(37,"Be300K","4009.80c 0.1234855","be.61t",MLib);
  setMaterial(MObj);
// Material #38 Solid Pure Tungsten (0.063057)
  MObj.setMaterial(38,"Tungsten",
		   "74182.24c 0.016871 74183.24c 0.00911077 "
		   "74184.24c 0.019507618 74186.24c 0.018100573 ","",MLib);
  setMaterial(MObj);

  // Material #39: Aluminum 2024
  // Total atom density 0.06012961 - 2.76 grams per cc
  MObj.setMaterial(39,"Al2024",
		   "12000.60c 1.02876e-3 13027.24c 5.77646e-2 "
		   "25055.60c 1.82052e-4 "
		   "29000.50c 1.15420e-3","al.20t",MLib);
  setMaterial(MObj);

  // Material #40  Sapphire : 
  MObj.setMaterial(40,"Al2O3","13027.24c 0.03333333 "
		   "8016.24c 0.0666666", "",MLib);
  setMaterial(MObj);

  // Material #41 Silicon
  MObj.setMaterial(41,"SiPowder",
		   "14028.24c 0.0460848 14029.24c 0.00234 "
		   "14030.24c 0.0015403","",MLib);
  setMaterial(MObj);

  // Material #42 Niobium
  MObj.setMaterial(42,"Niobium","41093.24c 0.055558","",MLib);
  setMaterial(MObj);

  // Material #43  Low density Al foam (pure)
  MObj.setMaterial(43,"AlFoam","13027.24c 0.0054381","al.20t",MLib);
  setMaterial(MObj);

  // Material #44  Liquid methane + 10% Al at 100K
  MObj.setMaterial(44,"CH4Liq+Al","1001.70c 0.0595647 "
		   "6000.70c 0.014891 13027.24c 0.0058254",
		   "lmeth.01t al.20t",MLib);
  setMaterial(MObj);

  // Material #45: Aluminum 2214
  // Total atom density 0.058254 - 2.65 grams per cc (??)
  // Mg 0.7 : Si 0.7 : Fe 0.5 : NiTi 0.15 : Cr 0.15 : Mn 0.7 :  
  // Fe 0.4 : Cu 0.1 : Zn 0.25 :  
  MObj.setMaterial(45,"Al2214","12000.60c 0.000468292 "
		   "14028.24c 0.000405257 25055.60c 0.000266369 "
		   "26054.24c 8.50911e-06 26056.24c 0.000133575 "
		   "26057.24c 3.08482e-06 26058.60c 4.10533e-07 "
		   "29063.24c 0.000796451 29065.24c 0.000354989 "
		   "28058.24c 1.8863e-05 28060.24c 7.26565e-06 "
		   "28061.24c 3.15583e-07 "
		   "28062.24c 1.00687e-06 30000.70c 6.21466e-05 50000.40c 3.12712e-05 "
		   "13027.24c 0.0555923",
		   "al.20t",MLib);


  setMaterial(MObj);
  // Material #46 Boron (pure)
  MObj.setMaterial(46,"Boron","5010.70c 0.0273024 "
		   "5011.70c 0.1098957","",MLib);
  setMaterial(MObj);

      // Material #47 B4C 

  MObj.setMaterial(47,"B4C","5010.70c 0.021741 "
		   "5011.70c 0.087512 6000.70c 0.027313","",MLib);
  setMaterial(MObj);
  
  // Material #48 Poly:
  MObj.setMaterial(48,"Poly","6000.70c 0.0333333 "
		   "1001.70c 0.0666666666","poly.01t",MLib);
  setMaterial(MObj);

  // Material #49 Regular concrete
  // Regular concrete at 2.339 g/cc [supposedly]
  MObj.setMaterial(49,"Concrete",
		   "1001.70c  7.76555E-03 1002.24c  1.16501E-06 "
		   "8016.24c  4.38499E-02 11023.60c 1.04778E-03 "
		   "12000.60c 1.48662E-04 13027.24c 2.38815E-03 "
		   "14028.24c 1.58026E-02 16032.60c 5.63433E-05 "
		   "19000.60c 6.93104E-04 20000.60c 2.91501E-03 "
		   "26054.24c 1.84504E-05 26056.24c 2.86826E-04 "
		   "26057.24c 6.56710E-06 26058.60c 8.75613E-07",
		   "lwtr.01t",MLib);
  setMaterial(MObj);

  // Material #50 High density concrete:: 3.848g/cc
  MObj.setMaterial(50,"HDConcrete",
		   "1001.70c  9.03819E-03 1002.24c  1.35593E-06 "
		   "8016.24c  4.92287E-02 8017.60c  1.97389E-05 "
		   "11023.60c 1.02944E-05 12000.60c 2.09352E-04 "
		   "13027.24c 4.86813E-04 14028.24c 1.69797E-03 "
		   "15031.24c 7.64087E-06 16000.60c 8.11907E-05 "
		   "19000.60c 1.21062E-05 20000.60c 2.47130E-03 "
		   "22000.60c 2.64454E-04 25055.60c 3.87709E-05 "
		   "26054.24c 1.44497E-03 26056.24c 2.27918E-02 "
		   "26057.24c 5.21380E-04 26058.60c 6.95173E-05",
		   "lwtr.01t",MLib);
  setMaterial(MObj);

  // Material #51: Lithium Nitride
  // Total atom density 0.0219560 - 1.270 g/cc
  MObj.setMaterial(51,"Li3N",
		   "3006.60c  1.25007E-03  3007.60c  1.52199E-02 "
		   "7014.24c  5.46980E-03 7015.55c  2.02030E-05","",MLib);
  setMaterial(MObj);

  // Material #52: 5% borated poly  
  // Total atom density 0.1154 atom/barn-cm

  MObj.setMaterial(52,"B-Poly","1001.70c  0.0752 6000.70c "
		   "0.0376 5010.70c  0.00051 5011.70c  0.002053",
		   "poly.01t",MLib);
  setMaterial(MObj);

  // Material #53: Lithium 
  // Total atom density 0.0219560 - 1.270 g/cc
  MObj.setMaterial(53,"Lithium","3006.60c 0.0047136 3007.60c 0.0424228","",MLib);
  setMaterial(MObj);

  // Material #54 Standard Cast Iron [Grey -ASTM A48]:
  // Carbon 3.4%w/w Si 1.8, Mn 0.5 Rest Fe. [ 7.05g/cc] 
  MObj.setMaterial( 54,"CastIron","6000.70c 0.00636 "
		    "14028.24c 0.0051404 25055.60c 0.000386 "
		    "26054.24c 0.004190 26056.24c 0.065789 26057.24c 0.00152",
		    "",MLib);
  setMaterial(MObj);

  // Material #55 Silicon with no-bragg
  MObj.setMaterial(55,"SiCrystal","14028.24c 0.05","si.80t",MLib);
  setMaterial(MObj);

  // Material #56 Tin [Density 7310kg/m3]:
  MObj.setMaterial(56,"Tin","50000.42c 0.037088","",MLib);
  setMaterial(MObj);
  
  // Material #57: Lithium Carbonate
  // Total atom density 0.103 Atom/A3 - 2.11 g/cc
  MObj.setMaterial(57,"LithiumCarbonate",
		   "3006.60c 1.2875e-3 3007.60c 1.5889e-2 "
		   "6000.70c 1.7166e-2 8016.24c 5.15e-2","",MLib);
  setMaterial(MObj);

  // Material #58 : Methane + 10% Al [type 2 at 20K]
  MObj.setMaterial(58,"CH4TypeII","6000.70c 0.01841004 "
		   "1001.70c 0.0736395","smeth.02t",MLib);
  setMaterial(MObj);

  //Material #59 Nimonic 8.19g/cc
  // Total atom density 0.091321 - 8.9 grams per cc 

  MObj.setMaterial(59,"Nimonic","28058.24c 0.0444126 28060.24c 0.0171069 "
		   "28061.24c 0.000743037 28062.24c 0.00237067 "
		   "28064.24c 0.000603432 24052.24c 0.0169356 "
		   "24053.24c 0.0042339 ","",MLib);
  setMaterial(MObj);

  // Material #60 Air (Dry, near sea level)
  // Total atom density 4.9873E-05 - 1.2045E-03 grams per cc
  MObj.setMaterial(60,"Air","6000.70c 7.0000e-9 7014.24c 3.91280E-05 "
		   "8016.24c 1.05120E-05 18036.24c 8.1682E-10 "
		   "18038.24c 1.3987E-10 18040.24c 2.3207E-07 ",
		   "",MLib);
  setMaterial(MObj);

  // Material #61 Moly [10.28g/cc : number density 0.06399]

  MObj.setMaterial(61,"Molybdimum","42092.70c 0.00953094 "
		   "42094.70c 0.00595603 42095.70c 0.0102601 "
		   "42096.70c 0.0107634 42097.70c 0.00616898 "
		   "42098.70c 0.0156096 42100.70c 0.00623996 ",
		   "",MLib);
  setMaterial(MObj);

  // Material #62 Para : Ortho 50/50 hydrogen
  MObj.setMaterial(62,"ParaOrtho","1001.70c 0.0209785 1004.70c 0.0209785",
		   "parah.20t orthh.99t",MLib);
  setMaterial(MObj);

  // Material #63 Ortho Pure hydrogen
  MObj.setMaterial(63,"Ortho","1001.70c 0.041957","orthh.99t",MLib);
  setMaterial(MObj);

    // Material #64 : Methane + 10% Al [type 2  about 4K]
  MObj.setMaterial(64,"CH4AlTypeII",
		   "6000.70c 0.01841004 1001.70c 0.0736395",
		   "smeth.03t",MLib);
  setMaterial(MObj);

    // Material #65 Regular concrete [half density]
    // Regular concrete at 2.339/2 g/cc [supposedly]
  MObj.setMaterial(65,"HalfConcrete","1001.70c 3.88277E-03 "
		   "1002.24c 0.582505E-06 "
		   "8016.24c  2.19249E-02 11023.60c 0.523890E-03 "
		   "12000.60c 0.74331E-04 13027.24c 1.194075E-03 "
		   "14028.24c 0.79013E-02 16032.60c 2.817165E-05 "
		   "19000.60c 3.46551E-04 20000.60c 1.457500E-03 "
		   "26054.24c 0.92252E-05 26056.24c 1.434125E-04 "
		   "26057.24c 3.28355E-06 26058.60c 4.378065E-07",
		   "",MLib);

  setMaterial(MObj);
  // Material #66 Boral
  // density 0.084235 
  MObj.setMaterial(66,"B4CAluminium","5010.70c 0.006885 "
		   "5011.70c 0.031367 6000.70c 0.009563 "
		   "13027.24c 0.036420","",MLib);
  setMaterial(MObj);

  // Material #67 NELCO Concrete
  // density 2.455g/cc   
  MObj.setMaterial(67,"NELCOConcrete",
		   "26054.24c 1.547397e-05 26056.24c 2.429082e-04 "
		   "26057.24c 5.609809e-06 26058.24c 7.465626e-07 "
		   "20040.70c 8.582522e-03 20042.70c 5.728115e-05 "
		   "20043.70c 1.195202e-05 20044.70c 1.846808e-04 "
		   "20046.70c 3.541339e-07 20048.70c 1.655576e-05 "
		   "14028.70c 6.797051e-03 14029.70c 3.451377e-04 "
		   "14030.70c 2.275173e-04 "
		   "1001.70c 1.466620e-02 1002.70c 1.686807e-06 "
		   "8016.70c 5.545897e-02","",MLib);
  setMaterial(MObj);

  // Material #68 Uranium for reactor [delft]
  // Somewhat burnt
  //  Minors left out
  // 95241  3.677946E-09
  // 95243  1.660551E-10
  // 96242  2.660332E-10
  // 96243  5.001810E-13
  MObj.setMaterial(68,"Uranium",
		   "5010.70c  1.800000E-07 5011.70c  7.330000E-07 "
		   "13027.24c 3.491000E-02 14028.24c 7.683000E-03 "
		   "92234.70c 1.453099E-05 92235.70c 1.719715E-03 "
		   "92236.70c 8.740910E-05 92237.70c 3.363964E-08 "
		   "92238.70c 8.662198E-03 93237.70c 8.149635E-07 "
		   "93239.70c 1.898293E-07 94238.70c 3.055927E-08 "
		   "94239.70c 2.874713E-05 94240.70c 2.248326E-06 "
		   "94241.70c 3.508395E-07 94242.70c 1.371115E-08 ","",MLib);
  setMaterial(MObj);
    // #69 Hafnium

  MObj.setMaterial(69,"Hafnium",
		   "72174.70c 7.18534e-05 72176.70c 0.00236218 "
		   "72177.70c 0.00835296 72178.70c 0.012251 "
		   "72179.70c 0.00611652 ","",MLib);
  setMaterial(MObj);

  // Material #70 Glass (Na/Ca/SiO2 : 2.44g/cc)
  MObj.setMaterial(70,"Glass",
		   "20040.70c 2.719913e-03 20042.70c 1.815314e-05 "
		   "20043.70c 3.787750e-06 20044.70c 5.852775e-05 "
		   "20046.70c 1.122296e-07 20048.70c 5.246735e-06 "
		   "11023.70c 6.423108e-03  8016.24c 4.307240e-02 "
		   "14028.24c 1.708774e-02 14029.24c 8.676740e-04 "
		   "14030.70c 5.719771e-04 ","",MLib);
  setMaterial(MObj);


  // Material #71 Steel (proposed for Chipir Collimators)
  // similar to P355NL1  (s/. EN 10028-3)
  // Total atom density 0.08432 - 7.8 grams per cc (assumed)
  MObj.setMaterial(71,"Steel71",
		   "6000.70c 1.011868e-04 14028.24c 2.951280e-04 "
		   "24000.50c 1.686446e-04 25055.70c 5.059338e-04 "
		   "26054.24c 4.841401e-03 26056.24c 7.599964e-02 "
		   "26057.24c 1.755163e-03 26058.24c 2.335800e-04 "
		   "28058.24c 1.722180e-04 28060.24c 6.633503e-05 "
		   "28061.24c 2.881261e-06 28062.24c 9.192715e-06 "
		   "28063.24c 2.339918e-06 29063.24c 1.166515e-04 "
		   "29065.24c 5.199313e-05 ","",MLib);
  setMaterial(MObj);

    // Material #72 : Methane + 10% Al.+ 0.18% number density Ar
    // Mole volume 0.18% 
  MObj.setMaterial(72,"CH4Al+Argon",
		   "6012.24c 0.0172756 1001.70c 0.0690944 "
		   "13027.24c 0.0054381 "
		   "18036.70c 1.040783e-07 18038.70c 1.962387e-08 "
		   "18040.70c 3.107482e-05 ","smeth.26t al.20t",MLib);
  setMaterial(MObj);

  // Material #73 : Copper
  // Density : 8.91g/cc rho=0.084438
  
  MObj.setMaterial(73,"Copper","29063.24c 0.02604927 "
		   "29065.24c 0.058389212","",MLib);
  setMaterial(MObj);

   //Material #74: ChipIR Guide Steel (K Jones spec)
   //Density : 7.800 g/cc
  MObj.setMaterial(74,"ChipIRSteel",
		   "5010.70c 3.459223e-06 5011.70c 1.392381e-05 " 
		   "6000.70c 4.694025e-04 " 
		   "7014.24c 3.019615e-05 " 
		   "13027.24c 2.176586e-04 " 
		   "14028.24c 1.672824e-04 " 
		   "25055.50c 3.420733e-04 "  
		   "15031.70c 7.584178e-05 "
		   "16032.70c 5.563524e-05 16033.70c 4.454102e-07 " 
		   "16034.70c 2.514223e-06 16036.70c 1.172132e-08 " 
		   "26054.24c 4.849594e-03 26056.24c 7.612826e-02 " 
		   "26057.24c 1.758134e-03 26058.24c 2.339753e-04 "
		   "27059.70c 1.594419e-05 "
		   "29063.24c 7.671067e-05 29065.24c 3.419098e-05 "
		   "28058.24c 5.450388e-05 28060.24c 2.099384e-05 "
		   "28061.24c 9.118672e-07 28062.24c 2.909329e-06 "
		   "28063.24c 7.405418e-07 "
		   "24050.24c 3.926008e-06 24052.24c 7.570916e-05 "
		   "24053.24c 8.584811e-06 24054.24c 2.136941e-06 "
		   "42092.70c 7.267852e-06 42094.70c 4.530164e-06 "
		   "42095.70c 7.796779e-06 42096.70c 8.168987e-06 "
		   "42097.70c 4.677088e-06 42098.70c 1.181761e-05 "
		   "42100.70c 4.716268e-06 "
		   "41093.24c 5.056930e-06 "
		   "23051.42c 9.222311e-06 "
		   "50112.70c 4.058814e-08 50114.70c 2.761667e-08 " 
		   "50115.70c 1.422677e-08 50116.70c 6.084036e-07 "
		   "50117.70c 3.213576e-07 50118.70c 1.013448e-06 "
		   "50119.70c 3.594352e-07 50120.70c 1.363259e-06 "
		   "50122.70c 1.937351e-07 "
		   "22046.70c 8.097528e-07 22047.70c 7.302498e-07 " 
		   "22048.70c 7.235755e-06 22049.70c 5.310015e-07 "
		   "22050.70c 5.084266e-07 ","",MLib);
  setMaterial(MObj);

  // Material #75: Titanium
  // Total atom density 0.057118 - 4.54 g/cc
  MObj.setMaterial(75,"Titanium","22046.70c 4.712234e-03 "
		   "22047.70c 4.249578e-03 22048.70c 4.210738e-02 "
		   "22049.70c 3.090083e-03 22050.70c 2.958712e-03 ",
		   "",MLib);
  setMaterial(MObj);

  // Material #76: Titanium alloy (Grade 5): Ti-6Al-4V 
  // Total atom density 0.057135 - 4.43 g/cc
  MObj.setMaterial(76,"Ti-6Al-4V",
		   "22046.70c 4.221076e-03 22047.70c 3.806643e-03 "
		   "22048.70c 3.771851e-02 22049.70c 2.768002e-03 "
		   "22050.70c 2.650324e-03 26054.24c 8.348879e-06 " 
		   "26056.24c 1.310595e-04 26057.24c 3.026736e-06 "
		   "26058.24c 4.028030e-07 8016.70c 1.142704e-04 "
		   "13027.24c 3.428111e-03 23051.42c 2.285407e-03 ","",MLib);
  setMaterial(MObj);


  // Material #77: Platinum 
  // Total atom density 0.066217 - 21.45 g/cc
  MObj.setMaterial(77,"Platinum",
		   "78190.80c 9.270400e-06 78192.80c 5.178180e-04 "
		   "78194.80c 2.182980e-02 78195.80c 2.240258e-02 "
		   "78196.80c 1.671453e-02 78198.80c 4.743134e-03 ",  
		   "",MLib);
  setMaterial(MObj);

  // Material: #78
  // Be alloy:  Berilio 1.847810 g/cc
  MObj.setMaterial(78,"Berilio",
		   "4009.80c  -1.8205E+00 8016.70c -1.7737E-02 "
		   "13027.70c -1.8476E-03 6012.70c -2.7714E-03 "
		   "26054.70c -1.4040E-04 26056.70c -2.2024E-03 "
		   "26057.70c -5.0902E-05 26058.70c -6.7734E-06 "
		   "12024.70c -1.1675E-03 12025.70c -1.4781E-04 "
		   "12026.70c -1.6274E-04 14028.70c -1.0225E-03 "
		   "14029.70c -5.1771E-05 14030.70c -3.4366E-05 ","",MLib);
  setMaterial(MObj);

  //Material: #79
  // Aluminio 6061
  // -2.698400 g/cc
  MObj.setMaterial(79,"Aluminium6061",
		   "13027.70c -2.6258E+00 24050.50c -2.2864E-04 "
		   "24052.70c -4.4092E-03 24053.70c -5.0001E-04 "
		   "24054.70c -1.2445E-04 29063.70c -5.1728E-03 "
		   "29065.70c -2.2877E-03 26054.70c -5.5209E-04 "
		   "26056.70c -8.6619E-03 26057.70c -2.0014E-04 "
		   "26058.70c -2.6633E-05 12024.70c -2.1315E-02 "
		   "12025.70c -2.6984E-03 12026.70c -2.9709E-03 "
		   "25055.70c -2.0238E-03 14028.70c -1.4933E-02 "
		   "14029.70c -7.5609E-04 14030.70c -5.0190E-04 "
		   "22046.70c -1.6698E-04 22047.70c -1.5057E-04 "
		   "22048.70c -1.4919E-03 22049.70c -1.0950E-04 "
		   "22050.70c -1.0483E-04 30064.70c -1.6404E-03 "
		   "30066.70c -9.4120E-04 30067.70c -1.3829E-04 "
		   "30068.70c -6.3250E-04 30070.70c -2.0913E-05 ",
		   "al.20t",MLib);


  // Material #81 Silicon with no-bragg (20K)
  MObj.setMaterial(81,"Silicon20K","14028.24c 0.0460848 "
		   "14029.24c 0.00234 14030.24c 0.0015403","si.81t",MLib);
  setMaterial(MObj);
  // Material #82 Silicon with no-bragg (20K)
  MObj.setMaterial(82,"Silicon80K","14028.24c 0.0460848 "
		   "14029.24c 0.00234 14030.24c 0.0015403","si.82t",MLib);
  setMaterial(MObj);
  // Material #83 Silicon with no-bragg (20K)
  MObj.setMaterial(83,"Silicon300K","14028.24c 0.0460848 "
		   "14029.24c 0.00234 14030.24c 0.0015403","si.83t",MLib);
  setMaterial(MObj);

  // Material #84 Mercury
  MObj.setMaterial(84,"Mercury","80196.24c 6.094517e-05 "
		   "80198.24c 4.050822e-03 80199.24c 6.854300e-03 "
		   "80200.24c 9.385556e-03 80201.24c 5.355049e-03 "
		   "80202.24c 1.213215e-02 80204.24c 2.791289e-03 ",
		   "",MLib);
  setMaterial(MObj);
    
  // Material #86: 90% Be + 10% D2O + H2O mixture (80% D2O & 20% H2O) 
  //  (by VOLUME)
  // Total atom density 0.1212102
  MObj.setMaterial(86,"Be90/8D20/2H2O",
		   "4009.80c 1.112839e-01 "
		   "1001.70c 1.323088e-03 1002.24c 5.277874e-03 "
		   "8016.24c 0.0033253","be.61t lwtr.01t hwtr.01t",
		   MLib);
  setMaterial(MObj);
  
  // Material #87: Light water with 17-O
  // Total atom density 0.0998517 - 1.0 grams per cc
  MObj.setMaterial(87,"H2O_17O","1001.70c 0.06637790 "
		   "1002.24c 7.634337e-06 8016.24c 0.03345347 "    
		   "8017.70c 1.271715e-05","lwtr.01t",MLib);
  setMaterial(MObj);

  
  // Material #88: D2O + H2O mixture (80% D2O & 20% H2O)    (by VOLUME)
  // Total atom density 0.0996049 - 1.0856 grams per cc
  MObj.setMaterial(88,"80D2O/20H2O","1001.70c 1.327648e-02 "
		   "1002.24c 5.296063e-02 8016.24c 0.03336781",
		   "lwtr.01t hwtr.01t",MLib);
  setMaterial(MObj);

  // Material #89 Boral (95% Aluminium & 5% Boron) 
  // density 0.06213
  MObj.setMaterial(89,"Boral5","5010.70c 0.0014482 "
		   "5011.70c 0.0057928 13027.24c 0.054889",
		   "al.20t",MLib);
  setMaterial(MObj);

  // Material #91 Boral (95% Aluminium & 5% Boron) degraded: 
  // 10% of initial B-10 left
  // density 0.062979
  MObj.setMaterial(91,"Boral5Degrade","3007.60c 2.008524e-03 "
		   "5010.70c 1.168329e-04 5011.70c 5.724811e-03 "
		   "13027.24c 5.512916e-02","al.20t",MLib);       
  setMaterial(MObj);
  
  // Material #92: Beryllium + heavy water 
  // (Dave Bellenger version 90% Be + 10% D2O - by VOLUME)
  // Total atom density 0.121202
  MObj.setMaterial(92,"Be/10D2O","4009.80c 1.112996e-01 1001.70c 1.651935e-05 "
		   "1002.24c 6.568682e-03 8016.24c 0.003316877",
		   "hwtr.01t be.61t",MLib);
  setMaterial(MObj);

  // Material #93: 90% Beryllium + 10% light water
  // Total atom density 0.121264
  MObj.setMaterial(93,"Be/10H2O","4009.80c 1.112186e-01 "
		   "1001.70c 6.677949e-03 1002.24c 7.680525e-07 "
		   "8016.24c 3.366946e-03","lwtr.01t be.61t",MLib);  
  setMaterial(MObj);

  // Material #94 Stainless 316L
  // Total atom density 0.086927    
  MObj.setMaterial(94,"Stainless316L",
		   "6000.70c 1.203355e-04 14028.24c 1.715374e-03 "
		   "24000.50c 1.575140e-02 25055.60c 1.753871e-03 "
		   "26054.24c 3.301281e-03 26056.24c 5.182305e-02 "
		   "26057.24c 1.196820e-03 26058.24c 1.592748e-04 "
		   "28058.24c 6.705771e-03 28060.24c 2.582933e-03 "
		   "28061.24c 1.121897e-04 28062.24c 3.579432e-04 "    
		   "28064.24c 9.111102e-05 42092.70c 1.854269e-04 "
		   "42094.70c 1.158761e-04 42095.70c 1.996132e-04 "
		   "42096.70c 2.094056e-04 42097.70c 1.200190e-04 "
		   "42098.70c 3.036883e-04 42100.70c 1.214000e-04",    
		   "",MLib);
  setMaterial(MObj);

  // Material #95 Steel XC-06: Soft magnetic (yoke) steel
  // Total atom density 0.086481    
  MObj.setMaterial(95,"SteelXC06",
		   "6000.70c 1.203355e-04 14028.24c 2.229986e-04 "
		   "24000.50c 3.706212e-05 25055.60c 2.630807e-04 "
		   "26054.24c 5.017221e-03 26056.24c 7.875963e-02 "
		   "26057.24c 1.818903e-03 26058.24c 2.420627e-04",
		   "",MLib);
  setMaterial(MObj);

  // Material #96 Carbon, Graphite (Reactor Grade): 
  // density 0.085238 
  MObj.setMaterial(96,"Graphite",  
		   "6000.70c 8.523759e-02 5010.70c "
		   "1.894100e-08 5011.70c 7.576401e-08",
		   "",MLib);
  setMaterial(MObj);

  // Material #97 is liquid methane at 110K (based on kernel T) - this is new TS1 default!!!
  MObj.setMaterial(97,"CH4Liq110K",
		   "1001.70c 0.066183 6000.70c 0.016546",
		   "lmeth.11t",MLib);
  setMaterial(MObj);

    // Material #98 Boral (this is TS1 Boral = Alloy 1100 + B) ->>> 94.7% Al, 5.3% Boron (B-10 enrichment: 98.2%)
    // Density 2.7 g/cm^3
    // Total atom density 0.06566     
  MObj.setMaterial(98,"TS1Boral","5010.70c 8.436522e-03 "
		   "5011.70c 1.546409e-04 13027.24c 5.706872e-02",
		   "al.20t",MLib);
  setMaterial(MObj);

  // Material #99: 90% Be + 10% D2O + H2O mixture (50% D2O & 50% H2O) 
  //  (by VOLUME)
  // Total atom density 0.1212533
  MObj.setMaterial(99,"Be90/5D20/5H2O",
		   "4009.80c 1.112839e-01 "
		   "1001.70c 3.347044e-03 1002.24c 3.337337e-03 "
		   "8016.24c 0.0033681","be.61t lwtr.01t hwtr.01t",
		   MLib);
  setMaterial(MObj);

  return;
}

void
DBMaterial::setMaterial(const MonteCarlo::Material& MO)
  /*!
    Store are material in the data base
    \param MO :: Material object
   */
{
  ELog::RegMethod RegA("DBMaterial","setMaterial");

  const std::string& MName=MO.getName();
  const int MIndex=MO.getNumber();

  checkNameIndex(MIndex,MName);
  MStore.insert(MTYPE::value_type(MIndex,MO));
  IndexMap.insert(SCTYPE::value_type(MName,MIndex));
  return;
}

bool
DBMaterial::createMaterial(const std::string& MName)
  /*!
    Creates a material based on the name fraction
    \param MName :: Material name based on fraction load
    \return 0 if not possible / 1 on material existing or created
  */
{
  ELog::RegMethod RegA("DBMaterial","createMaterial");
  if (hasKey(MName)) return 1;

  // Now key found
  const std::string::size_type pos=MName.find('%');
  if (pos!=std::string::npos)
    {
      double PFrac;
      const std::string AKey=MName.substr(0,pos);
      const std::string BKey=MName.substr(pos+1);
      if (StrFunc::convert(BKey,PFrac))
	{
	  if (AKey=="ParaOrtho")
	    {
	      createOrthoParaMix(MName,PFrac/100.0);
	      return 1;
	    }
	}
    }
  return 0;
}


int
DBMaterial::createOrthoParaMix(const std::string& Name,
			       const double PFrac)
  /*!
    Creates an ortho/Para Mixture
    \param Name :: Name of object
    \param PFrac :: fraction of Para
    \return current number
   */
{
  ELog::RegMethod RegA("DBMaterial","createOrthoParaMix");

  const int matNum(getFreeNumber());
  const double H2density(0.041975);
  if (PFrac<-1e-5 || PFrac>1.00001)
    throw ColErr::RangeError<double>(PFrac,0.0,1.0,"Para fraction");

  MonteCarlo::Material MObj;
  std::string Unit;
  std::string SQW;
  const std::string MLib="hlib=.70h pnlib=70u";
  if (PFrac>1e-5)
    {
      Unit+=" 1001.70c "+StrFunc::makeString(H2density*PFrac);
      SQW+="parah.20t ";
    }
  if (1.0-PFrac>1e-5)
    {
      Unit+=" 1004.70c "+StrFunc::makeString(H2density*(1.0-PFrac));
      SQW+="orthh.99t ";
    }
  
  MObj.setMaterial(matNum,Name,Unit,SQW,MLib);
  setMaterial(MObj);
  return matNum;
}


void
DBMaterial::resetMaterial(const MonteCarlo::Material& MO)
  /*!
    Store are material in the data base
    Allows the material to already exist
    \param MO :: Material object
   */
{
  ELog::RegMethod RegA("DBMaterial","resetMaterial");

  const std::string& MName=MO.getName();
  const int MIndex=MO.getNumber();

  MTYPE::iterator mc=MStore.find(MIndex);
  if (mc!=MStore.end()) MStore.erase(mc);
  SCTYPE::iterator sc=IndexMap.find(MName);
  if (sc!=IndexMap.end()) IndexMap.erase(sc);

  MStore.insert(MTYPE::value_type(MIndex,MO));
  IndexMap.insert(SCTYPE::value_type(MName,MIndex));
  return;
}


void
DBMaterial::initMXUnits()
  /*!
    Initialize the MX options
  */
{
  ELog::RegMethod RegA("DBMaterial","initMXUnits");

  typedef boost::tuple<int,int,char,std::string,
		       std::string> MXTYPE;
  std::vector<MXTYPE> mxVec;

  mxVec.push_back(MXTYPE(6000,70,'c',"h","6012.70h"));
  mxVec.push_back(MXTYPE(4009,24,'c',"h","model"));
  mxVec.push_back(MXTYPE(4009,70,'c',"h","model"));

  mxVec.push_back(MXTYPE(4009,80,'c',"h","4009.80h"));
  mxVec.push_back(MXTYPE(4010,80,'c',"h","4010.80h"));

  mxVec.push_back(MXTYPE(78190,80,'c',"h","78190.80h"));
  mxVec.push_back(MXTYPE(78192,80,'c',"h","78192.80h"));
  mxVec.push_back(MXTYPE(78194,80,'c',"h","78194.80h"));
  mxVec.push_back(MXTYPE(78195,80,'c',"h","78195.80h"));
  mxVec.push_back(MXTYPE(78196,80,'c',"h","78196.80h"));
  mxVec.push_back(MXTYPE(78198,80,'c',"h","78198.80h"));


  // NOTE : u is an illegal particle so how does MX work here??
  //  mxVec.push_back(MXTYPE(6000,70,'c',"u","6012.70u"));
  
  std::vector<MXTYPE>::const_iterator vc;
  for(vc=mxVec.begin();vc!=mxVec.end();vc++)
    {
      MTYPE::iterator mc;  
      for(mc=MStore.begin();mc!=MStore.end();mc++)
	mc->second.setMXitem(vc->get<0>(),vc->get<1>(),vc->get<2>(),
			     vc->get<3>(),vc->get<4>());
    }
  return;
}

const MonteCarlo::Material&
DBMaterial::getMaterial(const int MIndex) const
  /*!
    Get the material based on the index value
    \param MIndex :: Material 
    \return Material
   */
{
  ELog::RegMethod RegA("DBMaterial","getMaterial<int>");

  MTYPE::const_iterator mc=MStore.find(MIndex);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(MIndex,"MStore");
  return mc->second;
}

const MonteCarlo::Material&
DBMaterial::getMaterial(const std::string& MName) const
  /*!
    Get the material based on the name value
    \param MName :: Material name
    \return Material
   */
{
  ELog::RegMethod RegA("DBMaterial","getMaterial<string>");

  SCTYPE::const_iterator mc=IndexMap.find(MName);
  if (mc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(MName,"IndexMap");
  return getMaterial(mc->second);
}

bool
DBMaterial::hasKey(const std::string& Key) const
  /*!
    Determine if the string exists in the data base
    \param Key :: KeyName
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","hasKey<string>");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  return (sc==IndexMap.end()) ? 0 : 1;
}

bool
DBMaterial::hasKey(const int KeyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","hasKey<int>");
  
  MTYPE::const_iterator mc=MStore.find(KeyNum);
  return (mc==MStore.end()) ? 0 : 1;
}

const std::string&
DBMaterial::getKey(const int KeyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return matNumber / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","getKey");
  
  MTYPE::const_iterator mc=MStore.find(KeyNum);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(KeyNum,"KeyNum");
  
  const std::string& OutStr=mc->second.getName();
  SCTYPE::const_iterator sc=IndexMap.find(OutStr);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(OutStr,"IndexMap");
  if (sc->second!=KeyNum)
    throw ColErr::MisMatch<int>(KeyNum,sc->second,"KeyNum/IndexMap");
  
  return OutStr;
}

int
DBMaterial::getIndex(const std::string& Key) const
  /*!
    Determine if the index exists in the data base
    \param Key :: key name
    \return matNumber 
  */
{
  ELog::RegMethod RegA("DBMaterial","getIndex");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");

  return sc->second;
}


void
DBMaterial::setActive(const int M)
  /*!
    Set a item in the active list
    \param M :: Material number
   */
{
  active.insert(M);
  return;
}

void
DBMaterial::resetActive()
  /*!
    Reset the active list
  */
{
  active.erase(active.begin(),active.end());
  return;
}

bool
DBMaterial::isActive(const int matN) const
  /*!
    Determine if material is active or not
    \param matN :: Material number
    \return true is active is set.
   */
{
  return (active.find(matN)!=active.end()) ? 1 : 0;
}

void
DBMaterial::setENDF7()
  /*!
    Convert to ENDF7 format [if not .24c type]
  */
{
  ELog::RegMethod RegA("DBMaterial","setENDF7");

  MTYPE::iterator mc;
  for(mc=MStore.begin();mc!=MStore.end();mc++)
    mc->second.setENDF7();
  return;
}

void
DBMaterial::writeCinder(std::ostream& OX) const
  /*!
    Write out a material stream for Cinder
    -- Note the format is format(a6,i4,e10.0):
    mat N::index:Number density 
    There is not space between the number and the number density
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writeCinder");

  std::set<int>::const_iterator sc;
  for(sc=active.begin();sc!=active.end();sc++)
    {
      if (*sc)
	{
	  MTYPE::const_iterator mx=MStore.find(*sc);
	  if (mx==MStore.end())
	    throw ColErr::InContainerError<int>(*sc,"MStore");	  
	  mx->second.writeCinder(OX);
	}
    }
  return;
}

void
DBMaterial::writeMCNPX(std::ostream& OX) const
  /*!
    Write everything out to the stream
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writeMCNPX");

  std::set<int>::const_iterator sc;
  for(sc=active.begin();sc!=active.end();sc++)
    {
      if (*sc)
	{
	  MTYPE::const_iterator mp=MStore.find(*sc);
	  if (mp==MStore.end())
	    throw ColErr::InContainerError<int>
	      (*sc,"MStore find(active item)");
	  if (mp->first)
	    mp->second.write(OX);
	}
    }
  return;
}

} // NAMESPACE ModelSupport
