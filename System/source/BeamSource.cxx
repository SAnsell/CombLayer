/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/BeamSource.cxx
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
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "varList.h"
#include "inputSupport.h"
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
#include "BeamSource.h"

namespace SDef
{

BeamSource::BeamSource(const std::string& keyName) : 
  FixedOffset(keyName,0),SourceBase(),
  radius(1.0),angleSpread(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

BeamSource::BeamSource(const BeamSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  radius(A.radius),angleSpread(A.angleSpread)
  /*!
    Copy constructor
    \param A :: BeamSource to copy
  */
{}

BeamSource&
BeamSource::operator=(const BeamSource& A)
  /*!
    Assignment operator
    \param A :: BeamSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      radius=A.radius;
      angleSpread=A.angleSpread;
    }
  return *this;
}

BeamSource::~BeamSource() 
  /*!
    Destructor
  */
{}

BeamSource*
BeamSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new BeamSource(*this);
}
  
  
  
void
BeamSource::populate(const ITYPE& inputMap)
  /*!
    Populate Varaibles
    \param inputMap :: Control variables
   */
{
  ELog::RegMethod RegA("BeamSource","populate");

  attachSystem::FixedOffset::populate(inputMap);
  SourceBase::populate(inputMap);
  // default neutron
  
  angleSpread=mainSystem::getDefInput(inputMap,"aSpread",0,0.0);
  radius=mainSystem::getDefInput(inputMap,"radius",0,radius);

  return;
}

void
BeamSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Component
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("BeamSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();
  return;
}

void
BeamSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("BeamSource","rotate");
  FixedComp::applyRotation(LR);  
  return;
}
  
void
BeamSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a simple beam sampled uniformly in a
    circle
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("BeamSource","createSource");

  sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));
  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("ara",M_PI*radius*radius);         
    
  sourceCard.setComp("pos",Origin);
  
  // RAD
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1;
  SDef::SrcProb SP1;
  SI1.addData(0.0);
  SI1.addData(radius);
  SP1.setFminus(-21,1.0);
  D1.addUnit(SI1);
  D1.addUnit(SP1);
  sourceCard.setData("rad",D1);

  SourceBase::createEnergySource(sourceCard);

  return;
}  

void
BeamSource::createAll(const attachSystem::FixedComp& FC,
		      const long int linkIndex)
  /*!
    Create all with out using Control variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
  */
{
  ELog::RegMethod RegA("BeamSource","createAll(FC)");
  createUnitVector(FC,linkIndex);
  return;
}

  
void
BeamSource::createAll(const ITYPE& inputMap,
		      const attachSystem::FixedComp& FC,
		      const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: Fixed Point for origin/axis of beam
    \param linkIndex :: link Index				
   */
{
  ELog::RegMethod RegA("BeamSource","createAll<FC,linkIndex>");
  populate(inputMap);
  createUnitVector(FC,linkIndex);

  return;
}

void
BeamSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("BeamSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;
}

void
BeamSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("BeamSource","writePHITS");

  boost::format fFMT("%1$11.6g%|14t|");

  const double phi=180.0*acos(Y[0])/M_PI;
  
  OX<<"  s-type =  1        # axial source \n";
  OX<<"  r0 =   "<<(fFMT % radius)   <<"   # radius [cm]\n";
  OX<<"  x0 =   "<<(fFMT % Origin[0])<<"  #  center position of x-axis [cm]\n";
  OX<<"  y0 =   "<<(fFMT % Origin[1])<<"  #  center position of y-axis [cm]\n";
  OX<<"  z0 =   "<<(fFMT % Origin[2])<<"  #  mininium of z-axis [cm]\n";
  OX<<"  z1 =   "<<(fFMT % Origin[2])<<"  #  maximum of z-axis [cm]\n";
  OX<<" dir =   "<<(fFMT % Y[2])     <<" dir cosine direction of Z\n";
  OX<<" phi =   "<<(fFMT % phi)      <<" phi angle to X axis [deg]\n";
  if (angleSpread>Geometry::zeroTol)
    OX<<" dom =   "<<(fFMT % angleSpread)<<" solid angle to X axis [deg]\n";

  OX<<std::endl;
  return;
}

void
BeamSource::writeFLUKA(std::ostream& OX) const
  /*!
    Write out as a FLUKA source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("BeamSource","writeFLUKA");

  const particleConv& PC=particleConv::Instance();

  // can be two for an energy range
  if (Energy.size()!=1)
    throw ColErr::SizeError<size_t>
      (Energy.size(),1,"Energy only single point supported");

  std::ostringstream cx;
  // energy : energy divirgence : angle spread [mrad]
  // radius : innerRadius : -1 t o means radius
  cx<<"BEAM "<<-0.001*Energy.front()<<" 0.0 "<<M_PI*angleSpread/0.180
    <<" "<<radius<<" 0.0 -1.0 ";
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
