/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/ParabolicSource.cxx
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
#include "ParabolicSource.h"

namespace SDef
{

ParabolicSource::ParabolicSource(const std::string& keyName) : 
  attachSystem::FixedOffset(keyName,0),
  SourceBase(),decayPower(2.0),
  nWidth(5),nHeight(5),
  width(1.0),height(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

ParabolicSource::ParabolicSource(const ParabolicSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  decayPower(A.decayPower),
  nWidth(A.nWidth),nHeight(A.nHeight),
  width(A.width),height(A.height)
  /*!
    Copy constructor
    \param A :: ParabolicSource to copy
  */
{}

ParabolicSource&
ParabolicSource::operator=(const ParabolicSource& A)
  /*!
    Assignment operator
    \param A :: ParabolicSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      decayPower=A.decayPower;
      nWidth=A.nWidth;
      nHeight=A.nHeight;
      width=A.width;
      height=A.height;
    }
  return *this;
}

ParabolicSource::~ParabolicSource() 
  /*!
    Destructor
  */
{}

ParabolicSource*
ParabolicSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new ParabolicSource(*this);
}
  
  
void
ParabolicSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("ParabolicSource","populate");

  FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);
  
  decayPower=Control.EvalDefVar<double>(keyName+"DecayPower",decayPower);
  height=Control.EvalDefVar<double>(keyName+"Height",height);
  width=Control.EvalDefVar<double>(keyName+"Width",width);
  
  return;
}

void
ParabolicSource::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("ParabolicSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();
  return;
}
  
void
ParabolicSource::setRectangle(const double W,const double H)
  /*!
    Set the width/height
    \param W :: Width (full)
    \param H :: Height (full)
   */
{
  width=W;
  height=H;
  return;
}

void
ParabolicSource::setNPts(const size_t NW,const size_t NH)
  /*!
    Set the width / height number of points
    \param NW :: number of point in width
    \param NH :: number of point in height
   */
{
  nWidth=(NW) ? NW : 1;
  nHeight=(NH) ? NH : 1;
  return;
}
  
void
ParabolicSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a parabolic source
    - note that nWidth / nHeight are never 0
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("ParabolicSource","createSource");

  sourceCard.setActive();
  SourceBase::createEnergySource(sourceCard);    

  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Y);
  sourceCard.setComp("y",Origin.dotProd(Y));

  std::vector<double> XPts(nWidth+1);
  std::vector<double> XProb(nWidth+1);
  const double xRange=width/2.0;
  const double xPower(std::pow(xRange,decayPower));
  const double xStep=(2.0*xRange)/static_cast<double>(nWidth);
  
  double XValue= -xRange;
  double pValue(0.0);
  for(size_t i=0;i<=nWidth;i++)
    {
      XPts[i]  = XValue;
      XProb[i] = pValue;
      XValue += xStep/2.0;
      pValue = (std::abs(decayPower)>Geometry::zeroTol) ?
	1.0-std::pow(std::abs<double>(XValue),decayPower)/xPower : 1.0;
      XValue += xStep/2.0;
    }

  std::vector<double> ZPts(nHeight+1);
  std::vector<double> ZProb(nHeight+1);
  const double zRange=height/2.0;
  const double zPower(std::pow(zRange,decayPower));
  const double zStep=(2.0*zRange)/static_cast<double>(nHeight);
  
  double ZValue= -zRange;
  pValue=0.0;
  for(size_t i=0;i<=nHeight;i++)
    {
      ZPts[i]  = ZValue;
      ZProb[i] = pValue;
      ZValue += zStep/2.0;
      pValue = (std::abs(decayPower)>Geometry::zeroTol) ?
	1.0-std::pow(std::abs<double>(ZValue),decayPower)/zPower : 1.0;

      ZValue += zStep/2.0;
    }
    
  SrcData D1(1);  
  SrcData D2(2);
  
  SrcInfo SI1('A');
  SrcInfo SI2('A');
  SI1.setData(XPts);
  SI2.setData(ZPts);

  SrcProb SP1;
  SrcProb SP2;
  SP1.setData(XProb);
  SP2.setData(ZProb);

  D1.addUnit(SI1);
  D2.addUnit(SI2);
  D1.addUnit(SP1);
  D2.addUnit(SP2);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  sourceCard.setComp("ara",4.0*xRange*zRange);
  
  return;
}

void
ParabolicSource::createAll(const FuncDataBase& Control,
			   const attachSystem::FixedComp& FC,
			   const long int linkIndex)
  
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: FixedComp for origin
    \param linkIndex :: link point
   */
{
  ELog::RegMethod RegA("ParabolicSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);

  return;
}

void
ParabolicSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("ParabolicSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;

}

void
ParabolicSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("ParabolicSource","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}

} // NAMESPACE SDef
