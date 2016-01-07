/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
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
#include "WeightMesh.h"
#include "weightManager.h"
#include "WWG.h"
#include "ItemWeight.h"
#include "WWGWeight.h"

namespace WeightSystem
{


WWGWeight::WWGWeight()  :
  ItemWeight()
  /*! 
    Constructor 
  */
{}

WWGWeight::WWGWeight(const WWGWeight& A)  :
  ItemWeight(A)
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
      ItemWeight::operator=(A);
    }
  return *this;
}
  
void
WWGWeight::updateWM(WWG& wwg,
                    const double eCut,
                    const double scaleFactor,
                    const double minWeight)
  /*!
    Update the master mesh 
    It assumes that the mesh size and WWGWeight are compatable.
    \param wwg :: Weight window generator
    \param eCut :: Cut energy [MeV] (uses fractional if on boundary)
    \param scaleFactor :: Scale factor for weight track
   */
{
  ELog::RegMethod RegA("WWGWeight","updateWM");

  // quick way to get length of array
  // note that zero value is assumed but not infinity
  std::vector<double> EBin=wwg.getEBin();    
  std::vector<double> DVec=EBin;
  std::fill(DVec.begin(),DVec.end(),1.0);
  
  double maxW(0.0);
  double minW(1e38);
  double aveW(0.0);
  int cnt(0);
  for(const CMapTYPE::value_type& cv : Cells)
    {
      const double W=(exp(-cv.second.weight*sigmaScale*scaleFactor));
      if (W>maxW) maxW=W;
      if (W<minW && minW>1e-38) minW=W;
      aveW+=W;
      cnt++;
    }
  aveW/=cnt;
  // Work on minW first:
  const double factor=(minW>minWeight) ?
    log(minWeight)/log(minW) : 1.0;

  const WeightMesh& WGrid=wwg.getGrid();

  const size_t NX=WGrid.getXSize();
  const size_t NY=WGrid.getYSize();
  const size_t NZ=WGrid.getZSize();
  long int cN(1);
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      for(size_t k=0;k<NZ;k++)
	{
          CMapTYPE::const_iterator cv=Cells.find(cN);
          if (cv==Cells.end())
            throw ColErr::InContainerError<long int>(cN,"Cells");
          
          const double W=(exp(- cv->second.weight*sigmaScale*
                              scaleFactor*factor));
          for(size_t i=0;i<EBin.size();i++)
            {
              if (eCut<-1e-10 && EBin[i] <= -eCut)
                DVec[i]=W;
              else if (EBin[i]>=eCut)
                DVec[i]=W;
            }
          /// SET WEIGHTS:
          wwg.scaleMeshItem(cN,DVec);
        }
            


  return;
}
  
  
} // Namespace WeightSystem
