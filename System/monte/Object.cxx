/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Object.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "mcnpStringSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
#include "interPoint.h"
#include "LineIntersectVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfImplicates.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Token.h"
#include "particle.h"
#include "masterWrite.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Importance.h"
#include "Object.h"

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

Object::Object() :
  ObjName(0),listNum(-1),Tmp(300.0),
  matPtr(ModelSupport::DBMaterial::Instance().getVoidPtr()),
  trcl(0),populated(0),
  activeMag(0),magMinStep(1e-3),magMaxStep(1e-1),
  activeElec(0),elecMinStep(1e-3),elecMaxStep(1e-1),
  objSurfValid(0)
   /*!
     Defaut constuctor, set temperature to 300C and material to vacuum
   */
{}

Object::Object(const int N,const int M,
	       const double T,const std::string& Line) :
  ObjName(N),listNum(-1),Tmp(T),
  matPtr(ModelSupport::DBMaterial::Instance().getMaterialPtr(M)),
  trcl(0),populated(0),
  activeMag(0),magMinStep(1e-3),magMaxStep(1e-1),
  activeElec(0),elecMinStep(1e-3),elecMaxStep(1e-1),
  objSurfValid(0)
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

Object::Object(const int N,const int M,
	       const double T,HeadRule  HR) :
  ObjName(N),listNum(-1),Tmp(T),
  matPtr(ModelSupport::DBMaterial::Instance().getMaterialPtr(M)),
  trcl(0),populated(0),
  activeMag(0),magMinStep(1e-3),magMaxStep(1e-1),
  activeElec(0),elecMinStep(1e-3),elecMaxStep(1e-1),
  HRule(std::move(HR)),objSurfValid(0)
 /*!
   Constuctor, set temperature to 300C 
   \param N :: number
   \param M :: material
   \param T :: temperature (K)
   \param HR :: HeadRule
 */
{}

Object::Object(std::string  FCName,const int N,const int M,
	       const double T,const std::string& Line) :
  FCUnit(std::move(FCName)),ObjName(N),listNum(-1),Tmp(T),
  matPtr(ModelSupport::DBMaterial::Instance().getMaterialPtr(M)),
  trcl(0),populated(0),
  activeMag(0),magMinStep(1e-3),magMaxStep(1e-1),
  activeElec(0),elecMinStep(1e-3),elecMaxStep(1e-1),
  objSurfValid(0)
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

Object::Object(std::string  FCName,const int N,const int M,
	       const double T,HeadRule  HR) :
  FCUnit(std::move(FCName)),ObjName(N),listNum(-1),Tmp(T),
  matPtr(ModelSupport::DBMaterial::Instance().getMaterialPtr(M)),
  trcl(0),populated(0),
  activeMag(0),magMinStep(1e-3),magMaxStep(1e-1),
  activeElec(0),elecMinStep(1e-3),elecMaxStep(1e-1),
  HRule(std::move(HR)),
  objSurfValid(0)
 /*!
   Constuctor, set temperature to 300C 
   \param N :: number
   \param M :: material
   \param T :: temperature (K)
   \param HR :: HeadRule of object
 */
{}

Object::Object(const Object& A) :
  FCUnit(A.FCUnit),ObjName(A.ObjName),
  listNum(A.listNum),Tmp(A.Tmp),matPtr(A.matPtr),
  trcl(A.trcl),imp(A.imp),populated(A.populated),
  activeMag(A.activeMag),magMinStep(A.magMinStep),
  magMaxStep(A.magMaxStep),activeElec(A.activeElec),
  elecMinStep(A.elecMinStep),elecMaxStep(A.elecMaxStep),
  HRule(A.HRule),objSurfValid(0),
  surfSet(A.surfSet),surNameSet(A.surNameSet)
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
      FCUnit=A.FCUnit;
      ObjName=A.ObjName;
      listNum=A.listNum;
      Tmp=A.Tmp;
      matPtr=A.matPtr;
      trcl=A.trcl;
      imp=A.imp;
      populated=A.populated;
      activeMag=A.activeMag;
      magMinStep=A.magMinStep;
      magMaxStep=A.magMaxStep;
      activeElec=A.activeElec;
      elecMinStep=A.elecMinStep;
      elecMaxStep=A.elecMaxStep;
      HRule=A.HRule;
      objSurfValid=0;
      surfSet=A.surfSet;
      surNameSet=A.surNameSet;
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

void
Object::clearValid()
  /*!
    Function to remove all items that depended
    on an old valid HRule.
   */
{
  populated=0;
  surNameSet.clear();
  surfSet.clear();
  objSurfValid=0;
  return;
}
  
int
Object::getMatID() const
  /*!
    Get Material ID
    \return ID number
  */
{
 return matPtr->getID();
}

double
Object::getDensity() const
  /*!
    Accessor to denstiy
    \return Density [Atom/A3]
   */
{
  return matPtr->getAtomDensity();
}

void
Object::setMaterial(const int matID)
  /*!
    Given a material id , set the new(?) material pointer
    This function should not be called under most cases.
    \param matID :: Material id
   */
{
  ELog::RegMethod RegA("Object","setMaterial");

  matPtr=ModelSupport::DBMaterial::Instance().getMaterialPtr(matID);
  return;
}

void
Object::setMaterial(const std::string& matName)
  /*!
    Given a material id , set the new(?) material pointer
    This function should not be called under most cases.
    \param matName :: Material id
   */
{
  ELog::RegMethod RegA("Object","setMaterial(string)");

  matPtr=ModelSupport::DBMaterial::Instance().getMaterialPtr(matName);
  return;
}

void
Object::setMagStep(const double minV,const double maxV)
  /*!
    Set the min/max steps for the steps of charged particle
    in a magnetic field
    \param minV :: min step value
    \param maxV :: max step value
  */
{
  ELog::RegMethod RegA("Object","setMagStep");

  magMinStep=std::min(minV,maxV);
  magMaxStep=std::max(minV,maxV);

  return;
}

void
Object::setElecStep(const double minV,const double maxV)
  /*!
    Set the min/max steps for the steps of charged particle
    in a magnetic field
    \param minV :: min step value
    \param maxV :: max step value
  */
{
  ELog::RegMethod RegA("Object","setMagStep");

  elecMinStep=std::min(minV,maxV);
  elecMaxStep=std::max(minV,maxV);

  return;
}


int
Object::complementaryObject(const int Cnum,std::string& Ln)
  /*!
    Calcluate if there are any complementary components in
    the object. That is lines with #(....)
    \throws ColErr::InvalidLine :: Error with processing
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
  while (brackCnt)
    {
      posB=Ln.find_first_of("()",posB);
      if (posB==std::string::npos)
	throw ColErr::InvalidLine(Ln,"Bracket closure",posA);
      brackCnt+=(Ln[posB] == '(') ? 1 : -1;
      posB++;
    }

  std::string Part=Ln.substr(posA,posB-(posA+1));

  ObjName=Cnum;
  if (!HRule.procString(Part))
    throw ColErr::InvalidLine(Part,"HRule::procString");

  surfSet.clear();
  surNameSet.clear();
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
  int matN(0);
  if (!StrFunc::section(Ln,matN) || matN<0)
    throw ColErr::InvalidLine("Material Index",Ln);
  
  // Density
  double density;
  if (matN>0 && !StrFunc::section(Ln,density))
    throw ColErr::InvalidLine("density read",Ln);
  
  // Check is we can strip and remove [allows a density to be
  // set with a variable material that can go to zero]
  if (matN==0)
    {
      if (StrFunc::convert(Ln,density) && 
	  density<1.0 && density>0.0)
	StrFunc::section(Ln,density);  // dump. phantom density
    }

  std::string Extract;
  std::string Value;
  double lineTemp(0.0);
  int lineTRCL(0);
  int lineIMP(1);
  while(mcnpFunc::keyUnit(Ln,Extract,Value))
    {
      if ((Extract=="tmp" && 
	   StrFunc::convert(Value,lineTemp) && lineTemp>=0.0)  ||

	  (Extract=="trcl" && 
	   StrFunc::convert(Value,lineTRCL) && lineTRCL>=0)  ||

	  (Extract=="imp:n" && 
	   StrFunc::convert(Value,lineIMP) && lineIMP>=0)  )
	{ } 
      else
	throw ColErr::InvalidLine("Invalid key :: ",Extract+":"+Value);
      
    }
  if (std::find_if(Ln.begin(), Ln.end(),
                   [](char c) { return std::isalpha(c); }) != Ln.end())
    ColErr::InvalidLine("Junk letters in line ",Ln);

  if (!HRule.procString(Ln))   // fails on empty
    throw ColErr::InvalidLine("procString failure :: ",Ln);


  // note that this invalidates the read density:

    

  setMaterial(matN);
  Tmp=lineTemp;
  imp.setImp(static_cast<double>(lineIMP));
  trcl=lineTRCL;

  clearValid();
  
  return 1;   // SUCCESS
}

int
Object::procString(const std::string& cellStr)
  /*!
    Process a cell string
    \param cellStr :: Object string
    \return O on failure / 1 on success
   */
{
  if (HRule.procString(cellStr))
    {
      clearValid();
      return 1;
    }
  return 0;
}

int
Object::procHeadRule(const HeadRule& cellRule)
  /*!
    Process a cell string
    \param cellStr :: Object string
    \return O on failure / 1 on success
  */
{
  clearValid();
  HRule=cellRule;
  return 1;
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
  matPtr=ModelSupport::DBMaterial::Instance().
    getMaterialPtr(matNum);

  std::ostringstream cx;
  for(const Token& tItem : TVec)
    tItem.write(cx);

  if (!HRule.procString(cx.str()))
    throw ColErr::InvalidLine("Token string",cx.str());

  clearValid();
  return 1;
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

void
Object::addIntersection(const HeadRule& HR)
  /*!
    Adds a HR as an intersection
    \param HR :: headrule to add
  */
{
  ELog::RegMethod RegA("Object","addIntersection");
  
  HRule.addIntersection(HR);
  clearValid();
  return;
}

void
Object::addUnion(const HeadRule& HR)
  /*!
    Adds a HR as an union
    \param HR :: headrule to add
  */
{
  ELog::RegMethod RegA("Object","addIntersection");
  
  HRule.addUnion(HR);
  clearValid();
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

  if (!XE.empty())
    {
      HeadRule HR;
      if (!HR.procString(XE)) return 0;
      HRule*=HR;
      clearValid();
    }
 
  return 1;
}

int
Object::isOnSurface(const Geometry::Vec3D& Pt) const
  /*!
    Determine if the point is on a surface in the object
    THIS SHOULD NOT be used if you want if the point is 
    at an exiting boundary (use Object::isOnSide). But 
    it is for determining if a boundary check is needed.
    \param Pt :: Point to check
    \returns SurfNumber if the point is on the surface [signed]
  */
{
  ELog::RegMethod RegA("Object","isOnSurface");

  std::map<int,int> surMap=mapValid(Pt);
  for(const auto& [sn , sideFlag] : surMap)
    {
      if (!sideFlag)
	return sn;
    }
  return 0;
}


std::set<int>
Object::isOnSide(const Geometry::Vec3D& Pt) const
  /*!
    Determine if the point is in/out on the object
    The requirement is that for all sides that the points is
    on, moving the boolean value of that from -1 to +1 results
    in the point leaving and entering the object.
    \param Pt :: Point to check
    \returns SurfNumber if the point is on the surface 
  */
{
  ELog::RegMethod RegA("Object","isOnSide");

  std::map<int,int> surMap=mapValid(Pt);

  std::set<int> onSurf;
  for(const auto& [SN , sideFlag] : surMap)
    {
      if (!sideFlag)
	onSurf.emplace(SN);
    }
  if (onSurf.empty()) return onSurf;

  std::set<int> sideSurfSet;
  for(const int SN : onSurf)
    {
      bool aFlag=HRule.isValid(Pt,-SN);
      bool bFlag=HRule.isValid(Pt,SN);
      if (aFlag!=bFlag)
	sideSurfSet.emplace(SN);
    }
  return sideSurfSet;
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

const Geometry::Surface*
Object::getSurf(const int SN) const
  /*!
    Get a surface from the object. It is unsigned.
    \param SN :: Surface number
    \return Surface Ptr
   */
{
  if (surNameSet.find(SN)!=surNameSet.end() ||
      surNameSet.find(-SN)!=surNameSet.end())
    {
      const int ASN=std::abs(SN);
      for(const Geometry::Surface* SPtr : surfSet)
	if (SPtr->getName()==ASN)
	  return SPtr;
    }
  return 0;
}

int
Object::trackDirection(const int SN,
		       const Geometry::Vec3D& Pt,
		       const Geometry::Vec3D& Norm) const
  /*!
    Determine if a point is valid by checking both
    directions of the normal away from the line
    A good point will have one valid and one invalid.
    \param SN :: surface number to test
    \param Pt :: Point on a basic surface to check 
    \param Norm :: Direction +/- to be checked
    \retval +1 ::  Entering the Object
    \retval 0 :: No-change
    \retval -1 ::  Exiting the object
  */
{
  const int pAB=HRule.isValid(Pt,SN);   // true/false [1/0]
  const int mAB=HRule.isValid(Pt,-SN);	    

  if (pAB!=mAB)    // exiting/entering
    {
      const Geometry::Surface* SPtr=getSurf(SN);
      if (!SPtr)
	throw ColErr::InContainerError<int>
	  (SN,"Surf not in object"+str());

      const int normD=SPtr->sideDirection(Pt,Norm);
      return (normD == pAB || normD == -mAB ) ? 1 : -1;
    }
  return 0;
}
		

int
Object::trackDirection(const Geometry::Vec3D& Pt,
		       const Geometry::Vec3D& Norm) const
			  
  /*!
    Determine if a point is valid by checking both
    directions of the normal away from the line
    A good point will have one valid and one invalid.
    \param Pt :: Point on a basic surface to check 
    \param Norm :: Direction +/- to be checked
    \retval +1 ::  Entering the Object
    \retval 0 :: No-change
    \retval -1 ::  Exiting the object
  */
{
  ELog::RegMethod RegA("Object","trackDirection(all)");
  
  // first determine if on surface :
  const std::set<int> SSet = isOnSide(Pt);
  if (SSet.empty()) return 0;

  for(const int SN : SSet)
    {
      const int result=trackDirection(SN,Pt,Norm);
      if (result) return result;
    }
  return 0;
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
		const int SN) const
  /*! 
    Determines is Pt is within the object 
    or on the surface
    \param Pt :: Point to be tested
    \param SN :: Excluded surf Number (signed)
    \returns 1 if true and 0 if false
  */
{
  return HRule.isValid(Pt,SN);
}

int
Object::isSideValid(const Geometry::Vec3D& Pt,
		      const int SN) const
  /*! 
    Determines is Pt is within the object 
    or on the surface
    \param Pt :: Point to be tested
    \param SN :: Excluded surf Number 
    \returns 1 if true and 0 if false
  */
{
  return HRule.isSideValid(Pt,SN);
}

std::set<int>
Object::surfValid(const Geometry::Vec3D& Pt) const
  /*! 
    Determines the surface set the point is on
    or on the surface
    \param Pt :: Point to be tested
    \param ExSN :: Excluded surf Number [unsigned]
    \returns 1 if true and 0 if false
  */
{
  std::set<int> out= HRule.surfValid(Pt);
  return out;
}

int
Object::isAnyValid(const Geometry::Vec3D& Pt,
		const std::set<int>& ExSN) const
/*! 
  Determines is Pt is within the object 
  or on the surface
  \param Pt :: Point to be tested
  \param ExSN :: Excluded surf Numbers
  \returns 1 if true and 0 if false
*/
{
  return HRule.isAnyValid(Pt,ExSN);
}


std::vector<std::pair<int,int>>
Object::getImplicatePairs(const int SN) const
  /*!
    Determine all the implicate pairs for the object
    The map is plane A has (sign A) implies plane B has (sign B)
    \param SN :: Number of force side
    \return Map of surf -> surf
  */
{
  ELog::RegMethod RegA("Object","getImplicatePairs(int)");

  const Geometry::surfImplicates& SImp=
      Geometry::surfImplicates::Instance();

  const ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();

  std::vector<std::pair<int,int>> Out;

  const Geometry::Surface* APtr=SurI.getSurf(SN);
  if (!APtr)
    throw ColErr::InContainerError<int>(SN,"Surface not found");

  for(const Geometry::Surface* BPtr : surfSet)
    {
      ELog::EM<<"BPR == "<<BPtr->getName()<<ELog::endDiag;
      if (APtr!=BPtr)
	{
	  std::pair<int,int> dirFlag=SImp.isImplicate(APtr,BPtr);
	  if (dirFlag.first)
	    Out.push_back
	      (std::pair<int,int>
	       (dirFlag.first*SN,dirFlag.second*BPtr->getName()));
	}
    }

  return Out;
}

std::vector<std::pair<int,int>>
Object::getImplicatePairs() const
  /*!
    Determine all the implicate pairs for the object
    The map is plane A has (sign A) implies plane B has (sign B)
    \return vector of implicate pairs
  */
{
  ELog::RegMethod RegA("Object","getImplicatePairs");

  const Geometry::surfImplicates& SImp=
      Geometry::surfImplicates::Instance();

  std::vector<std::pair<int,int>> Out;

  // Dont get smart :: A->B not same as B->A.
  for(const Geometry::Surface* APtr : surfSet)
    for(const Geometry::Surface* BPtr : surfSet)
      if (APtr!=BPtr)
	{
	  // This is JUST surface SIGNS:
	  const std::pair<int,int> dirFlag=SImp.isImplicate(APtr,BPtr);
	  if (dirFlag.first)
	    {
	      Out.push_back(std::pair<int,int>
			    (dirFlag.first * APtr->getName(),
			     dirFlag.second * BPtr->getName()));
	    }
	}

  return Out;
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

int
Object::isValid(const Geometry::Vec3D& Pt,
		const std::map<int,int>& SMap) const
/*! 
  Determines is group of surface maps are valid
  \param Pt :: Point to use if SMap incomplete
  \param SMap :: map of SurfaceNumber : status [-1/ 0(either) / 1]
  \returns 1 if true and 0 if false
*/
{
  return HRule.isValid(Pt,SMap);
}

std::map<int,int>
Object::mapValid(const Geometry::Vec3D& Pt) const
  /*! 
    Populates the validity map the surfaces
    For each surface determine if the surface is true(1)/false(-1)/
    onSide(0)
    
    \param Pt :: Point to testsd
    \returns Map [ surfNumber: -1/0/1 ]
  */
{
  ELog::RegMethod RegA("Object","mapValid");

  std::map<int,int> SMap;
  for(const Geometry::Surface* SPtr : surfSet)
    SMap.emplace(SPtr->getName(),SPtr->side(Pt));

  return SMap;
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

  surfSet.clear();
  surNameSet.clear();

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
		  surfSet.emplace(SurX->getKey());
		  surNameSet.insert(SurX->getKeyN()*SurX->getSign());
		}
	    }
	}
    }
  
  // sorted list will have zeros at front if there is a problem
  // (e.g not populated)
  if (surfSet.empty() || (*surfSet.begin()==0))
    {
      ELog::EM<<"surfSet Failure "<<ELog::endCrit;
      ELog::EM<<"CX == "<<debugCX.str()<<ELog::endCrit;
      ELog::EM<<"Found zero Item "<<this->getName()<<" "
	      <<" "<<populated<<" :: "<<ELog::endCrit;
      ELog::EM<<"SList size "<<surfSet.size()<<ELog::endCrit;
      ELog::EM<<"Cell == "<<*this<<ELog::endErr;
      throw ColErr::ExitAbort("Empty surf List");
    }

  return 1;
}

bool
Object::isVoid() const
  /*!
    Is the material a void material
    \return true if void
  */
{
  return matPtr->isVoid();
}


std::vector<int>
Object::getSurfaceIndex() const
  /*!
    Returns all of the numbers of surfaces
    \return surface indexes (no sign)
  */
{
  std::vector<int> out;
  for(const Geometry::Surface* SPtr : surfSet)
    out.push_back(SPtr->getName());

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
  return (surNameSet.find(SN)!=surNameSet.end()) ? 1 : 0;
}

int 
Object::surfSign(const int SN) const
  /*!
    Do we have the signed surface in this object
    \param SN :: signed number
    \return +/-1 or 0 [both/nosurf]
   */
{
  int sign=(surNameSet.find(SN)!=surNameSet.end()) ? 1 : 0;
  sign+=(surNameSet.find(-SN)!=surNameSet.end()) ? -1 : 0;
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
Object::substituteSurf(const int SurfN,
		       const int NsurfN,
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
  if (out)
    {
      populated=0;
      createSurfaceList();
    }
  return out;
}

int
Object::hasForwardIntercept(const Geometry::Vec3D& IP,
			    const Geometry::Vec3D& UV) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects)

    \param IP :: Initial point
    \param UV :: Forward going vector
    \return True(1) / Fail(0)
  */
{
  ELog::RegMethod RegA("Object","hadForwardIntercept");

  MonteCarlo::LineIntersectVisit LI(IP,UV);

  const std::vector<Geometry::interPoint>& IPts=
    LI.getIntercept(HRule);
  // sorted -ve to +ve ... only need one
  // +ve point to have intercept
  if (!IPts.empty() &&
      IPts.back().D>0.0)
    return 1;

  return 0;
}

int
Object::hasIntercept(const Geometry::Vec3D& IP,
		     const Geometry::Vec3D& UV) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects)

    \param IP :: Initial point
    \param UV :: direction vector
    \return True(1) / Fail(0)
  */
{
  ELog::RegMethod RegA("Object","hasIntercept");
  MonteCarlo::LineIntersectVisit LI(IP,UV);

  const std::vector<Geometry::interPoint>& IPts=
    LI.getIntercept(HRule);

  return (IPts.empty()) ? 0 : 1;
}


Geometry::interPoint 
Object::trackSurfIntersect(const Geometry::Vec3D& Org,
			   const Geometry::Vec3D& unitAxis) const
  /*!
    Track a line into an object. It effectively converts the
    object into a HeadRule, then tracks the line into the object.
    \param Org :: Origin of line
    \param unitAxis :: track of line
    \return Tuple of SurfNumber[signed]/surfacePointer/ImpactPoint/distance
  */
{
  return HRule.trackSurfIntersect(Org,unitAxis);
}

int
Object::trackSurf(const Geometry::Vec3D& Org,
		  const Geometry::Vec3D& unitAxis) const
  /*!
    Transfer function to move Object into headrule
    \param Org :: Origin of line
    \param unitAxis :: track of line
    \return Signed surf number
  */
{
  return 0; //HRule.trackSurf(Org,unitAxis);

}

Geometry::Vec3D
Object::trackPoint(const Geometry::Vec3D& Org,
		   const Geometry::Vec3D& unitAxis) const
  /*!
    Transfer function to move Object into HeadRule
    This calculates the line and return the first point
    that the line intersects
    \param Org :: Origin of line
    \param unitAxis :: track of line
    \return Signed surf number
  */
{
  ELog::RegMethod RegA("Object","trackPoint");
  
  return HRule.trackPoint(Org,unitAxis);
}

Geometry::Vec3D
Object::trackClosestPoint(const Geometry::Vec3D& Org,
			  const Geometry::Vec3D& unitAxis,
			  const Geometry::Vec3D& aimPt) const
  /*!
    Transfer function to move Object into HeadRule
    This calculates the line and return the first point
    that the line intersects
    \param Org :: Origin of line
    \param unitAxis :: track of line
    \return Signed surf number
  */
{
  ELog::RegMethod RegA("Object","trackClosetPoint");
  return HRule.trackClosestPoint(Org,unitAxis,aimPt);
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
Object::trackCell(const MonteCarlo::particle& N,
		  double& D,
		  const Geometry::Surface*& surfPtr,
		  const int startSurf) const
  /*!
    Track to a particle into/out of a cell. 
    \param N :: Particle 
    \param D :: Distance traveled to the cell [get added too]
    \param surfPtr :: Surface at exit [output]
    \param startSurf :: Start surface [to be ignored]
    \return surface number of intercept
   */
{
  ELog::RegMethod RegA("Object","trackCell[D,dir]");

  std::vector<Geometry::interPoint> IPts;
  HRule.calcSurfIntersection(N.Pos,N.uVec,IPts);
  // Remove the stuf about surNameSet etc... 
  const int absSN(std::abs(startSurf));
  const int signSN(startSurf>0 ? 1 : -1);   // pAB/mAB is 1 / 0 
  for(const Geometry::interPoint& ipt : IPts)
    {
      if (ipt.D>Geometry::zeroTol &&
	  ipt.SNum!=startSurf)
	{
	  if (ObjName==3720008)
	    ELog::EM<<"I == "<<ipt<<ELog::endDebug;

	  surfPtr=ipt.SPtr;
	  D=ipt.D;
	  return ipt.SNum;
	}
    }
  return 0;
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
  cx<<ObjName<<" "<<matPtr->getID();
  if (!matPtr->isVoid())
    cx<<" "<<matPtr->getAtomDensity()<<" ";
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
  cx<<matPtr->getID()<<" ";

  if (!matPtr->isVoid())
    cx<<matPtr->getAtomDensity()<<" ";
  
  cx<<HRule.display();
  return cx.str();
}

std::string
Object::cellStr(const std::map<int,Object*>& MList) const
  /*!
    Returns just the cell string object. Processes complement
    and self include.
    \param MList :: List of indexable Hulls
    \return Cell String (from TopRule)
    \todo Break infinite recusion
  */
{
  ELog::RegMethod RegA("Object","cellStr");

  const char compUnit[]="%#";
  std::string TopStr=this->topRule()->display();
  std::string::size_type pos=TopStr.find_first_of(compUnit);
  std::ostringstream cx;
  while(pos!=std::string::npos)
    {
      const int compFlag(TopStr[pos]=='%' ? 0 : 1); 
      pos++;
      cx<<TopStr.substr(0,pos);            // Everything including the #
      int cN(0);
      const size_t nLen=StrFunc::convPartNum(TopStr.substr(pos),cN);
      if (nLen>0)
        {
	  std::map<int,MonteCarlo::Object*>::const_iterator vc=MList.find(cN);
	  if (vc==MList.end() || cN==this->getName())
	    {
	      ELog::EM<<"Cell:"<<getName()<<" comp unit:"
		      <<cN<<ELog::endCrit;
	      ELog::EM<<"full string == "
		      <<topRule()->display()<<ELog::endCrit;
	      cx<<compUnit[compFlag]<<cN;
	      throw ColErr::InContainerError<int>
		(cN,"Object::cellStr unknown complementary unit");
	    }
	  else
	    {
	      if (compFlag) cx<<"(";
	      // Not the recusion :: This will cause no end of problems 
	      // if there is an infinite loop.
	      cx<<vc->second->cellStr(MList);
	      if (compFlag) cx<<")";
	    }
	  cx<<" ";
	  pos+=nLen;
	}
      TopStr.erase(0,pos);
      pos=TopStr.find_first_of(compUnit);
    }

  cx<<TopStr;
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
  if (trcl)
    cx<<" "<<"trcl="<<trcl;

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

  std::ostringstream cx;
  cx<<"ASSIGNMAT ";

  const int matID=matPtr->getID();
  if (imp.isZero())
    cx<<"BLCKHOLE";
  else if (matID>0)
    cx<<"M"+std::to_string(matID);
  else
    cx<<"VACUUM";
  
  cx<<" R"+std::to_string(ObjName);
  if (activeMag && !imp.isZero())
    cx<<" - - 1 ";
  
  
  StrFunc::writeFLUKA(cx.str(),OX);
  
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

  std::ostringstream cx;
  cx<<"* "<<FCUnit<<" "<<ObjName<<std::endl;
  cx<<"R"<<ObjName<<" "<<surfSet.size()<<" ";
  cx<<HRule.displayFluka()<<std::endl;
  StrFunc::writeMCNPX(cx.str(),OX);
  
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
  
  if (!isVoid())
    {
      // do not render global objects (outer void and black hole)
      //      if (objName.empty())
        //	return; 
      OX<<"// Cell "<<FCUnit<<" "<<ObjName<<"\n";
      OX<<"intersection{\n"
	<<HRule.displayPOVRay()<<"\n"
	<< " texture {mat" <<MW.NameNoDot(matPtr->getName()) <<"}\n"
	<< "}"<<std::endl;
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
  if (ObjName==1 && imp.isZero())
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

