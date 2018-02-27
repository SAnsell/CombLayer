/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SynchrotonBeam.cxx
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
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"
#include "particleConv.h"

#include "SourceBase.h"
#include "SynchrotonBeam.h"

namespace SDef
{

SynchrotonBeam::SynchrotonBeam(const std::string& keyName) : 
  FixedOffset(keyName,0),SourceBase()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

SynchrotonBeam::SynchrotonBeam(const SynchrotonBeam& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
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
SynchrotonBeam::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("SynchrotonBeam","populate");

  attachSystem::FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);
  // default neutron
  electronEnergy=Control.EvalDefVar<double>(keyName+"ElectronEnergy",3000.0);
  magneticField=Control.EvalVar<double>(keyName+"MagneticField");
  lowEnergyLimit=Control.EvalDefVar<double>(keyName+"LowEnergyLimit",1e-4);
  arcLength=Control.EvalDefVar<double>(keyName+"ArcLength",10.0); 

  beamXYZ=Control.EvalDefVar<Geometry::Vec3D>
    (keyName+"BeamXYZ",Geometry::Vec3D(0,0,0)); 

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
SynchrotonBeam::createSource(SDef::Source& sourceCard) const
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
    Create all with out using Control variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","createAll(FC)");
  createUnitVector(FC,linkIndex);
  return;
}

  
void
SynchrotonBeam::createAll(const FuncDataBase& Control,
		      const attachSystem::FixedComp& FC,
		      const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
   */
{
  ELog::RegMethod RegA("SynchrotonBeam","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);

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
SynchrotonBeam::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  return;
}

void
SynchrotonBeam::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SynchrotonBeam","writeFLUKA");
  boost::format FMTnum("%1$.4g");

  const particleConv& PC=particleConv::Instance();
  

  // beam : -energy X X X X X  : Partiles
  //  std::istringstream cx;
  //  cx<<(FMTnum % -energy);
  //  StrFunc::writeFLUKAhead("BEAM",PC.mcnpToPhits(particleType),cx.str(),OX)
  //  cx.str("");
  
  
  return;
}




} // NAMESPACE SDef
