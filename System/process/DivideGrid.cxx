/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/DivideGrid.cxx
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

#include "DivideGrid.h"

namespace ModelSupport
{

size_t 
DivideGrid::hash(const size_t AIndex,
		 const size_t BIndex,
		 const size_t CIndex)
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param AIndex :: Sector index
    \param BIndex :: Vertical index
    \param CIndex :: Cylinder index
    \return Hash number
  */
{
  //Numbers are 64 256 256
  return 65536UL*AIndex+256UL*BIndex+CIndex;
}

DivideGrid::DivideGrid(const std::string& defMat) :
  IJKnames({"Sector","Vertical","Radial"})
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param defMat :: default material
  */
{
  MatMap.emplace(0,defMat);
}

DivideGrid::DivideGrid(const DivideGrid& A) : 
  IJKnames(A.IJKnames),
  MatMap(A.MatMap),PointMap(A.PointMap)
  /*!
    Copy constructor
    \param A :: DivideGrid to copy
  */
{}

DivideGrid&
DivideGrid::operator=(const DivideGrid& A)
  /*!
    Assignment operator
    \param A :: DivideGrid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      IJKnames=A.IJKnames;
      MatMap=A.MatMap;
      PointMap=A.PointMap;
    }
  return *this;
}

DivideGrid::~DivideGrid()
  /*!
    Destructor
  */
{}

void
DivideGrid::setKeyNames(const std::string& AN,
			const std::string& BN,
			const std::string& CN)
  /*!
    Set the key names for each sector
    \param AN :: I index name
    \param BN :: J index name
    \param CN :: K index name
   */
{
  IJKnames[0]=AN;
  IJKnames[1]=BN;
  IJKnames[2]=CN;
  return;
}
  
int
DivideGrid::loadXML(const std::string& FName,
		    const std::string& objName) 
  /*!
    Process an XML file to set/add variables
    
    \param FName :: filename 
    \param objName :: Object name
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("DivideGrid","loadXML");
  
  XML::XMLcollect CO;
  if (FName.empty() || CO.loadXML(FName))
    return 0;


  MatMap.clear();
  // Check parameters;
  XML::XMLobject* AR;
  // Parse for variables:
  AR=CO.findObj(objName);
  std::string MatName;
  if (!AR)
    throw ColErr::InContainerError<std::string>(objName,"ObjName not in XML");

  while(AR)
    {

      std::vector<size_t> SVec,VVec,RVec;
      std::string SStr=AR->getItem<std::string>(IJKnames[0]);    
      std::string VStr=AR->getItem<std::string>(IJKnames[1]);
      std::string RStr=AR->getItem<std::string>(IJKnames[2]);

      // Input form is for type A:B:C / A,B,C
      // Take input and convert the range into number

      StrFunc::sectionRange(SStr,SVec);
      StrFunc::sectionRange(VStr,VVec);
      StrFunc::sectionRange(RStr,RVec);


      for(const size_t SN : SVec)
	for(const size_t VN : VVec)
	  for(const size_t RN : RVec)
	    {
	      MatName= AR->getNamedItem<std::string>("Material");
	      MatName=StrFunc::fullBlock(MatName);
	      const size_t HN=DivideGrid::hash(SN,VN,RN);
	      std::map<size_t,std::string>::iterator mc=MatMap.find(HN);
	      if (mc!=MatMap.end())
		mc->second=MatName;
	      else
		MatMap.emplace(HN,MatName);
	    }
      CO.deleteObj(AR);      
      AR=CO.findObj(objName);
    }
  return (MatMap.empty()) ? 0 : 1;
}

void
DivideGrid::setPoints(const size_t AIndex,
		      const size_t BIndex,
		      const size_t CIndex,
		      const std::vector<Geometry::Vec3D>& Corners)
  /*!
    Sets a point system
    \param AIndex :: Sector index
    \param BIndex :: Vertical index
    \param CIndex :: Cylinder index
    \param Corner :: corner points to add
  */
{
  const size_t HN=DivideGrid::hash(AIndex,BIndex,CIndex);
  PointMap.emplace(HN,Corners);
  return;
}


  
void
DivideGrid::setMaterial(const size_t AIndex,
			    const size_t BIndex,
			    const size_t CIndex,
			    const std::string& MatName)
  /*!
    Sets a given material element
    \param AIndex :: Sector index
    \param BIndex :: Vertical index
    \param cIndex :: Cylinder index
    \param MatName :: Material to set
   */
{
  ELog::RegMethod RegA("DivideGrid","setMaterial");
  
  const size_t HN=DivideGrid::hash(AIndex,BIndex,CIndex);
  MatMap.emplace(HN,MatName);
  return;
}
  
int
DivideGrid::getMaterial(const size_t AIndex,
			const size_t BIndex,
			const size_t CIndex) const

  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param AIndex :: Sector index
    \param BIndex :: Vertical index
    \param CIndex :: Cylinder index
    \return Material Number
  */
{
  ELog::RegMethod RegA("DivideGrid","getMaterial");

  const std::string& MName=
    getMatString(AIndex,BIndex,CIndex);

  return (MName.size()) ? 
    ModelSupport::EvalMatName(MName) : 0;
}

const std::string&
DivideGrid::getMatString(const size_t AIndex,
			 const size_t BIndex,
			 const size_t CIndex) const
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros] 
    \param AIndex :: Sector indext
    \param BIndex :: Vertical index
    \param CIndex :: Cylinder index
    \return Material Name [Empty of fail]
  */
{
  ELog::RegMethod RegA("DivideGrid","getMatString");
  static const std::string empty;

  // Need to check a sequence of possibles :
  // Only those that make sense
  size_t HN[8];
  HN[0]=DivideGrid::hash(AIndex,BIndex,CIndex);
  HN[1]=DivideGrid::hash(AIndex,BIndex,0);
  HN[2]=DivideGrid::hash(AIndex,0,CIndex);
  HN[3]=DivideGrid::hash(0,BIndex,CIndex);
  HN[4]=DivideGrid::hash(AIndex,0,0);
  HN[5]=DivideGrid::hash(0,BIndex,0);
  HN[6]=DivideGrid::hash(0,0,CIndex);
  HN[7]=DivideGrid::hash(0,0,0);

  std::map<size_t,std::string>::const_iterator mc=
    MatMap.end();
  for(size_t i=0;i<8 && mc==MatMap.end();i++)
    mc=MatMap.find(HN[i]);

  return (mc!=MatMap.end()) ? mc->second : empty;
}


const std::vector<Geometry::Vec3D>&
DivideGrid::getPoints(const size_t AIndex,
		      const size_t BIndex,
		      const size_t CIndex) const
  /*!
    Get the set of points on a system
    \param AIndex :: Sector indext
    \param BIndex :: Vertical index
    \param CIndex :: Cylinder index
    \return Points
  */
{
  ELog::RegMethod RegA("DivideGrid","getPoints");

  const size_t HN=DivideGrid::hash(AIndex,BIndex,CIndex);
  
  std::map<size_t,std::vector<Geometry::Vec3D>>::const_iterator mc=
    PointMap.find(HN);
  if (mc==PointMap.end())
    throw ColErr::InContainerError<size_t>(HN,"Hash index");
    
  return mc->second;
}

			   
void
DivideGrid::writeXML(const std::string& FName,
		     const std::string& objName,
		     const size_t nA,
		     const size_t nB,
		     const size_t nC) const
  
  /*!
    Carries out the actual writing of an XML file

    \param FName :: Filename for output
    \param nA :: number of angular sectors
    \param nB :: number of Vertical components
    \param nC :: number of radial components

   */
{
  ELog::RegMethod RegA("DivideGrid","writeXML");

  if (FName.empty())
    {
      ELog::EM<<"No XML to write"<<ELog::endErr;
      return;
    }

  XML::XMLcollect activeXOut;

  activeXOut.addGrp("DivideGrid");
  for(size_t i=0;i<nA;i++)
    for(size_t j=0;j<nB;j++)
      for(size_t k=0;k<nC;k++)
	{
	  const std::string& MStr=getMatString(i+1,j+1,k+1);
	  if (!MStr.empty())
	    {
	      activeXOut.addComp(objName,MStr);
	      activeXOut.addObjAttribute(IJKnames[0],i+1);
	      activeXOut.addObjAttribute(IJKnames[1],j+1);
	      activeXOut.addObjAttribute(IJKnames[2],k+1);
	    }
	}
  
  activeXOut.closeGrp();

  std::ofstream WFile(FName.c_str());
  activeXOut.writeXML(WFile);
  WFile.close();

  return;  
}  

} // NAMESPACE ModelSupport
