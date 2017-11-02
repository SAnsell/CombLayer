/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ObjTrackItem.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "ObjTrackItem.h"


namespace ModelSupport
{

std::ostream&
operator<<(std::ostream& OX,const ObjTrackItem& A)
  /*!
    Write out to a stream
    \param OX :: Output stream
    \param A :: ObjTrackItem to write
    \return Stream
  */
{
  A.write(OX);
  return OX;
}

ObjTrackItem::ObjTrackItem(const Geometry::Vec3D& IP,
			   const Geometry::Vec3D& EP) :
  InitPt(IP),EndPt(EP),aimDist((EP-IP).abs()),
  objName(0),TDist(0.0),impDist(0.0)
  /*! 
    Constructor 
    \param IP :: Initial point
    \param EP :: End point
  */
{}

ObjTrackItem::ObjTrackItem(const ObjTrackItem& A) :
  InitPt(A.InitPt),EndPt(A.EndPt),aimDist(A.aimDist),
  objName(A.objName),TDist(A.TDist),impDist(A.impDist),
  MTrack(A.MTrack)
  /*! 
    Copy Constructor 
    \param A :: ObjTrackItem to copy
  */
{}

void
ObjTrackItem::clearAll()
  /*!
    Clears all the data
  */
{
  TDist=0.0;
  impDist=0.0;
  MTrack.erase(MTrack.begin(),MTrack.end());
  return;
}

void
ObjTrackItem::addDistance(const int MatN,const double D) 
  /*!
    Adds a distance to the material track / creates
    a new material if one doesn't exist
    \param MatN :: Material Number 
    \param D :: Distance in material
    \return 1 if the aimPoint has been exceeded / 0 otherwise
   */
{
  ELog::RegMethod RegA("ObjTrackItem","addDistance");

  
  typedef std::map<int,double> MTYPE;
  std::pair<MTYPE::iterator,bool> Res=
    MTrack.emplace(MatN,0.0);
  MTYPE::iterator mc=Res.first;
  mc->second+=D;
  TDist+=D;
  return;
}

double
ObjTrackItem::getMatSum() const 
  /*!
    Determine the track sum : simple non-void sum
    \return sum of lenghts
   */
{
  double sum(0.0);
  std::map<int,double>::const_iterator mc;
  for(mc=MTrack.begin();mc!=MTrack.end();mc++)
    if (mc->first)
      sum+=mc->second;
  return sum;
}
  
void
ObjTrackItem::write(std::ostream& OX) const
  /*!
    Write out the track
    \param OX :: Output stream
  */
{
  boost::format FMT("%1$d %2$5.2e ");

  OX<<"Pts == "<<InitPt<<"::"<<EndPt<<std::endl;

  std::map<int,double>::const_iterator mc;
  for(mc=MTrack.begin();mc!=MTrack.end();mc++)
    OX<<(FMT % mc->first % mc->second);

  OX<<std::endl;
  return;
}

  
} // Namespace ModelSupportf
