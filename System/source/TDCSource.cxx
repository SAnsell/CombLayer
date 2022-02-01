/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/TDCSource.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "Vec3D.h"
#include "inputSupport.h"
#include "Source.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"

#include "SourceBase.h"
#include "TDCSource.h"

namespace SDef
{
  
  
TDCSource::TDCSource(const std::string& keyName) : 
  FixedRotateUnit(keyName,0),SourceBase(),
  energyMin(0.0),energyMax(1.0),radius(1.0),length(100.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

TDCSource::TDCSource(const TDCSource& A) :
  FixedRotateUnit(A),SourceBase(A),
  energyMin(A.energyMin),energyMax(A.energyMax),
  radius(A.radius),length(A.length)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

TDCSource::~TDCSource() 
  /*!
    Destructor
  */
{}

TDCSource*
TDCSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new TDCSource(*this);
}
  
 
void
TDCSource::populate(const ITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Control variables
   */
{
  ELog::RegMethod RegA("TDCSource","populate");

  attachSystem::FixedRotate::populate(inputMap);
  SourceBase::populate(inputMap);

  std::string unitName("tdc");
  size_t NPts=mainSystem::sizeInput(inputMap,unitName);

  double DValues[4]={0.0,1.0,radius,length};
  for(size_t index=0;index<NPts && index<4;index++)
    {
      double D;
      const std::string IStr=
	mainSystem::getInput<std::string>(inputMap,unitName,index);
      if (StrFunc::convert(IStr,D))
	DValues[index]=D;
    }
  
  energyMin=DValues[0];
  energyMax=DValues[1];
  radius=DValues[2];
  length=DValues[3];

  energyMin=mainSystem::getDefInput(inputMap,"energyMin",0,energyMin);
  energyMax=mainSystem::getDefInput(inputMap,"energyMax",0,energyMax);
  radius=mainSystem::getDefInput(inputMap,"raduis",0,radius);
  length=mainSystem::getDefInput(inputMap,"length",0,length);

  return;
}


void
TDCSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("TDCSource","rotate");

  FixedComp::applyRotation(LR);  
  return;
}
  
void
TDCSource::createSource(SDef::Source&) const
  /*!
    Creates a simple beam sampled uniformly in a
    circle
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("TDCSource","createSource");
  return;
}  

void
TDCSource::createAll(const attachSystem::FixedComp& FC,
		       const long int linkIndex)
  /*!
    Create all with out using Control variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
  */
{
  ELog::RegMethod RegA("TDCSource","createAll(FC)");
  createUnitVector(FC,linkIndex);
  return;
}
  
void
TDCSource::createAll(const ITYPE& inputMap,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

 /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
   */
{
  ELog::RegMethod RegA("TDCSource","createAll<Map,FC,linkIndex>");
  populate(inputMap);
  createUnitVector(FC,linkIndex);

  return;
}

void
TDCSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("TDCSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;
}

void
TDCSource::writePHITS(std::ostream&) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("TDCSource","writePHITS");

  return;
}

void
TDCSource::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("TDCSource","writeFLUKA");

  std::ostringstream cx;

  cx.str("");
  cx<<"SOURCE ";
  
  cx<<" "<<energyMin<<" "<<energyMax<<" "<<radius<<" "<<length<<" - -";
  cx<<" TDC";
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"SOURCE ";  
  cx<<X<<" "<<Y;
  cx<<" &";
  StrFunc::writeFLUKA(cx.str(),OX);

  SourceBase::writeFLUKA(OX);
  return;
}

} // NAMESPACE SDef
