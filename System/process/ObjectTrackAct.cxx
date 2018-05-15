/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ObjectTrackAct.cxx
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
#include <memory>

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
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "BnId.h"
#include "Rules.h"
#include "neutron.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "Simulation.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"

namespace ModelSupport
{

std::ostream&
operator<<(std::ostream& OX,const ObjectTrackAct& A) 
 /*!
   Write out object
   \param OX :: Output stream
   \param A :: ObjectTrackAct object to write
   \return Output stream
  */
{
  A.write(OX);
  return OX;
}

ObjectTrackAct::ObjectTrackAct() 
  /*! 
    Constructor 
  */
{}

ObjectTrackAct::ObjectTrackAct(const ObjectTrackAct& A) :
  Items(A.Items)
   /*! 
    Copy Constructor 
    \param A :: ObjectTrackAct to copy
  */
{}


ObjectTrackAct&
ObjectTrackAct::operator=(const ObjectTrackAct& A) 
   /*! 
     Assignment operator
     \param A :: ObjectTrackAct to copy
     \return *this
   */
{
  if (this!=&A)
    {
      Items=A.Items;
    }
  return *this;
}

void
ObjectTrackAct::clearAll()
  /*!
    Clears all the data
  */
{
  Items.erase(Items.begin(),Items.end());
  return;
}


double
ObjectTrackAct::getMatSum(const long int objN) const
  /*!
    Calculate the sum in the material
    \param objN :: Cell number to use
    \return sum of distance in non-void
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","getMatSum");

  std::map<long int,LineTrack>::const_iterator mc=Items.find(objN);
  if (mc==Items.end())
    throw ColErr::InContainerError<long int>(objN,"objN in Items");
  
  // Get Two Paired Vectors
  const std::vector<MonteCarlo::Object*>& OVec=
    mc->second.getObjVec();
  const std::vector<double>& TVec=
    mc->second.getTrack();
  double sum(0.0);
  for(size_t i=0;i<TVec.size();i++)
    if (OVec[i]->getMat()!=0)
      sum+=TVec[i];
  return sum;
}
  
double
ObjectTrackAct::getAttnSum(const long int objN) const
  /*!
    Calculate the sum in the material
    \param objN :: Cell number to use
    \return sum of distance in non-void
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","getAttnSum");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  std::map<long int,LineTrack>::const_iterator mc=Items.find(objN);
  if (mc==Items.end())
    throw ColErr::InContainerError<long int>(objN,"objN in Items");
  
  // Get Two Paired Vectors
  const std::vector<MonteCarlo::Object*>& OVec=
    mc->second.getObjVec();
  const std::vector<double>& TVec=mc->second.getTrack();
  
  double sum(0.0);
  for(size_t i=0;i<TVec.size();i++)
    {
      const long int matN=OVec[i]->getMat();
      if (matN)
	{
	  const MonteCarlo::Material& matInfo=
	    DB.getMaterial(static_cast<int>(matN));
	  const double density=matInfo.getAtomDensity();
	  const double AMean=matInfo.getMeanA();
	  sum+=TVec[i]*std::pow(AMean,0.66)*density;
	}
    }
  return sum;
}

double
ObjectTrackAct::getAttnSum(const long int objN,const double E) const
  /*!
    Calculate the sum in the material
    \param objN :: Cell number to use
    \return sum of distance in non-void
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","getAttnSum(E)");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  std::map<long int,LineTrack>::const_iterator mc=Items.find(objN);
  if (mc==Items.end())
    throw ColErr::InContainerError<long int>(objN,"objN in Items");
  
  // Get Two Paired Vectors
  const std::vector<MonteCarlo::Object*>& OVec=
    mc->second.getObjVec();
  const std::vector<double>& TVec=mc->second.getTrack();
  
  double sum(0.0);
  for(size_t i=0;i<TVec.size();i++)
    {
      const long int matN=OVec[i]->getMat();
      if (matN)
	{
	  const MonteCarlo::Material& matInfo=
	    DB.getMaterial(static_cast<int>(matN));
	  const double density=matInfo.getAtomDensity();
	  const double AMean=matInfo.getMeanA();
	  sum+=TVec[i]*std::pow(AMean,0.66)*density;
	}
    }
  return sum/E;
}

double
ObjectTrackAct::getDistance(const long int objN) const
  /*!
    Calculate the sum of the distance
    \param objN :: Cell number to use
    \return sum of distance in non-void
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","getDistance");

  std::map<long int,LineTrack>::const_iterator mc=Items.find(objN);
  if (mc==Items.end())
    throw ColErr::InContainerError<long int>(objN,"objN in Items");
  return mc->second.getTotalDist();
}

void
ObjectTrackAct::createAttenPath(std::vector<long int>& cellN,
				std::vector<double>& attnD) const
  /*!
    Calculate the sum of the distance
    \
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","createAttenPath");
  for(const std::map<long int,LineTrack>::value_type& mc : Items)
    mc.second.createAttenPath(cellN,attnD);
  return;
}

void 
ObjectTrackAct::write(std::ostream& OX) const
  /*!
    Write out track (mainly debug)
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("ObjectTrackAct","write");

  OX<<"WRITE"<<std::endl;
  std::map<long int,LineTrack>::const_iterator mc;
  for(mc=Items.begin();mc!=Items.end();mc++)
    OX<<mc->first<<" : "<<mc->second<<std::endl;
  return;
}
  
} // Namespace ModelSupport
