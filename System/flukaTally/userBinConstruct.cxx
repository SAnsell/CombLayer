/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userBinConstruct.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "inputParam.h"

#include "TallySelector.h"
#include "meshConstrut.h"
#include "userBinConstruct.h" 



namespace flukaTallySystem
{

void 
userBinConstruct::createTally(SimFLUKA& System,
				const std::string& PType,
				const int fortranTape,
				const Geometry::Vec3D& APt,
				const Geometry::Vec3D& BPt,
				const std::array<size_t,3>& MPts) 
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param KeyWords :: KeyWords to add to the tally
    \param APt :: Lower point 
    \param BPt :: Upper point 
    \param MPts :: Points ot use
  */
{
  ELog::RegMethod RegA("userBinConstruct","createTally");

  userBin* UB=new userBin();
  // Find next available number

}

std::string
userBinConstruct::convertTallyType(const std::string& TType)
  /*!
    Construct to convert words into an index
    \param TTYPE :: incoming value / particle
    \return particle name [upper case] or string of number
  */
{
  ELog::RegMethod RegA("userBinConstruct","convertTallyType");
  
  static const std::map<std::string,int> tMap
    ( { "energy",      208 },              // energy 
      { "em-energy",   211 },           // electro magnetic energy
      { "dose"     ,   228 },
      { "unb-energy",  229 },           // ????
      { "dose-eq",     240 }            // Dose equivilent [needs auxscore]
      );

  std::map<std::string,int>::const_iterator tc=tMap.find(TType);
  std::istringstream cx;
  if (tc!=tMap.end())
    cx<<std::setw(10)<<std::right<<std::to_string(tc->second)+".0";
  else if (particleConv::hasFlukaName(TType))
    cx<<std::setw(10)<<std::right<<StrFunc::toUpperString(TType);
  else
    throw ColErr::InContainerError<std::string>(TType,"TType not in TMap");
  
  return cx.str();
}


  
void
userBinConstruct::processMesh(SimFLUKA& System,
			      const mainSystem::inputParam& IParam,
			      const size_t Index) 
  /*!
    Add mesh tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("userBinConstruct","processMesh");
  
  const std::string tallyType=
    IParam.getValueError<std::string>("tally",Index,1,"tallyType");
  const int what2Number=userBinConstruct::convertTallyType(tallyType);

  // This needs to be more sophisticated
  const size_t nextId=Index+25;

  
  const std::string PType=
    IParam.getValueError<std::string>("tally",Index,2,"object/free"); 
  Geometry::Vec3D APt,BPt;
  std::array<size_t,3> Nxyz;
  
  if (PType=="object")
    meshConstruct::getObjectMesh(IParam,Index,3,APt,BPt,Nxyz);
  else if (PType=="free")
    meshConstruct::getFreeMesh(IParam,Index,3,APt,BPt,Nxyz);

  userbinConstruct::createTally(System,tallyType,APt,BPt,Nxyz);
  
  return;      
}
  
void
userBinConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "tallyType free Vec3D Vec3D Nx Ny Nz \n"
    "tallyType object objectName LinkPt  Vec3D Vec3D Nx Ny Nz \n"
    "  -- Object-link point is used to construct basis set \n"
    "     Then the Vec3D are used as the offset points "

  return;
}

}  // NAMESPACE tallySystem
