/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ObjectTrackAct.cxx
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
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
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
  const std::vector<MonteCarlo::Object*>& ObjVec= mc->second.getObjVec();
  const std::vector<double>& TVec= mc->second.getSegmentLen();

  double sum(0.0);
  for(size_t i=0;i<TVec.size();i++)
    {
      const MonteCarlo::Object* OPtr=ObjVec[i];
      const MonteCarlo::Material* MPtr=OPtr->getMatPtr();
      if (!MPtr->isVoid())
	sum+=TVec[i];
    }
  return sum;
}
  
double
ObjectTrackAct::getAttnSum(const long int objN,
			   const double energy) const
  /*!
    Calculate the attenuatio of a beam traveling in teh object
    \param objN :: Cell number to use
    \return sum of distance in non-void
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","getAttnSum");


  std::map<long int,LineTrack>::const_iterator mc=Items.find(objN);
  if (mc==Items.end())
    throw ColErr::InContainerError<long int>(objN,"objN in Items");


  // Get Two Paired Vectors
  const std::vector<MonteCarlo::Object*>& ObjVec= mc->second.getObjVec();
  const std::vector<double>& TVec=mc->second.getSegmentLen();

  double sum(0.0);
  for(size_t i=0;i<ObjVec.size();i++)
    {
      const MonteCarlo::Object* OPtr=ObjVec[i];
      const MonteCarlo::Material* MPtr=OPtr->getMatPtr();
      if (!MPtr->isVoid())
	{
	  const double density=MPtr->getAtomDensity();
	  const double AMean=MPtr->getMeanA();
	  sum+=TVec[i]*std::pow(AMean,0.66)*density;
	}
    }
  // currently no use for epsilon
  return sum;
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
    \param cellN :: cell numbers [why long int?]
    \param attnD :: distances in cell
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","createAttenPath");
  for(const std::map<long int,LineTrack>::value_type& mc : Items)
    mc.second.createAttenPath(cellN,attnD);
  return;
}

void
ObjectTrackAct::createMatPath(std::vector<int>& matN,
			      std::vector<double>& attnD) const
  /*!
    Calculate the sum of the distance
    \param matN :: material number
    \param attnD :: distances in cell
  */
{
  ELog::RegMethod RegA("ObjectTrackAct","createMatPath");
  
  for(const std::map<long int,LineTrack>::value_type& mc : Items)
    mc.second.createMatPath(matN,attnD);
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
