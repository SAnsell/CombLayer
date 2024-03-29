/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SurfNormSource.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include "SrcData.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "SurfNormSource.h"

namespace SDef
{

SurfNormSource::SurfNormSource(const std::string& K) :
  attachSystem::FixedRotate(K,0),SourceBase(),
  angleSpread(0.0),surfNum(0),
  width(0.0),height(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param K :: main keyname 
  */
{}

SurfNormSource::SurfNormSource(const SurfNormSource& A) : 
  attachSystem::FixedRotate(A),SourceBase(A),
  angleSpread(A.angleSpread),
  surfNum(A.surfNum),width(A.width),height(A.height)
  /*!
    Copy constructor
    \param A :: SurfNormSource to copy
  */
{}

SurfNormSource&
SurfNormSource::operator=(const SurfNormSource& A)
  /*!
    Assignment operator
    \param A :: SurfNormSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      SourceBase::operator=(A);
      angleSpread=A.angleSpread;
      surfNum=A.surfNum;
      height=A.height;
      width=A.width;
    }
  return *this;
}

SurfNormSource::~SurfNormSource() 
  /*!
    Destructor
  */
{}


SurfNormSource*
SurfNormSource::clone() const
  /*!
    Clone method
    \return new (*this)
  */
{
  return new SurfNormSource(*this);
}
  
void
SurfNormSource::populate(const mainSystem::MITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Map varaibles
   */
{
  ELog::RegMethod RegA("SurfNormSource","populate");

  attachSystem::FixedRotate::populate(inputMap);
  SourceBase::populate(inputMap);

  angleSpread=mainSystem::getDefInput<double>(inputMap,"aSpread",0,0.0);
  height=mainSystem::getDefInput<double>(inputMap,"height",0,0.0);
  width=mainSystem::getDefInput<double>(inputMap,"width",0,0.0);

  return;
}

void
SurfNormSource::setSurf(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Set the surface number. Also set centre of the system
    and determine the Z axis [to be done]
    \param FCPtr :: FixedComponent [can be zero]
    \param sideIndex :: surface index
  */
{
  ELog::RegMethod RegA("SurfNormSource","setSurf");

  surfNum=FC.getLinkSurf(sideIndex);
  return;
}
  
void
SurfNormSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("SurfNormSource","rotate");
  FixedComp::applyRotation(LR);  
  return;
}

  
void
SurfNormSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SurfNormSource","createSource");
  
  SourceBase::createEnergySource(sourceCard);
  sourceCard.setComp("sur",std::abs(surfNum));

  // Direction:

  if(angleSpread>Geometry::zeroTol)
    {
      SDef::SrcData D1(1);
      SDef::SrcInfo SI1;
      SI1.addData(-1.0);
      SI1.addData(cos(M_PI*angleSpread/180.0));
      SI1.addData(1.0);
      
      SDef::SrcProb SP1;
      SP1.addData(0.0);
      SP1.addData(0.0);
      SP1.addData(1.0);
      D1.addUnit(SI1);  
      D1.addUnit(SP1);  
      sourceCard.setData("dir",D1);
    }
  else
    {
      sourceCard.setComp("dir",1.0);
    }
      

  return;
}  

void
SurfNormSource::createAll(const mainSystem::MITYPE& inputMap,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)

  /*!
    Create all the source
    \param inputMap :: DataBase for variables
    \param FC :: FixedComp to get surface from
    \param sideIndex :: link point for surface
  */
{
  ELog::RegMethod RegA("SurfNormSource","createAll<inputMap,FC,linkIndex>");

  populate(inputMap);
  setSurf(FC,sideIndex);
  return;
}

void
SurfNormSource::createAll(const attachSystem::FixedComp& FC,
			  const long int sideIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: FixedComp to get surface from
    \param sideIndex :: link point for surface
  */
{
  ELog::RegMethod RegA("SurfNormSource","createAll<FC,linkIndex>");

  setSurf(FC,sideIndex);
  return;
}


void
SurfNormSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SurfNormSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;

}

void
SurfNormSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SurfNormSource","writePHITS");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}

void
SurfNormSource::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SurfNormSource","writeFLUKA");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}

} // NAMESPACE SDef
