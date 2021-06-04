/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/BeamSource.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "inputSupport.h"
#include "Source.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "phitsWriteSupport.h"
#include "flukaGenParticle.h"

#include "SourceBase.h"
#include "BeamSource.h"

namespace SDef
{

BeamSource::BeamSource(const std::string& keyName) : 
  FixedRotateUnit(keyName,0),SourceBase(),
  radius(1.0),angleSpread(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

BeamSource::BeamSource(const BeamSource& A) : 
  attachSystem::FixedRotateUnit(A),SourceBase(A),
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
      attachSystem::FixedRotate::operator=(A);
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

  attachSystem::FixedRotate::populate(inputMap);
  SourceBase::populate(inputMap);
  // default neutron
  
  angleSpread=mainSystem::getDefInput(inputMap,"aSpread",0,0.0);
  radius=mainSystem::getDefInput(inputMap,"radius",0,radius);

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

  ELog::EM<<"linkunit: == "<<FC.getKeyName()<<" "<<linkIndex<<ELog::endDiag;

  populate(inputMap);
  createUnitVector(FC,linkIndex);

  return;
}

void
BeamSource::createAll(const ITYPE& inputMap,
		      const Geometry::Vec3D& Org,
		      const Geometry::Vec3D& Axis,
		      const Geometry::Vec3D& ZAxis)
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param Org :: Origin
    \param Axis :: Axis
    \param ZAxis :: ZAxis
   */
{
  ELog::RegMethod RegA("BeamSource","createAll<Vec3D>");

  populate(inputMap);
  FixedComp::createUnitVector(Org,Axis,ZAxis);

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

  const double phi=180.0*acos(Y[0])/M_PI;

  StrFunc::writePHITS(OX,1,"s-type",1,"axial source");
  StrFunc::writePHITS(OX,2,"r0",radius,"radius [cm]");
  StrFunc::writePHITS(OX,2,"x0",Origin[0],"center position of x-axis [cm]");
  StrFunc::writePHITS(OX,2,"y0",Origin[1],"center position of y-axis [cm]");
  StrFunc::writePHITS(OX,2,"z0",Origin[2],"min position of z-axis [cm]");
  StrFunc::writePHITS(OX,2,"z1",Origin[2],"max position of z-axis [cm]");
  StrFunc::writePHITS(OX,2,"dir",Y[2],"Dir cosine direction of Z");
  StrFunc::writePHITS(OX,2,"phi",phi,"Phi angle to X axis [deg]");

  if (angleSpread>Geometry::zeroTol)
    StrFunc::writePHITS(OX,2,"dom",angleSpread,"Solid angle fo X axis [deg]");

  SourceBase::writePHITS(OX);
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

  const flukaGenParticle& PC=flukaGenParticle::Instance();
  masterWrite& MW=masterWrite::Instance();
  
  // can be two for an energy range
  if (Energy.size()!=1)
    throw ColErr::SizeError<size_t>
      (Energy.size(),1,"Energy only single point supported");

  std::ostringstream cx;
  // energy : energy divergence : angle spread [mrad]
  // radius : innerRadius : -1 t o means radius
  const double scaleValue=
    (PC.mass(particleType)<Geometry::zeroTol) ? 0.001 : -0.001;
  cx<<"BEAM "<<scaleValue*Energy.front()<<" 0.0 "<<M_PI*angleSpread/0.180
    <<" "<<radius<<" 0.0 -1.0 ";
  cx<<StrFunc::toUpperString(PC.nameToFLUKA(particleType));
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");

  //Y Axis is Z in fluka, X is X
  // ELog::EM<<"Beam Direction[X] == "<<X<<ELog::endDiag;
  // ELog::EM<<"Beam Direction[Y] == "<<Y<<ELog::endDiag;
  // ELog::EM<<"Beam Direction[X*Y] == "<<X*Y<<ELog::endDiag;
  cx<<"BEAMAXES "<<MW.Num(X)<<" "<<MW.Num(Y);
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");

  // Note the cos directs fro the beamPos are for particle
  // leaving the beam NOT the orientation of the disk
  cx<<"BEAMPOS "<<MW.Num(Origin);

    
  StrFunc::writeFLUKA(cx.str(),OX);

  SourceBase::writeFLUKA(OX);
  return;
}
  
} // NAMESPACE SDef
