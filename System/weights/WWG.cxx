/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWG.cxx
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
#include <memory>
#include <boost/multi_array.hpp>
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
#include "Mesh3D.h"
#include "WWGWeight.h"
#include "WWG.h"

namespace WeightSystem
{

WWG::WWG() :
  pType({"n"}),wupn(8.0),wsurv(1.4),maxsp(5),
  mwhere(-1),mtime(0),switchn(-2),
  EBin({1e8}),WMesh(1,Grid)
  /*!
    Constructor : 
    set mwhere[-1] - collisions only 
  */
{}

WWG::WWG(const WWG& A) : 
  pType(A.pType),wupn(A.wupn),wsurv(A.wsurv),maxsp(A.maxsp),
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
      pType=A.pType;
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
WWG::setParticles(const std::set<std::string>& AP)
  /*!
    Set active particles
    \param AP :: Active particle set
   */
{
  pType=AP;
  return;
}

void
WWG::calcGridMidPoints() 
  /*!
    Given a grid produce a linearized Vec3D set
  */
{
  ELog::RegMethod RegA("WWG","calcGridMidPoint");

  GridMidPt=Grid.midPoints();
  if (GridMidPt.empty())
    ELog::EM<<"Failed to populate GridMidPt"<<ELog::endErr;
  return;
}
  
void
WWG::resetMesh(const std::vector<double>&)
  /*!
    Resize the mesh
    \param W :: Default Weight 
   */
{
  ELog::RegMethod RegA("WWG","resetMesh");
  

  // boundaries need to be  2 or greater
  const long int LX=static_cast<long int>(Grid.getXSize());
  const long int LY=static_cast<long int>(Grid.getYSize());
  const long int LZ=static_cast<long int>(Grid.getZSize());
  const long int EBSize=static_cast<long int>(EBin.size());

  if (LX<=1 && LY<=1 && LZ<=1 && EBSize<=0)
    {
      const long int LPtr[]={LX,LY,LZ,EBSize};
      const long int BPtr[]={2,2,2,1};
      throw ColErr::DimensionError<4,long int>
	(LPtr,BPtr,"WGrid size");
    }
  
  if (!WMesh.isSized(LX,LY,LZ,EBSize))
    WMesh.resize(LX,LY,LZ,EBSize);
  WMesh.zeroWGrid();
  
  return;
}

void
WWG::setRefPoint(const Geometry::Vec3D& RP)
  /*!
    Set the reference point
    \param RP :: reference point
   */
{
  Grid.setRefPt(RP);
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
  if (EBin.empty())
    EBin.push_back(1e5);
  resetMesh(DefWeight);
  return;
}

    
void
WWG::updateWM(const WWGWeight& UMesh,const double scaleFactor)
  /*!
    Mulitiply the wwg:master mesh by factors in WWGWeight
    It assumes that the mesh size and WWGWeight are compatable.
    \param UMesh :: Weight window mesh to update
    \param scaleFactor :: Scale factor for track [exp(-scale)]
   */
{
  ELog::RegMethod RegA("WWG","updateWM");

  WMesh=UMesh;
  ELog::EM<<"Calling updateWM"<<ELog::endDiag;
  WMesh.scaleGrid(scaleFactor);
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

  for(const std::string& P : pType)
    {
      cx.str("");  
      cx<<"wwp:"<<P<<" ";
      cx<<wupn<<" "<<wsurv<<" "<<maxsp<<" "<<mwhere
	<<" "<<switchn<<" "<<mtime;
      StrFunc::writeMCNPX(cx.str(),OX);
      
      if (EBin.size()>15)
	throw ColErr::RangeError<size_t>(EBin.size(),0,15,
					 "MCNP Energy Bin size limit");
      cx.str("");
      cx<<"wwge:"<<P<<" ";
      for(const double E : EBin)
	cx<<E<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  
  return;
}

void
WWG::powerRange(const double pR)
  /*!
    After normalization calculate W^p
    \param pR :: power value
  */
{
  ELog::RegMethod RegA("WWG","powerRange");

  WMesh.scalePower(pR);
  return;
}

void
WWG::scaleRange(const double minR,
		const double maxR,
		const double fullRange)
  /*!
    Normalize the mesh to have a max at 1.0
    \param minR :: Min value
    \param maxR :: Max value
    \param fullRange :: range between 0 - fullRange [negative]
  */
{
  ELog::RegMethod RegA("WWG","scaleRange");

  WMesh.scaleRange(minR,maxR,fullRange);
  return;
}
  
  
void
WWG::scaleMeshItem(const size_t I,const size_t J,const size_t K,
                   const size_t EI,const double W)
  /*!
    Scale a given mesh index [based on second index]
    \param I :: index for i,j,k
    \param J :: index for i,j,k
    \param K :: index for i,j,k
    \param EI :: energy bin
    \param W :: scaling vector for energy bins
  */
{
  ELog::RegMethod RegA("WWG","scaleMeshItem");

  WMesh.scaleMeshItem(static_cast<long int>(I),
		      static_cast<long int>(J),
		      static_cast<long int>(K),
		      static_cast<long int>(EI),
		      W);
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
  ELog::RegMethod RegA("WWG","writeWWINP");
  
  boost::format TopFMT("%10i%10i%10i%10i%28s\n");
  boost::format neFMT("%10i");
  const std::string date("10/07/15 15:37:51");

  const size_t nParticle(pType.size());
  std::ofstream OX;
  OX.open(FName.c_str());
  
  // IF[1] : timeIndependent : No. particleType : 10(rectangular) : date
  OX<<(TopFMT % 1 % 1  % nParticle % 10 % date);


  for(size_t i=0;i<nParticle;i++)
    {
      OX<<(neFMT % EBin.size());
      if ( ((i+1) % 7) == 0) OX<<std::endl;
    }
  if (nParticle % 7) OX<<std::endl;
  
  Grid.writeWWINP(OX);
  for(size_t i=0;i<nParticle;i++)
    {
      size_t itemCnt=0;
      for(const double& E : EBin)
	StrFunc::writeLine(OX,E,itemCnt,6);
      if (itemCnt!=0)
	OX<<std::endl;
      WMesh.writeWWINP(OX);
    }
  
  OX.close();
		       
  return;
}  

void
WWG::writeVTK(const std::string& FName,
	      const long int EIndex) const
  /*!
    Write out a VTK file
    \param FName :: filename 
    \param EIndex :: energy index
  */
{
  ELog::RegMethod RegA("WWG","writeVTK");

  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());

  const long int XSize=WMesh.getXSize();
  const long int YSize=WMesh.getYSize();
  const long int ZSize=WMesh.getZSize();
  
  boost::format fFMT("%1$11.6g%|14t|");  
  OX<<"# vtk DataFile Version 2.0"<<std::endl;
  OX<<"WWG-MESH Data"<<std::endl;
  OX<<"ASCII"<<std::endl;
  OX<<"DATASET RECTILINEAR_GRID"<<std::endl;

  OX<<"DIMENSIONS "<<XSize<<" "<<YSize<<" "<<ZSize<<std::endl;
  OX<<"X_COORDINATES "<<XSize<<" float"<<std::endl;
  for(long int i=0;i<XSize;i++)
    OX<<(fFMT % Grid.getXCoordinate(static_cast<size_t>(i)));
  OX<<std::endl;
  
  OX<<"Y_COORDINATES "<<YSize<<" float"<<std::endl;
  for(long int i=0;i<YSize;i++)
    OX<<(fFMT % Grid.getYCoordinate(static_cast<size_t>(i)));
  OX<<std::endl;

  OX<<"Z_COORDINATES "<<ZSize<<" float"<<std::endl;
  for(long int i=0;i<ZSize;i++)
    OX<<(fFMT % Grid.getZCoordinate(static_cast<size_t>(i)));
  OX<<std::endl;
  
  OX<<"POINT_DATA "<<XSize*YSize*ZSize<<std::endl;
  OX<<"SCALARS cellID float 1.0"<<std::endl;
  OX<<"LOOKUP_TABLE default"<<std::endl;

  WMesh.writeVTK(OX,EIndex);
  
  
  OX.close();

  return;
}

  
}  // NAMESPACE WeightSystem
