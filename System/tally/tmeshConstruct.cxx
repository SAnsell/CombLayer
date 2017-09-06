/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/meshConstruct.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "tmeshTally.h"

#include "TallySelector.h"
#include "meshConstruct.h"
#include "tmeshConstruct.h" 



namespace tallySystem
{

tmeshConstruct::tmeshConstruct() : meshConstruct()
  /// Constructor
{}

tmeshConstruct::tmeshConstruct(const tmeshConstruct& A) :
  meshConstruct(A)
  /*!
    Copy Constructor
    \param A :: tmeshConstruct
   */
{}

tmeshConstruct&
tmeshConstruct::operator=(const tmeshConstruct& A) 
  /*!
    Copy Constructor
    \param A :: tmeshConstruct
    \return *this
  */

{
  if (this!=&A)
    {
      meshConstruct::operator=(A);
    }    
  return *this;
}

void 
tmeshConstruct::rectangleMesh(Simulation& System,const int type,
			      const std::string& KeyWords,
			      const Geometry::Vec3D& APt,
			      const Geometry::Vec3D& BPt,
			      const std::array<size_t,3>& MPts) const
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
  ELog::RegMethod RegA("tmeshConstruct","rectangleMesh");

  // Find next available number
  int tallyN(type);
  while(System.getTally(tallyN))
    tallyN+=10;

  // Create tally:
  tmeshTally MT(tallyN);
  if (type==1)
    MT.setParticles("n");
  MT.setCoordinates(APt,BPt);
  MT.setIndex(MPts);
  MT.setActive(1);
  MT.setKeyWords("");
      
  if (KeyWords=="DOSE")
    {
      MT.setResponse(getDoseConversion());
    }
  else if (KeyWords=="DOSEPHOTON")
    {
      MT.setParticles("p");
      MT.setResponse(getPhotonDoseConversion());
    }
  else if (KeyWords=="InternalDOSE")
    {
      MT.setKeyWords("DOSE");
      MT.setIndexLine("40 1 2 1e6");
    }
  else if (KeyWords=="void" || KeyWords=="VOID" ||
	   KeyWords=="flux" || KeyWords=="FLUX")
    { }
  else 
    {
      ELog::EM<<"TMesh keyword options:\n"
	      <<"  DOSE :: ICRP-116 Flux to Dose conversion (uSv/hour per n/cm2/sec)\n"
	      <<"  DOSEPHOTON :: ICRP-116 Flux to Dose conversion (uSv/hour per n/cm2/sec)\n"
	      <<"  InternalDOSE :: MCNPX Flux to Dose conversion (mrem/hour)\n"
	      <<"  void  ::  flux \n"
	      <<"  flux  ::  flux \n"
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
tmeshConstruct::writeHelp(std::ostream& OX) const
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
