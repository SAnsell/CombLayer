/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimFLUKA.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell / Konstantin Batkov
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
#include "MapSupport.h"
#include "MXcards.h"
#include "Element.h"
#include "Zaid.h"
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
#include "tallyFactory.h"
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
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SimFLUKA.h"

SimFLUKA::SimFLUKA() :
  Simulation(),
  alignment("*...+.WHAT....+....1....+....2....+....3....+....4....+....5....+....6....+.SDUM")
  /*!
    Constructor
  */
{}


SimFLUKA::SimFLUKA(const SimFLUKA& A) :
  Simulation(A),
  alignment(A.alignment)
 /*! 
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimFLUKA&
SimFLUKA::operator=(const SimFLUKA& A)
  /*!
    Assignment operator
    \param A :: SimFLUKA to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
    }
  return *this;
}


void
SimFLUKA::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  OX<<"c -----------------------------------------------------------"<<std::endl;
  OX<<"c ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"c -----------------------------------------------------------"<<std::endl;
  // The totally insane line below does the following
  // It iterats over the Titems and since they are a map
  // uses the mathSupport:::PSecond
  // _1 refers back to the TItem pair<int,tally*>
  for(const TallyTYPE::value_type& TI : TItem)
    TI.second->write(OX);

  return;
}

void
SimFLUKA::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  OX<<"[transform]"<<std::endl;

  TransTYPE::const_iterator vt;
  for(vt=TList.begin();vt!=TList.end();vt++)
    {
      vt->second.write(OX);
    }
  return;
}


void
SimFLUKA::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard FLUKA output 
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeCells");
  OX<<"* CELL CARDS "<<std::endl;

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writeFLUKA(OX);
  OX<<"END"<<std::endl;
  return;
}

void
SimFLUKA::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  OX<<"* SURFACE CARDS "<<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();

  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->writeFLUKA(OX);
  OX<<"END"<<std::endl;
  return;
} 

void
SimFLUKA::writeElements(std::ostream& OX) const
/*!
  Write all the used isotopes.
  To be used by material definitions in the COMPOUND cards.
 */
{
  ELog::RegMethod RegA("SimFLUKA","writeElements");

  OX<<"* ELEMENTS "<<std::endl;
  OX<<alignment<<std::endl;
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();

  size_t za;
  std::set<size_t> setZA;
  //  for(mp=OList.begin();mp!=OList.end();mp++)
  for(const OTYPE::value_type& mp : OList)
    {
      MonteCarlo::Material m = DB.getMaterial(mp.second->getMat());
      std::vector<MonteCarlo::Zaid> zaidVec = m.getZaidVec();
      for (const MonteCarlo::Zaid& ZC : zaidVec)
	{
	  za = ZC.getZaidNum();
	  setZA.insert(za);
	}
    }

  std::ostringstream cx,lowmat;
  for (const size_t &za : setZA)
    {
      if (!za) continue;
      const std::string mat("E"+std::to_string(za));
      cx<<"MATERIAL "<<za / 1000<<". - "<<" 1."<<" - - "<<
	za%1000<<". "<<mat<<" ";
      lowmat<<"LOW-MAT "<<mat<<" - - - - - - "; // \todo define it correctly
    }

  StrFunc::writeFLUKA(cx.str(),OX);
  //  StrFunc::writeFLUKA(lowmat.str(),OX);

  OX<<alignment<<std::endl;

  return;
}

void
SimFLUKA::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in fixed FLUKA format 
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimFLUKA","writeMaterial");

  OX<<"* MATERIAL CARDS "<<std::endl;
  OX<<alignment<<std::endl;
  // WRITE OUT ASSIGNMENT:
  for(const OTYPE::value_type& mp : OList)
    mp.second->writeFLUKAmat(OX);
    
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());

  writeElements(OX);

  DB.writeFLUKA(OX);
  
  OX<<alignment<<std::endl;
  return;
}
  


void
SimFLUKA::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  
  OX<<"[weight]"<<std::endl;
  WM.write(OX);
  return;
}


void
SimFLUKA::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */

{  
  ELog::RegMethod RegA("SimFLUKA","writePhysics");
  // Processing for point tallies
  std::map<int,tallySystem::Tally*>::const_iterator mc;
  std::vector<int> Idum;
  std::vector<Geometry::Vec3D> Rdum;
  for(mc=TItem.begin();mc!=TItem.end();mc++)
    {
      const tallySystem::pointTally* Ptr=
	dynamic_cast<const tallySystem::pointTally*>(mc->second);
      if(Ptr && Ptr->hasRdum())
        {
	  Idum.push_back(Ptr->getKey());
	  for(size_t i=0;i<4;i++)
	    Rdum.push_back(Ptr->getWindowPt(i));
	}
    }
  if (!Idum.empty())
    {
      OX<<"idum "<<Idum.size()<<" ";
      copy(Idum.begin(),Idum.end(),std::ostream_iterator<int>(OX," "));
      OX<<std::endl;
      OX<<"rdum       "<<Rdum.front()<<std::endl;
      for(const Geometry::Vec3D& rN : Rdum)
	OX<<"           "<<rN<<std::endl;	
    }

  // Remaining Physics cards
  PhysPtr->writeFLUKA(OX);
  return;
}

void
SimFLUKA::write(const std::string& Fname) const
  /*!
    Write out all the system (in PHITS output format)
    \param Fname :: Output file 
  */
{
  ELog::RegMethod RegA("SimFLUKA","write");

  std::ofstream OX(Fname.c_str());
  const size_t nCells(OList.size());
  const size_t maxCells(20000);
  if (nCells>maxCells)
    {
      ELog::EM<<"Number of regions in geometry exceeds FLUKA max: "<<nCells
	      <<" > "<<maxCells<<ELog::endCrit;
      ELog::EM<<"See the GLOBAL card documentation"<<ELog::endCrit;
    }

  StrFunc::writeFLUKA("GLOBAL "+std::to_string(nCells),OX);
  OX<<"TITLE"<<std::endl;
  OX<<" Fluka model from CombLayer http://github.com/SAnsell/CombLayer"<<std::endl;
  Simulation::writeVariables(OX,'*');
  StrFunc::writeFLUKA("DEFAULTS - - - - - - EM-CASCADE",OX);
  ELog::EM<<"FLUKA defaults is EM-CASCADE. No low energy neutrons transported."<<ELog::endCrit;
  StrFunc::writeFLUKA("BEAM -2.0 - - 14.0 3.2 1.0 PROTON",OX);
  StrFunc::writeFLUKA("BEAMPOS 0.0 -50.0 0.0 0.0 1.0 0.0",OX);

  StrFunc::writeFLUKA("GEOBEGIN - - - - - - COMBNAME",OX);
  OX<<"  0 0 FLUKA Geometry from CombLayer"<<std::endl;
  writeSurfaces(OX);
  writeCells(OX);
  OX<<"GEOEND"<<std::endl;
  writeMaterial(OX);
  writePhysics(OX);
  OX.close();
  return;
}
