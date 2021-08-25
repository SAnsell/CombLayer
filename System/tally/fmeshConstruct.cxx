/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fmeshConstruct.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "fmeshTally.h"

#include "doseFactors.h"
#include "meshConstruct.h"
#include "fmeshConstruct.h" 

namespace tallySystem
{

namespace meshC = mainSystem::meshConstruct;
namespace doseF = mainSystem::doseFactors;

void
fmeshConstruct::processMesh(SimMCNP& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
  /*!
    Add mesh tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("meshConstruct","processMesh");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,4,
				     "Insufficient items for tally");

  const std::string doseType=
    IParam.getValueError<std::string>("tally",Index,1,"Dose type");

  const std::string PType=
    IParam.getValueError<std::string>("tally",Index,2,"object/free"); 

  Geometry::Vec3D APt,BPt;
  std::array<size_t,3> Nxyz;
  
  if (PType=="object" || PType=="heatObject")
    meshC::getObjectMesh(System,IParam,"tally",Index,3,APt,BPt,Nxyz);
  else if (PType=="free" || PType=="heat")
    meshC::getFreeMesh(IParam,"tally",Index,3,APt,BPt,Nxyz);

  if (PType=="heatObject" || PType=="heat")
    rectangleMesh(System,3,"void",APt,BPt,Nxyz);

  else if (PType=="free" || PType=="flux" ||
	   PType=="object")
    rectangleMesh(System,1,doseType,APt,BPt,Nxyz);
  
  else
    throw ColErr::InContainerError<std::string>
      (PType,"Unknown Mesh type :");
  
  return;      
}

void 
fmeshConstruct::rectangleMesh(SimMCNP& System,const int type,
			      const std::string& KeyWords,
			      const Geometry::Vec3D& APt,
			      const Geometry::Vec3D& BPt,
			      const std::array<size_t,3>& MPts)
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimMCNP to add tallies
    \param type :: type of tally[1,2,3]
    \param KeyWords :: KeyWords to add to the tally
    \param APt :: Lower point 
    \param BPt :: Upper point 
    \param MPts :: Points ot use
  */
{
  ELog::RegMethod RegA("fmeshConstruct","rectangleMesh");

  // Find next available number
  int tallyN(type);
  while(System.getTally(tallyN))
    tallyN+=10;

  // Create tally:
  fmeshTally MT(tallyN);
  if (type==1)
    MT.setParticles("n");
  MT.setCoordinates(APt,BPt);  
  MT.setIndex(MPts);
  MT.setActive(1);
  if (KeyWords=="DOSE")
    {
      MT.setKeyWords("DOSE 1");
      MT.setResponse(doseF::getDoseConversion());
    }
  else if (KeyWords=="DOSEPHOTON")
    {
      MT.setParticles("p");
      MT.setKeyWords("DOSE 1");
      MT.setResponse(doseF::getPhotonDoseConversion());
    }
  else if (KeyWords=="void")
    {
      MT.setKeyWords("");
    }
  else 
    {
      ELog::EM<<"Mesh keyword options:\n"
	      <<"  DOSE :: ICRP-116 Flux to Dose conversion (uSv/hour per n/cm2/sec)\n"
	      <<"  DOSEPHOTON :: ICRP-116 Flux to Dose conversion (uSv/hour per n/cm2/sec)\n"
	      <<"  InternalDOSE :: MCNPX Flux to Dose conversion (mrem/hour)\n"
	      <<"  void ::  Flux \n"
	      <<ELog::endDiag;
      ELog::EM<<"Using unknown keyword :"<<KeyWords<<ELog::endErr;
    }

  ELog::EM<<"Adding tally "<<ELog::endTrace;
  ELog::EM<<"Coordinates  : "<<ELog::endTrace;
  MT.writeCoordinates(ELog::EM.Estream());
  ELog::EM<<ELog::endTrace;

  System.addTally(MT);

  return;
}
  
void
fmeshConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "free dosetype Vec3D Vec3D Nx Ny Nz \n"
    "object objectName LinkPt dosetype Vec3D Vec3D Nx Ny Nz \n"
    "  -- Object-link point is used to construct basis set \n"
    "     Then the Vec3D are used as the offset points \n"
    "heat Vec3D Vec3D Nx Ny Nz";
  return;
}

}  // NAMESPACE tallySystem
