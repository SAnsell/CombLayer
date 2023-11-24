/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/MarkovProcess.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <list>
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
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "BasicMesh3D.h"
#include "BaseMap.h"

#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "WWGWeight.h"
#include "WWG.h"

#include "MarkovProcess.h"


namespace WeightSystem
{


MarkovProcess::MarkovProcess()
 /*! 
    Constructor 
  */
{}

MarkovProcess::MarkovProcess(const MarkovProcess& A) : 
  nIteration(A.nIteration),WX(A.WX),WY(A.WY),WZ(A.WZ),
  FSize(A.FSize),fluxField(A.fluxField)
  /*!
    Copy constructor
    \param A :: MarkovProcess to copy
  */
{}

MarkovProcess&
MarkovProcess::operator=(const MarkovProcess& A)
  /*!
    Assignment operator
    \param A :: MarkovProcess to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nIteration=A.nIteration;
      WX=A.WX;
      WY=A.WY;
      WZ=A.WZ;
      FSize=A.FSize;
      fluxField=A.fluxField;
    }
  return *this;
}

MarkovProcess::~MarkovProcess()
  /*!
    Destructor
  */
{}
  
void
MarkovProcess::initializeData(const WWG& wSet,
			      const std::string& meshIndex)
  /*!
    Initialize all the values before execusion
    \param wSet :: wwg
    \param meshIndex to process
  */
{
  ELog::RegMethod RegA("MarkovProcess","initialize");

  const WWGWeight& wMesh=wSet.getMesh(meshIndex);
  const Geometry::BasicMesh3D& grid=wMesh.getGeomGrid();
  
  WX=grid.getXSize();
  WY=grid.getYSize();
  WZ=grid.getZSize();

  FSize=WX*WY*WZ;
  
  fluxField.resize(FSize,FSize);
  
  return;
}

void
MarkovProcess::computeMatrix(const Simulation& System,
			     const WWG& wSet,
			     const std::string& meshIndex,
			     const double densityFactor,
			     const double r2Length,
			     const double r2Power)
  /*!
    Calculate the Markov chain process
    \param System :: Simualation
    \param wSet :: WWG set for grid
    \param meshIdnex :: mesh to use  	
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
   */
{
  ELog::RegMethod RegA("MarkovProcess","computeMatrix");

  const WWGWeight& wMesh=wSet.getMesh(meshIndex);
  const Geometry::BasicMesh3D& grid=wMesh.getGeomGrid();
  const std::vector<Geometry::Vec3D> midPts=grid.midPoints();

  if (midPts.size()!=FSize)
    throw ColErr::MisMatch<size_t>
      (midPts.size(),FSize,"MidPts.size != FSize");

  for(size_t i=0;i<FSize;i++)
    fluxField.get()[i][i]=1.0;

  for(size_t i=0;i<FSize;i++)
    {
      ModelSupport::ObjectTrackPoint OTrack(midPts[i]); 
      for(size_t j=i+1;j<FSize;j++)
	{
	  OTrack.addUnit(System,j,midPts[j]);
	  double DistT=OTrack.getDistance(static_cast<long int>(j))/r2Length;
	  if (DistT<1.0) DistT=1.0;
	  const double AT=OTrack.getAttnSum(static_cast<long int>(j));  // this can take an
	  const double WFactor= -densityFactor*AT-r2Power*log(DistT);
	  if (WFactor>-20)
	    {
	      fluxField.get()[i][j]= -WFactor;
	      fluxField.get()[j][i]= -WFactor;
	    }
	  else
	    {
	      fluxField.get()[i][j]= 0.0;
	      fluxField.get()[j][i]= 0.0;
	    }
	}
    }
  return;
}

void
MarkovProcess::multiplyOut(const size_t index)
{
  return;
}

void
MarkovProcess::rePopulateWWG() 
{
  return;
}
  
} // namespace WeightSystem
