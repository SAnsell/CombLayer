/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Object.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Track.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Token.h"
#include "neutron.h"
#include "RuleCheck.h"
#include "objectRegister.h"
#include "masterWrite.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Object.h"

#include "Debug.h"

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OX,const Object& A)
/*!
  Standard Output stream
  \param OX :: Output stream
  \param A :: Object to write
  \return OX
 */

{
  A.write(OX);
  return OX;
}

bool
Object::keyUnit(std::string& Ln,std::string& Key,
		std::string& value)
/*!
    Given a string extract the first XXX = YYY parts
    \param Ln :: Line to extract and cut
    \param Key :: Key component [left side/xxx]
    \param value :: Value component [right side/yyy]
    \return true if unit found.
   */
{
  std::string::size_type posA=Ln.find('=');
  std::string::size_type posB(posA);
  if (posA==std::string::npos)
    return 0;

  Key.clear();
  int spc(0);
  for(;posA!=0 && spc!=2;posA--)
    {
      if (isspace(Ln[posA-1]))
	spc=(Key.empty()) ? 1 : 2;
      else
	Key+=Ln[posA-1];
    }
  if (Key.empty())
    return 0;
  std::reverse(Key.begin(),Key.end());
  std::transform(Key.begin(),Key.end(),
		 Key.begin(),::tolower);
  if (spc==2) posA++;
  
  value.clear();
  spc=0;
  for(posB++;posB!=Ln.size() && spc!=2;posB++)
    {
      if (isspace(Ln[posB]))
	spc=(value.empty()) ? 1 : 2;
      else
	value+=Ln[posB];
    }
  if (value.empty())
    return 0;

  Ln.erase(posA,posB-posA);  
  return 1;
}

int
Object::startLine(const std::string& Line) 
  /*!
    Static object to deterimine if the
    input string 'Line' starts an object 
    \param Line :: Object to test
    \retval 1 :: Start
    \retval 0 :: Not the start of aline
  */
{
  int n,matN;
  double rho;
  // Need line of type id matNumber
  std::string Tst=Line;
  if (StrFunc::section(Tst,n) && n>0 &&           // must start id + matN
      StrFunc::section(Tst,matN) && matN>=0)      
    {
      if (matN==0)
	return 1;

      std::string A;
      if (StrFunc::section(Tst,A) &&  StrFunc::convert(A,rho))
	{
	  if (A.find('.')!=std::string::npos &&    // avoid problem of 0.0
	      rho<10.0)
	    return 1;
	}
    }

  return 0;         
}

Object::Object() :
  ObjName(0),listNum(-1),Tmp(300),MatN(-1),fill(0),trcl(0),
  universe(0),imp(1),density(0.0),placehold(0),populated(0),
  objSurfValid(0)
 /*!
   Defaut constuctor, set temperature to 300C and material to vacuum
 */
{}

Object::Object(const int N,const int M,const double T,
	       const std::string& Line) :
  ObjName(N),listNum(-1),Tmp(T),MatN(M),fill(0),trcl(0),
  universe(0),imp(1),density(0.0),placehold(0),
  populated(0),objSurfValid(0)
 /*!
   Constuctor, set temperature to 300C 
   \param N :: number
   \param M :: material
   \param T :: temperature (K)
   \param Line :: Line to use
 */
{
  HRule.procString(Line);
}

Object::Object(const Object& A) :
  ObjName(A.ObjName),listNum(A.listNum),Tmp(A.Tmp),MatN(A.MatN),
  fill(A.fill),trcl(A.trcl),universe(A.universe),imp(A.imp),
  density(A.density),placehold(A.placehold),populated(A.populated),
  HRule(A.HRule),objSurfValid(0),SurList(A.SurList),SurSet(A.SurSet)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

Object&
Object::operator=(const Object& A)
  /*!
    Assignment operator 
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ObjName=A.ObjName;
      listNum=A.listNum;
      Tmp=A.Tmp;
      MatN=A.MatN;
      fill=A.fill;
      trcl=A.trcl;
      universe=A.universe;
      imp=A.imp;
      density=A.density;
      placehold=A.placehold;
      populated=A.populated;
      HRule=A.HRule;
      objSurfValid=0;
      SurList=A.SurList;
      SurSet=A.SurSet;
    }
  return *this;
}

Object::~Object()
  /*!
    Delete operator : removes Object tree
  */
{}

Object*
Object::clone() const 
  /*!
    Virtual copy constructor
    \return copy ( *this )
   */
{
  return new Object(*this);
}

int
Object::complementaryObject(const int Cnum,std::string& Ln)
  /*!
    Calcluate if there are any complementary components in
    the object. That is lines with #(....)
    \throws ColErr::ExBase :: Error with processing
    \param Cnum :: Number for cell since we don't have one
    \param Ln :: Input string must:  ID Mat {Density}  {rules}
    \retval 0 on no work to do
    \retval 1 :: A (maybe there are many) #(...) object found
 */
{
  ELog::RegMethod RegA("Object","complementaryObject");

  std::string::size_type posA=Ln.find("#(");
  // No work to do ?
  if (posA==std::string::npos)
    return 0;
 
  posA+=2;
 
  // First get the area to be removed
  int brackCnt;
  std::string::size_type posB;
  posB=Ln.find_first_of("()",posA);
  if (posB==std::string::npos)
    throw ColErr::InvalidLine(Ln,"Bracket find",posA);

  brackCnt=(Ln[posB] == '(') ? 1 : 0;
  while (posB!=std::string::npos && brackCnt)
    {
      posB=Ln.find_first_of("()",posB);
      if (posB==std::string::npos)
	throw ColErr::InvalidLine(Ln,"Bracket closure",posA);
      brackCnt+=(Ln[posB] == '(') ? 1 : -1;
      posB++;
    }

  std::string Part=Ln.substr(posA,posB-(posA+1));

  ObjName=Cnum;
  MatN=0;
  density=0.0;
  if (!HRule.procString(Part))
    throw ColErr::ExBase(0,RegA.getFull()+"\n"+Part);

  SurList.clear();
  SurSet.erase(SurSet.begin(),SurSet.end());
  Ln.erase(posA-1,posB+1);  //Delete brackets ( Part ) .
  std::ostringstream CompCell;
  CompCell<<Cnum<<" ";
  Ln.insert(posA-1,CompCell.str());
  objSurfValid=0;
  return 1;
}

int
Object::hasComplement() const
  /*!
    Determine if the object has a complementary object

    \retval 1 :: true 
    \retval 0 :: false
  */
{
  return HRule.isComplementary();
}

int
Object::setObject(std::string Ln)
 /*! 
    Object line ==  Number MateralN/0 cell 
    \param Ln :: Input string must:  ID Mat {Density}  {rules}
    \retval 1 on success
    \retval -1 on continuation (ie has a complementary component)
    \retval 0 on failure
 */
{
  ELog::RegMethod RegA("Object","setObject");

  // Split line
  std::string part;  
  if (!StrFunc::section(Ln,ObjName) || ObjName<=0)
    {
      ELog::EM<<"Name not valid:"<<ObjName<<ELog::endErr;
      return 0;
    }

  // Material (0 == vacuum)
  if (!StrFunc::section(Ln,MatN) || MatN<0) 
    {
      ELog::EM<<"MatN not valid"<<MatN<<std::endl;
      ELog::EM<<"Line == "<<Ln<<ELog::endErr;
      return 0;
    }
  
  // Density 
  if (MatN>0)
    {
      if (!StrFunc::section(Ln,density))
        throw ColErr::InvalidLine("density read",Ln,0);
    }

  else
    {
      // Check is we can strip and remove [allows a density to be
      // set with a variable material that can go to zero]
      if (StrFunc::convert(Ln,density) && 
	  density<1.0 && density>0.0)
	StrFunc::section(Ln,density);  // dump. phantom density
      density=0.0;          // Vacuum
    }

  std::string Extract;
  std::string Value;
  double tval;
  int iVal;
  while(keyUnit(Ln,Extract,Value))
    {
      if (Extract=="tmp" && 
	  StrFunc::convert(Value,tval) && tval>=0.0)
	Tmp=tval;
      else if (Extract=="fill" && 
	       StrFunc::convert(Value,iVal))
	fill=iVal;
      else if (Extract=="trcl" && 
	       StrFunc::convert(Value,iVal))
	trcl=iVal;
      else if (Extract=="u" && 
	       StrFunc::convert(Value,iVal))
	universe=iVal;
      else if (Extract=="imp:n" && 
	       StrFunc::convert(Value,iVal))
	imp=iVal;
      else
	{
	  ELog::EM<<"Failed due to non understood key = value"<<ELog::endErr;
	  return 0;
	}
    }
  if (std::find_if(Ln.begin(), Ln.end(),
                   [](char c) { return std::isalpha(c); }) != Ln.end())
    {
      ELog::EM<<"Junk letters in cell definition:"<<Ln<<ELog::endErr;
      return 0;
    }

  populated=0;
  if (HRule.procString(Ln))     // this currently does not fail:
    {
      SurList.clear();
      SurSet.erase(SurSet.begin(),SurSet.end());
      objSurfValid=0;
      return 1;
    }

  // failure
  ELog::EM<<"Failed to process:"<<Ln<<ELog::endErr;
  return 0;
}

int
Object::procString(const std::string& cellStr)
  /*!
    Process a cell string
    \param cellStr :: Object string
    \return -ve on error
   */
{
  populated=0;
  return HRule.procString(cellStr);
}

int
Object::setObject(const int N,const int matNum,
		  const std::vector<Token>& TVec)
 /*! 
   Set the object line from a token list
   \param N :: Cell Number 
   \param matNum :: Material number
   \param TVec :: Token input of cell
   \retval 1 on success
   \retval 0 on failure
 */
{
  ELog::RegMethod RegA("Object","setObject");
  
  ObjName=N;
  MatN=matNum;
  density=0.0;

  std::vector<Token>::const_iterator mc;
  std::ostringstream cx;
  for(mc=TVec.begin();mc!=TVec.end();mc++)
    mc->write(cx);

  if (HRule.procString(cx.str()))     // this currently does not fail:
    {
      SurList.clear();
      SurSet.erase(SurSet.begin(),SurSet.end());
      objSurfValid=0;
      return 1;
    }

  // failure
  ELog::EM<<"Failed to process:"<<cx.str()<<ELog::endErr;
  return 0;
}

void
Object::populate()
  /*! 
     Goes through the cell objects and adds the pointers
     to the SurfPoint keys (using their keyN).
     Addition ot remove NullSurface
  */
{
  ELog::RegMethod RegA("Object","populate");

  if (!populated) 
    {
      HRule.populateSurf();
      populated=1;
    }
  return;
}

void
Object::rePopulate()
  /*! 
     Goes through the cell objects and adds the pointers
     to the SurfPoint keys (using their keyN).
     Addition ot remove NullSurface
     \retval 1000+ keyNumber :: Error with keyNumber
     \retval 0 :: successfully populated all the whole OSbject.
  */
{
  ELog::RegMethod RegA("Object","rePopulate");
  HRule.populateSurf();
  populated=1;
  return;
}

int
Object::addSurfString(const std::string& XE)
  /*!
    Adds a string in the most ugly fashion...
    \param XE Bit to add (at the global point)
    \retval 1 on success
    \retval 0 on failure
  */
{
  ELog::RegMethod RegA("Object","addSurfString");
  const double Temp=Tmp;                   // need to set later
  std::string Line;
  if (HRule.isUnion())
    Line = headStr()+" ("+cellCompStr()+")"+XE;
  else
    Line = str()+XE;

  const int flag=setObject(Line);
  Tmp=Temp;      
  populated=0;
  objSurfValid=0;
  return flag;
}


int
Object::isOnSide(const Geometry::Vec3D& Pt) const
  /*!
    Determine if the point is in/out on the object
    \param Pt :: Point to check
    \returns SurfNumber if the point is on the surface [signed]
  */
{
  ELog::RegMethod RegA("Object","isOnSide");

  std::map<int,int> surMap=mapValid(Pt);
  std::map<int,int>::iterator mc;

  std::set<int> onSurf;
  for(mc=surMap.begin();mc!=surMap.end();mc++)
    {
      if (!mc->second)
	onSurf.insert(mc->first);
    }
  // Definately not on the surface
  if (onSurf.empty()) return 0;
  std::set<int>::const_iterator sc;
  for(sc=onSurf.begin();sc!=onSurf.end();sc++)
    {
      mc=surMap.find(*sc);
      mc->second=-1;
      const int Adir=HRule.isValid(surMap);
      mc->second=1;
      const int Bdir=HRule.isValid(surMap);

      if ((Adir ^ Bdir)==1)        // Opposite 
	return (Bdir) ? *sc : -(*sc);
      mc->second=0;
    }

  return 0;
}

int
Object::checkExteriorValid(const Geometry::Vec3D& C,
			   const Geometry::Vec3D& Nm) const
  /*!
    Determine if a point is valid by checking both
    directions of the normal away from the line
    A good point will have an invalid point as this
    point is already deamed to be valid
    \param C :: Point on a basic surface to check 
    \param Nm :: Direction +/- to be checked
    \retval 0 :: All interiaor
    \retval 1 :: An exterior point
  */
{
  Geometry::Vec3D tmp=C+Nm*(Geometry::shiftTol*5.0);
  if (!isValid(tmp)) return 1;
  tmp-= Nm*(Geometry::shiftTol*10.0);
  return (!isValid(tmp)) ? 1 : 0;
}

int
Object::checkSurfaceValid(const Geometry::Vec3D& C,
			  const Geometry::Vec3D& Nm) const
  /*!
    Determine if a point is valid by checking both
    directions of the normal away from the line
    A good point will have one valid and one invalid.
    \param C :: Point on a basic surface to check 
    \param Nm :: Direction +/- to be checked
    \retval +/-1 ::  C+Nm or C-Nm are both the same
    \retval 0 :: otherwize
  */
{
  int status(0);
  Geometry::Vec3D tmp=C+Nm*(Geometry::shiftTol*5.0);
  status= (!isValid(tmp)) ? 1 : -1;
  tmp-= Nm*(Geometry::shiftTol*10.0);
  status+= (!isValid(tmp)) ? 1 : -1;
  return status/2;
}

int
Object::trackDirection(const Geometry::Vec3D& C,
		       const Geometry::Vec3D& Nm) const
			  
  /*!
    Determine if a point is valid by checking both
    directions of the normal away from the line
    A good point will have one valid and one invalid.
    \param C :: Point on a basic surface to check 
    \param Nm :: Direction +/- to be checked
    \retval +1 ::  Entering the Object
    \retval 0 :: No-change
    \retval -1 ::  Exiting the object
  */
{
  Geometry::Vec3D tmp=C+Nm*(Geometry::shiftTol*5.0);
  const int inStatus=isValid(tmp);    
  tmp-= Nm*(Geometry::shiftTol*10.0);
  const int outStatus=isValid(tmp);   
  return inStatus-outStatus;
}  

int
Object::isValid(const Geometry::Vec3D& Pt) const
/*! 
  Determines is Pt is within the object 
  or on the surface
  \param Pt :: Point to be tested
  \returns 1 if true and 0 if false
*/
{
  return HRule.isValid(Pt);
}

int
Object::isValid(const Geometry::Vec3D& Pt,
		const int ExSN) const
/*! 
  Determines is Pt is within the object 
  or on the surface
  \param Pt :: Point to be tested
  \param ExSN :: Excluded surf Number
  \returns 1 if true and 0 if false
*/
{
  return HRule.isValid(Pt,ExSN);
}

int
Object::isDirectionValid(const Geometry::Vec3D& Pt,
			 const int ExSN) const
/*! 
  Determines is Pt is within the object 
  or on the surface
  \param Pt :: Point to be tested 
  \param ExSN :: Excluded surf Number [signed]
  \returns 1 if true and 0 if false
*/
{
  return HRule.isDirectionValid(Pt,ExSN);
}


int
Object::isValid(const Geometry::Vec3D& Pt,
		const std::set<int>& ExSN) const
/*! 
  Determines is Pt is within the object 
  or on the surface
  \param Pt :: Point to be tested
  \param ExSN :: Excluded surf Numbers
  \returns 1 if true and 0 if false
*/
{
  return HRule.isValid(Pt,ExSN);
}

int
Object::isValid(const std::map<int,int>& SMap) const
/*! 
  Determines is group of surface maps are valid
  \param SMap :: map of SurfaceNumber : status [0 / 1]
  \returns 1 if true and 0 if false
*/
{
  return HRule.isValid(SMap);
}

std::map<int,int>
Object::mapValid(const Geometry::Vec3D& Pt) const
/*! 
  Populates the validity map
  the surface 
  \param Pt :: Point to testsd
  \returns Map [ surfNumber: -1/1 ]
*/
{
  ELog::RegMethod RegA("Object","mapValid");

  std::map<int,int> SMap;
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SurList.begin();vc!=SurList.end();vc++)
    SMap.insert(std::map<int,int>::value_type
		((*vc)->getName(),(*vc)->side(Pt)));

  return SMap;
}

int
Object::pairValid(const int SN,const Geometry::Vec3D& Pt) const
  /*!
    Given a surface number determine if the 
    Point is invalid/valid with respect to the
    surface SN.
    - 0 
    \param SN :: surface number
    \param Pt :: Point to test
    \retval 0 : Not valid [SN true/false]   
    \retval 1 : valid [SN false only]
    \retval 2 : valid [SN true only]
    \retval 3 : valid [SN true/false]
  */
{
  return HRule.pairValid(SN,Pt);
}

int
Object::createSurfaceList()
  /*! 
    Uses the topRule* to create a surface list
    by iterating throught the tree.    
    \return 1 (should be number of surfaces)
  */
{ 
  ELog::RegMethod RegA("Object","createSurfaceList");
  
  populate();  // checked in populate

  std::ostringstream debugCX;

  SurList.clear();
  SurSet.erase(SurSet.begin(),SurSet.end());

  std::stack<const Rule*> TreeLine;
  TreeLine.push(HRule.getTopRule());
  while(!TreeLine.empty())
    {
      const Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      if (tmpA)
	{
	  const Rule* tmpB=tmpA->leaf(0);
	  const Rule* tmpC=tmpA->leaf(1);
	  if (tmpB || tmpC)
	    {
	      if (tmpB)
		TreeLine.push(tmpB);
	      if (tmpC)
		TreeLine.push(tmpC);
	    }
	  else
	    {
	      const SurfPoint* SurX=dynamic_cast<const SurfPoint*>(tmpA);
	      if (SurX)
		{
		  if (SurX->getKey()==0)
		    debugCX<<" "<<SurX->getKey()<<" "<<SurX->getKeyN();
		  SurList.push_back(SurX->getKey());
		  SurSet.insert(SurX->getKeyN()*SurX->getSign());
		}
	    }
	}
    }
  sort(SurList.begin(),SurList.end());
  
  std::vector<const Geometry::Surface*>::iterator sc=
    unique(SurList.begin(),SurList.end());
  if (sc!=SurList.end())
    SurList.erase(sc,SurList.end());

  // sorted list will have zeros at front
  if (*SurList.begin()==0)
    {
      ELog::EM<<"SurList Failure "<<ELog::endCrit;
      ELog::EM<<"CX == "<<debugCX.str()<<ELog::endCrit;
      ELog::EM<<"Found zero Item "<<this->getName()<<" "
	      <<" "<<populated<<" :: "<<ELog::endCrit;
      ELog::EM<<"SList size "<<SurList.size()<<ELog::endCrit;
      ELog::EM<<"Cell == "<<*this<<ELog::endErr;
      throw ColErr::ExitAbort("Empty surf List");
    }

  createLogicOpp();
  return 1;
}

void
Object::createLogicOpp()
  /*!
    Find logically opposite surfaces in the object for valid check later
   */
{
  ELog::RegMethod RegA("Object","createLogicOpp");
  // create Logical
  //  static int cnt(0);

  logicOppSurf=HRule.getOppositeSurfaces();
  return;
}


std::vector<int>
Object::getSurfaceIndex() const
  /*!
    Returns all of the numbers of surfaces
    \return surface indexes (no sign)
  */
{
  std::vector<int> out;
  transform(SurList.begin(),SurList.end(),
	    std::insert_iterator<std::vector<int> >(out,out.begin()),
	    std::mem_fun(&Geometry::Surface::getName));
  return out;
}

bool
Object::hasSurface(const int SN) const
  /*!
    Do we have the signed surface in this object
    \param SN :: signed number
    \return true/false
   */
{
  return (SurSet.find(SN)!=SurSet.end()) ? 1 : 0;
}

int 
Object::surfSign(const int SN) const
  /*!
    Do we have the signed surface in this object
    \param SN :: signed number
    \return +/-1 or 0 [both/nosurf]
   */
{
  int sign=(SurSet.find(SN)!=SurSet.end()) ? 1 : 0;
  sign+=(SurSet.find(-SN)!=SurSet.end()) ? -1 : 0;
  return sign;
}

int
Object::removeSurface(const int SurfN)
  /*! 
    Removes a surface and then re-builds the 
    cell. This could be done by just removing 
    the surface from the object.
    \param SurfN :: Number for the surface
    \return number of surfaces removes
  */
{ 
  ELog::RegMethod RegA("Object","removeSurface");

  const int cnt=HRule.removeItems(SurfN);
  if (cnt>0)
    {
      createSurfaceList();
      objSurfValid=0;
    }
  return cnt;
}

int
Object::substituteSurf(const int SurfN,const int NsurfN,
		       Geometry::Surface* SPtr)
  /*! 
    Removes a surface and then re-builds the 
    cell.
    \param SurfN :: Number for the surface
    \param NsurfN :: New surface number
    \param SPtr :: Surface pointer for surface NsurfN
    \return number of surfaces substituted
  */
{ 
  ELog::RegMethod RegA("Object","substituteSurf");

  if (!SPtr)
    SPtr=ModelSupport::surfIndex::Instance().getSurf(abs(NsurfN));
  
  const int out=HRule.substituteSurf(SurfN,NsurfN,SPtr);
  if ( out )
    {
      populated=0;
      populate();
      createSurfaceList();
    }
  return out;
}

int
Object::hasIntercept(const Geometry::Vec3D& IP,
		     const Geometry::Vec3D& UV) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects)

    \param IP :: Initial point
    \param UV :: Forward going vector
    \return True(1) / Fail(0)
  */
{
  ELog::RegMethod RegA("Object","hadIntercept");

  MonteCarlo::LineIntersectVisit LI(IP,UV);
 
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SurList.begin();vc!=SurList.end();vc++)
    (*vc)->acceptVisitor(LI);

  const std::vector<double>& dPts(LI.getDistance());
  for(size_t i=0;i<dPts.size();i++)
    {
      if (dPts[i]>0.0)  // only interested in forward going points
	return 1;
    }
  // Definately missed
  return 0;
}

std::pair<const Geometry::Surface*,double>
Object::forwardIntercept(const Geometry::Vec3D& IP,
			 const Geometry::Vec3D& UV) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects).
    This does not descriminate between ingoing and outgoing
    tracks.

    \param IP :: Initial point
    \param UV :: Forward going vector
    \return surface + distance forward : -ve on failure
  */
{
  ELog::RegMethod RegA("Object","forwardIntercept");
  

  MonteCarlo::LineIntersectVisit LI(IP,UV);
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SurList.begin();vc!=SurList.end();vc++)
    (*vc)->acceptVisitor(LI);

  const std::vector<Geometry::Vec3D>& IPts(LI.getPoints());
  const std::vector<double>& dPts(LI.getDistance());
  const std::vector<const Geometry::Surface*>& surfIndex(LI.getSurfIndex());
  double minDist(1e38);
  const Geometry::Surface* surfPtr(0);
  // NOTE: we only check for and exiting surface by going
  // along the line.
  for(size_t i=0;i<dPts.size();i++)
    {
      if (dPts[i]>Geometry::shiftTol && dPts[i]<minDist)
	{
	  const int pAB=pairValid(surfIndex[i]->getName(),IPts[i]);
	  if (pAB==1 || pAB==2)            // Going either way
	    {
	      minDist=dPts[i];
	      surfPtr=surfIndex[i];
	    }
	}
    }
  // surface + distance
  return std::pair<const Geometry::Surface*,double>(surfPtr,minDist);
}


int
Object::trackOutCell(const MonteCarlo::neutron& N,double& D,
		     const Geometry::Surface*& SPtr,
		     const int startSurf) const
  /*!
    Track the distance to exit the cell 
    - if already out of the cell, distance is to the cell+to the exit
    \param N :: Neutron
    \param D :: Distance to exit
    \param SPtr :: Surface at exit
    \param startSurf :: Start surface (not to be used) [0 to ignore]
    \return surface number on exit
  */
{
  return trackCell(N,D,-1,SPtr,startSurf);
}

int
Object::trackIntoCell(const MonteCarlo::neutron& N,double& D,
		      const Geometry::Surface*& SPtr,
		      const int startSurf) const
  /*!
    Track the distance to a cell
    \param N :: Neutron
    \param D :: Distance to entrance
    \param SPtr :: Surface at exit
    \param startSurf :: Start surface 
    \return surface number on exit
  */
{
  return trackCell(N,D,1,SPtr,startSurf);
}

int
Object::calcInOut(const int pAB,const int N) const
  /*!
    Given a pair and a normal direction calculate the sense
    \param pAB :: [0/3 void ] / 1 false required / 2 true required
    \param N :: normal direction -ve : leaving : +ve entering
    \retval -ve : Leaving
    \retval +ve : Entering
    \retval 0 :: No transition
   */
{
  switch (pAB)
    {
    case 1:        // false required [
      return -N;
    case 2:
      return N;
    }
  return 0;
}

int
Object::trackCell(const MonteCarlo::neutron& N,double& D,
		  const int direction,
		  const Geometry::Surface*& surfPtr,
		  const int startSurf) const
  /*!
    Track to a neutron into/out of a cell. 
    \param N :: Neutron 
    \param D :: Distance traveled to the cell [get added too]
    \param direction :: direction to track [+1/-1 : in/out ] 
    \param surfPtr :: Surface at exit
    \param startSurf :: Start surface [to be ignored]
    \return surface number of intercept
   */
{
  ELog::RegMethod RegA("Object","trackCell[D,dir]");

  MonteCarlo::LineIntersectVisit LI(N);
  for(const Geometry::Surface* isptr : SurList)
    isptr->acceptVisitor(LI);

  const std::vector<Geometry::Vec3D>& IPts(LI.getPoints());
  const std::vector<double>& dPts(LI.getDistance());
  const std::vector<const Geometry::Surface*>& surfIndex(LI.getSurfIndex());

  D=1e38;
  surfPtr=0;
  int touchUnit(0);
  // NOTE: we only check for and exiting surface by going
  // along the line.
  int bestPairValid(0);
  for(size_t i=0;i<dPts.size();i++)
    {
      // Is point possible closer
      if ( ( surfIndex[i]->getName()!=startSurf || 
	     dPts[i]>10.0*Geometry::zeroTol) &&
	   (dPts[i]>0.0 && dPts[i]<D) )
	{
	  const int NS=surfIndex[i]->getName();	    // NOT SIGNED
	  const int pAB=isDirectionValid(IPts[i],NS);
	  const int mAB=isDirectionValid(IPts[i],-NS);
	  const int normD=surfIndex[i]->sideDirection(IPts[i],N.uVec);
          
	  if (direction<0)
	    {
	      if (pAB!=mAB)  // out going positive surface
		{
		  bestPairValid=normD;
		  if (dPts[i]>Geometry::zeroTol)
		    D=dPts[i];
		  else
		    touchUnit=1;
		  surfPtr=surfIndex[i];
		}
	    }
	}
    }
  if (touchUnit && D>1e37)
    D=Geometry::zeroTol;

  if (!surfPtr) return 0;
  const int NSsurf=surfPtr->getName();
  const bool pSurfFound(SurSet.find(NSsurf)!=SurSet.end());
  const bool mSurfFound(SurSet.find(-NSsurf)!=SurSet.end());
  
  if (pSurfFound && mSurfFound)
    return bestPairValid*NSsurf;

  return (pSurfFound) ? -NSsurf : NSsurf;
}

		  
std::pair<const Geometry::Surface*,double>
Object::forwardInterceptInit(const Geometry::Vec3D& IP,
			     const Geometry::Vec3D& UV) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects).
    Has special test for the case that the neutron 
    starts on a surface and we need to effectively move it inside.

    \param IP :: Initial point
    \param UV :: Forward going vector
    \return distance forward : -ve on failure
  */
{
  ELog::RegMethod RegA("Object","forwardInterceptInit");
  
  MonteCarlo::LineIntersectVisit LI(IP,UV);
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SurList.begin();vc!=SurList.end();vc++)
    (*vc)->acceptVisitor(LI);

  const std::vector<Geometry::Vec3D>& IPts(LI.getPoints());
  const std::vector<double>& dPts(LI.getDistance());
  const std::vector<const Geometry::Surface*>& surfIndex(LI.getSurfIndex());
  double minDist(1e38);
  const Geometry::Surface* surfPtr(0);
  // NOTE: we only check for and exiting surface by going
  // along the line.
  for(size_t i=0;i<dPts.size();i++)
    {
      if (dPts[i]>Geometry::shiftTol && dPts[i]<minDist)
	{
	  const int pAB=pairValid(surfIndex[i]->getName(),IPts[i]);
	  if (pAB==1 || pAB==2)            // Going either way
	    {
	      minDist=dPts[i];
	      surfPtr=surfIndex[i];
	    }
	}
    }
  
  if (!surfPtr)
    {
      for(size_t i=0;i<dPts.size();i++)
	{
	  if (dPts[i]>-Geometry::shiftTol && dPts[i]<Geometry::shiftTol && 
	      dPts[i]<minDist)
	    {
	      const int pAB=pairValid(surfIndex[i]->getName(),IPts[i]);
	      if (pAB==1 || pAB==2)            // Going either way
		{
		  minDist=dPts[i];
		  surfPtr=surfIndex[i];
		}
	    }
	}
    }
  
  return std::pair<const Geometry::Surface*,double>(surfPtr,minDist);
}

void
Object::makeComplement()
  /*!
    Takes the complement of a group
   */
{
  HRule.makeComplement();
  return;
}

std::vector<Token>
Object::cellVec() const
  /*!
    Creates a token vector for the cell
    This includes only rules.
    \return Object Vector
  */
{
  std::vector<Token> Out;
  HRule.displayVec(Out);
  return Out;
}

std::string
Object::headStr() const
  /*!
    Write the head string to a string
    \return Object Head Line
  */
{
  std::ostringstream cx;
  cx<<ObjName<<" "<<MatN;
  if (MatN!=0)
    cx<<" "<<density;
  return cx.str();
}

std::string
Object::cellCompStr() const
  /*!
    Write the object to a string.
    This includes only rules.
    \return Object Line
  */
{
  return HRule.display();
}

std::string
Object::pointStr(const Geometry::Vec3D& Pt) const
  /*!
    Write the object to a string shown the validity of 
    the point on the rules
    \param Pt :: Point to report on
    \return Object Line
  */
{
  return HRule.display(Pt);
}

std::string
Object::str() const
  /*!
    Write the object to a string.
    This includes the Name , material and density but 
    not post-fix operators
    \return Object Line
  */
{
  std::ostringstream cx;
  cx<<ObjName<<" ";
  if (imp)
    {
      cx<<MatN;
      if (MatN!=0)
	cx<<" "<<density;
    }
  else
    {
      cx<<"0 ";
    }
  
  cx<<" "<<HRule.display();
  return cx.str();
}

void 
Object::write(std::ostream& OX) const
  /*!
    Write the object to a standard stream
    in standard MCNPX output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  std::ostringstream cx;
  cx.precision(10);
  cx<<str();
  if (Tmp>1.0 && fabs(Tmp-300.0)>1.0)
    cx<<" "<<"tmp="<<Tmp*8.6173422e-11;
  if (fill)
    cx<<" "<<"fill="<<fill;
  if (trcl)
    cx<<" "<<"trcl="<<trcl;
  if (universe)
    cx<<" "<<"u="<<universe;

  if (placehold)
    StrFunc::writeMCNPXcomment(cx.str(),OX);
  else
    StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void
Object::writeFLUKAmat(std::ostream& OX) const
  /*!
    Write the object material assignment to a standard stream
    in standard FLUKA output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  ELog::RegMethod RegA("Object","writeFLUKAmat");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  if (!placehold)
    {
      std::string objName=OR.inRenumberRange(ObjName);
      std::ostringstream cx;
      cx<<"ASSIGNMAT ";

      if (!imp)
	cx<<"BLCKHOLE";
      else if (MatN)
	cx<<"M"+std::to_string(MatN);
      else
	cx<<"VACUUM";

      cx<<" R"+std::to_string(ObjName);
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  
  return;
}
  
void 
Object::writeFLUKA(std::ostream& OX) const
  /*!
    Write the object to a standard stream
    in standard FLUKA output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  ELog::RegMethod RegA("Object","writeFLUKA");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  if (!placehold)
    {
      std::string objName=OR.inRenumberRange(ObjName);
      if (objName.empty())
	objName="global";
      std::ostringstream cx;
      cx<<"* "<<objName<<" "<<ObjName<<std::endl;
      cx.precision(10);

      cx<<"R"<<ObjName<<" "<<SurList.size()<<" ";
      cx<<HRule.displayFluka()<<std::endl;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  
  return;
}

void 
Object::writePOVRay(std::ostream& OX) const
  /*!
    Write the object to a standard stream
    in POVRay output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  ELog::RegMethod RegA("Object","writePOVRay");

  masterWrite& MW=masterWrite::Instance();
  
  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  if (!placehold && MatN>0)
    {
      const std::string objName=OR.inRenumberRange(ObjName);
      
      // do not render global objects (outer void and black hole)
      //      if (objName.empty())
        //	return; 
      OX<<"// Cell "<<objName<<" "<<ObjName<<"\n";
      OX<<"intersection{\n"
	<<HRule.displayPOVRay()<<"\n"
	<< " texture {mat" <<MW.NameNoDot(DB.getKey(MatN)) <<"}\n"
	<< "}"<<std::endl;
    }
  
  return;
}

void
Object::writePOVRaymat(std::ostream& OX) const
  /*!
    Write the object material assignment to a standard stream
    in POVRay output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  ELog::RegMethod RegA("Object","writePOVRaymat");
  return;
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  if (!placehold)
    {
      const std::string objName=OR.inRenumberRange(ObjName);
      if (objName.empty())
	return;
      
      OX<<"POVRay dummy material string    ";
      if (!MatN)   // are we really rendering vacuum ????
	OX<<" VACUUM";
      else
	OX<<"    M"<<MatN;
      
      OX<<"    "<<objName<<"_"<<ObjName<<std::endl;
    }
  
  return;
}

  
void 
Object::writePHITS(std::ostream& OX) const
  /*!
    Write the object to a standard stream
    in standard PHITS output format.
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  std::ostringstream cx;

  cx.precision(10);
  if (placehold)
    {
      cx<<str();
      StrFunc::writeMCNPXcomment(cx.str(),OX);
    }
  else if (ObjName==1 && imp==0)
    {
      cx<<ObjName<<" -1 "<<HRule.display();
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  else 
    {
      cx<<str();
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  return;
}

void
Object::checkPointers() const
  /*!
    Test the surface pointers.
    Does not return but shows an output string
    Debug function only
   */
{
  ELog::RegMethod RegA("Object","checkPointers");
  int flag(0);
  std::ostringstream debugCX;
  std::stack<const Rule*> TreeLine;
  TreeLine.push(HRule.getTopRule());
  while(!TreeLine.empty())
    {
      const Rule* tmpA=TreeLine.top();
      TreeLine.pop();
      if (tmpA)
	{
	  const Rule* tmpB=tmpA->leaf(0);
	  const Rule* tmpC=tmpA->leaf(1);
	  if (tmpB || tmpC)
	    {
	      if (tmpB)
		TreeLine.push(tmpB);
	      if (tmpC)
		TreeLine.push(tmpC);
	    }
	  else
	    {
	      const SurfPoint* SurX=dynamic_cast<const SurfPoint*>(tmpA);
	      if (SurX)
		{
		  if (SurX->getKey()==0)
		    {
		      debugCX<<" "<<SurX->getKey()<<"->"<<SurX->getKeyN();
		      flag=1;
		    }
		}
	    }
	}
    }
  if (flag)
    ELog::EM<<"CX == "<<debugCX.str()<<ELog::endCrit;
  else
    ELog::EM<<"Success"<<ELog::endDebug;
  return;
}



} // NAMESPACE MonteCarlo

