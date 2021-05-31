/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/PointSource.cxx
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
#include <boost/format.hpp>

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
#include "PointSource.h"

namespace SDef
{

PointSource::PointSource(const std::string& keyName) : 
  FixedOffsetUnit(keyName,0),SourceBase(),
  angleSpread(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

PointSource::PointSource(const PointSource& A) : 
  attachSystem::FixedOffsetUnit(A),SourceBase(A),
  angleSpread(A.angleSpread)
  /*!
    Copy constructor
    \param A :: PointSource to copy
  */
{}

PointSource&
PointSource::operator=(const PointSource& A)
  /*!
    Assignment operator
    \param A :: PointSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      angleSpread=A.angleSpread;
    }
  return *this;
}

PointSource*
PointSource::clone() const
  /*!
    Clone function 
    \return new this
  */
{
  PointSource* Ptr=new PointSource(*this);
  return Ptr;  
}

  
PointSource::~PointSource() 
  /*!
    Destructor
  */
{}

void
PointSource::populate(const mainSystem::MITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Control variables
   */
{
  ELog::RegMethod RegA("PointSource","populate");

  FixedOffset::populate(inputMap);
  SourceBase::populate(inputMap);

  angleSpread=mainSystem::getDefInput<double>(inputMap,"aSpread",0,0.0);
  angleSpread=
    mainSystem::getDefInput<double>(inputMap,"angleSpread",0,angleSpread);

  return;
}

void
PointSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("PointSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();

  return;
}

void
PointSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("PointSource","rotate");
  FixedComp::applyRotation(LR);  
  return;
}
  
void
PointSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("PointSource","createSource");

  if (angleSpread<360.0)
    {
      sourceCard.setComp("vec",Y);
      sourceCard.setComp("axs",Y);
      sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));
    }
  sourceCard.setComp("pos",Origin);
  SourceBase::createEnergySource(sourceCard);

  return;
}  

void
PointSource::createAll(const mainSystem::MITYPE& inputMap,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Create all the source
    \param inputMap :: DataBase for variables
    \param FC :: Fixed point to get orientation from
    \param linkIndex :: link Index						
   */
{
  ELog::RegMethod RegA("PointSource","createAll<inputMap,FC,linkIndex>");
  populate(inputMap);
  createUnitVector(FC,linkIndex);
  return;
}
  
void
PointSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("PointSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;

}

void
PointSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("PointSource","writePHITS");
  boost::format fFMT("%1$11.6g%|14t|");
  
  OX<<"  s-type =  9        # spherical source \n";
  OX<<"  x0  =   "<<(fFMT % Origin[0])<<"  #  center position of x-axis [cm]\n";
  OX<<"  y0  =   "<<(fFMT % Origin[1])<<"  #  center position of y-axis [cm]\n";
  OX<<"  z0  =   "<<(fFMT % Origin[2])<<"  #  mininium of z-axis [cm]\n";
  OX<<"  r0  =   0.0 \n";
  OX<<"  r1  =   0.0 \n";
  OX<<"  dir =   all   # Isotropic from centre \n";

  SourceBase::writePHITS(OX);
  OX<<std::endl;
  return;
}

void
PointSource::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("PointSource","writeFLUKA");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}


} // NAMESPACE SDef
 
