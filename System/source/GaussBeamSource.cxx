/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/GaussBeamSource.cxx
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
#include "GaussBeamSource.h"

namespace SDef
{

GaussBeamSource::GaussBeamSource(const std::string& keyName) : 
  FixedOffset(keyName,0),SourceBase(),
  xWidth(1.0),zWidth(1.0),angleSpread(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

GaussBeamSource::GaussBeamSource(const GaussBeamSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  xWidth(A.xWidth),zWidth(A.zWidth),
  angleSpread(A.angleSpread)
  /*!
    Copy constructor
    \param A :: GaussBeamSource to copy
  */
{}

GaussBeamSource&
GaussBeamSource::operator=(const GaussBeamSource& A)
  /*!
    Assignment operator
    \param A :: GaussBeamSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      xWidth=A.xWidth;
      zWidth=A.zWidth;
      angleSpread=A.angleSpread;
    }
  return *this;
}

GaussBeamSource::~GaussBeamSource() 
  /*!
    Destructor
  */
{}

GaussBeamSource*
GaussBeamSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new GaussBeamSource(*this);
}
  
void
GaussBeamSource::setSize(const double W,const double H)
  /*!
    Set the beam width/height
    \param W :: FWHM [horrizontal]
    \param H :: FWHM [vertical]
   */
{
  xWidth=W;
  zWidth=H;
  return;
}
  
void
GaussBeamSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("GaussBeamSource","populate");

  attachSystem::FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);
  
  xWidth=Control.EvalDefVar<double>(keyName+"XWidth",xWidth);
  zWidth=Control.EvalDefVar<double>(keyName+"ZWidth",zWidth);
  angleSpread=Control.EvalDefVar<double>(keyName+"ASpread",0.0); 
  return;
}

void
GaussBeamSource::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("GaussBeamSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();

  return;
}
  
void
GaussBeamSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GaussBeamSource","createSource");
  
  sourceCard.setActive();

  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("par",particleType);   // neutron (1)/photon(2)
  sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));         /// 
  sourceCard.setComp("pos",Origin);
  sourceCard.setComp("y",Origin.dotProd(Y));

  SrcData D1(1);
  SrcProb SP1(1);
  SP1.setFminus(-41,xWidth,0);
  D1.addUnit(SP1);

  SrcData D2(2);
  SrcProb SP2(1);
  SP2.setFminus(-41,zWidth,0);
  D2.addUnit(SP2);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  SourceBase::createEnergySource(sourceCard);

  return;
}  

void
GaussBeamSource::createAll(const FuncDataBase& Control,
			   const attachSystem::FixedComp& FC,
			   const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
    \param sourceCard :: Source Term
   */
{
  ELog::RegMethod RegA("GaussBeamSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);
  return;
}



void
GaussBeamSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("GaussBeamSource","write");

  Source sourceCard;
  sourceCard.setActive();
  createSource(sourceCard);
  sourceCard.write(OX);
  return;
}

void
GaussBeamSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    This is an approximate gaussian sauce b
    base no 100 x / 100 z units  
    - Rotation done by transform
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("GaussBeamSource","write");

  SourceBase::writePHITS(OX);
  

  
  return;
}

} // NAMESPACE SDef
