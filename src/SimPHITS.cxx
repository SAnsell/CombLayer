/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimPHITS.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "version.h"
#include "Element.h"
#include "Zaid.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "ArbPoly.h"
#include "Cylinder.h"
#include "Cone.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Torus.h"
#include "General.h"
#include "surfaceFactory.h"
#include "surfProg.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "Debug.h"
#include "BnId.h"
#include "AcompTools.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "phitsPhysics.h"
#include "phitsTally.h"
#include "Simulation.h"
#include "SimPHITS.h"


SimPHITS::SimPHITS() :
  Simulation(),icntl(0),nps(10000),rndSeed(1234567871),
  PhysPtr(new phitsSystem::phitsPhysics())
  /*!
    Constructor
  */
{}


SimPHITS::SimPHITS(const SimPHITS& A) :
  Simulation(A),nps(A.nps),rndSeed(A.rndSeed),
  PhysPtr(new phitsSystem::phitsPhysics(*PhysPtr))
 /*! 
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimPHITS&
SimPHITS::operator=(const SimPHITS& A)
  /*!
    Assignment operator
    \param A :: SimPHITS to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
      nps=A.nps;
      rndSeed=A.rndSeed;
      *PhysPtr=*PhysPtr;
    }
  return *this;
}

void
SimPHITS::setICNTL(const std::string& ICName)
  /*!
    Given the icntl name find the correct icntl number
    \param ICName :: Name to convert to number
   */
{
  ELog::RegMethod RegA("SimPHITS","setICNTL");

  int icn(0);
  if (StrFunc::convert(ICName,icn))
    {
      icntl=icn;
      return;
    }
  else if (ICName=="plot")
    {
      icntl=8;
      return;
    }
    
  throw ColErr::InContainerError<std::string>(ICName,"ICName");
  return;
}

void
SimPHITS::writeSource(std::ostream& OX) const
  /*!
    Writes out the sources 
    construction.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPHITS","writeSource");

  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();
  
  OX<<"[Source]"<<std::endl;

  const SDef::SourceBase* SBPtr=
    SDB.getSource<SDef::SourceBase>(sourceName);
  if (SBPtr)
    SBPtr->writePHITS(OX);
  return;
}

void
SimPHITS::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies 
    construction.
    \param OX :: Output stream
   */
{
  OX<<"$ -----------------------------------------------------------"<<std::endl;
  OX<<"$ ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"$ -----------------------------------------------------------"<<std::endl;
  // The totally insane line below does the following
  // It iterats over the Titems and since they are a map
  // uses the mathSupport:::PSecond
  // _1 refers back to the TItem pair<int,tally*>
  //  for(const TallyTYPE::value_type& TI : TItem)
  //    TI.second->write(OX);

  return;
}

void
SimPHITS::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  if (!TList.empty())
    {
      OX<<"[transform]"<<std::endl;
      TransTYPE::const_iterator vt;
      for(vt=TList.begin();vt!=TList.end();vt++)
	vt->second.write(OX);
    }
  return;
}


void
SimPHITS::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  boost::format FmtStr("  %1$d%|20t|%2$d\n");
  OX<<"[cell]"<<std::endl;
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writePHITS(OX);

  OX<<std::endl;  // Empty line manditory for MCNPX

  OX<<"[temperature]"<<std::endl;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      const double T=mp->second->getTemp();
      if (std::abs(T-300.0)>1.0 && std::abs<double>(T)>1e-6)
	{
	  OX<<FmtStr % mp->second->getName() % (T*8.6173422e-5);
	}
    }
  OX<<std::endl;
  
  return;
}

void
SimPHITS::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"[surface] " <<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap=
    ModelSupport::surfIndex::Instance().surMap();

  for(const std::map<int,Geometry::Surface*>
	::value_type& mp : SurMap)
    mp.second->write(OX);

  return;
} 

void
SimPHITS::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  OX<<"[ Material ]"<<std::endl;

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  //  if (!PhysPtr->getMode().hasElm("h"))
  //    DB.deactivateParticle("h");
  
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());
  
  ModelSupport::DBMaterial::Instance().writePHITS(OX);
  
  return;
}


void
SimPHITS::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  
  WM.writePHITS(OX);
  return;
}


void
SimPHITS::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */

{  
  ELog::RegMethod RegA("SimPHITS","writePhysics");
  // Processing for point tallies

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  boost::format FMT("%1$8.0d ");
  OX<<"[Parameters]"<<std::endl;

  OX<<" icntl       =        "<<(FMT % icntl)<<std::endl;
  OX<<" maxcas      =        "<<(FMT % (nps/10))<<std::endl;
  OX<<" maxbch      =        "<<(FMT % 10)<<std::endl;
  OX<<" negs        =        "<<(FMT % 0)<<std::endl;  // photo nuclear?
  OX<<" file(1)     = /home/ansell/phits"<<std::endl;  
  OX<<" file(6)     = phits.out"<<std::endl;
  OX<<" rseed       =        "<<(FMT % rndSeed)<<std::endl;  

  ELog::EM<<"NOTE NOT WRITING PHYSICS"<<ELog::endDiag;
  //  PhysPtr->writePHITS(OX);


  if (WM.hasParticle("n"))
    {
      ELog::EM<<"WEIGHT"<<ELog::endErr;
      const WeightSystem::WForm* NWForm=WM.getParticle("n");
      NWForm->writePHITSHead(OX);
    }
  OX<<"$ ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;

  
  OX<<std::endl;
  return;
}

void
SimPHITS::write(const std::string& Fname) const
  /*!
    Write out all the system (in PHITS output format)
    \param Fname :: Output file 
  */
{
  std::ofstream OX(Fname.c_str());
  OX<<"[Title]"<<std::endl;
  writePhysics(OX);
  //  Simulation::writeVariables(OX);
  OX<<std::endl;
  
  writeCells(OX);
  writeSurfaces(OX);
  writeMaterial(OX);
  writeWeights(OX);
  writeTransform(OX);
  writeTally(OX);
  writeSource(OX);

  OX<<"[end]"<<std::endl;
  OX.close();
  return;
}
