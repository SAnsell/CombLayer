/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/surfIndex.cxx
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "support.h"
#include "regexBuild.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "ArbPoly.h"
#include "CylCan.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "General.h"
#include "MBrect.h"
#include "Plane.h"
#include "Sphere.h"
#include "Torus.h"
#include "surfEqual.h"
#include "surfaceFactory.h"
#include "surfRegister.h"
#include "surfIndex.h"

#include "Debug.h"

namespace ModelSupport
{

surfIndex::surfIndex() : uniqNum(1)
  /*!
    Constructor
  */
{}

surfIndex& 
surfIndex::Instance() 
  /*!
    Effective this object			
    \return surfIndex object
  */
{
  static surfIndex A;
  return A;
}

surfIndex::~surfIndex()
  /*!
    Destructor
  */
{
  STYPE::iterator mc;
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    delete mc->second;
}

void
surfIndex::reset()
  /*!
    Big delete
  */
{
  STYPE::iterator mc;
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    delete mc->second;
  SMap.erase(SMap.begin(),SMap.end());
  return;
}

int
surfIndex::getUniq() 
  /*!
    Get a new unique surface number
    \return new surface number
  */
{
  STYPE::const_iterator mc;

  while((mc=SMap.find(uniqNum))!=SMap.end())
    {
      uniqNum++;
    }
  return uniqNum;
}

Geometry::Surface*
surfIndex::addSurface(Geometry::Surface* SPtr)
  /*!
    Adds and manages a surface, creates a unique name 
    if required
    \param SPtr :: surface to check
    \return Surface Ptr
  */
{
  // First off make the surface a uniq
  STYPE::iterator vc=SMap.find(SPtr->getName());
  if (vc!=SMap.end())
    {
      const int NP=getUniq();
      SPtr->setName(NP);
    }
  Geometry::Surface* NewPtr=ModelSupport::equalSurface(SPtr);
  // Now find if we have copy
  if (NewPtr==SPtr)
    SMap.insert(STYPE::value_type(SPtr->getName(),SPtr));
  else
    delete SPtr;

  return NewPtr;
}

template<typename T>
T* 
surfIndex::addTypeSurface(T* SPtr)
  /*!
    Adds and manages a surface, creates a unique name 
    if required. This version returns a specified template 
    type of surface rather than Surface*
    \param SPtr :: surface to check
    \return Surface Ptr
  */
{
  return dynamic_cast<T*>(addSurface(SPtr));
}

void 
surfIndex::insertSurface(Geometry::Surface* SPtr)
  /*!
    Adds and manages a surface (no check)
    \param SPtr :: surface to check
  */
{
  ELog::RegMethod RegA("surfIndex","insertSurface");

  if (!SPtr) 
    throw ColErr::EmptyValue<Geometry::Surface*>("SPtr empty");

  STYPE::iterator vc=SMap.find(SPtr->getName());
  if (vc!=SMap.end())
    {
      throw ColErr::InContainerError<int>
	(SPtr->getName(),"SPtr name");
    }

  SMap.insert(STYPE::value_type(SPtr->getName(),SPtr));

  return;
}

int
surfIndex::findOpposite(const Geometry::Surface* SPtr)  const
  /*!
    Determine if an opposite is needed
    \param SPtr :: Surface object
   */
{
  ELog::RegMethod RegA("surfIndex","findOpposite");

  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (PPtr)
    {
      STYPE::const_iterator mc;
      for(mc=SMap.begin();mc!=SMap.end();mc++)
	if (ModelSupport::oppositeSurfaces(PPtr,mc->second)) 
	  return mc->first;
    }
  return 0;
}

void
surfIndex::removeOpposite(const int SN)
  /*!
    Remove opposite planes from the reverse planes list:
    \param SN :: Surface number
   */
{
  ELog::RegMethod RegA("surfIndex","removeOpposite");

  STYPE::iterator sc=SMap.find(SN);
  if (sc!=SMap.end())
    {
      delete sc->second;
      SMap.erase(sc);
    }

  return;
}

Geometry::Surface*
surfIndex::removeEqualSurface(const int SNum)
  /*!
    Remove a surface if an equivilent exists
    \param SNum :: surface to check
    \return Surface Ptr
  */
{
  ELog::RegMethod RegA("surfIndex","removeEqualSurface");
  // First off make the surface a uniq
  STYPE::iterator vc=SMap.find(SNum);
  if (vc==SMap.end())
    return 0;
  
  Geometry::Surface* NewPtr=
    ModelSupport::equalSurface(vc->second);
  
  if (NewPtr!=vc->second)
    {
      delete vc->second;
      SMap.erase(vc);
    }
  return NewPtr;
}

Geometry::Surface*
surfIndex::cloneSurface(const Geometry::Surface* SPtr)
  /*!
    Adds a surface clone object
    \param SPtr :: surface to check
    \return Surface Ptr
  */
{
  Geometry::Surface* SOut=SPtr->clone();
  return addSurface(SOut);
}

template<typename T>
T*
surfIndex::createSurf(const int surfN)
  /*!
    Helper function to determine the surface object
    given a particular surface number and create if not in 
    existence. 
    \param surfN : Number of the Surface to create
    \returns Surface pointer 
  */
{
  ELog::RegMethod RegA("surfIndex","createSurf<"
		       +T::classType()+std::string(">"));
  
  T* outPtr;
  
  STYPE::iterator mp=SMap.find(surfN);
  if (mp!=SMap.end())
    {
      outPtr=dynamic_cast<T*>(mp->second);
      if (outPtr)
	return outPtr;
      delete mp->second;
      outPtr=new T(surfN,0);
      mp->second=outPtr;
      ELog::EM<<"Reasigned exiting surface"<<surfN<<ELog::endWarn;
      return outPtr;
    }
  outPtr=new T(surfN,0);
  SMap.insert(STYPE::value_type(surfN,outPtr));
  return outPtr;
}

template<typename T>
T*
surfIndex::createUniqSurf(const int surfN)
  /*!
    Helper function to determine the surface object
    given a particular surface number and create if not in 
    existence. 
    \param surfN : Number of the Surface to create or greater
    \returns Surface pointer 
  */
{
  int surfX(surfN);
  return createNewSurf<T>(surfX);
}

template<typename T>
T*
surfIndex::createNewSurf(int& surfN)
  /*!
    Helper function to determine the surface object
    given a particular surface number and create if not in 
    existence. Does NOT insert the surface into the map.

    \param surfN : Number of the Surface to create or greater
    \returns Surface pointer 
  */
{
  ELog::RegMethod RegA("surfIndex","createNewSurf<"
		       +T::classType()+">");
  
  STYPE::const_iterator mp=SMap.find(surfN);
  while (mp!=SMap.end())
    {
      surfN++;
      mp=SMap.find(surfN);
    }
  T* outPtr=new T(surfN,0);
  return outPtr;
}

void
surfIndex::createSurface(const int SN,const std::string& SLine)
  /*!
    Given a surface adds the line
    \param SN :: Surface Number
    \param SLine :: Line to use.
  */
{
  createSurface(SN,0,SLine);
  return;
}

void
surfIndex::createSurface(const int SN,const int TN,const std::string& SLine)
  /*!
    Given a surface adds the line
    \param SN :: Surface Number
    \param TN :: Transform number
    \param SLine :: Line to use.
  */
{
  ELog::RegMethod RegA("surfIndex","createSurface");

  Geometry::Surface* SPtr=
    Geometry::surfaceFactory::Instance().processLine(SLine);
  SPtr->setName(SN);
  SPtr->setTrans(TN);
  STYPE::iterator mc=SMap.find(SN);
  if (mc!=SMap.end())
    throw ColErr::InContainerError<int>(SN,"Surface in use");
  SMap.emplace(SN,SPtr);
  return; 
}

void
surfIndex::deleteSurface(const int surfN)
  /*!
    Delete a specific surface
    \param surfN :: surface number
   */
{
  ELog::RegMethod RegA("surfIndex","deleteSurface");

  STYPE::iterator mf=SMap.find(surfN);
  if (mf==SMap.end())
    throw ColErr::InContainerError<int>(surfN,"surfN");

  delete mf->second;
  SMap.erase(mf);

  return;
}

int
surfIndex::checkSurface(const int Snum,const Geometry::Vec3D& Pt) const
  /*!
    Returns the value of the surface sence realtive to the point 
    \param Snum :: Surface key number to check
    \param Pt :: Point to check
    \retval +/-1 dependint on side of Pt to surface
    \retval 0  No surface found 
  */
{
  STYPE::const_iterator mf=SMap.find(Snum);
  if (mf==SMap.end())
    throw ColErr::InContainerError<int>(Snum,"surfIndex::checkSurface");

  return mf->second->side(Pt);
}

Geometry::Surface*
surfIndex::getSurf(const int Index) const
  /*!
    Get a surface based on the index
    \param Index :: Index varaible
    \return Surface Ptr / 0
   */
{
  STYPE::const_iterator mc=SMap.find(Index);
  return (mc==SMap.end()) ? 0 : mc->second;
}

void 
surfIndex::renumber(const int origNum,const int newNum)
  /*!
    Convert from origNumb to new number
    \param origNum :: original number
    \param newNum :: New number
  */
{
  ELog::RegMethod RegA("surfIndex","renumber");
  STYPE::iterator mc=SMap.find(origNum);
  if (mc==SMap.end())
    {
      ELog::EM<<"Surface "<<origNum<<" does not exist"<<ELog::endWarn;
      return;
    }
  Geometry::Surface* SPtr=mc->second;
  SMap.erase(mc);
  SPtr->setName(newNum);
  insertSurface(SPtr);
  return;
}

int
surfIndex::calcRenumber(const int allowedSurf,
			std::vector<std::pair<int,int> >& ChangeList) const
  /*!
    Calculate the changes required for a renumbering
    \param allowedSurf :: Below this number surfaces left unmoved
    \param ChangeList :: vector of original:new
    \return surface number
   */
{
  ELog::RegMethod RegA("surfIndex","renumberSurfaces");

  int index(1);
  int cnt(0);
  STYPE::const_iterator mc;
  // Get Total size
  long int SSize=static_cast<long int>(SMap.size());
  if (SSize<allowedSurf)
    SSize=allowedSurf;
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    {
      if (mc->first>SSize)        // Only change if outside range
	{
	  // Get free index point
	  while(SMap.find(index)!=SMap.end())
	    index++;
	  mc->second->setName(index);
	  ChangeList.push_back(std::pair<int,int>(mc->first,index));
	  index++;
	  cnt++;
	}
    }
  return cnt;
}

int
surfIndex::calcRenumber(const int rLow,const int rHigh,int offset,
			std::vector<std::pair<int,int> >& ChangeList) const
  /*!
    Calculate the changes required for a renumbering
    \param rLow :: lowRange number to put into offset
    \param rHigh :: highRange number to put into offset
    \param offset :: offset number
    \param ChangeList :: vector of original:new
    \return surface number
   */
{
  ELog::RegMethod RegA("surfIndex","renumberSurfaces");

  int index(1);
  int cnt(0);
  STYPE::const_iterator mc;
  // Get Total size:
  const int SSize=static_cast<int>(SMap.size());
  if (offset<=SSize)  
    offset=SSize+1000; 

  // Surfaces that are either at (rLow->rHigh)+offset
  // are not moved.

  // Objective is to move those surface that are within
  // the list to the NEXT available.
  // The problem is that (a) -> (b) and (b)->(A)
  
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    {
      if (mc->first<rLow || mc->first>rHigh)
	{
	  if (mc->first>=SSize)
	    {
	      // Get free index point
	      while(SMap.find(index)!=SMap.end())
		index++;
	      mc->second->setName(index);
	      ChangeList.push_back(std::pair<int,int>(mc->first,index));
	      index++;
	      cnt++;
	    }
	}
      else  
	{
	  // Avoid a=>b b=>a mapping
	  int newNum=offset+mc->first-rLow;
	  ChangeList.push_back(std::pair<int,int>(mc->first,newNum));
	  mc->second->setName(newNum);
	  cnt++;
	}
    }
  return cnt;
}

int
surfIndex::calcRenumber(const std::vector<int>& rLow,
			const std::vector<int>& rRange,
			int offset,
			std::vector<std::pair<int,int> >& ChangeList) const
  /*!
    Calculate the changes required for a renumbering
    \param rLow :: lowRange number to put into offset
    \param rHigh :: highRange number to put into offset
    \param offset :: offset number
    \param ChangeList :: vector of original:new
    \return surface number
   */
{
  ELog::RegMethod RegA("surfIndex","renumberSurfaces");

  int index(1);
  int cnt(0);
  STYPE::const_iterator mc;
  // Get Total size:
  const int SSize=static_cast<int>(SMap.size());
  if (offset<=SSize)  
    offset=SSize+1000; 

  // Surfaces that are either at (rLow->rHigh)+offset
  // are not moved.

  // Objective is to move those surface that are within
  // the list to the NEXT available.
  // The problem is that (a) -> (b) and (b)->(A)
  
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    {
      size_t i;
      for(i=0;i<rLow.size() && 
	    (mc->first < rLow[i] || mc->first > (rLow[i]+rRange[i]));
	  i++) ;
      if (i==rLow.size())
	{
	  if (mc->first>=SSize)
	    {
	      // Get free index point
	      while(SMap.find(index)!=SMap.end())
		index++;
	      mc->second->setName(index);
	      ChangeList.push_back(std::pair<int,int>(mc->first,index));
	      index++;
	      cnt++;
	    }
	}
      else  // Hit in range
	{
	  // Avoid a=>b b=>a mapping
	  const int newNum=offset*static_cast<int>(i+1)+
	    mc->first - rLow[i];
	  ChangeList.push_back(std::pair<int,int>(mc->first,newNum));
	  mc->second->setName(newNum);
	  cnt++;
	}
    }
  return cnt;
}

bool
surfIndex::mapValid() const
  /*!
    Check to see if the map is valid
    \return true if good and false if not
  */
{
  ELog::RegMethod RegA("surfIndex","mapValid");
  bool out(1);
  STYPE::const_iterator mc;
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    if (mc->first!=mc->second->getName())
      {
	out=0;
	ELog::EM<<"Error with surface "<<mc->first<<" "
		<<mc->second->getName()<<ELog::endErr;
	throw ColErr::ExitAbort("Map check");
      }
  return out;
}

void
surfIndex::setKeep(const int SN,const int status)
  /*!
    \param SN :: Surface number
    \param status :: Status number
   */
{
  ELog::RegMethod RegA("surfIndex","setKeep");
  if (!status)
    {
      std::map<int,int>::iterator mc=holdMap.find(SN);
      if (mc!=holdMap.end())
	holdMap.erase(mc);
    }
  else
    holdMap.insert(std::map<int,int>::value_type(SN,status/abs(status)));

  return;
}

int
surfIndex::keepFlag(const int SN) const
  /*!
    Given a surface number determine if we write this cell
    \retval 1 :: hold and write
    \retval -1 :: hold and no-write
    \retval 0 :: Dont hold
   */
{
  std::map<int,int>::const_iterator mc=holdMap.find(abs(SN));
  return (mc==holdMap.end()) ? 0 : mc->second;
}

std::vector<int>
surfIndex::keepVector() const
  /*!
    Return the set of kept objects
    \return vector
  */
{

  std::vector<int> Out;
  std::map<int,int>::const_iterator mc;
  for(mc=holdMap.begin();mc!=holdMap.end();mc++)
    Out.push_back(mc->first);
  return Out;
}

int
surfIndex::readOutputSurfaces(const std::string& FName)
  /*!
    Reads the surface from the output file from MCNPX
    \param FName :: File to read
    \return -ve on error / 0 on success
  */
{
  ELog::RegMethod RegA("surfIndex","readOutputSurfaces");

  int Scount(0);

#ifndef NO_REGEX

  if (FName.empty()) return -1;
  std::ifstream IX(FName.c_str(),std::ios::in);
  //failed to open
  if (!IX) 
    {
      ELog::EM<<"Failed to find file "<<FName<<ELog::endErr;
      return -1;
    }

  const std::string startSea("SURFACE CARDS");
  const std::string surfUnit("^\\s*(\\d+)-\\s+(\\S.*)$");
  const std::string stopSea("\\+\\+\\s*END\\s*\\+\\+");

  std::string InputLine;
  std::string Line;
  if (!StrFunc::findPattern(IX,startSea,InputLine))
    {
      ELog::EM<<"No start"<<ELog::endErr;
      return -2;
    }
  
  std::string readLine=StrFunc::getLine(IX);
  Line="";
  std::vector<std::string> errLine;
  while(IX.good() && !StrFunc::StrLook(readLine,stopSea))
    {
      if (StrFunc::StrComp(readLine,surfUnit,InputLine,1))
	{
	  Line+=InputLine+" ";
	  const int monoLine=processSurfaces(Line);
	  // Good line / comment / void surface
	  if (monoLine==1 || monoLine==2 || monoLine==3)   
	    {
	      Line="";
	      errLine.clear();
	      Scount++;
	    }
	  else if ((!errLine.empty() && monoLine>1) ||
		   errLine.size()>4)
	    {
	      ELog::EM<<"Error with line grp:"<<ELog::endCrit;
	      for(unsigned int i=0;i<errLine.size();i++)
		ELog::EM<<"   "<<errLine[i]<<ELog::endCrit;
	      throw ColErr::InvalidLine(Line,"Line",0);
	    }
	  else
	    {
	      errLine.push_back(InputLine);
	    }
	}
      readLine=StrFunc::getLine(IX);
    }
  IX.close();
  ELog::EM<<"Read in "<<Scount<<" surfaces"<<ELog::endDiag;

#endif

  return (Scount) ? 0 : -3;
}

int
surfIndex::processSurfaces(const std::string& InputLine)
/*! 
  Job is to decide which type of surface the 
  current line is attached too. It also must decide if 
  there is enough information on the current line 
  \param InputLine :: Line to process (Stripped of comments on return)
  \retval 0 More information required
  \retval -1 failed
  \retval 1 good
  \retval 2 pure comment 
  \retval 3 valid but void surface
*/
{
  ELog::RegMethod RegItem("SurfData","processSurface");

  std::string Line=StrFunc::fullBlock(InputLine);
  StrFunc::stripComment(Line);
  if (Line.size()<1 ||               // comments blank line, ^c or ^c<spc> 
      (tolower(Line[0])=='c' && 
       (Line.size()==1 || isspace(Line[1])) ))
    return 2;

  StrFunc::lowerString(Line);  
  int name;
  if (!StrFunc::section(Line,name) || Line.empty())
    return -1; 
  
  // Get transform if it exists.
  int transN(0);
  StrFunc::section(Line,transN);

  createSurface(name,transN,Line);

  return 1;
}

int
surfIndex::findEqualSurf(const int sBegin,const int sEnd,
			 std::map<int,Geometry::Surface*>& EQMap) const
  /*!
    Loop [long] though and find equal surfaces
    \param sBegin :: begining number
    \param sBegin :: end number
    \param EQMap :: Map of equal surfaces
    \return number found
   */
{
  typedef std::map<int,Geometry::Surface*> EQTYPE;
  STYPE::const_iterator mc;
  STYPE::const_iterator nc;
  for(mc=SMap.begin();mc!=SMap.end();mc++)
    {
      if (mc->first>=sBegin && mc->first<sEnd)
	{
	  for(nc=SMap.begin();nc!=SMap.end();nc++)
	    {
	      if ((nc->first<sBegin || nc->first>mc->first) &&
		  ModelSupport::cmpSurfaces(mc->second,nc->second))
		{
		  EQMap.insert(EQTYPE::value_type(mc->first,nc->second));
		  break;
		}		  
	    }
	}
    }
  return static_cast<int>(EQMap.size());
}




///\cond TEMPLATE 

template Geometry::Sphere* 
surfIndex::createSurf<Geometry::Sphere>(const int);
template Geometry::Cone* 
surfIndex::createSurf<Geometry::Cone>(const int);
template Geometry::Cylinder* 
surfIndex::createSurf<Geometry::Cylinder>(const int);
template Geometry::EllipticCyl* 
surfIndex::createSurf<Geometry::EllipticCyl>(const int);
template Geometry::Plane* 
surfIndex::createSurf<Geometry::Plane>(const int);

template Geometry::Sphere* 
surfIndex::createUniqSurf<Geometry::Sphere>(const int);
template Geometry::Cone* 
surfIndex::createUniqSurf<Geometry::Cone>(const int);
template Geometry::Cylinder* 
surfIndex::createUniqSurf<Geometry::Cylinder>(const int);
template Geometry::EllipticCyl* 
surfIndex::createUniqSurf<Geometry::EllipticCyl>(const int);
template Geometry::Plane* 
surfIndex::createUniqSurf<Geometry::Plane>(const int);

template Geometry::General* 
surfIndex::createUniqSurf<Geometry::General>(const int);

template Geometry::Sphere* 
surfIndex::createNewSurf<Geometry::Sphere>(int&);
template Geometry::Cylinder* 
surfIndex::createNewSurf<Geometry::Cylinder>(int&);
template Geometry::EllipticCyl* 
surfIndex::createNewSurf<Geometry::EllipticCyl>(int&);
template Geometry::Cone* 
surfIndex::createNewSurf<Geometry::Cone>(int&);
template Geometry::Plane* 
surfIndex::createNewSurf<Geometry::Plane>(int&);
template Geometry::General* 
surfIndex::createNewSurf<Geometry::General>(int&);

template Geometry::Plane*
surfIndex::addTypeSurface(Geometry::Plane*);
template Geometry::Cylinder*
surfIndex::addTypeSurface(Geometry::Cylinder*);

///\endcond TEMPLATE

} // NAMESPACE ModelSupport
