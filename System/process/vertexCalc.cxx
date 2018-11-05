/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ModelSupport.cxx
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
#include "Quaternion.h"
#include "OutputLog.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "SurfVertex.h"
#include "Line.h"
#include "SurInter.h"
#include "vertexCalc.h"

namespace ModelSupport
{


int
getIntersect(const MonteCarlo::Object& obj,
	     const Geometry::Surface* SurfX,
	     const Geometry::Surface* SurfY,
	     const Geometry::Surface* SurfZ,
	     std::vector<MonteCarlo::SurfVertex>& VList)
  /*!
    Calculates the intersection between three surfaces.
    Adds the point to the vertex list if the point is valid 
    and is on a side.
    
    \param SurfX :: Surface pointer
    \param SurfY :: Surface pointer
    \param SurfZ :: Surface pointer
    \param VList :: Vector to add vertex to
    \returns Number intersections found
  */
{
  ELog::RegMethod RegA("vertexCacl","getIntersect");

  const int AS=SurfX->getName();
  const int BS=SurfY->getName();
  const int CS=SurfZ->getName();

  std::vector<Geometry::Vec3D> PntOut=
    SurInter::processPoint(SurfX,SurfY,SurfZ);

  int Ncnt(0);
  std::set<int> ExSN={AS,BS,CS};
	
  for(const Geometry::Vec3D& VC : PntOut)
    {
      if (obj.isValid(VC,ExSN) &&         // Is point in/on the object
	  VC.abs()<1e8)          // Tracked onto the very large points
	{
	  MonteCarlo::SurfVertex tmp;
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfX));
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfY));
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfZ));
	  tmp.setPoint(VC);
	  VList.emplace_back(tmp);
	  Ncnt++;
	}
    }      
  return Ncnt;
}
  
std::vector<MonteCarlo::SurfVertex>
calcVertex(const MonteCarlo::Object& obj)
  /*!
    Calculate the vertex of an object
    \param obj :: Object
    \return vector of vertex [surf + point]
   */
{
  ELog::RegMethod RegA("vertexCalc[F]","calcVertex");

  std::vector<MonteCarlo::SurfVertex> VList;
  const std::vector<const Geometry::Surface*>& surfList=
    obj.getSurList();

  std::vector<const Geometry::Surface*>::const_iterator ic,jc,kc;
  for(ic=surfList.begin();ic!=surfList.end();ic++)
    for(jc=ic+1;jc!=surfList.end();jc++)
      for(kc=jc+1;kc!=surfList.end();kc++)
        {
	  // This adds intersections to the VList
	  getIntersect(obj,*ic,*jc,*kc,VList);
	}
  // return number of item found
  return VList;
}

std::vector<Geometry::Vec3D>
calcVertexPoints(const MonteCarlo::Object& obj)
  /*!
    Calculate the vertex of an object
    \param obj :: Object
    \return vector of vertex points
   */
{
  ELog::RegMethod RegA("vertexCalc[F]","calcVertexPoints");

  std::vector<MonteCarlo::SurfVertex> VList=
    calcVertex(obj);

  std::vector<Geometry::Vec3D> Out;
  for(const MonteCarlo::SurfVertex& SV : VList)
    Out.emplace_back(SV.getPoint());

  
  return Out;
} 

Geometry::Vec3D
calcCOFM(const MonteCarlo::Object& obj)
  /*!
    Calculate the Centre of Mass (based on vertex
    intersection)
    \param obj :: object
    \return Vec3D of CofM [0,0,0] in failed]
  */
{
  ELog::RegMethod RegA("vertexCalc[F]","calcVertexPoints");
  const std::vector<Geometry::Vec3D> CVec=
    calcVertexPoints(obj);

  Geometry::Vec3D CPoint;

  if (!CVec.empty())
    {
      for(const Geometry::Vec3D& Pt : CVec)
	CPoint+=Pt;
      CPoint /= static_cast<double>(CVec.size());
    }
  return CPoint;
}

}  // NAMESPACE ModelSupport
