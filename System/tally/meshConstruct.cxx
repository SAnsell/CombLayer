/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/meshConstruct.cxx
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
#include <iterator>
#include <memory>

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
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "inputParam.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "meshTally.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "meshConstruct.h" 



namespace tallySystem
{

meshConstruct::meshConstruct() 
  /// Constructor
{}

meshConstruct::meshConstruct(const meshConstruct&) 
  /// Copy Constructor
{}

meshConstruct&
meshConstruct::operator=(const meshConstruct&) 
  /// Assignment operator
{
  return *this;
}

void
meshConstruct::processMesh(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) const
  /*!
    Add mesh tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("meshConstruct","processMesh");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,4,
				     "Insufficient items for tally");

  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 

  const masterRotate& MR=masterRotate::Instance();
  
  if (PType=="free" || PType=="heat" ||
      PType=="freeRotated" || PType=="heatRotated")
    {
      const std::string doseType=
	inputItem<std::string>(IParam,Index,2,"Dose type");
      Geometry::Vec3D APt=
	inputItem<Geometry::Vec3D>(IParam,Index,3,"Low Corner");
      Geometry::Vec3D BPt=
	inputItem<Geometry::Vec3D>(IParam,Index,6,"High Corner");
      
      // Rotation:
      size_t nxyzIndex(9);
      std::string revStr;
      const int flag=checkItem<std::string>(IParam,Index,9,revStr);
      if (!flag && revStr!="r" &&
	  PType!="freeRotated" && PType!="heatRotated")
	{
	  ELog::EM<<"Reverse rotating"<<ELog::endDiag;
	  APt=MR.reverseRotate(APt);
	  BPt=MR.reverseRotate(BPt);
	  //	  nxyzIndex+=(flag && revStr=="r") ? 1 : 0;
	}
      
      size_t Nxyz[3];
      Nxyz[0]=inputItem<size_t>(IParam,Index,nxyzIndex++,"NXpts");
      Nxyz[1]=inputItem<size_t>(IParam,Index,nxyzIndex++,"NYpts");
      Nxyz[2]=inputItem<size_t>(IParam,Index,nxyzIndex++,"NZpts");
      if (PType=="heat" || PType=="heatRotated")
	rectangleMesh(System,3,"void",APt,BPt,Nxyz);
      else
	rectangleMesh(System,1,doseType,APt,BPt,Nxyz);
      return;
    }
  ELog::EM<<"Unknown Mesh type :"<<PType<<ELog::endWarn;
  return;
}

void 
meshConstruct::rectangleMesh(Simulation& System,const int type,
			     const std::string& KeyWords,
			     const Geometry::Vec3D& APt,
			     const Geometry::Vec3D& BPt,
			     const size_t* MPts) const
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: Simulation to add tallies
    \param type :: type of tally[1,2,3]
    \param KeyWords :: KeyWords to add to the tally
    \param APt :: Lower point 
    \param BPt :: Upper point 
    \param MPts :: Points ot use
  */
{
  ELog::RegMethod RegA("meshConstruct","rectangleMesh");

  // Find next available number
  int tallyN(type);
  while(System.getTally(tallyN))
    tallyN+=10;

  // Create tally:
  meshTally MT(tallyN);
  if (type==1)
    MT.setParticles("n");
  MT.setCoordinates(APt,BPt);
  MT.setIndex(MPts);
  MT.setActive(1);
  if (KeyWords=="DOSE")
    {
      MT.setKeyWords("DOSE 1");
      MT.setResponse(getDoseConversion());
    }
  else if (KeyWords=="DOSEPHOTON")
    {
      MT.setParticles("p");
      MT.setKeyWords("DOSE 1");
      MT.setResponse(getPhotonDoseConversion());
    }
  else if (KeyWords=="InternalDOSE")
    {
      MT.setKeyWords("DOSE");
      MT.setIndexLine("40 1 2 1e6");
    }
  else if (KeyWords=="void")
    {
      MT.setKeyWords("");
    }
  else 
    {
      ELog::EM<<"Mesh keyword options:\n"
	      <<"  DOSE :: SNS Flux to Dose conversion (mrem/hour)\n"
	      <<"  InternalDOSE :: MCNPX Flux to Dose conversion (mrem/hour)\n"
	      <<"  void ::  Flux \n"
	      <<ELog::endDiag;
      ELog::EM<<"Using unknown keyword :"<<KeyWords<<ELog::endErr;
    }

  ELog::EM<<"Adding tally "<<MT<<ELog::endTrace;
  System.addTally(MT);

  return;
}

const std::string& 
meshConstruct::getDoseConversion()
  /*!
    Return the dose string  for a mshtr
    Uses the FTD files values [Flux to Dose conversion].
    - These values are in mrem/hour. You need a conversion factor
    of 360 to go to 1uSv/hour from particles/sec.
    \return FTD string
  */
{
  static std::string fcdString=
    " 0.         1.8864E-03  1.0000E-09  1.8864E-03  1.0000E-08  2.3580E-03 "
    "2.5000E-08  2.7360E-03  1.0000E-07  3.5820E-03  2.0000E-07  4.0320E-03 "
    "5.0000E-07  4.6080E-03  1.0000E-06  4.9680E-03  2.0000E-06  5.2200E-03 "
    "5.0000E-06  5.4000E-03  1.0000E-05  5.4360E-03  2.0000E-05  5.4360E-03 "
    "5.0000E-05  5.3280E-03  1.0000E-04  5.2560E-03  2.0000E-04  5.1840E-03 "
    "5.0000E-04  5.1120E-03  1.0000E-03  5.1120E-03  2.0000E-03  5.1840E-03 "
    "5.0000E-03  5.6520E-03  1.0000E-02  6.5880E-03  2.0000E-02  8.5680E-03 "
    "3.0000E-02  1.0440E-02  5.0000E-02  1.3860E-02  7.0000E-02  1.6992E-02 "
    "1.0000E-01  2.1528E-02  1.5000E-01  2.8872E-02  2.0000E-01  3.5640E-02 "
    "3.0000E-01  4.7880E-02  5.0000E-01  6.7680E-02  7.0000E-01  8.3160E-02 "
    "9.0000E-01  9.6120E-02  1.0000E+00  1.0152E-01  1.2000E+00  1.1160E-01 "
    "2.0000E+00  1.3788E-01  3.0000E+00  1.5552E-01  4.0000E+00  1.6488E-01 "
    "5.0000E+00  1.7064E-01  6.0000E+00  1.7388E-01  7.0000E+00  1.7640E-01 "
    "8.0000E+00  1.7784E-01  9.0000E+00  1.7892E-01  1.0000E+01  1.7964E-01 "
    "1.2000E+01  1.7964E-01  1.4000E+01  1.7856E-01  1.5000E+01  1.7784E-01 "
    "1.6000E+01  1.7676E-01  1.8000E+01  1.7496E-01  2.0000E+01  1.7280E-01 "
    "3.0000E+01  1.6488E-01  5.0000E+01  1.5732E-01  7.5000E+01  1.5444E-01 "
    "1.0000E+02  1.5444E-01  1.3000E+02  1.5552E-01  1.5000E+02  1.5768E-01 "
    "1.8000E+02  1.6020E-01  2.0000E+02  1.7028E-01  5.0000E+02  2.2680E-01 "
    "1.0000E+03  3.4920E-01  2.0000E+03  5.0724E-01  ";

  return fcdString;
}

const std::string& 
meshConstruct::getPhotonDoseConversion()
  /*!
    Return the dose string  for a mshtr
    Uses the FtD files values. Tihs is the H*(10) values which are higher
    than the FtD values!
    - These values are in mrem/hour. You need a conversion factor
    of 360 to go to 1uSv/hour from particles/sec.
    \return FCD string
  */
{
  static std::string fcdString=
    "1.0000E-09 2.3760E-03 1.0000E-08 3.2400E-03 2.5300E-08 3.8160E-03 "
    "1.0000E-07 4.6440E-03 2.0000E-07 4.8600E-03 1.0000E-06 4.7880E-03 "
    "2.0000E-06 4.6440E-03 5.0000E-06 4.3200E-03 1.0000E-05 4.0680E-03 "
    "2.0000E-05 3.8160E-03 5.0000E-05 3.5640E-03 1.0000E-04 3.3840E-03 "
    "2.0000E-04 3.2040E-03 5.0000E-04 2.9880E-03 1.0000E-03 2.8440E-03 "
    "2.0000E-03 2.7720E-03 5.0000E-03 2.8800E-03 1.0000E-02 3.7800E-03 "
    "2.0000E-02 5.9760E-03 3.0000E-02 8.5320E-03 5.0000E-02 1.4796E-02 "
    "7.0000E-02 2.1600E-02 1.0000E-01 3.1680E-02 2.0000E-01 6.1200E-02 "
    "3.0000E-01 8.3880E-02 5.0000E-01 1.1592E-01 7.0000E-01 1.3500E-01 "
    "9.0000E-01 1.4400E-01 1.0000E+00 1.4976E-01 1.2000E+00 1.5300E-01 "
    "2.0000E+00 1.5120E-01 3.0000E+00 1.4832E-01 4.0000E+00 1.4688E-01 "
    "5.0000E+00 1.4580E-01 6.0000E+00 1.4400E-01 7.0000E+00 1.4580E-01 "
    "8.0000E+00 1.4724E-01 9.0000E+00 1.5120E-01 1.0000E+01 1.5840E-01 "
    "1.2000E+01 1.7280E-01 1.4000E+01 1.8720E-01 1.6000E+01 1.9980E-01 "
    "1.8000E+01 2.0520E-01 2.0000E+01 2.1600E-01 3.0000E+01 1.8540E-01 "
    "5.0000E+01 1.4400E-01 7.5000E+01 1.1880E-01 1.0000E+02 1.0260E-01 "
    "1.2500E+02 9.3600E-02 1.5000E+02 8.8200E-02 1.7500E+02 9.0000E-02 "
    "2.0100E+02 9.3600E-02 ";
    
  return fcdString;
}

void
meshConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param Output stream
  */
{
  OX<<
    "free dosetype Vec3D Vec3D Nx Ny Nz \n"
    "heat Vec3D Vec3D Nx Ny Nz";
  return;
}

}  // NAMESPACE tallySystem
