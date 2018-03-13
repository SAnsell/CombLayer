/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Algebra.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MapSupport.h"
#include "BnId.h"
#include "AcompTools.h"
#include "Acomp.h"
#include "Algebra.h"

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OX,const Algebra& A)
  /*!
    Write to standard stream
    \param OX :: Output stream
    \param A :: Algebra to write
   */
{
  OX<<A.display();
  return OX;
}

Algebra::Algebra() :
  F(Union)
  /*!
    Constructor
  */
{}

Algebra::Algebra(const Algebra& A) :
  SurfMap(A.SurfMap),F(A.F),
  ImplicateVec(A.ImplicateVec)
  /*!
    Copy Constructor 
    \param A :: Algebra to copy
  */
{}

Algebra&
Algebra::operator=(const Algebra& A) 
  /*!
    Assignment operator
    \param A :: object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      SurfMap=A.SurfMap;
      F=A.F;
      ImplicateVec=A.ImplicateVec;
    }
  return *this;
}

Algebra::~Algebra()
  /*! Destructor */
{ }


bool
Algebra::operator==(const Algebra& A) const
  /*!
    Equality operator
    \param A :: object to compary
    \returns this==A
  */
{
  if (this==&A)
    return 1;
  return F==A.F;
}

bool
Algebra::operator!=(const Algebra& A) const
  /*!
    Inequality operator
    \param A :: object to compary
    \returns this!=A (via ==)
  */
{

  return !(F==A.F);
}

Algebra&
Algebra::operator+=(const Algebra& M)
  /*!
    Adds this by M algebrically 
    \param M :: Algebric object to add
    \return *this
  */
{
  F+=M.F;
  return *this;
}

Algebra&
Algebra::operator-=(const Algebra& M)
  /*!
    Adds this by M algebrically 
    \param M :: Algebric object to subtract/remove
    \return *this
  */
{
  F-=M.F;
  return *this;
}

Algebra&
Algebra::operator*=(const Algebra& M)
  /*!
    Multiplies this by M algebrically 
    \param M :: Algebric object to multiply by
    \return *this
  */
{
  F*=M.F;
  return *this;
}

Algebra
Algebra::operator+(const Algebra& M) const
  /*!
    Addition operator (or construction)
    \param M :: Algebra to add
    \return this+M
  */
{
  Algebra T(*this);
  T+=M;
  return T;
}

Algebra
Algebra::operator-(const Algebra& M) const
  /*!
    Subtraction operator (or removal)
    \param M :: Algebra to add
    \return this-M
  */
{
  Algebra T(*this);
  T-=M;
  return T;
}

Algebra
Algebra::operator*(const Algebra& M) const
  /*!
    Addition operator (and construction)
    \param M :: Algebra to multiply (and)
    \return this+M
  */
{
  Algebra T(*this);
  T*=M;
  return T;
}

size_t
Algebra::countComponents() const
  /*!
    Count the nubmer of components in the object
    \return compennts size
  */
{
  ELog::RegMethod RegA("Algebra","countComponents");
  
  return F.countComponents();
}
  
void
Algebra::expandBracket()
  /*!
    Expand all the brackets into DNF form
    Not need full sort before /after
  */
{
  F.Sort();
  F.expandBracket();
  F.Sort();
  return;
}


void
Algebra::expandCNFBracket()
  /*!
    Expand all the brackets into DNF form
    Not need full sort before /after
  */
{
  F.Sort();
  F.expandCNFBracket();
  F.Sort();
  return;
}

void
Algebra::merge()
  /*!
    MERGE all the brackets into DNF form
  */
{
  F.merge();
  return;
}
  
void
Algebra::Complement()
  /*!
    Takes the complement of the algebric
    function.
  */
{
  F.complement();
  return;
}

std::pair<Algebra,Algebra>
Algebra::algDiv(const Algebra& D) const
  /*!
    Divide by D algebrically
    \param D :: Divisor
    \returns Quotian + Remainder
   */
{
  Algebra Q;
  Algebra R;
  Acomp Tf=F;
  std::pair<Acomp,Acomp> QR=Tf.algDiv(D.F);
  if (!QR.first.isNull() && 
      !QR.second.isNull())
    {
      Q.setFunction(QR.first);
      R.setFunction(QR.second);
    }
  return std::pair<Algebra,Algebra>(Q,R);
}

int
Algebra::convertMCNPSurf(const int mcnpSN) const
  /*!
    Convert the surf number to interal index
    \param mcnpSN :: mcnpSurf number
    \return internal number
  */
{
  std::map<int,int>::const_iterator ac=
    SurfMap.find(std::abs(mcnpSN));
  if (ac==SurfMap.end())
    throw ColErr::InContainerError<int>(mcnpSN,"mcnpSN -> SurfMap");
  return (mcnpSN>0) ? ac->second : -ac->second;
}

void
Algebra::addImplicates(const std::vector<std::pair<int,int> > & IM)
  /*!
    Adds the implicates to the from a list of surface numbers
    [signed]
    \param IM :: Implicate list 
    
   */
{
  ELog::RegMethod RegA("Algebra","addImplicates");

  for(const std::pair<int,int>& mc : IM)
    {
      std::map<int,int>::const_iterator ac=
	SurfMap.find(std::abs(mc.first));
      std::map<int,int>::const_iterator bc=
	SurfMap.find(std::abs(mc.second));

      if (ac!=SurfMap.end() && bc!=SurfMap.end())
	{
	  const int surfA=(mc.first>0) ? ac->second  : -ac->second;
	  const int surfB=(mc.second>0) ? bc->second  : -bc->second;
	  ImplicateVec.push_back(std::pair<int,int>(surfA,surfB));
	}
    }
  return;
}


bool
Algebra::constructShannonDivision(const int mcnpSN)
  /*!
    Given a surface number SN [signed]
    can we divide base on implicates of that surfac
    removing the +/- part 
    \parma mcnpSN :: Surface for divide and implicate
    \return true if a surface can be removed
  */
{
  ELog::RegMethod RegA("Algebra","constructShannonDivision");
  
  const int SN=convertMCNPSurf(mcnpSN);
  const int ASN(std::abs(SN));
  Acomp FX(F);
  FX.expandCNFBracket();
  Acomp AD=FX;

  for(const std::pair<int,int>& IP : ImplicateVec)
    {
      int SNA=IP.first;
      int SNB=IP.second;
      int ANA=std::abs(SNA);
      int ANB=std::abs(SNB);
      if (ANB==ASN)
	{
	  std::swap(ANA,ANB);
	  std::swap(SNA,SNB);
	  SNA*=-1;
	  SNB*=-1;
	}

      if (SNA==SN)
	AD.resolveTrue(SNB);
    }
  F=AD;

  return 1;  
}
  
bool
Algebra::constructShannonExpansion()
  /*!
    Overall schema -
       - make algebra with ALL implicates
       - for each +/- pair resolve shannon expansion
       - Compute F=a'F(00)+a'bF(01)+bF(11)
          assuming that a->b is true 
       -- if a -> -b then apply -1 to flag of b
       - if F(01) is null and either F(11) or F(00) is null
             then remove either/and  a and b from the equation.
       - re-resolve for next literal
    \return true if a surface was removed
   */
{
  ELog::RegMethod RegA("Algebra","constructShannonExpansion");

  Acomp FX(F);
  FX.expandCNFBracket();

  std::set<int> LitM;
  FX.getLiterals(LitM);


  for(const std::pair<int,int>& IP : ImplicateVec)
    {
      //
      // now do shannon expansion about both literals [together]
      // if the literals are opposite signed then we reverse one nad
      // continue/

      const int& SNA=IP.first;
      const int& SNB=IP.second;
      
      Acomp FaFbT(FX);
      FaFbT.resolveTrue(-SNA);     // a=0
      FaFbT.resolveTrue(SNB);    // b=1
      if (FaFbT.isFalse())
	{
	  Acomp FaFbF(FX);
	  Acomp FaTbT(FX);
	  FaFbF.resolveTrue(-SNA);
	  FaFbF.resolveTrue(-SNB);

	  FaTbT.resolveTrue(SNA);
	  FaTbT.resolveTrue(SNB);

	  // POST PROCESS
	  if (FaFbF.isFalse())  // kill by either removing a or using FaTbT?
	    {
	      ELog::EM<<"REMOVAL of "<<Acomp::strUnit(SNA)<<ELog::endDiag;
	      FX=FaTbT;
	      FX.addIntersect(SNB);
	    }
	  if (FaTbT.isFalse())
	    {
	      ELog::EM<<"REMOVAL of "<<Acomp::strUnit(SNB)<<ELog::endDiag;
	      FX=FaFbF;
	      FX.addIntersect(-SNA);	      
	    }
	}
	      
    }

  return 1;
}
  
int
Algebra::setFunctionObjStr(const std::string& A)
  /*!
    Fill the algebra (AComp) with an object given an 
    MCNPX String. 
    The string type is of 3 : 5 : 6 -4 #( xx  ) 
    - where #( ) surroud the string
    \param A :: string to process (stripped of id,density etc)
    \retval -1 ::  Failure
    \retval 0 ::  Success
  */
{
  ELog::RegMethod RegA("Algebra","setFunctionObjStr");

  // get first item
  std::ostringstream cx;
  int nLitIndex=1;
  size_t ipt(0);    // start of component
  while (ipt<A.length())
    {

      if (A[ipt]=='(' || A[ipt]==')')
        {
	  cx<<A[ipt];
	  ipt++;
	}
      else if (A[ipt]=='-' || isdigit(A[ipt]) )
        {
	  int N;
	  int neg(1);
	  size_t nCount=
	    StrFunc::convPartNum(A.substr(ipt,std::string::npos),N);
	  if (nCount)
	    {
	      if (N<0)
	        {
		  N*=-1;
		  neg=-1;
		}
	      std::map<int,int>::iterator mc=SurfMap.find(N);
	      if (mc==SurfMap.end())
	        {
		  SurfMap.emplace(N,nLitIndex);
		  cx<<Acomp::strUnit(neg*nLitIndex);
		  nLitIndex++;
		}
	      else
		cx<<Acomp::strUnit(neg*mc->second);
	      // Add to the number
	      ipt+=nCount;
	    }
	  else
	    throw ColErr::MisMatch<size_t>(nCount,0,"Ncount");
	}
      else if (A[ipt]==':')
        {
	  cx<<"+";
	  ipt++;
	}
      else if (A[ipt]=='#')
        {
	  cx<<"#";
	  ipt++;
	}
      else         // Space
	ipt++;
    }
  setFunction(cx.str());
  return 0;
}

int
Algebra::setFunction(const std::string& A)
  /*!
    Set the function using a basic string (abc etc)
    \param A :: String to use for the function
    \retval -1 Error
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("Algebra","setFunction");
  // Get copy
  std::string Ln=A;

  // Strip spaces.
  std::string::size_type pos;
  while((pos=Ln.find(' '))!=std::string::npos)
    Ln.erase(pos,1);

  try
    {
      F.setString(Ln);
    }
  catch (ColErr::ExBase&)
    {
      ELog::EM<<"Algebra String Error:"<<A<<ELog::endErr;
      return 1;
    }
  return 0;
}

void
Algebra::resolveTrue(const std::string& unit)
  /*!
    Given a string resolve for that unit to be removed
    \param unit :: string in form a' or a
   */
{
  if (unit.empty())
    return;
  const int index=Acomp::unitStr(unit);
  F.resolveTrue(index);
  return;
}
  
int
Algebra::setFunction(const Acomp& A)
  /*!
    Set the function using a toplevel
    Acomp.
    \param A :: Acomp to be copied to F.
    \returns 0 on success
  */
{
  F=A;
  return 0;
}

int
Algebra::getSurfIndex(const int SN) const
  /*!
    Convert a surf index into a mcnp object surface number
    \param SN :: Surface number						
    \return mcnp surf number
  */
{  
  const int ASN=std::abs(SN);
  // Converts a character to a surface number
  std::map<int,int>::const_iterator vc=
    find_if(SurfMap.begin(),SurfMap.end(),
	    MapSupport::valEqual<int,int>(ASN));
  if (vc==SurfMap.end())
    throw ColErr::InContainerError<int>(SN,"Algebra::SurfMap");

  return (SN<0) ? -vc->first : vc->first;
}
  
int
Algebra::getSurfIndex(std::string SName) const
  /*!
     Convert a string into an index value
     \param SName :: String name
     \return signed value
  */
{
  if (SName.empty()) return 0;

  const int SN = Acomp::unitStr(SName);
  return getSurfIndex(SN);
}

std::string
Algebra::getSurfKey(const int SN) const
  /*!
     Convert a string into an index value
     \param SName :: String name
     \return signed value
  */
{
  std::map<int,int>::const_iterator vc=SurfMap.find(std::abs(SN));
  if (vc!=SurfMap.end())
    {
      return (SN<0) ? Acomp::strUnit(-vc->second) :
	Acomp::strUnit(vc->second);
    }
  return "VOID";
}
  
int
Algebra::countLiterals() const
  /*!
    Count the number of different literals
    in the algebraic function
    Does this by generating the map of literals
    \returns number of literals found
  */
{
  std::set<int> Lit;
  F.getLiterals(Lit);
  return static_cast<int>(Lit.size());
}

bool
Algebra::logicalEqual(const Algebra& A) const
  /*!
    Calculate if two functions are logically
    equivilent (exhaustive search)
    \param A :: Algrebra to test
    \return True/False
   */
  
{
  return F.logicalEqual(A.F);
}


std::string
Algebra::display() const
  /*!
    Write out the algoritnm to a string
    \return reduced form
  */
{
  return F.display();
}

std::ostream&
Algebra::write(std::ostream& Out) const
  /*!
    Output function
    \param Out :: Ostream to write out
    \return Out
  */
{
  Out<<"F == "<<F.display()<<std::endl;
  return Out;
}

std::string
Algebra::writeMCNPX() const
  /*!
    Writes out the string in terms
    of surface numbers for MCNPX.
    Note the ugly use of valEqual to find the cell
    since the SurfMap is the wrong way round.
    This also has the problem that Algebra uses
    intersection as master but MCNPX uses union 
    \return MCNPX string of algebra
  */
{
  ELog::RegMethod RegA("Algebra","writeMCNPX");

  // First wrap the function if it is a union.
  std::string Out;
  Out = (F.isInter()) ? F.display() : "("+F.display()+")";
  const size_t lenOut=Out.length();
  Out+=" ";      // Guard string

  std::ostringstream cx;
  for(size_t i=0;i<lenOut;i++)
    {
      if (islower(Out[i]) || isupper(Out[i]) || Out[i]=='%')
        {
	  // Handle overflow
	  std::string item;
	  if (Out[i]=='%')
	    {
	      do
		{
		  item+=Out[i];
		  i++;
		}
	      while (i<lenOut && isdigit(Out[i]));
	      i--;

	    }
	  else
	    item=std::string(1,Out[i]);
	  
	  const int SN = getSurfIndex(item);	    

	  if (Out[i+1]=='\'')
	    {
	      cx<<" "<<-SN;
	      i++;
	    }
	  else
	    cx<<" "<<SN;
	}
      else if (Out[i]=='+')
        {
	  cx<<" :";
	}
      else       // brackets are constant
        {
	  cx<<" "<<Out[i];
	}
    }
  return cx.str();
}

} // NAMESPACE MonteCarlo
