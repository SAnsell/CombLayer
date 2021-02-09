/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMat/DBNeutMaterial.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "Vec3D.h"
#include "SQWtable.h"
#include "SEtable.h"
#include "ENDFmaterial.h"
#include "CifStore.h"
#include "neutMaterial.h"
#include "Material.h"
#include "SQWmaterial.h"
#include "CryMat.h"
#include "DBMaterial.h"
#include "DBNeutMaterial.h"

namespace scatterSystem
{

DBNeutMaterial::DBNeutMaterial() 
  /*!
    Constructor
  */
{
  initMaterial();
}

DBNeutMaterial&
DBNeutMaterial::Instance() 
  /*!
    Singleton constructor
    \return DBNeutMaterial object
  */
{
  static DBNeutMaterial DObj;
  return DObj;
}

void
DBNeutMaterial::initMaterial()
  /*!
     Initialize the database of materials
   */
{
  ELog::RegMethod RegA("DBNeutMaterial","initMaterial");

  CryMat Silicon("Silicon",28.09,0.0499,4.1491,2.16,0.01,0.171);
  Silicon.setTemperatures(420,420/6);

  neutMaterial Aluminium("Aluminium",27,0.07,3.449,1.5,0.01,0.23);

  SQWmaterial Poly("Poly",4.6767,0.3,-0.27733,4.9054,51.3846,0.22);
  //  Poly.setENDF7("tsl-HinCH2.endf");
  Poly.setExtra("Carbon",0.333333,12.01,6.6484,5.55,0.00,0.0035);

  neutMaterial H2O("Water",6.0,0.0992,-0.5589,2.5867,53.2667,0.22);

  neutMaterial D2O("D2O",6.6666,0.0992,-0.5589,2.5867,53.2667,0.22);
  neutMaterial Li6Cl6DMol("Li6Cl6DMol",7.6064,0.0992,6.3049,1.7056,
			 1.4382,31.1149);
  neutMaterial Li7Cl6DMol("Li7Cl6DMol",7.6364,0.0992,6.1810,1.7207,
			 1.4675,3.4936);
  neutMaterial TiZr("TiZr",61.9651,0.050,0.050,3.0161,
		    1.8551,4.1744);
  neutMaterial Van("Vanadium",50.9420,0.072,-0.3824,0.02,5.1900,5.08);
  neutMaterial B4C("B4C",11.05,0.07,5.5697,3.9420,1.36,613.6);
  //  scatterSystem::Material Silicon("Silicon",0.1,-0.27733,4.9054,51.3846,0.22);
  SQWmaterial ParaH2("ParaH2",1.0,0.041957,-3.7409,1.79,79.9,0.33);
  //  ParaH2.setENDF7("tsl-para-H.endf");
  
  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();
  
  MStore.insert(MTYPE::value_type(DB.getIndex("Aluminium"),Aluminium.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("H2O"),H2O.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("D2O"),D2O.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("ParaH2"),ParaH2.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("SiCrystal"),Silicon.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("Poly"),Poly.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("TiZr"),TiZr.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("Vanadium"),Van.clone()));
  MStore.insert(MTYPE::value_type
		(DB.getIndex("Li6ClD2O6Mol"),Li6Cl6DMol.clone()));
  MStore.insert(MTYPE::value_type
		(DB.getIndex("Li7ClD2O6Mol"),Li7Cl6DMol.clone()));
  MStore.insert(MTYPE::value_type(DB.getIndex("B4C"),B4C.clone()));

  // Set MCNPX numbers:
  for(const auto& [matN , matPtr] : MStore)
    matPtr->setID(matN);
  
  return;
}

void
DBNeutMaterial::setActive(const int M)
  /*!
    Set a item in the active list
    \param M :: Material number
   */
{
  active.insert(M);
  return;
}

void
DBNeutMaterial::resetActive()
  /*!
    Reset the active list
  */
{
  active.erase(active.begin(),active.end());
  return;
}

bool
DBNeutMaterial::isActive(const int matN) const
  /*!
    Determine if material is active or not
    \param matN :: Material number
    \return true is active is set.
   */
{
  return (active.find(matN)!=active.end()) ? 1 : 0;
}

const scatterSystem::neutMaterial* 
DBNeutMaterial::getMat(const int ID) const
  /*!
    Convert an MCNPX number to a neutron material
    \param ID :: MCNPX Material number
    \return neutron Material ptr.
   */
{
  ELog::RegMethod RegA("DBNeutMaterial","getMat");

  if (!ID) return 0;
  
  MTYPE::const_iterator mc=MStore.find(ID);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(ID,"MStore");

  return mc->second;
}
    

void
DBNeutMaterial::write(std::ostream& OX) const
  /*!
    Write everything out to the stream
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBNeutMaterial","write");
  OX<<"Unable to write materials"<<std::endl;
  return;
}

} // NAMESPACE scatterSystem
