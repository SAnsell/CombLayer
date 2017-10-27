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
  SourceBase()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

ParabolicSource::ParabolicSource(const ParabolicSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
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
  
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  
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
ParabolicSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("ParabolicSource","createSource");

  sourceCard.setActive();
  SourceBase::createEnergySource(sourceCard);    

  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Y);
  sourceCard.setComp("y",Origin.dotProd(Y));

  const double xRange=width/2.0;
  const double step(0.5);  
  std::vector<double> XPts;
  std::vector<double> XProb;
  double XValue= -xRange-step;
  do
    {
      XValue+=step;
      XPts.push_back(XValue);
      XProb.push_back(1.0-(XValue*XValue)/(xRange*xRange));
    } while (XValue<xRange);

  const double zRange=height/2.0;
  std::vector<double> ZPts;
  std::vector<double> ZProb;
  double ZValue= -zRange-step;
  do
    {
      ZValue+=step;
      ZPts.push_back(ZValue);
      ZProb.push_back(1.0-(ZValue*ZValue)/(zRange*zRange));
    } while (ZValue<zRange);
    
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
ParabolicSource::createAll(const attachSystem::FixedComp& FC,
			   const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: FixedComp for origin
    \param linkIndex :: link point
   */
{
  ELog::RegMethod RegA("ParabolicSource","createAll<FC,linkIndex>");
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
