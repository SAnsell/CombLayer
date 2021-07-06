/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWG.cxx
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "writeSupport.h"
#include "phitsWriteSupport.h"
#include "BasicMesh3D.h"
#include "WWGWeight.h"
#include "WWG.h"

namespace WeightSystem
{

WWG::WWG() :
  wupn(8.0),wsurv(1.4),maxsp(5),
  mwhere(-1),mtime(0),switchn(-2)
  /*!
    Constructor : 
    set mwhere[-1] - collisions only 
  */
{}

WWG::WWG(const WWG& A) : 
  wupn(A.wupn),wsurv(A.wsurv),maxsp(A.maxsp),
  mwhere(A.mwhere),mtime(A.mtime),switchn(A.switchn),
  refPt(A.refPt),defUnit(A.defUnit)
  /*!
    Copy constructor
    \param A :: WWG to copy
  */
{
  for(const auto [ Name,WPtr ] : A.WMeshMap)
    WMeshMap.emplace(Name,new WWGWeight(*WPtr));
}

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
      wupn=A.wupn;
      wsurv=A.wsurv;
      maxsp=A.maxsp;
      mwhere=A.mwhere;
      mtime=A.mtime;
      switchn=A.switchn;
      refPt=A.refPt;
      for(auto& [ Name,WPtr ] : WMeshMap)
	delete WPtr;
      WMeshMap.clear();
      for(const auto& [ Name,WPtr ] : A.WMeshMap)
	WMeshMap.emplace(Name,new WWGWeight(*WPtr));
    }
  return *this;
}

WWG::~WWG()
  /*!
    Deletion (needed for map)
  */
{
  for(auto& [ Name,WPtr ] : WMeshMap)
    delete WPtr;
}
  

WWGWeight&
WWG::createMesh(const std::string& meshIndex)
  /*!
    Create a mesh [Must be unique]
    \param meshIndex :: name
  */
{
  MeshTYPE::const_iterator mc=WMeshMap.find(meshIndex);
  if (mc!=WMeshMap.end())
    throw ColErr::InContainerError<std::string>(meshIndex,"Mesh exists");

  // zero ID means dont write
  size_t index(0);
  StrFunc::convert(meshIndex,index);  // leave as zero if not a nuber
  WMeshMap.emplace(meshIndex,new WWGWeight(index));

  if (defUnit.empty()) defUnit=meshIndex;
  return getMesh(meshIndex);
}

WWGWeight&
WWG::copyMesh(const std::string& newUnit,
	      const std::string& oldUnit)
  /*!
    Create a mesh [Must be unique]
    \param newUnit :: new name of mesh
    \param oldUnit :: old name name of mesh
  */
{
  MeshTYPE::const_iterator mc=WMeshMap.find(newUnit);
  if (mc!=WMeshMap.end())
    throw ColErr::InContainerError<std::string>
      (newUnit,"new mesh already exists");

  mc=WMeshMap.find(oldUnit);
  if (mc==WMeshMap.end())
    throw ColErr::InContainerError<std::string>(oldUnit,"oldUnit");

  size_t index(0);
  StrFunc::convert(newUnit,index);  // leave as zero if not a number
  WMeshMap.emplace(newUnit,new WWGWeight(index,*(mc->second)));

  if (defUnit.empty()) defUnit=newUnit;
  
  return getMesh(newUnit);
}

bool
WWG::hasMesh(const std::string& meshIndex) const 
  /*!
    Has a mesh
    \param meshIndex :: name
  */
{
  MeshTYPE::const_iterator mc=WMeshMap.find(meshIndex);
  return (mc!=WMeshMap.end()) ? 1 : 0;
}

WWGWeight&
WWG::getMesh(const std::string& meshIndex)
  /*!
    Get a mesh
    \
  */
{
  MeshTYPE::const_iterator mc=WMeshMap.find(meshIndex);
  if (mc==WMeshMap.end())
    throw ColErr::InContainerError<std::string>
      (meshIndex,"MeshIndex in MeshMap");

  return *(mc->second);
}

const WWGWeight&
WWG::getMesh(const std::string& meshIndex) const
  /*!
    Get a mesh
    \param meshIndex :: String identifier
  */
{
  MeshTYPE::const_iterator mc=WMeshMap.find(meshIndex);
  if (mc==WMeshMap.end())
    throw ColErr::InContainerError<std::string>
      (meshIndex,"MeshIndex in MeshMap");

  return *(mc->second);
}
  


void
WWG::setEnergyBin(const std::string& meshIndex,
		  const std::vector<double>& EB)
  /*!
    Set the energy bins and resize the WMesh
    \param meshIndex :: Index to use
    \param EB :: Energy bins [MeV]
  */
{
  ELog::RegMethod RegA("WWG","setEnergyBin");

  WWGWeight& WMesh=getMesh(meshIndex);

  WMesh.setEnergy(EB);
  return;
}
  
void
WWG::setRefPoint(const Geometry::Vec3D& RP)
  /*!
    Set the reference point
    \param RP :: reference point
   */
{
  refPt=RP;

  return;
}
    
void
WWG::scaleMesh(const std::string& meshIndex,
	       const double scaleFactor)
  /*!
    \todo was called:: updateWM
    Mulitiply the wwg:master mesh by factors in WWGWeight
    It assumes that the mesh size and WWGWeight are compatable.
    \param meshIndex :: mesh to update
    \param scaleFactor :: Scale factor for track [exp(-scale)]
   */
{
  ELog::RegMethod RegA("WWG","scaleMesh");

  WWGWeight& WMesh=getMesh(meshIndex);  

  WMesh.scaleGrid(scaleFactor);
  return;
}
  


void
WWG::powerRange(const std::string& meshIndex,
		const double pR)
  /*!
    After normalization calculate W^p
    \param meshIndex :: mesh to access						
    \param pR :: power value
  */
{
  ELog::RegMethod RegA("WWG","powerRange");

  WWGWeight& WMesh=getMesh(meshIndex);

  WMesh.scalePower(pR);
  return;
}
  
  
void
WWG::scaleMeshItem(const std::string& meshIndex,
		   const size_t I,const size_t J,const size_t K,
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

  WWGWeight& WMesh=getMesh(meshIndex);

  WMesh.scaleMeshItem(static_cast<long int>(I),
		      static_cast<long int>(J),
		      static_cast<long int>(K),
		      static_cast<long int>(EI),
		      W);
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

  // find energy :
  const WWGWeight& WMesh=getMesh(defUnit);

  const std::vector<double>& EBin=
    WMesh.getEnergy();

  const std::set<std::string>& pType=WMesh.getParticles();
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
	if (E>=1e-9 && E<1e5)
	  cx<<E<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  
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
  const WWGWeight& WMesh=getMesh(defUnit);
  WMesh.writeGrid(OX);
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

  
  const WWGWeight& WMesh=getMesh(defUnit);
  if (!FName.empty())
    {
      std::ofstream OX;
      OX.open(FName.c_str());
      WMesh.writeWWINP(OX);
    }
  return;
}  

void
WWG::writeVTK(const std::string& FName,
	      const bool logFlag,
	      const std::string& meshName,
	      const long int EIndex) const
  /*!
    Write out a VTK file
    \param FName :: filename 
    \param meshName :: Mesh name
    \param EIndex :: energy index
  */
{
  ELog::RegMethod RegA("WWG","writeVTK");

  ELog::EM<<"WRITE VTK"<<" "<<FName<<" : "<<meshName<<" "<<EIndex<<ELog::endDiag;
  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());

  const WWGWeight& WMesh=getMesh(meshName);
  const Geometry::BasicMesh3D& Grid=WMesh.getGeomGrid();

  // const long int XSize=WMesh.getXSize();
  // const long int YSize=WMesh.getYSize();
  // const long int ZSize=WMesh.getZSize();
  
  boost::format fFMT("%1$11.6g%|14t|");  
  OX<<"# vtk DataFile Version 2.0"<<std::endl;
  OX<<"WWG-MESH Data"<<std::endl;
  OX<<"ASCII"<<std::endl;
  Grid.writeVTK(OX);
  WMesh.writeVTK(OX,EIndex,logFlag);
  
  OX.close();

  return;
}

void
WWG::writePHITS(std::ostream& OX) const
  /*!
    Write out a WWG mesh as a PHITS system
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("WWG","writePHITS");

  for(const auto& [Name,WMeshPtr] : WMeshMap)
    {
      if (WMeshPtr->getID()>0)
	WMeshPtr->writePHITS(OX);
    }
  
  return;
}

void
WWG::writeFLUKA(std::ostream& OX) const
  /*!
    Write out a WWG mesh as a FLUKA system
    This uses the usrini table. This is a S. Ansell
    extension to FLUKA.
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("WWG","writeFLUKA");


  std::ostringstream cx;

  for(const auto& [Name,WMeshPtr] : WMeshMap)
    {
      if (WMeshPtr->getID()>0)
	WMeshPtr->writeFLUKA(OX);
    }
  return;
}
  
}  // NAMESPACE WeightSystem
