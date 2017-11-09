/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/PointPointDetector.cxx
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

#include "MersenneTwister.h"
#include "RefCon.h"
#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "neutron.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Detector.h"
#include "PointDetector.h"


namespace Transport
{

PointDetector::PointDetector(const size_t ID) : 
  Detector(ID),angle(0.0),Cent(Geometry::Vec3D(0,0,0))
  /*!
    Default constructor
    \param ID :: Idenitification number
  */
{}

PointDetector::PointDetector(const size_t ID,const Geometry::Vec3D& CV) : 
  Detector(ID),angle(0.0),Cent(CV)
 /*!
   Constructor 
   \param ID :: Idenitification number
   \param CV :: Centre Point (middle)
  */
{}

PointDetector::PointDetector(const PointDetector& A) : 
  Detector(A),angle(A.angle),
  Cent(A.Cent),cnt(A.cnt)
  /*!
    Copy constructor
    \param A :: PointDetector to copy
  */
{}

PointDetector&
PointDetector::operator=(const PointDetector& A)
  /*!
    Assignment operator
    \param A :: PointDetector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Detector::operator=(A);
      angle=A.angle;
      Cent=A.Cent;
      cnt=A.cnt;
    }
  return *this;
}

PointDetector::~PointDetector()
  /*!
    Destructor
  */
{}

void
PointDetector::clear()
  /*!
    Clear all the array
  */
{
  nps=0;
  cnt.clear();
  return;
}
		       
void
PointDetector::setCentre(const Geometry::Vec3D& Cp)
  /*!
    Set the detector Positon
    \param Cp :: Plane centre
   */
{
  Cent=Cp;
  return;
}

void
PointDetector::normalize(const size_t TN) 
  /*!
    Normallize the detectors to incident particles
    \param TN :: Incident particle
  */
{
  const double DN(static_cast<double>(TN));
  for(std::pair<const int,double>& MItem  : cnt)
    MItem.second/=DN;
  return;
}

void
PointDetector::addEvent(const MonteCarlo::neutron& N) 
  /*!
    Add a point to the detector
    Tracks from the point to the detector.
    Added correction for solid angle
    \param N :: Neutron
  */
{
  ELog::RegMethod RegA("PointDetector","addEvent");
  
  if (N.OPtr)
    {
      const int matNum=(N.OPtr) ? N.OPtr->getMat() : -1;
      const double W=N.weight/(N.travel*N.travel);   // r^2 term +
                                                     // weight
      
      std::map<int,double>::iterator mc=cnt.find(matNum);
      if (mc==cnt.end())
	cnt.emplace(matNum,W);
      else
        mc->second += W;
    }
  nps++;
  return;
}

double
PointDetector::project(const MonteCarlo::neutron& Nin,
		       MonteCarlo::neutron& Nout) const
  /*!
    Project the neutron into the detector.
    The output neutron is copied from Nin and then
    pointed to the detector point 
    \param Nin :: Original neutron
    \param Nout :: secondary neutron
    \return distance moved by neutron
  */
{
  ELog::RegMethod RegA("PointDetector","project");

  Nout=Nin;
  Nout.uVec=(Cent-Nin.Pos).unit();
  Nout.travel=0.0;
  Nout.addCollision();
  return Cent.Distance(Nin.Pos);
}

void
PointDetector::writeHeader(std::ostream& OX) const
  /*!
    Write out a header
    \param OX :: Output stream
  */
{
  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  OX<<"# ";
  for(const std::pair<int,double>& MItem  : cnt)
    OX<<DB.getKey(MItem.first)<<" ";
  OX<<std::endl;
  OX<<"# NumberPts == "<<nps<<std::endl;
  ELog::EM<<"Number of points == "<<nps<<ELog::endDiag;
  return;
}

void
PointDetector::write(std::ostream& OX) const
  /*!
    Write the detector to a stream
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("PointDetector","write(stream,double)");
  
  OX<<index<<" "<<angle<<" ";
  for(const std::pair<int,double>& MItem  : cnt)
    OX<<MItem.second<<" ";
  OX<<std::endl;
  return;
}

} // NAMESPACE Transport


  
