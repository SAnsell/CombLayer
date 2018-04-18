/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/RectangleSource.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "doubleErr.h"
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
#include "inputSupport.h"
#include "SourceBase.h"
#include "particleConv.h"
#include "RectangleSource.h"

namespace SDef
{

RectangleSource::RectangleSource(const std::string& keyName) : 
  attachSystem::FixedOffset(keyName,0),
  SourceBase(),
  width(1.0),height(1.0),angleSpread(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

RectangleSource::RectangleSource(const RectangleSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  width(A.width),height(A.height),
  angleSpread(A.angleSpread)
  /*!
    Copy constructor
    \param A :: RectangleSource to copy
  */
{}

RectangleSource&
RectangleSource::operator=(const RectangleSource& A)
  /*!
    Assignment operator
    \param A :: RectangleSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      width=A.width;
      height=A.height;
      angleSpread=A.angleSpread;
    }
  return *this;
}

RectangleSource::~RectangleSource() 
  /*!
    Destructor
  */
{}

RectangleSource*
RectangleSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new RectangleSource(*this);
}
  
  
void
RectangleSource::populate(const mainSystem::MITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Control variables
   */
{
  ELog::RegMethod RegA("RectangleSource","populate");

  FixedOffset::populate(inputMap);
  SourceBase::populate(inputMap);
  
  mainSystem::findInput<double>(inputMap,"height",0,height);
  mainSystem::findInput<double>(inputMap,"width",0,width);
  mainSystem::findInput<double>(inputMap,"aSpread",0,angleSpread);
  
  return;
}

void
RectangleSource::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("RectangleSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();
  return;
}
  
void
RectangleSource::setRectangle(const double W,const double H)
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
RectangleSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("RectangleSource","rotate");
  FixedComp::applyRotation(LR);

  if (!X.masterDir() || !Y.masterDir() || !Z.masterDir() ||
      std::abs(std::abs(Origin.dotProd(Y))-Origin.abs())>Geometry::zeroTol)
    {
      SourceBase::createTransform(Origin,X,Y,Z);
    }
  else
    {
      delete TransPtr;
      TransPtr=0;
    }
  return;
}


void
RectangleSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a parabolic source
    - note that nWidth / nHeight are never 0
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("RectangleSource","createSource");

  const particleConv& pConv=particleConv::Instance();
  const int mcnpPIndex=pConv.mcnpITYP(particleType);

  sourceCard.setComp("par",mcnpPIndex);   // neutron (1)/photon(2)
  sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));         /// 
  
    // are we aligned on the master direction:
  const int aR=(TransPtr) ? 2 : std::abs(Y.masterDir());

  const std::string xyz[]={"x","y","z"};
  if (!TransPtr)
    {
      sourceCard.setComp(xyz[aR-1],Origin.dotProd(Y));
      sourceCard.setComp("vec",Y);
      sourceCard.setComp("axs",Y);
    }
  else
    {
      // transform moves source from 0,0,0 : axis = Y
      sourceCard.setComp("y",0.0);
      sourceCard.setComp("vec",Geometry::Vec3D(0,1.0,0));
      sourceCard.setComp("axs",Geometry::Vec3D(0,1.0,0));
    }
  
  const std::vector<double> XPts({-width/2.0,width/2.0});
  const std::vector<double> XProb({0.0,1.0});
  const std::vector<double> ZPts({-height/2.0,height/2.0});
  const std::vector<double> ZProb({0.0,1.0});
    
  SrcData D1(1);  
  SrcData D2(2);
  
  SrcInfo SI1('H');
  SrcInfo SI2('H');
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


  sourceCard.setData(xyz[(aR+1) % 3],D1); // equiv of +2/-1
  sourceCard.setData(xyz[aR % 3],D2);
  sourceCard.setComp("ara",height*width);

  if (TransPtr)
    sourceCard.setComp("tr",TransPtr->getName());
  
  SourceBase::createEnergySource(sourceCard);    
  
  return;
}

void
RectangleSource::createAll(const mainSystem::MITYPE& inputMap,
			   const attachSystem::FixedComp& FC,
			   const long int linkIndex)
  
  /*!
    Create all the source
    \param inputMap :: DataBase for variables
    \param FC :: FixedComp for origin
    \param linkIndex :: link point
   */
{
  ELog::RegMethod RegA("RectangleSource","createAll<FC,linkIndex>");
  populate(inputMap);
  createUnitVector(FC,linkIndex);

  return;
}

void
RectangleSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("RectangleSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  if (TransPtr)
    TransPtr->write(OX);
  return;

}

void
RectangleSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("RectangleSource","writePHITS");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  
  return;
}

void
RectangleSource::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("RectangleSource","writePHITS");

  // can be two for an energy range not more
  const size_t NE=Energy.size();
  if (NE!=1 || NE!=2)
    throw ColErr::SizeError<size_t>
      (NE,2,"Energy only single point or range [2 points]");

  std::ostringstream cx;
  // energy : energy divirgence : angle spread [mrad]
  // width : height : - means rectangel
  if (NE==1)
    cx<<"BEAM "<<-0.001*Energy[0]<<" 0.0 ";
  else
    {
      const double EMid=(Energy[1]+Energy[0])/2.0;
      const double ERange= Energy[1]-Energy[0];
      cx<<"BEAM "<<-0.001*EMid<<" "<<0.001*ERange;
    }
  cx<<M_PI*angleSpread/0.180<<" "<<width<<" "<<height<<" - ";
  cx<<StrFunc::toUpperString(particleType);
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");

  // Y Axis is Z in fluka, X is X
  cx<<"BEAMAXES "<<X<<" "<<Y;
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");
  cx<<"BEAMPOS "<<Origin;
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");

  
  return;
}

} // NAMESPACE SDef
