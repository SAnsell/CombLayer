/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerMainWall.cxx
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <climits>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
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
#include "XMLwriteVisitor.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcollect.h"
#include "MaterialSupport.h"


#include "BunkerMainWall.h"

namespace essSystem
{

size_t 
BunkerMainWall::hash(const size_t SectIndex,
		     const size_t VIndex,
		     const size_t RIndex)
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param SectIndex :: Sector index
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \return Hash number
  */
{
  //Numbers are 64 256 256
  return 65536UL*SectIndex+256UL*VIndex+RIndex;
}

BunkerMainWall::BunkerMainWall(const std::string& defMat)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param defMat :: default material
  */
{
  MatMap.emplace(0,defMat);
}

BunkerMainWall::BunkerMainWall(const BunkerMainWall& A) : 
  MatMap(A.MatMap),PointMap(A.PointMap)
  /*!
    Copy constructor
    \param A :: BunkerMainWall to copy
  */
{}

BunkerMainWall&
BunkerMainWall::operator=(const BunkerMainWall& A)
  /*!
    Assignment operator
    \param A :: BunkerMainWall to copy
    \return *this
  */
{
  if (this!=&A)
    {
      MatMap=A.MatMap;
      PointMap=A.PointMap;
    }
  return *this;
}

BunkerMainWall::~BunkerMainWall()
  /*!
    Destructor
  */
{}

int
BunkerMainWall::loadXML(const std::string& FName) 
  /*!
    Process an XML file to set/add variables
    
    \param FName :: filename 
    
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("BunkerMainWall","loadXML");
  
  XML::XMLcollect CO;
  if (FName.empty() || CO.loadXML(FName))
    return 0;


  ELog::EM<<"Load mat "<<FName<<ELog::endDiag;

  MatMap.clear();
  // Check parameters;
  XML::XMLobject* AR;
  // Parse for variables:
  AR=CO.findObj("WallMat");
  std::string MatName;

  while(AR)
    {
      std::vector<size_t> SVec,VVec,RVec;
      std::string SStr=AR->getItem<std::string>("Sector");    
      std::string VStr=AR->getItem<std::string>("Vertical");
      std::string RStr=AR->getItem<std::string>("Radial");

      StrFunc::sectionRange(SStr,SVec);
      StrFunc::sectionRange(VStr,VVec);
      StrFunc::sectionRange(RStr,RVec);

      for(const size_t SN : SVec)
	for(const size_t VN : VVec)
	  for(const size_t RN : RVec)
	    {
	      MatName= AR->getNamedItem<std::string>("Material");
	      MatName=StrFunc::fullBlock(MatName);
	      const size_t HN=BunkerMainWall::hash(SN,VN,RN);
	      std::map<size_t,std::string>::iterator mc=MatMap.find(HN);
	      if (mc!=MatMap.end())
		mc->second=MatName;
	      else
		MatMap.emplace(HN,MatName);
	    }
      CO.deleteObj(AR);      
      AR=CO.findObj("WallMat");
    }
  return (MatMap.empty()) ? 0 : 1;
}

void
BunkerMainWall::setPoints(const size_t SectIndex,
			  const size_t VIndex,
			  const size_t RIndex,
			  const std::vector<Geometry::Vec3D>& Corners)
  /*!
    Sets a point system
    \param SectIndex :: Sector index
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \param Corner :: corner points to add
  */
{
  const size_t HN=BunkerMainWall::hash(SectIndex,VIndex,RIndex);
  PointMap.emplace(HN,Corners);
  return;
}


  
void
BunkerMainWall::setMaterial(const size_t SectIndex,
			    const size_t VIndex,
			    const size_t RIndex,
			    const std::string& MatName)
  /*!
    Sets a given material element
    \param SectIndex :: Sector index
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \param MatName :: Material to set
   */
{
  ELog::RegMethod RegA("BunkerMainWall","setMaterial");
  
  const size_t HN=BunkerMainWall::hash(SectIndex,VIndex,RIndex);
  MatMap.emplace(HN,MatName);
  return;
}
  


int
BunkerMainWall::getMaterial(const size_t SectIndex,
			    const size_t VIndex,
			    const size_t RIndex,
			    const int defMat) const
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param SectIndex :: Sector index
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \param defMat :: Default material if nothing set
    \return Material Number
  */
{
  ELog::RegMethod RegA("BunkerMainWall","getMaterial");

  const std::string& MName=
    getMatString(SectIndex,VIndex,RIndex);

  return (MName.size()) ? 
    ModelSupport::EvalMatName(MName) : defMat;
}

const std::string&
BunkerMainWall::getMatString(const size_t SectIndex,
			     const size_t VIndex,
			     const size_t RIndex) const
/*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros] 
    \param SectIndex :: Sector indext
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \return Material Name [Empty of fail]
  */
{
  ELog::RegMethod RegA("BunkerMainWall","getMatString");
  static const std::string empty;

  // Need to check a sequence of possibles :
  // Only those that make sense
  size_t HN[6];
  HN[0]=BunkerMainWall::hash(SectIndex,VIndex,RIndex);
  HN[1]=BunkerMainWall::hash(SectIndex,0,RIndex);
  HN[2]=BunkerMainWall::hash(SectIndex,VIndex,0);
  HN[3]=BunkerMainWall::hash(0,VIndex,RIndex);
  HN[4]=BunkerMainWall::hash(0,0,RIndex);
  HN[5]=BunkerMainWall::hash(0,0,0);

  std::map<size_t,std::string>::const_iterator mc=
    MatMap.end();
  for(size_t i=0;i<6 && mc==MatMap.end();i++)
    mc=MatMap.find(HN[i]);

  return (mc!=MatMap.end()) ? mc->second : empty;
}


const std::vector<Geometry::Vec3D>&
BunkerMainWall::getPoints(const size_t SectIndex,
			  const size_t VIndex,
			  const size_t RIndex) const
  /*!
    Get the set of points on a system
    \param SectIndex :: Sector indext
    \param VIndex :: Vertical index
    \param RIndex :: Cylinder index
    \return Points
  */
{
  ELog::RegMethod RegA("BunkerMainWall","getPoints");

  const size_t HN=BunkerMainWall::hash(SectIndex,VIndex,RIndex);
  
  std::map<size_t,std::vector<Geometry::Vec3D>>::const_iterator mc=
    PointMap.find(HN);
  if (mc==PointMap.end())
    throw ColErr::InContainerError<size_t>(HN,"Hash index");
    
  return mc->second;
}

			   
void
BunkerMainWall::writeXML(const std::string& FName,
			 const size_t nSectors,
			 const size_t nVerts,
			 const size_t nLayers) const
  
  /*!
    Carries out the actual writing of an XML file

    \param FName :: Filename for output
    \param nSector :: number of angular sectors
    \param nVerts :: number of Vertical components
    \param nLayers :: number of radial components

   */
{
  ELog::RegMethod RegA("BunkerMainWall","writeXML");

  if (FName.empty())
    {
      ELog::EM<<"No XML to write"<<ELog::endErr;
      return;
    }

  XML::XMLcollect activeXOut;

  activeXOut.addGrp("BunkerMainWall");

  for(size_t i=0;i<nSectors;i++)
    for(size_t j=0;j<nVerts;j++)
      for(size_t k=0;k<nLayers;k++)
	{
	  const std::string& MStr=getMatString(i+1,j+1,k+1);
	  if (!MStr.empty())
	    {
	      activeXOut.addComp("WallMat",MStr);
	      activeXOut.addObjAttribute("Sector",i+1);
	      activeXOut.addObjAttribute("Vertical",j+1);
	      activeXOut.addObjAttribute("Radial",k+1);
	    }
	}
  
  activeXOut.closeGrp();

  std::ofstream WFile(FName.c_str());
  activeXOut.writeXML(WFile);
  WFile.close();

  return;  
}  

} // NAMESPACE essSystem
