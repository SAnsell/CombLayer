/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/PointSource.cxx
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
#include <memory>

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

#include "SourceBase.h"
#include "PointSource.h"

namespace SDef
{

PointSource::PointSource(const std::string& keyName) : 
  FixedOffset(keyName,0),SourceBase(),
  angleSpread(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

PointSource::PointSource(const PointSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
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
  return new PointSource(*this);
}

  
PointSource::~PointSource() 
  /*!
    Destructor
  */
{}

void
PointSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("PointSource","populate");

  attachSystem::FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);

  angleSpread=Control.EvalDefVar<double>(keyName+"ASpread",0.0); 

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

  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));   
  sourceCard.setComp("pos",Origin);
  SourceBase::createEnergySource(sourceCard);

  return;
}  

void
PointSource::createAll(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed point to get orientation from
    \param linkIndex :: link Index						
   */
{
  ELog::RegMethod RegA("PointSource","createAll<Control,FC,linkIndex>");
  populate(Control);
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
  ELog::RegMethod RegA("PointSource","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}


} // NAMESPACE SDef
