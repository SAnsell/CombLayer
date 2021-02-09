/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaPhysics/flukaPhysics.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"

#include "cellValueSet.h"
#include "pairValueSet.h"
#include "flukaPhysics.h"

namespace flukaSystem
{

flukaPhysics::flukaPhysics() :

  // note flag: -1 particle / 0 cell / 1 material / -100 : generic
  // secondary values : scaling for value
  flagValue({
      { "photonuc",cellValueSet<0>("photonuc","PHOTONUC","") },
      { "mupair",cellValueSet<0>("mupair","PHOTONUC","MUMUPAIR") },
      { "muphoton",cellValueSet<0>("muphoton","MUPHOTON","") },
      { "emfray",cellValueSet<0>("emfray","EMFRAY","") },
      { "elecnucl",cellValueSet<0>("elecnucl","PHOTONUC","ELECTNUC") },
      { "coalescence",cellValueSet<0>("coalescence","PHYSICS","COALESCE") },
      { "ionsplit",cellValueSet<0>("ionsplit","PHYSICS","IONSPLIT") },
      { "evaporation",cellValueSet<0>("evaporation","PHYSICS","EVAPORAT") },
      { "evap-noheavy",cellValueSet<0>("evap-noheavy","PHYSICS","EVAPORAT") }
    }),

  impValue({
      { "bias-off",   cellValueSet<1>("bias-off","BIASING","USER")},
      { "partthr",    cellValueSet<1>("partthr","PART-THR","",{-1e-3}) },
      { "gas",        cellValueSet<1>("gas","MAT-PROP","") },
      { "rho",        cellValueSet<1>("rho","MAT-PROP","") },
      { "emffluo",    cellValueSet<1>("emffluo","EMFFLUO","") },
      { "lowbias",    cellValueSet<1>("lowbias","LOW-BIAS","") },
      { "exptrans",   cellValueSet<1>("exptrans","EXPTRANS","") },
      { "exppart",    cellValueSet<1>("exppart","EXPTRANS","") },
      { "plambias",   cellValueSet<1>("plambias","LAM-BIAS","INEPRI",{1.0}) },
      { "lambias",    cellValueSet<1>("lambias","LAM-BIAS","",{1.0}) }

    }),

  emfFlag({
      { "emfcut",  cellValueSet<2>("emfcut","EMFCUT","",{-1e-3,1e-3}) },
      { "prodcut", cellValueSet<2>("prodcut","EMFCUT","PROD-CUT",{-1e-3,1e-3})},
      { "pho2thr", cellValueSet<2>("pho2thr","EMFCUT","PHO2-THR",{1e-3,1e-3})},
      { "pairbrem", cellValueSet<2>("pairbrem","PAIRBREM","",{1e-3,1e-3})},
      { "lpb",  cellValueSet<2>("lpb","EMF-BIAS","LPBEMF",{1e-3,1e-3}) },
      { "lambbrem",cellValueSet<2>("lambbrem","EMF-BIAS","LAMBBREM",{1.0,1}) },
      { "stepsize",cellValueSet<2>("stepsize","STEPSIZE","",{1.0,1}) }
    }),

  threeFlag({
      { "bias",cellValueSet<3>("bias","BIASING","PRINT") },
      { "bias-user",cellValueSet<3>("bias-user","BIASING","USER")},
      { "elpothr",cellValueSet<3>("elpothr","EMFCUT","ELPO-THR",
	{1e-3,1e-3,1e-3}) },
      { "photthr", cellValueSet<3>("pho2thr","EMFCUT","PHOT-THR",
	{1e-3,1e-3,1e-3}) },
      { "mulsopt", cellValueSet<3>("mulsopt","MULSOPT","",{1,1,1}) },
      { "lambemf",cellValueSet<3>("lambemf","EMF-BIAS","LAMBEMF",
				  {1.0,1.0,1.0}) }

    }),

  lamPair({
      { "lamlength",pairValueSet<6>("lamlength","LAM-BIAS","",
				    {1.0,1.0,1.0,1.0,1.0,1.0}) },
      { "lamprimary",pairValueSet<6>("lamprimary","LAM-BIAS","INEPRI",
				    {1.0,1.0,1.0,1.0,1.0,1.0}) }

    }),

  formatMap({
      { "lowbias", unitTYPE(0," %2 0.0 - R0 R1 1.0 ") },
      { "elecnucl", unitTYPE(1,"1.0 - - M0 M1 1.0 ") },
      { "emfray", unitTYPE(0,"4.0 R0 R1 1.0 - - ") },

      { "bias", unitTYPE(0,"%2 %3 %4 R0 R1 1.0 ") },
      { "bias-user", unitTYPE(0,"%2 %3 %4 R0 R1 1.0 ") },
      { "bias-off", unitTYPE(0,"%2 1.0 1.0 R0 R1 1.0 ") },
      { "coalescence", unitTYPE(-100,"1.0 - - - - - ") },
      { "ionsplit", unitTYPE(1,"1.0 0.1 5.0 2 500 1.0 ") },
      { "exptrans", unitTYPE(0," 1.0 %2 R0 R1 1.0 - ") },
      { "exppart", unitTYPE(0," -1.0 %2 %2 1.0 - - ") },

      { "emfcut", unitTYPE(0,"%2 %3 0.0 R0 R1 1.0") },
      { "emffluo", unitTYPE(1,"%2 M0 M1 1.0 - - ") },
      { "evaporation", unitTYPE(-100,"3.0 - - - - - ") },
      { "evap-noheavy", unitTYPE(-100,"2.0 - - - - - ") },

      { "prodcut", unitTYPE(1,"%2 %3 1.0 M0 M1 1.0") },
      { "photthr", unitTYPE(1,"%2 %3 %4 M0 M1 1.0") },
      { "pho2thr", unitTYPE(1,"%2 %3 -  M0 M1 1.0") },
      { "elpothr", unitTYPE(1,"%2 %3 %4 M0 M1 1.0") },
      { "pairbrem", unitTYPE(1,"3.0 %2 %3 M0 M1 1.0") },
      { "photonuc", unitTYPE(1,"1.0 - - M0 M1 1.0 ") },
      { "mupair", unitTYPE(1,"1111.0 - - M0 M1 1.0 ") },
      { "muphoton", unitTYPE(1,"1.0 - - M0 M1 1.0 ") },
      { "mulsopt", unitTYPE(1,"%2 %3 %4 M0 M1 1.0 ") },
      { "lpb", unitTYPE(0,"1022 %2 %3 R0 R1 1.0 ") },
      { "lambbrem", unitTYPE(1,"%2 0.0 %3 M0 M1 1.0 ") },
      { "lambemf", unitTYPE(1,"%2 %3 %4 M0 M1 1.0 ") },

      { "lamlength", unitTYPE(-1,"0.0 %2 M1 P0 P0 1.0 ") },
      { "lamprimary", unitTYPE(-1,"0.0 %2 M1 P0 P0 1.0 ") },

      { "gas", unitTYPE(1," %2 0.0 0.0 M0 M1 1.0 ") },
      { "rho", unitTYPE(1," 0.0 %2 0.0 M0 M1 1.0 ") },

      { "partthr", unitTYPE(-1,"%2 P0 P1 1.0 0.0 -") },

      { "stepsize", unitTYPE(0,"%2 %3 M0 M1 1.0 - ") }

    })

  /*!
    Constructor
  */
{}

flukaPhysics::flukaPhysics(const flukaPhysics& A) :
  cellVec(A.cellVec),matVec(A.matVec),
  flagValue(A.flagValue),impValue(A.impValue),
  emfFlag(A.emfFlag),threeFlag(A.threeFlag),
  lamPair(A.lamPair),formatMap(A.formatMap)
  /*!
    Copy constructor
    \param A :: flukaPhysics to copy
  */
{}

flukaPhysics&
flukaPhysics::operator=(const flukaPhysics& A)
  /*!
    Assignment operator
    \param A :: flukaPhysics to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellVec=A.cellVec;
      matVec=A.matVec;
      flagValue=A.flagValue;
      impValue=A.impValue;
      emfFlag=A.emfFlag;
      threeFlag=A.threeFlag;
      lamPair=A.lamPair;
      formatMap=A.formatMap;
      polarVec=A.polarVec;
      polarFrac=A.polarFrac;
    }
  return *this;
}

flukaPhysics::~flukaPhysics()
  /*!
    Destructor
  */
{}

void
flukaPhysics::clearAll()
  /*!
    The big reset
  */
{
  cellVec.clear();
  matVec.clear();
  for(std::map<std::string,cellValueSet<0>>::value_type& mc : flagValue)
    mc.second.clearAll();

  for(std::map<std::string,cellValueSet<1>>::value_type& mc : impValue)
    mc.second.clearAll();


  for(std::map<std::string,cellValueSet<2>>::value_type& mc : emfFlag)
    mc.second.clearAll();

  for(std::map<std::string,cellValueSet<3>>::value_type& mc : threeFlag)
    mc.second.clearAll();

  polarVec=Geometry::Vec3D(0,0,0);

  return;
}


void
flukaPhysics::setCellNumbers(const std::vector<int>& cellInfo)
  /*!
    Process the list of the valid cells
    over each importance group.
    \param cellInfo :: list of cells
  */

{
  ELog::RegMethod RegA("flukaPhysics","setCellNumbers");

  cellVec=cellInfo;
  return;
}

void
flukaPhysics::setMatNumbers(const std::set<int>& matInfo)
  /*!
    Process the list of the valid cells
    over each importance group.
    \param matInfo :: list of materials
  */
{
  ELog::RegMethod RegA("flukaPhysics","setMatNumbers");

  matVec.assign(matInfo.begin(),matInfo.end());
  return;
}

void
flukaPhysics::setLAMPair(const std::string& keyName,
			 const std::string& pName,
			 const int cellNumber,
			 const std::string& V1,
			 const std::string& V2)
  /*!
    Set the importance list
    \param keyName :: all/hadron/electron/low
    \param pName :: particle number
    \param cellNumber :: Cell number
    \param V1 :: Electron cut values
    \param V2 :: Cur of energy
  */
{
  ELog::RegMethod RegA("flukaPhysics","setLAMPair(int)");

  std::map<std::string,pairValueSet<6>>::iterator mc=
    lamPair.find(keyName);

  if (mc==lamPair.end())
    throw ColErr::InContainerError<std::string>(keyName,"keyName in lamPair");

  mc->second.setValues(pName,std::to_string(cellNumber),V1,V2,"","");
  return;
}


void
flukaPhysics::setFlag(const std::string& keyName,
		      const std::string& nameID)
  /*!
    Set the importance list
    \param keyName :: typename
    \param nameID :: Name of particle/object
  */
{
  ELog::RegMethod RegA("flukaPhysics","setFlag(string)");

  std::map<std::string,cellValueSet<0>>::iterator mc=
    flagValue.find(keyName);
  if (mc==flagValue.end())
    throw ColErr::InContainerError<std::string>(keyName,"flagValue");

  mc->second.setValues(nameID);
  return;
}


void
flukaPhysics::setIMP(const std::string& keyName,
		     const std::string& nameID,
		     const std::string& value)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param nameID :: Name of particle/object
    \param value :: Value to use
  */
{
  ELog::RegMethod RegA("flukaPhysics","setIMP(string)");

  std::map<std::string,cellValueSet<1>>::iterator mc=
    impValue.find(keyName);

  if (mc==impValue.end())
    throw ColErr::InContainerError<std::string>(keyName,"impValue");

  mc->second.setValues(nameID,value);
  return;
}

void
flukaPhysics::setEMF(const std::string& keyName,
		     const std::string& cellNumber,
		     const std::string& electronCut,
		     const std::string& photonCut)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellNumber :: Cell number
    \param electronCut :: Electron cut values
    \param photonCut :: Electron cut values
  */
{
  ELog::RegMethod RegA("flukaPhysics","setEMF(string)");

  std::map<std::string,cellValueSet<2>>::iterator mc=
    emfFlag.find(keyName);

  if (mc==emfFlag.end())
    throw ColErr::InContainerError<std::string>(keyName,"keyName");

  mc->second.setValues(cellNumber,electronCut,photonCut);
  return;
}


void
flukaPhysics::setTHR(const std::string& keyName,
		     const std::string& cellName,
		     const std::string& V1,
		     const std::string& V2,
		     const std::string& V3)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellName :: Cell number
    \param V1 :: Electron cut values
    \param V2 :: photon cut values
    \param V3 :: some cut values
  */
{
  ELog::RegMethod RegA("flukaPhysics","setTHR(str)");

  std::map<std::string,cellValueSet<3>>::iterator mc=
    threeFlag.find(keyName);

  if (mc==threeFlag.end())
    throw ColErr::InContainerError<std::string>(keyName,"threeFlag");
  mc->second.setValues(cellName,V1,V2,V3);
  return;
}


void
flukaPhysics::setFlag(const std::string& keyName,
		      const int cellID)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellID :: Cell number
  */
{
  ELog::RegMethod RegA("flukaPhysics","setFlag");

  std::map<std::string,cellValueSet<0>>::iterator mc=
    flagValue.find(keyName);
  if (mc==flagValue.end())
    throw ColErr::InContainerError<std::string>(keyName,"flagValue");

  mc->second.setValues(cellID);
  return;
}

void
flukaPhysics::setIMP(const std::string& keyName,
		     const int cellID,
		     const std::string& value)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellID :: Cell number
    \param value :: Value to use
  */
{
  ELog::RegMethod RegA("flukaPhysics","setIMP");

  std::map<std::string,cellValueSet<1>>::iterator mc=
    impValue.find(keyName);
  if (mc==impValue.end())
    throw ColErr::InContainerError<std::string>(keyName,"impValue");

  mc->second.setValues(cellID,value);
  return;
}

void
flukaPhysics::setEMF(const std::string& keyName,
		     const int cellNumber,
		     const std::string& electronCut,
		     const std::string& photonCut)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellNumber :: Cell number
    \param electronCut :: Electron cut values
    \param photonCut :: Electron cut values
  */
{
  ELog::RegMethod RegA("flukaPhysics","setEMF(int)");

  std::map<std::string,cellValueSet<2>>::iterator mc=
    emfFlag.find(keyName);

  if (mc==emfFlag.end())
    throw ColErr::InContainerError<std::string>(keyName,"keyName");

  mc->second.setValues(cellNumber,electronCut,photonCut);
  return;
}

void
flukaPhysics::setTHR(const std::string& keyName,
		     const int cellNumber,
		     const std::string& V1,
		     const std::string& V2,
		     const std::string& V3)
  /*!
    Set the importance list
    \param keyName :: cellValue Keyname
    \param cellNumber :: Cell number
    \param V1 :: Electron cut values
    \param V2 :: photon cut values
    \param V3 :: some cut values
  */
{
  ELog::RegMethod RegA("flukaPhysics","setTHR");

  std::map<std::string,cellValueSet<3>>::iterator mc=
    threeFlag.find(keyName);

  if (mc==threeFlag.end())
    throw ColErr::InContainerError<std::string>(keyName,"keyName");

  mc->second.setValues(cellNumber,V1,V2,V3);
  return;
}


void
flukaPhysics::writeFLUKA(std::ostream& OX) const
  /*!
    Write out all the FLUKA physics
    \param OX :: Output stream
 */
{
  ELog::RegMethod RegA("flukaPhysics","writeFLUKA");

  typedef std::map<std::string,unitTYPE> FMAP;

  for(const std::map<std::string,cellValueSet<0>>::value_type& flagV :
	flagValue)
    {
      FMAP::const_iterator mc=formatMap.find(flagV.first);
      const int materialFlag(std::get<0>(mc->second));
      const std::string& fmtSTR(std::get<1>(mc->second));
      if (!materialFlag)  // cell
	flagV.second.writeFLUKA(OX,cellVec,fmtSTR);
      else if (materialFlag>0)
	flagV.second.writeFLUKA(OX,matVec,fmtSTR);
      else
	flagV.second.writeFLUKA(OX,fmtSTR);

    }

  for(const std::map<std::string,cellValueSet<1>>::value_type& impV : impValue)
    {
      FMAP::const_iterator mc=formatMap.find(impV.first);
      const int materialFlag(std::get<0>(mc->second));
      const std::string& fmtSTR(std::get<1>(mc->second));
      if (!materialFlag)  // cell
	impV.second.writeFLUKA(OX,cellVec,fmtSTR);
      else if (materialFlag>0)
	impV.second.writeFLUKA(OX,matVec,fmtSTR);
      else
	impV.second.writeFLUKA(OX,fmtSTR);
    }

  for(const std::map<std::string,cellValueSet<2>>::value_type& empV : emfFlag)
    {
      FMAP::const_iterator mc=formatMap.find(empV.first);
      const int materialFlag(std::get<0>(mc->second));
      const std::string& fmtSTR(std::get<1>(mc->second));

      if (!materialFlag)  // cell
	empV.second.writeFLUKA(OX,cellVec,fmtSTR);
      else if (materialFlag>0)       // mat
	empV.second.writeFLUKA(OX,matVec,fmtSTR);
      else
	empV.second.writeFLUKA(OX,fmtSTR);
    }

  for(const std::map<std::string,cellValueSet<3>>::value_type& thrV :
	threeFlag)
    {
      FMAP::const_iterator mc=formatMap.find(thrV.first);
      const int flag(std::get<0>(mc->second));
      const std::string& fmtSTR(std::get<1>(mc->second));

      if (!flag)  // cell
	thrV.second.writeFLUKA(OX,cellVec,fmtSTR);
      else if (flag>0)      // mat
	thrV.second.writeFLUKA(OX,matVec,fmtSTR);
      else                 // particle
	thrV.second.writeFLUKA(OX,fmtSTR);
    }

  for(const std::map<std::string,pairValueSet<6>>::value_type& lamV :
	lamPair)
    {
      FMAP::const_iterator mc=formatMap.find(lamV.first);
      const std::string& fmtSTR(std::get<1>(mc->second));
      lamV.second.writeFLUKA(OX,fmtSTR);
    }

  return;
}


} // NAMESPACE flukaSystem
