/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
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
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cone.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "CellWeight.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "ObjectTrackPlane.h"
#include "weightManager.h"
#include "WWG.h"
#include "WWGItem.h"
#include "WWGWeight.h"

namespace WeightSystem
{

WWGWeight::WWGWeight(const size_t EB,
		     const Geometry::Mesh3D& Grid)  :
  WX(static_cast<long int>(Grid.getXSize())),
  WY(static_cast<long int>(Grid.getYSize())),
  WZ(static_cast<long int>(Grid.getZSize())),
  WE(static_cast<long int>(EB)),
  WGrid(boost::extents[WX][WY][WZ][WE])
  /*! 
    Constructor 
    \param EB :: Size of energy bin
    \param Grid :: 3D Mesh to build points on [boundary]
  */
{
  zeroWGrid();
}

WWGWeight::WWGWeight(const WWGWeight& A)  :
  WX(A.WX),WY(A.WY),WZ(A.WZ),WE(A.WE),
  WGrid(A.WGrid)
  /*! 
    Copy Constructor 
    \param A :: WWGWeight to copy
  */
{}

WWGWeight&
WWGWeight::operator=(const WWGWeight& A)
  /*! 
    Assignment operator
    \param A :: WWGWeight to copy
    \return *this
  */
{
  if (this!=&A)
    {
      WGrid=A.WGrid;
    }
  return *this;
}

	
void
WWGWeight::zeroWGrid()
  /*!
    Zero WGrid
  */
{
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
        for(long int e=0;e<WE;e++)
          {
            WGrid[i][j][k][e]=0.0;
          }
  
  return;
}

void
WWGWeight::setPoint(const long int index,
                    const long int IE,
                    const double V)
   /*!
     Set a point assuming x,y,z indexing and z fastest point
     \param index :: index for linearization
     \param eIndex :: Energy bin
     \param V :: value to set
   */
{
  const long int IX=index/(WY*WZ);
  const long int IY=(index/WZ) % WY;
  const long int IZ=index % WZ;

  if (IX>=WX)
    throw ColErr::IndexError<long int>
      (index,WX,"setPoint::Index out of range");

  if (IE>=WE)
    throw ColErr::IndexError<long int>
      (IE,WE,"setPoint::eIndex out of range");
  
  WGrid[IX][IY][IZ][IE]=V;
  return;
}

double
WWGWeight::calcMaxAttn(const long int eIndex) const
  /*!
    Calculate the adjustment factor to get to the 
    max weight correction
    \param eIndex :: Index
    \return factor for exponent (not yet taken exp)
  */
{
  double maxW(-1e38);
  if (WE>eIndex)
    {
      for(long int i=0;i<WX;i++)
        for(long int  j=0;j<WY;j++)
          for(long int k=0;k<WZ;k++)
            {
              if (WGrid[i][j][k][eIndex]>maxW)
                maxW=WGrid[i][j][k][eIndex];
            }
    }
  return maxW;
}


  
double
WWGWeight::calcMaxAttn() const
  /*!
    Calculate the adjustment factor to get to the 
    max weight correction
    \param eIndex :: Index
    \return factor for exponent (not yet taken exp)
  */
{
  double maxW(-1e38);
  for(long int eIndex=0;eIndex<WE;eIndex++)
    {
      const double W=calcMaxAttn(eIndex);
      if (W>maxW) maxW=W;
    }
  return maxW;
}

void
WWGWeight::makeSource(const double MValue)
  /*!
    Convert a set of value [EXP not taken]
    into  a source. 
    - 0 is full beam 
    - MValue is lowest attentuation factor
    \param MValue :: Lowest acceptable attenuation fraction [+ve only]
  */

{
  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();
  for(size_t i=0;i<NData;i++)
    {
       if (TData[i] < MValue)
	 TData[i]= MValue;
       else if (TData[i]>0.0)
	 TData[i]=0.0;
    }
  return;
}

void
WWGWeight::makeAdjoint(const double MValue)
  /*!
    Remormalize the grid to MValue minimum
    \param MValue :: LOWEST value [-ve]
   */
{
  ELog::EM<<"Mind == "<<MValue<<ELog::endDiag;
  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();
 
  for(size_t i=0;i<NData;i++)
    {
      TData[i]= MValue-TData[i];
      if (TData[i] < MValue)
        TData[i]= MValue;
      else if (TData[i]>0.0)
	TData[i]=0.0;
    }
  return;
}
  
void
WWGWeight::wTrack(const Simulation& System,
		  const Geometry::Vec3D& initPt,
		  const std::vector<double>& EBin,
		  const std::vector<Geometry::Vec3D>& MidPt,
		  const double densityFactor,
		  const double r2Length,
		  const double r2Power)
  /*!
    Calculate a specific track from sourcePoint to position
    \param System :: Simulation to use    
    \param initPt :: Point for outgoing track
    \param EBin :: Energy points
    \param MidPt :: Grid points
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
  */
{
  ELog::RegMethod RegA("WWGWeight","wTrack(Vec3D)");

  ModelSupport::ObjectTrackPoint OTrack(initPt);

  long int cN(1);
  ELog::EM<<"Processing  "<<MidPt.size()<<" for WWG"<<ELog::endDiag;
  double minV(1.0);
  for(const Geometry::Vec3D& Pt : MidPt)
    {
      ModelSupport::ObjectTrackPoint OTrack(initPt);
      OTrack.addUnit(System,cN,Pt);
      double DistT=OTrack.getDistance(cN)/r2Length;
      if (DistT<1.0) DistT=1.0;
      const double AT=OTrack.getAttnSum(cN);    // this can take an
                                                // energy
      for(long int index=0;index<WE;index++)
        {
	  //	  ELog::EM<<"Pt["<<Pt<<"] == "<<densityFactor*AT
	  //		  <<" "<<r2Power*log(DistT)<<ELog::endDiag;
	  // exp(-Sigma)/r^2  in log form
          setPoint(cN-1,index,-densityFactor*AT-r2Power*log(DistT));
        }

      if (!(cN % 10000))
	ELog::EM<<"Item "<<cN<<" "<<MidPt.size()<<" "<<densityFactor<<" "
		<<r2Length<<ELog::endDiag;
      cN++;
    }
  return;
}


void
WWGWeight::wTrack(const Simulation& System,
		  const Geometry::Plane& initPlane,
		  const std::vector<double>& EBin,
		  const std::vector<Geometry::Vec3D>& MidPt,
		  const double densityFactor,
		  const double r2Length,
		  const double r2Power)
  /*!
    Calculate a specific trac from sourcePoint to  postion
    \param System :: Simulation to use    
    \param initPlane :: Plane for outgoing track
    \param EBin :: Energy points
    \param MidPt :: Grid points
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
  */
{
  ELog::RegMethod RegA("WWGWeight","wTrack(Plane)");

  ModelSupport::ObjectTrackPlane OTrack(initPlane);
  long int cN(1);
  double minV(1.0);
  for(const Geometry::Vec3D& Pt : MidPt)
    {
      OTrack.addUnit(System,cN,Pt);
      double DistT=OTrack.getDistance(cN)/r2Length;
      if (DistT<1.0) DistT=1.0;
      const double AT=OTrack.getAttnSum(cN);    // this can take an energy
      double V= -densityFactor*AT-r2Power*log(DistT);
      if (V<minV)
	{
	  minV=V;
	  ELog::EM<<"Min == "<<V<<ELog::endDiag;
	}
      for(long int index=0;index<WE;index++)
        {
          // exp(-Sigma)/r^2  in log form
          setPoint(cN-1,index,-densityFactor*AT-r2Power*log(DistT));
        }
      cN++;
    }
  return;
}

void
WWGWeight::write(std::ostream& OX) const
  /*!
    Debug output
    \param OX :: Output stream
   */
{
  OX<<"# NPts == "<<WGrid.size()<<std::endl;
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
        {
          OX<<i<<" "<<j<<" "<<k;
          for(long int e=0;e<WE;e++)
            {
              OX<<" "<<WGrid[i][j][k][e];
            }
          OX<<std::endl;
        }

  return;
}
  
} // Namespace WeightSystem
