/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SynchrotonBeam.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Source.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "SynchrotonBeam.h"

namespace SDef
{

SynchrotonBeam::SynchrotonBeam(const std::string& keyName) : 
  FixedOffsetUnit(keyName,0),SourceBase()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

SynchrotonBeam::SynchrotonBeam(const SynchrotonBeam& A) : 
  attachSystem::FixedOffsetUnit(A),SourceBase(A),
  electronEnergy(A.electronEnergy),magneticField(A.magneticField),
  lowEnergyLimit(A.lowEnergyLimit),arcLength(A.arcLength),
  beamXYZ(A.beamXYZ)
  /*!
    Copy constructor
    \param A :: SynchrotonBeam to copy
  */
{}

SynchrotonBeam&
SynchrotonBeam::operator=(const SynchrotonBeam& A)
  /*!
    Assignment operator
    \param A :: SynchrotonBeam to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      electronEnergy=A.electronEnergy;
      magneticField=A.magneticField;
      lowEnergyLimit=A.lowEnergyLimit;
      arcLength=A.arcLength;
      beamXYZ=A.beamXYZ;
    }
  return *this;
}

SynchrotonBeam::~SynchrotonBeam() 
  /*!
    Destructor
  */
{}

SynchrotonBeam*
SynchrotonBeam::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new SynchrotonBeam(*this);
}
  
  
  
void
SynchrotonBeam::populate(const mainSystem::MITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Control variables
   */
{
  ELog::RegMethod RegA("SynchrotonBeam","populate");

  attachSystem::FixedOffset::populate(inputMap);
  SourceBase::populate(inputMap);
  // default neutron

  electronEnergy=
    mainSystem::getDefInput<double>(inputMap,"electronEnergy",0,3000.0);
  magneticField=
    mainSystem::getDefInput<double>(inputMap,"magneticField",0,1.0);
  lowEnergyLimit=
    mainSystem::getDefInput<double>(inputMap,"lowEnergyLimit",0,1e-4);
  arcLength=
    mainSystem::getDefInput<double>(inputMap,"arcLength",0,10.0); 

  beamXYZ=mainSystem::getDefInput<Geometry::Vec3D>
    (inputMap,"beamXYZ",0,Geometry::Vec3D(0,0,0)); 

  return;
}

void
SynchrotonBeam::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("SynchrotonBeam","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();

  return;
}

void
SynchrotonBeam::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("SynchrotonBeam","rotate");
  FixedComp::applyRotation(LR);  
  return;
}
  
void
SynchrotonBeam::createSource(SDef::Source&) const
  /*!
    Creates a simple beam sampled uniformly in a
    circle
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","createSource");

  return;
}  

void
SynchrotonBeam::createAll(const attachSystem::FixedComp& FC,
		      const long int linkIndex)
  /*!
    Create all without using variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","createAll(FC)");
  createUnitVector(FC,linkIndex);
  FixedOffset::applyOffset();
  return;
}

  
void
SynchrotonBeam::createAll(const mainSystem::MITYPE& inputMap,
		      const attachSystem::FixedComp& FC,
		      const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
   */
{
  ELog::RegMethod RegA("SynchrotonBeam","createAll<inputMap,FC,linkIndex>");
  populate(inputMap);
  createUnitVector(FC,linkIndex);
  FixedOffset::applyOffset();
  return;
}

void
SynchrotonBeam::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;
}

void
SynchrotonBeam::writePHITS(std::ostream&) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  return;
}

void
SynchrotonBeam::writeFLUKA(std::ostream&) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","writeFLUKA");

  //  const particleConv& PC=particleConv::Instance();
  

  // beam : -energy X X X X X  : Partiles
  //  std::istringstream cx;
  //  cx<<(FMTnum % -energy);
  //  StrFunc::writeFLUKAhead("BEAM",PC.mcnpToPhits(particleType),cx.str(),OX)
  //  cx.str("");
  
  
  return;
}




} // NAMESPACE SDef
