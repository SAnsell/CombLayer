/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "weightManager.h"
#include "WWG.h"
#include "WWGItem.h"
#include "WWGWeight.h"

namespace WeightSystem
{

WWGWeight::WWGWeight()  :
  sigmaScale(0.06914)
  /*! 
    Constructor 
  */
{}

WWGWeight::WWGWeight(const WWGWeight& A)  :
  sigmaScale(A.sigmaScale),GCells(A.GCells)
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
      GCells=A.GCells;
    }
  return *this;
}

double
WWGWeight::calcMinWeight(const double scaleFactor,
                          const double weightPower) const
  /*!
    Calculate the adjustment factor to get to the correct
    minimum weight.
    \param scaleFactor :: Scalefactor for density equivilent
    \param weightPower :: power for final factor W**power
    \return factor for exponent
   */
{
  double minW(1e38);
  for(const WWGItem& wI : GCells)
    {
      double W=exp(-wI.weight*sigmaScale*scaleFactor);
      if (W>1e-20)
        {
          W=std::pow(W,weightPower);
          if (W<minW) minW=W;
        }
    }
  return minW;
}

  
  
void
WWGWeight::updateWM(WWG& wwg,
                    const double eCut,
                    const double scaleFactor,
                    const double minWeight,
                    const double weightPower) const
  /*!
    Mulitiply the wwg:master mesh by factors in WWGWeight
    It assumes that the mesh size and WWGWeight are compatable.
    \param wwg :: Weight window generator
    \param eCut :: Cut energy [MeV] (uses fractional if on boundary)
    \param scaleFactor :: Scale factor for weight track
    \param minWeight :: min weight scale factor
    \param weightPower :: power for final factor W**power
   */
{
  ELog::RegMethod RegA("WWGWeight","updateWM");

  // quick way to get length of array
  // note that zero value is assumed but not infinity
  std::vector<double> EBin=wwg.getEBin();    
  std::vector<double> DVec=EBin;
  std::fill(DVec.begin(),DVec.end(),1.0);

    // Work on minW first:
  const double minW=calcMinWeight(scaleFactor,weightPower);
  const double factor=(minW<minWeight) ?
    log(minWeight)/log(minW) : 1.0;

  ELog::EM<<"Min W = "<<minW<<" "<<factor<<ELog::endDiag;
  
  const Geometry::Mesh3D& WGrid=wwg.getGrid();

  const size_t NX=WGrid.getXSize();
  const size_t NY=WGrid.getYSize();
  const size_t NZ=WGrid.getZSize();
  size_t cN(0);

  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      for(size_t k=0;k<NZ;k++)
	{
          const WWGItem& Cell(GCells[cN]);
          
          double W=(exp(-Cell.weight*sigmaScale*scaleFactor*factor));
          if (W<minWeight) W=1.0;    // avoid sqrt(-ve number etc)
          W=std::pow(W,weightPower);
          if (W>=minWeight)
            {
              for(size_t i=0;i<EBin.size();i++)
                {
                  if (eCut<-1e-10 && EBin[i] <= -eCut)
                    DVec[i]=W;
                  else if (EBin[i]>=eCut)
                    DVec[i]=W;
                }
              wwg.scaleMeshItem(cN,DVec);              
            }
          cN++; 
        }
  
  return;
}


void
WWGWeight::invertWM(WWG& wwg,
                    const double eCut,
                    const double scaleFactor,
                    const double minWeight,
                    const double weightPower) const
  /*!
    Mulitiply the wwg:master mesh by factors in WWGWeight
    Invertion [adjoint] system
    It assumes that the mesh size and WWGWeight are compatable.
    \param wwg :: Weight window generator
    \param eCut :: Cut energy [MeV] (uses fractional if on boundary)
    \param scaleFactor :: Scale factor for weight track
    \param minWeight :: min weight scale factor
    \param weightPower :: power for final factor W**power
   */
{
  ELog::RegMethod RegA("WWGWeight","invertWM");

  // quick way to get length of array
  // note that zero value is assumed but not infinity
  std::vector<double> EBin=wwg.getEBin();    
  std::vector<double> DVec=EBin;
  std::fill(DVec.begin(),DVec.end(),1.0);

    // Work on minW first:
  const double minW=calcMinWeight(scaleFactor,weightPower);
  const double factor=(minW<minWeight) ?
    log(minWeight)/log(minW) : 1.0;

  ELog::EM<<"Min W = "<<minW<<" "<<factor<<ELog::endDiag;
  
  const Geometry::Mesh3D& WGrid=wwg.getGrid();

  const size_t NX=WGrid.getXSize();
  const size_t NY=WGrid.getYSize();
  const size_t NZ=WGrid.getZSize();
  size_t cN(0);

  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      for(size_t k=0;k<NZ;k++)
	{
          const WWGItem& Cell(GCells[cN]);
          
          double W=(exp(-Cell.weight*sigmaScale*scaleFactor*factor));

          if (W<minWeight) W=1.0;    // avoid sqrt(-ve number etc)
          W=std::pow(W,weightPower);
          if (W>=minWeight)
            {
              for(size_t i=0;i<EBin.size();i++)
                {
                  if (eCut<-1e-10 && EBin[i] <= -eCut)
                    DVec[i]=W;
                  else if (EBin[i]>=eCut)
                    DVec[i]=W;
                }
              wwg.scaleMeshItem(cN,DVec);              
            }
        }
  
  return;
}

void
WWGWeight::setPoints()
  /*!
    Set all the points in the grid
  */
{
  ELog::RegMethod Rega("WWGWeight","calcPoints");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();
  const Geometry::Mesh3D& WGrid=wwg.getGrid();  

  const size_t NX=WGrid.getXSize();
  const size_t NY=WGrid.getYSize();
  const size_t NZ=WGrid.getZSize();

  GCells.clear();
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      for(size_t k=0;k<NZ;k++)
	{
	  GCells.push_back(WWGItem(WGrid.point(i,j,k)));
	}
  
  return;
}
  
  
  
} // Namespace WeightSystem
