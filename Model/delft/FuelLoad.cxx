/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/FuelLoad.cxx
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

#include "FuelLoad.h"

namespace delftSystem
{

size_t 
FuelLoad::hash(const size_t XGrid,const size_t YGrid,
	       const size_t Element,const size_t Index) 
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param XGrid :: A-F grid position
    \param YGrid :: 1-N grid position
    \param Element :: Numbered plate index
    \param Index :: Vertical position in fuel
    \return Hash number
  */
{
  //Numbers are 64 64 256 256
  return 4194304UL*XGrid+65536UL*YGrid+256UL*Element+Index;
}

size_t 
FuelLoad::hash(const std::string& GName,
	       const size_t Element,
	       const size_t Index) 
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param GName :: A-F/Number grid position
    \param Element :: Numbered plate index
    \param Index :: Vertical position in fuel
    \return Hash number
  */
{
  if (GName.size()>=2)
    {
      
      const size_t XG=(GName[0]=='-') ? 0 :
	static_cast<size_t>(tolower(GName[0])-'a')+1;
      if (XG<27)
	{
	  size_t YG(0);
	  if (StrFunc::convert(GName.substr(1),YG))
	    YG++;
	  if (YG<27 && (YG || GName[1]=='-'))
	    return FuelLoad::hash(XG,YG,Element,Index);
	}
    }
  throw ColErr::InvalidLine(GName,"Hash::GName");
}

std::string
FuelLoad::gridName(const size_t NX,const size_t NY)
  /*!
    Calcuate the grid name based on NX / NY
    \param NX :: NX grid position [1-N] 
    \param NY :: NY grid position [1-N]
    \return GridName
   */
{
  std::string Out;
  Out=(!NX) ? '-' : static_cast<char>('A'+(NX-1));
  Out+=(!NY) ? "-" : StrFunc::makeString(NY-1);
  return Out;
}

FuelLoad::FuelLoad() :
  activeXOut(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

FuelLoad::FuelLoad(const FuelLoad& A) : 
  FuelMap(A.FuelMap),activeXOut(0)
  /*!
    Copy constructor
    \param A :: FuelLoad to copy
  */
{}

FuelLoad&
FuelLoad::operator=(const FuelLoad& A)
  /*!
    Assignment operator
    \param A :: FuelLoad to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FuelMap=A.FuelMap;
    }
  return *this;
}

FuelLoad::~FuelLoad()
  /*!
    Destructor
  */
{
  delete activeXOut;
}

int
FuelLoad::loadXML(const std::string& FName) 
  /*!
    Process an XML file to set/add variables
    Blade number is 1 - NE 
    Index Number if 1 - NI
    Grid Item : A1 - > GN  
    
    \param FName :: filename 
    
    \return 1 on success / 0 on fail
  */
{
  ELog::RegMethod RegA("FuelLoad","loadXML");

  ELog::EM<<"Load fuel "<<FName<<ELog::endDiag;
  XML::XMLcollect CO;
  if (FName.empty() || CO.loadXML(FName))
    return 0;

  FuelMap.clear();
  // Check parameters;
  XML::XMLobject* AR;
  // Parse for variables:
  AR=CO.findObj("Fuel");
  std::string MatName;
  while(AR)
    {
      const std::string GridItem=AR->getItem<std::string>("Grid");
      
      const size_t bladeN=AR->getDefItem<size_t>("Blade",0);
      const size_t IndexN=AR->getDefItem<size_t>("Index",0);
      MatName= AR->getNamedItem<std::string>("Material");
      MatName=StrFunc::fullBlock(MatName);
      const size_t HN=FuelLoad::hash(GridItem,bladeN,IndexN);
      FuelMap.emplace(HN,MatName);
      CO.deleteObj(AR);      
      AR=CO.findObj("Fuel");
    }

  // PARSE MATERIALS:

  XML::XMLgroup* BurnUp=CO.findGroup("Burnup");
  if (BurnUp)
    AR=BurnUp->findObj("Material");

  while(AR)
    {
      const std::string GridItem=AR->getItem<std::string>("Grid");
      const size_t IndexN=AR->getDefItem<size_t>("Index",0);
      MatName= AR->getNamedItem<std::string>("Material");
      MatName=StrFunc::fullBlock(MatName);
      const size_t HN=FuelLoad::hash(GridItem,0,IndexN);
      FuelMap.emplace(HN,MatName);
      CO.deleteObj(AR);
      AR=BurnUp->findObj("Material");
    }
  return (FuelMap.empty()) ? 0 : 1;
}

void
FuelLoad::setMaterial(const size_t XGrid,const size_t YGrid,
		      const size_t Element,const size_t Index,
		      const std::string& MatName)
  /*!
    Sets a given fuel element
    \param XGrid :: Letter position [as a number]
    \param YGrid :: Number index
    \param Element ::  Element number [plate in XY grid cell]
    \param Index :: Index number [grid on element]
   */
{
  ELog::RegMethod RegA("FuelLoad","setMaterial");
  
  const size_t HN=FuelLoad::hash(XGrid,YGrid,Element,Index);
  FuelMap.insert(std::map<size_t,std::string>::value_type(HN,MatName));
  return;
}
  


int
FuelLoad::getMaterial(const size_t XGrid,const size_t YGrid,
		      const size_t Element,const size_t Index,
		      const int defMat) const
  /*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros
    \param XGrid :: A-F grid position  [0 def]
    \param YGrid :: 1-N grid position  [0 def[
    \param Element :: Numbered plate index [0 def]
    \param Index :: Vertical position in fuel [0 def]
    \param defMat :: Default material if nothing set
    \return Material Number
  */
{
  ELog::RegMethod RegA("FuelLoad","getMaterial");

  const std::string& MName=
    getMatString(XGrid,YGrid,Element,Index);

  return (MName.size()) ? 
    ModelSupport::EvalMatName(MName) : defMat;
}

const std::string&
FuelLoad::getMatString(const size_t XGrid,const size_t YGrid,
		       const size_t Element,const size_t Index) const
/*!
    Calculate the hash from the input data [NOTE: Some of the 
    data is general so must accept zeros] 
    \param XGrid :: A-F grid position
    \param YGrid :: 1-N grid position
    \param Element :: Numbered plate index
    \param Index :: Vertical position in fuel
    \return Material Name [Empty of fail]
  */
{
  ELog::RegMethod RegA("FuelLoad","getMatString");
  static const std::string empty;

  // Need to check a sequence of possibles :
  size_t HN[5];
  HN[0]=FuelLoad::hash(XGrid,YGrid,Element,Index);
  HN[1]=FuelLoad::hash(XGrid,YGrid,Element,0);
  HN[2]=FuelLoad::hash(XGrid,YGrid,0,Index);
  HN[3]=FuelLoad::hash(XGrid,YGrid,0,0);
  HN[4]=FuelLoad::hash(0,0,0,0);
  std::map<size_t,std::string>::const_iterator mc=
    FuelMap.end();
  for(size_t i=0;i<5 && mc==FuelMap.end();i++)
    mc=FuelMap.find(HN[i]);

  return (mc!=FuelMap.end()) ? mc->second : empty;
}

void
FuelLoad::createActive()
  /*!
    Create the active system if not currente
   */
{
  ELog::RegMethod RegA("FuelLoad","creatActive");
  delete activeXOut;
  activeXOut=new XML::XMLcollect();
  activeXOut->addGrp("FuelLoad");
  
  
  return;
}


void
FuelLoad::addXML(const size_t nx,const size_t ny,
		 const size_t nElements,const size_t nCells,
		 const std::set<size_t>& exclude) 
  /*!
    Write out the XML file [complete view currently]
    \param nx :: Index of Grid Position
    \param ny :: Index of Grid Position
    \param nElements :: Elements in this cell
    \param nCells :: Cell dividion of this cell
    \param exclude :: Cells not unsed
  */
{
  ELog::RegMethod RegA("FuelLoad","addXML");

  if(!activeXOut) createActive();
  
  // First do the defaults:
  std::map<size_t,std::string>::const_iterator mc;
  for(size_t ne=1;ne<=nElements;ne++)
    {
      if (exclude.find(ne-1)==exclude.end())
	{
	  for(size_t ncell=1;ncell<=nCells;ncell++)
	    {
	      const std::string& MStr=getMatString(nx+1,ny+1,ne,ncell);
	      if (MStr.size())
		{
		  activeXOut->addComp("Fuel",MStr);
		  activeXOut->addObjAttribute("Grid",gridName(nx+1,ny+1));
		  activeXOut->addObjAttribute("Blade",ne);
		  activeXOut->addObjAttribute("Index",ncell);
		}
	    }
	}
    }
  return;
}

void
FuelLoad::writeXML(const std::string& FName)
  /*!
    Carries out the actual writing of an XML file
    \param FName :: Filename for output
   */
{
  ELog::RegMethod RegA("FuelLoad","writeXML");
  if (!activeXOut || FName.empty())
    {
      ELog::EM<<"No XML to write"<<ELog::endErr;
      return;
    }
  activeXOut->closeGrp();
  std::ofstream WFile(FName.c_str());
  activeXOut->writeXML(WFile);

  
  WFile.close();

  delete activeXOut;
  activeXOut=0;
  return;
  
  
}

} // NAMESPACE delftSystem
