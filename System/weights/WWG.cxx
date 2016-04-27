/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWG.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
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
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "WeightMesh.h"
#include "WWG.h"

namespace WeightSystem
{

WWG::WWG() :
  ptype('n'),wupn(8.0),wsurv(1.4),maxsp(5),
  mwhere(-1),mtime(0),switchn(-1),
  EBin({1e8})
  /*!
    Constructor : 
    set mwhere[-1] - collisions only 
  */
{}

WWG::WWG(const WWG& A) : 
  ptype(A.ptype),wupn(A.wupn),wsurv(A.wsurv),maxsp(A.maxsp),
  mwhere(A.mwhere),mtime(A.mtime),switchn(A.switchn),
  EBin(A.EBin),Grid(A.Grid),WMesh(A.WMesh)
  /*!
    Copy constructor
    \param A :: WWG to copy
  */
{}

WWG&
WWG::operator=(const WWG& A)
  /*!
    Assignment operator
    \param A :: WWG to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ptype=A.ptype;
      wupn=A.wupn;
      wsurv=A.wsurv;
      maxsp=A.maxsp;
      mwhere=A.mwhere;
      mtime=A.mtime;
      switchn=A.switchn;
      EBin=A.EBin;
      Grid=A.Grid;
      WMesh=A.WMesh;
    }
  return *this;
}

void
WWG::resetMesh(const std::vector<double>& W)
  /*!
    Resize the mesh
    \param W :: Weight 
   */
{
  ELog::RegMethod RegA("WWG","resetMesh");
  
  const size_t GSize=Grid.size();
  if (GSize && !EBin.empty())
    {
      WMesh.resize(GSize);
      const size_t ESize(EBin.size());
      std::vector<double>::const_iterator  vc=
        W.begin();
      for(std::vector<double>& MUnit : WMesh)
        {
          const double wVal=(vc!=W.end()) ? (*vc++) : 1.0;
          MUnit.resize(ESize);
          std::fill(MUnit.begin(),MUnit.end(),wVal);
        }
    }
  else 
    WMesh.clear();
  
  return;
}
  
void
WWG::setEnergyBin(const std::vector<double>& EB,
                  const std::vector<double>& DefWeight)
  /*!
    Set the energy bins and resize the WMesh
    \param EB :: Energy bins [MeV]
    \param DefWeight :: Initial weight
  */
{
  ELog::RegMethod RegA("WWG","setEnergyBine");
  EBin=EB;
  if (EBin.empty() || EBin.back()<1e5)
    EBin.push_back(1e5);
  resetMesh(DefWeight);
  return;
}
  
void 
WWG::writeHead(std::ostream& OX) const
  /*!
    Write out the header section from the file
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("WWG","writeHead");
  
  std::ostringstream cx;
  
  cx.str("");  
  cx<<"wwp:"<<ptype<<" ";
  cx<<wupn<<" "<<wsurv<<" "<<maxsp<<" "<<mwhere
    <<" "<<switchn<<" "<<mtime;
  StrFunc::writeMCNPX(cx.str(),OX);

  if (EBin.size()>15)
    throw ColErr::RangeError<size_t>(EBin.size(),0,15,
                                     "MCNP Energy Bin size limit");
  cx.str("");
  cx<<"wwge:"<<ptype<<" ";
  for(const double E : EBin)
    cx<<E<<" ";
  StrFunc::writeMCNPX(cx.str(),OX);
  
  return;
}

void
WWG::scaleMeshItem(const long int index,
                   const std::vector<double>& DVec)
  /*!
    Scale a given mesh index [based on second index]
    \param index :: index for i,j,k
    \param DVec :: scaling vector for energy bins
  */
{
  ELog::RegMethod RegA("WWG","scaleMeshItem");
  
  const size_t ID(static_cast<size_t>(index));
  if (ID>=WMesh.size())
    throw ColErr::IndexError<size_t>(ID,WMesh.size(),"WMesh!=ID");
  if (DVec.size()!=WMesh[ID].size())
    throw ColErr::IndexError<size_t>(WMesh[ID].size(),DVec.size(),
                                         "DVec!=WMesh");

  for(size_t i=0;i<DVec.size();i++)
    WMesh[ID][i]*=DVec[i];
  
  return;
}

void
WWG::write(std::ostream& OX) const
  /*!
    Write to the MCNP file [only grid]
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("WWG","write");
  writeHead(OX);
  Grid.write(OX);
      
  return;
}
  
void
WWG::writeWWINP(const std::string& FName) const
  /*!
    Write out separate WWINP file
    \param FName :: Output filename
  */
{
  std::ofstream OX;
  OX.open(FName.c_str());

  Grid.writeWWINP(OX,EBin.size());
  size_t itemCnt=0;
  for(const double& E : EBin)
    StrFunc::writeLine(OX,E,itemCnt,6);
  if (itemCnt!=0)
    OX<<std::endl;

  // MESH:
  itemCnt=0;
  for(const std::vector<double>& CV : WMesh)
    for(const double W : CV)
      StrFunc::writeLine(OX,W,itemCnt,6);
    
  OX.close();
		       
  return;
}  
  
}  // NAMESPACE WeightSystem
