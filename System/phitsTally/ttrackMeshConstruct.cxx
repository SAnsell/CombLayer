/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/ttrackMeshConstruct.cxx
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputParam.h"

#include "SimPHITS.h"
#include "meshConstruct.h"
#include "phitsTallySelector.h"
#include "MeshXYZ.h"
#include "eType.h"
#include "phitsTally.h"
#include "TMesh.h"
#include "ttrackMeshConstruct.h" 

namespace phitsSystem
{

void 
ttrackMeshConstruct::createTally(SimPHITS& System,
				 const std::string& PType,
				 const int ID,
				 const Geometry::Vec3D& APt,
				 const Geometry::Vec3D& BPt,
			    const std::array<size_t,3>& MPts) 
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param PType :: processed particle type
    \param fortranTape :: output stream
    \param APt :: Lower point 
    \param BPt :: Upper point 
    \param MPts :: Points ot use
  */
{
  ELog::RegMethod RegA("ttrackMeshConstruct","createTally");

  TMesh TM(ID);
  TM.setParticle(PType);
  TM.setCoordinates(APt,BPt);
  TM.setIndex(MPts);
  System.addTally(TM);

  return;
}

  
 
void
ttrackMeshConstruct::processMesh(SimPHITS& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
/*!
    Add t-cross tally (s) as needed
    \param System :: SimPHITS to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("ttrackMeshConstruct","processMesh");

  const int nextID=System.getNextTallyID();
  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");
  if (particleType=="help")
    {
      ELog::EM<<writeHelp()<<ELog::endDiag;
      return;
    }
  const std::string tallyParticle=
    convertTallyParticle(particleType);

  const std::string PType=
    IParam.getValueError<std::string>("tally",Index,2,"object/free"); 
  Geometry::Vec3D APt,BPt;
  std::array<size_t,3> Nxyz;
  
  if (PType=="object")
    mainSystem::meshConstruct::getObjectMesh
      (System,IParam,"tally",Index,3,APt,BPt,Nxyz);

  else if (PType=="free")
    mainSystem::meshConstruct::getFreeMesh
      (IParam,"tally",Index,3,APt,BPt,Nxyz);

  ttrackMeshConstruct::createTally(System,tallyParticle,nextID,APt,BPt,Nxyz);
  
  return;      
}
  
std::string
ttrackMeshConstruct::writeHelp()
  /*!
    Write out help
    \return :: Output stream
  */
{
  return 
    "-T mesh \n"
    "  particleType : name of particle / energy to tally\n"
    "  Option A : \n"
    "     free free Vec3D Vec3D Nx Ny Nz \n"    
    "  Option B : \n"
    "      object objectName LinkPt  Vec3D Vec3D Nx Ny Nz \n"
    "  -- Object-link point is used to construct basis set \n"
    "     Then the Vec3D are used as the offset points \n\n"
    "  Energy is set by TMod (otherwize single group)";
}

}  // NAMESPACE phitsSystem
