/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/MarkovProcess.cxx
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
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Mesh3D.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"

#include "Simulation.h"

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
MarkovProcess::initializeData(const WWG& wSet)
  /*!
    Initialize all the values before execusion
    \param wSet :: wwg
  */
{
  ELog::RegMethod RegA("MarkovProcess","initialize");

  const Geometry::Mesh3D& grid=wSet.getGrid();
  
  WX=static_cast<long int>(grid.getXSize());
  WY=static_cast<long int>(grid.getYSize());
  WZ=static_cast<long int>(grid.getZSize());

  FSize=WX*WY*WZ;
  
  fluxField.resize(boost::extents[FSize][FSize]);
  
  return;
}

void
MarkovProcess::computeMatrix(const Simulation& System,
			     const WWG& wSet,
			     const double densityFactor,
			     const double r2Length,
			     const double r2Power)
  /*!
    Calculate the makov chain process
    \param System :: Simualation
    \param wSet :: WWG set for grid
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
   */
{
  ELog::RegMethod RegA("MarkovProcess","computeMatrix");

  const Geometry::Mesh3D& grid=wSet.getGrid();
  const std::vector<Geometry::Vec3D> midPts=wSet.getMidPoints();

  if (static_cast<long int>(midPts.size())!=FSize)
    throw ColErr::MisMatch<long int>
      (static_cast<long int>(midPts.size()),FSize,"MidPts.size != FSize");

  for(long int i=0;i<FSize;i++)
    fluxField[i][i]=1.0;

  for(long int i=0;i<FSize;i++)
    {
      const size_t uI(static_cast<size_t>(i));
      ModelSupport::ObjectTrackPoint OTrack(midPts[uI]); 
      for(long int j=i+1;j<FSize;j++)
	{
	  const size_t uJ(static_cast<size_t>(i));
	  OTrack.addUnit(System,j,midPts[uJ]);
	  double DistT=OTrack.getDistance(j)/r2Length;
	  if (DistT<1.0) DistT=1.0;
	  const double AT=OTrack.getAttnSum(j);  // this can take an
	  const double WFactor= -densityFactor*AT-r2Power*log(DistT);
	  if (WFactor>-20)
	    fluxField[i][j]=fluxField[j][i]= -WFactor;
	  else
	    fluxField[i][j]=fluxField[j][i]=0.0;
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
