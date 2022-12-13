/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   simMC/SimPhoton.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <memory>
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <stack>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <random>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "writeSupport.h"
#include "Random.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ObjSurfMap.h"
#include "Surface.h"
#include "surfIndex.h"

#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"

#include "particle.h"
#include "neutron.h"
#include "photon.h"
#include "Beam.h"

#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "countUnit.h"
#include "ChannelDet.h"
#include "SimPhoton.h"

SimPhoton::SimPhoton() :
  Simulation(),TCount(0),B(0)
  /*!
    Start of simulation Object
    Initialise currentSample to Sample 
  */
{}

SimPhoton::SimPhoton(const SimPhoton& A)  :
  Simulation(A),
  TCount(A.TCount),
  B((A.B) ? A.B->clone() : nullptr)
  /*!
    Copy constructor:: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
  */
{
  for(const Transport::ChannelDet* DPtr : A.DSet)
    DSet.insert(DPtr->clone());
}


SimPhoton&
SimPhoton::operator=(const SimPhoton& A) 
  /*!
    Assignment operator :: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      delete B;
      B=(A.B) ? A.B->clone() : nullptr;
      for(const Transport::ChannelDet* DPtr : A.DSet)
	DSet.insert(DPtr->clone());
    }
  return *this;
}

SimPhoton::~SimPhoton()
  /*!
    Destructor :: deletes the memory in the SurMap 
    list
  */
{
  delete B;
  for(const Transport::ChannelDet* DPtr : DSet)
    delete DPtr;
}

void
SimPhoton::clearAll()
 /*!
   Thermonuclear distruction of the whole system
 */
{
  TCount=0;
  for(Transport::ChannelDet* DPtr : DSet)
    DPtr->clear();
  return;
}


void 
SimPhoton::setBeam(const Transport::Beam& BObj) 
  /*!
    Set the beam pointer
    \param BObj :: Point to a beam implementation
  */
{
  delete B;
  B=BObj.clone();
  return;
}

void
SimPhoton::addDetector(const Transport::ChannelDet& DObj)
  /*!
    Set the detector
    \param DObj :: Detector object to add
  */
{
  ELog::RegMethod RegA("SimPhoton","setDetector");
  DSet.insert(DObj.clone());
  return;
}

void
SimPhoton::setDefMaterial(const double E0)
  /*!
    Go round each material and convert it to a
    photonMaterial.
    \param E0 :: Elastic limit energy
  */
{
  ELog::RegMethod RegA("SimPhoton","setDefMaterial");

  
  return;
}


void
SimPhoton::runMonte(const size_t Npts)
  /*!
    Run a simulation
  */
{
  ELog::RegMethod RegA("SimMonte","runMonte");
  static MonteCarlo::Object* defObj(0);
  
  for(size_t i=0;i<Npts;i++)
    {
      MonteCarlo::photon p=B->generatePhoton();
      MonteCarlo::Object* OPtr=this->findCell(p.Pos,defObj);
      defObj=OPtr;
      while(OPtr && !OPtr->isZeroImp())
	{
	  OPtr=0;
	}
    }
  
  
  return;
}


void
SimPhoton::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- CELL CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      mp->second->write(OX);
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;  // Empty line manditory for MCNPX
  return;
}


void
SimPhoton::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- SURFACE CARDS -------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();
  
  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->write(OX);

  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;
  return;
}

void
SimPhoton::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- MATERIAL CARDS ------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  

    // set ordered otherwize output random [which is annoying]
  const std::map<int,const MonteCarlo::Material*> orderedMat=
    getOrderedMaterial();

  for(const auto& [matID,matPtr] : orderedMat)
    matPtr->write(OX);
  
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}

void
SimPhoton::write(const std::string& Fname) const
  /*!
    Write out all the system (in MCNPX output format)
    \param Fname :: Output file 
  */
{
  std::ofstream OX(Fname.c_str());
  
  OX<<"Input File:"<<inputFile<<std::endl;
  StrFunc::writeMCNPXcomment("RunCmd:"+cmdLine,OX);
  writeVariables(OX);
  writeCells(OX);
  writeSurfaces(OX);
  writeMaterial(OX);
  // writeTransform(OX);
  // writeWeights(OX);
  // writeTally(OX);
  // writeSource(OX);
  // writeImportance(OX);
  // writePhysics(OX);
  OX.close();
  return;
}
