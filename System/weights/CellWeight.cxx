/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/CellWeight.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "Mesh3D.h"
#include "weightManager.h"

#include "ItemWeight.h"
#include "CellWeight.h"

#include "debugMethod.h"

namespace WeightSystem
{


CellWeight::CellWeight()  :
  ItemWeight()
  /*! 
    Constructor 
  */
{}

CellWeight::CellWeight(const CellWeight& A)  :
  ItemWeight(A)
  /*! 
    Copy Constructor 
    \param A :: CellWeight to copy
  */
{}

CellWeight&
CellWeight::operator=(const CellWeight& A)
  /*! 
    Assignment operator
    \param A :: CellWeight to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ItemWeight::operator=(A);
    }
  return *this;
}
  
void
CellWeight::updateWM(const double eCut,
                     const double scaleFactor,
                     const double minWeight,
                     const double weightPower) const
  /*!
    Multiply weight mesh by factors in CellWeight.
    \param eCut :: Energy cut [-ve to scale below ] 
    \param scaleFactor :: Scalefactor for density equivilent
    \param minWeight :: min weight scale factor
    \param weightPower :: power for final factor W**power
  */
{
  ELog::RegMethod RegA("CellWeight","updateWM");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WForm* WF=WM.getParticle('n');
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","neutron has no WForm");

  // quick way to get length of array
  const std::vector<double> EVec=WF->getEnergy();
  std::vector<double> DVec=EVec;
  std::fill(DVec.begin(),DVec.end(),1.0);

    
  // Work on minW first:
  const double minW=calcMinWeight(scaleFactor,weightPower);
  const double factor=(minW<minWeight) ?
    log(minWeight)/log(minW) : 1.0;


  for(const CMapTYPE::value_type& cv : Cells)
    {
      double W=(exp(-cv.second.weight*sigmaScale*scaleFactor*factor));
      if (W<minWeight) W=1.0;    // avoid sqrt(-ve number etc)
      W=std::pow(W,weightPower);
      if (W>=minWeight)
        {
          for(size_t i=0;i<EVec.size();i++)
            {
              if (eCut<-1e-10 && EVec[i] <= -eCut)
                DVec[i]=W;
              else if (EVec[i]>=eCut)
                DVec[i]=W;
            }
          WF->scaleWeights(static_cast<int>(cv.first),DVec);
        }
    }
  return;
}

void
CellWeight::invertWM(const double eCut,
                     const double scaleFactor,
                     const double minWeight,
                     const double weightPower) const
  /*!
    Update WM with an adjoint
    \param eCut :: Energy cut [-ve to scale below ] 
    \param scaleFactor :: Scalefactor for density equivilent
    \param minWeight :: min weight scale factor
    \param weightPower :: scale power factor on W
  */
{
  ELog::RegMethod RegA("CellWeight","updateWM");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WForm* WF=WM.getParticle('n');
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","neutron has no WForm");

  // quick way to get length of array
  const std::vector<double> EVec=WF->getEnergy();
  std::vector<double> DVec=EVec;
  std::fill(DVec.begin(),DVec.end(),1.0);

  double minW=calcMinWeight(scaleFactor,weightPower);
  double factor(1.0);

  if (minW<minWeight)
    {
      factor= log(minWeight)/log(minW);
      minW=minWeight;
    }
  
  
  for(const CMapTYPE::value_type& cv : Cells)
    {
      double W=(exp(-cv.second.weight*sigmaScale*scaleFactor*factor));
      double WA=(exp(-cv.second.weight*sigmaScale*scaleFactor));
      if (WA>1e-20)
        {
          W=std::pow(W,weightPower);
          if (W<minW) continue;

          for(size_t i=0;i<EVec.size();i++)
            {
              if (eCut<-1e-10 && EVec[i] <= -eCut)
                DVec[i]=W;
              else if (EVec[i]>=eCut)
                DVec[i]=W;
            }
          WF->scaleWeights(static_cast<int>(cv.first),DVec);
        }
    }
  return;
}
  
  
} // namespace WeightSystem
