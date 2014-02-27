/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/Material.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "regexSupport.h"
#include "RefCon.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"

namespace MonteCarlo
{

Material::Material() :
  Mnum(-1),Name("Unnamed"),
  atomDensity(0.0)
  /*!
    Default Constructor
  */
{} 

Material::Material(const Material& A) :
  Mnum(A.Mnum),Name(A.Name),zaidVec(A.zaidVec),
  mxCards(A.mxCards),Libs(A.Libs),SQW(A.SQW),
  atomDensity(A.atomDensity)
  /*!
    Default Copy constructor
    \param A :: Material to copy
  */
{ }

Material&
Material::operator=(const Material& A)
  /*!
    Assignment operator
    \param A :: Material to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Mnum=A.Mnum;
      Name=A.Name;
      zaidVec=A.zaidVec;
      mxCards=A.mxCards;
      Libs=A.Libs;
      SQW=A.SQW;
      atomDensity=A.atomDensity;
    }
  return *this;
}

Material::~Material()
  /*!
    Standard Destructor
  */
{ }

Material&
Material::operator*=(const double V) 
  /*!
    Scale the material by V
    \param V :: Scale value
    \return this * V
   */
{
  ELog::RegMethod RegA("Material","operator*");
  
  std::vector<Zaid>::iterator vc;
  for(vc=zaidVec.begin();vc!=zaidVec.end();vc++)
    vc->setDensity(vc->getDensity()*V);
  
  return *this;
}

Material&
Material::operator+=(const Material& A) 
  /*!
    Material to add
    \param A :: Object to add
    \return this + A
   */
{
  ELog::RegMethod RegA("Material","operator*");
  
  // Need to match off Zaids:
  std::vector<Zaid>::const_iterator vc;
  std::vector<Zaid>::iterator lc;
  for(vc=A.zaidVec.begin();vc!=A.zaidVec.end();vc++)
    {
      lc=std::find(zaidVec.begin(),zaidVec.end(),*vc);
      if (lc!=zaidVec.end())
	lc->setDensity(vc->getDensity()+lc->getDensity());
      else
	zaidVec.push_back(*vc);
    }
  
  return *this;
}

void
Material::setENDF7()
  /*!
    Modifies the zaid to have ENDF7 70c tag
  */
{
  std::vector<Zaid>::iterator vc;
  for(vc=zaidVec.begin();vc!=zaidVec.end();vc++)
    {
      if (vc->getIso()>0 && vc->getTag()!=24 && vc->getTag()!=80)
	vc->setTag(70);
    }
  return;
}

size_t
Material::getZaidIndex(const int ZNum,const int TNum,const char C) const
  /*!
    Determine if a Zaid exists
    \param ZNum :: Zaid number
    \param TNum :: Tag number 
    \param C :: Character
    \return ULONG_MAX on no match / otherwize Index
  */
{
  for(size_t i=0;i<zaidVec.size();i++)
    if (zaidVec[i].isEquavilent(ZNum,TNum,C))
      return i;
  
  return ULONG_MAX;
}

void
Material::setMXitem(const int ZNum,const int TNum,const char C,
		    const std::string& P,const std::string& Item)
  /*!
    Subsisture a zaid if it exists
    \param ZNum :: Zaid number
    \param TNum :: Tag number
    \param C :: Type
    \param P :: Particle
    \param Item :: Item to substute
   */
{
  typedef std::map<std::string,MXcards> MXTYPE;

  const size_t NIndex=getZaidIndex(ZNum,TNum,C);
  if (NIndex!=ULONG_MAX)
    {
      MXTYPE::iterator mc= mxCards.find(P);
      if (mc==mxCards.end())
	mxCards.insert(MXTYPE::value_type
		       (P,MXcards(Mnum,zaidVec.size(),P)));
      mc= mxCards.find(P);
      mc->second.setItem(NIndex,Item);
    }
  return;
}

int
Material::lineType(std::string& Line)
  /*!
    Determine the type of the line. Removes
    trailing spaces etc
    \param Line :: Line to process
    \retval 0 :: no information / contline
    \retval -1 :: empty/comment line
    \retval -100 :: END statement
    \retval +ve :: m[tx] card
  */
{
  // END is in a comment (normally):
  if (Line.find("END")!=std::string::npos)
    return -100;

  boost::regex Re("^\\s{0,5}[mM]\\D*(\\d+)",boost::regex::perl);

  StrFunc::stripComment(Line);
  if (Line.size()<1 ||                     // comments blank line, ^c or ^c<spc> 
      (tolower(Line[0])=='c' && 
       (Line.size()==1 || isspace(Line[1]))))
    return -1;
  
  
  int index;
  if (StrFunc::StrComp(Line,Re,index,0))
    {
      Line=StrFunc::fullBlock(Line);              // remove first+last 
      return index;
    }

  Line=StrFunc::fullBlock(Line);              

  // Continuation line
  return 0;
}

int
Material::getExtraType(std::string& Line,std::string& particles)
  /*!
    Determine the extra type and remove from the string
    trailing spaces etc
    \param Line :: Line to process
    \param particles :: Extra particle after a :
    \retval 1 :: mt line
    \retval 2 :: mx line 
    \retval 0 :: nothing
  */
{
  boost::regex Re("(^\\s{0,5}([mM]\\D*)\\d+)",boost::regex::perl);
  
  std::string Item;

  if (StrFunc::StrFullCut(Line,Re,Item,1))
    {
      for(size_t i=0;i<Item.size();i++)
	Item[i]=static_cast<char>(tolower(Item[i]));
      if (Item=="mt") return 1;
      if (Item=="mx") 
	{
	  boost::regex ReP("^\\s*:\\s*(\\S)",boost::regex::perl);
	  if (StrFunc::StrFullCut(Line,ReP,Item))
	    particles=Item;
	  return 2;
	}
    }
  // Unimportant line:
  return 0;
}

int
Material::setMaterial(const int MIndex,
		      const std::string& N,
		      const std::string& MLine,
		      const std::string& MTLine,
		      const std::string& LibLine)
  /*!
    Sets the material given a full line from MCNPX.
    Passes strings of type 
     - m1 56097.70c 0.034 
     - mt24 be77.0k hlib=.24h
    \param MIndex :: Index number
    \param N :: Name
    \param MLine :: Zaid line
    \param MTLine :: Treatment line
    \param LibLine :: Libraries
    \retval -ve :: on error
    \retval 0 :: on success
  */
{

  ELog::RegMethod RegA("Material","setMaterial(i,s,s,s,s)");
  const int retVal=setMaterial(MIndex,MLine,MTLine,LibLine);
  if (!retVal) Name=N;
  return retVal; 
}

int
Material::setMaterial(const int MIndex,
		      const std::string& MLine,
		      const std::string& MTLine,
		      const std::string& LibLine)
  /*!
    Sets the material given a full line from MCNPX.
    Passes strings of type 
     - m1 56097.70c 0.034 
     - mt24 be77.0k hlib=.24h
    \param MIndex :: Index number
    \param MLine :: Zaid line
    \param MTLine :: Treamtment line
    \param LibLine :: Libraries
    \retval -ve :: on error
    \retval 0 :: on success
  */
{

  ELog::RegMethod RegA("Material","setMaterial(i,s,s,s)");

  Mnum=MIndex;
  // Divide line into zaid / density / extras:
  const boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(MLine,divSea);
  if (Items.size()<2) 
    {
      ELog::EM<<"MLine is empty: "<<MLine<<ELog::endErr;
      return -1;
    }
  
  zaidVec.clear();
  mxCards.clear();
  Libs.clear();
  SQW.clear();

  Zaid AZ;
  double Dens;                   // density
  std::vector<std::string>::iterator vc;
  int typeFlag(0);
  // skip first item
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (!typeFlag)
	{
	  if (!AZ.setZaid(*vc))
	    ELog::EM<<"Failed to convert Zaid:"<<*vc<<ELog::endErr;
	  typeFlag=1;
	}
      else
	{
	  if (!StrFunc::convert(*vc,Dens))
	    ELog::EM<<"Failed to convert Density:"<<*vc<<ELog::endErr;
	  AZ.setDensity(Dens);
	  zaidVec.push_back(AZ);
	  typeFlag=0;
	}
    }
  // PROCESS MT Line:
  std::vector<std::string> MTItems=StrFunc::StrParts(MTLine,divSea);
  for(vc=MTItems.begin();vc!=MTItems.end();vc++)
    SQW.push_back(StrFunc::fullBlock(*vc));
  
  // PROCESS LIBS
  std::vector<std::string> LibItems=StrFunc::StrParts(LibLine,divSea);
  Libs.insert(Libs.end(),LibItems.begin(),LibItems.end());

  calcAtomicDensity();

  return 0;
}

int
Material::setMaterial(const std::vector<std::string>& PVec)
  /*!
    Sets the material given a full line from MCNPX.
    Passes strings of type 
     - m1 56097.70c 0.034 hlib=24h 
     - mt24 be77.0k
    \param PVec :: full string from MCNPX
    \retval -ve :: on error
    \retval 0 :: on success
  */
{
  ELog::RegMethod RegA("Material","setMaterial");
  // Process First line : 
  std::string Pstr=PVec[0];

  // Divide line into zaid / density / extras:
  const boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Pstr,divSea);
  if (Items.size()<2)
    return -1;

  if (!StrFunc::StrComp(Items[0],boost::regex("^[mM](\\d+)"),Mnum))
    {
      ELog::EM<<"Non material card at "<<Items[0]<<ELog::endErr;
      return -2;
    }
  
  Zaid AZ;
  double Dens;                   // density
  std::vector<std::string>::iterator vc=Items.begin();
  int typeFlag(0);
  // skip first item
  for(vc++;vc!=Items.end();vc++)
    {
      if (!typeFlag)
	{
	  if (AZ.setZaid(*vc))
	    typeFlag=1;
	  else              // Maybe a modifier 
	    {
	      Libs.push_back(*vc);
	    }
	}
      else
	{
	  if (StrFunc::convert(*vc,Dens))
	    {
	      AZ.setDensity(Dens);
	      zaidVec.push_back(AZ);
	      typeFlag=0;
	    }
	}
    }
  // NOW PROCESS EXTRA LINES: 
  std::vector<std::string>::const_iterator sc=PVec.begin();
  for(sc++;sc!=PVec.end();sc++)
    {
      std::string ItemStr=*sc;
      std::string Particle;
      const int mType=getExtraType(ItemStr,Particle);
      switch (mType)
	{
	case 1:          // mt card
	  SQW.push_back(StrFunc::fullBlock(ItemStr));
	  break;
	case 2:          // mx card
	  ELog::EM.error("Un supported mx card");
	  break;
	}
    }
  calcAtomicDensity();
  return 0;
}

void
Material::setDensity(const double D)
  /*!
    Scale the density
    \param D :: Density [atom/A^3]
  */
{
  ELog::RegMethod RegA("Material","setDensity");

  std::vector<Zaid>::const_iterator zcc;
  std::vector<Zaid>::iterator zc;
  double FSum(0.0);
  for(zcc=zaidVec.begin();zcc!=zaidVec.end();zcc++)
    FSum+=zcc->getDensity();

  //    std::accumulate<double>(zaidVec.begin(),zaidVec.end(),
  //	       0.0,boost::bind<double>(&Zaid::getDensity,_1));

  if (fabs(FSum)<1e-7)
    throw ColErr::NumericalAbort("Sum of zaidDensity: zero");
      
  if (D>0.0)
    {
      for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
	zc->setDensity(D*zc->getDensity()/FSum);
      atomDensity=D;
    }
  else if (D<0.0)
    {
      double MAW(0.0);   // Mean atomic weight

      for(zcc=zaidVec.begin();zcc!=zaidVec.end();zcc++)
	MAW+=zcc->getAtomicMass()*zcc->getDensity()/FSum;
      if (fabs(MAW)<0.5)
	throw ColErr::NumericalAbort("MAW too low: <0.5 ");
      const double aRho= -D*RefCon::avogadro/MAW;

      for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
	zc->setDensity(aRho*zc->getDensity()/FSum);
      atomDensity=aRho;
    }
  return;
}

void
Material::calcAtomicDensity() 
  /*!
    Calculate the atomic density
   */
{
  atomDensity=0.0;
  std::vector<Zaid>::const_iterator zc;
  for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
    atomDensity+=zc->getDensity();
  return;
}

void
Material::listComponent() const
  /*!
    List the ZAID components to std::cout
  */
{
  std::cout<<"Material :"<<Name<<" : "<<Mnum<<std::endl;
  std::vector<Zaid>::const_iterator vc;
  for(vc=zaidVec.begin();vc!=zaidVec.end();vc++)
    std::cout<<*vc<<std::endl;

  return;
}


void 
Material::print() const
  /*!
    Prints out the information about the material
    in a humman readable form
  */
{
  std::cout<<"Material "<<Name<<" N == "<<zaidVec.size()<<std::endl;
  std::vector<Zaid>::const_iterator zc;
  std::vector<std::string>::const_iterator vc;

  for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
    std::cout<<*zc<<std::endl;

  std::cout<<"Libs == ";
  for(vc=Libs.begin();vc!=Libs.end();vc++)
    std::cout<<*vc<<"  ";
  std::cout<<std::endl;

  std::cout<<"S(q,w)== ";
  for(vc=SQW.begin();vc!=SQW.end();vc++)
    std::cout<<*vc<<"  ";
  std::cout<<std::endl;

  return;
} 

void
Material::writeCinder(std::ostream& OX) const
  /*!
    Writes out material card in Cinder format
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Material","writeCinder");

  std::ostringstream cx;
  cx.precision(10);
  std::vector<int> cZaid;      // Cinder zaid list
  std::vector<double> cFrac;   // Fraction 

  // Construct zaid and fractions
  std::vector<Zaid>::const_iterator vc;

  double sum(0.0);
  for(vc=zaidVec.begin();vc!=zaidVec.end();vc++)
    {
      Element::Instance().addZaid(*vc,cZaid,cFrac);
      sum+= vc->getDensity();;
    }
  transform(cFrac.begin(),cFrac.end(),cFrac.begin(),
	    std::bind2nd(std::divides<double>(),sum));
  
  if (!cZaid.empty())
    {
      std::ios_base::fmtflags saveFlag=OX.flags();
      OX.setf(std::ios_base::right,std::ios_base::adjustfield);
      OX<<"mat";
      OX.width(3);
      OX<<Mnum;
      OX.width(4);
      OX<<cZaid.size()<<atomDensity<<std::endl;
      for(size_t i=0;i<cZaid.size();i++)
	writeZaid(OX,cFrac[i],cZaid[i]);
      OX.flags(saveFlag);
    }
  return;
}

void
Material::changeLibrary(const int T,const char key)
  /*!
    Change the library type of all materials out
    of the 000 type.
    \param T :: New Tag
    \param key :: type-key
  */
{
  std::vector<Zaid>::iterator zc;
  for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
    {
      if (zc->getIso() && zc->getKey()==key)
	{
	  zc->setTag(T);
	}
    }
  return;
}

void 
Material::writeZaid(std::ostream& OX,const double F,const int ZD) 
  /*!
    Write the zaid file
    \param OX :: Output streamx
    \param F :: Fraction 
    \param ZD :: Zaid number
  */
{
  ELog::RegMethod RegA("Material","writeZaid");
  boost::format ZFMT("%3d%03d0  %10.7e");
  std::vector<int> ANat;
  std::vector<double> FNat;
  const int Z=ZD/1000;
  const int A=ZD % 1000;
  if (A>0)
    OX<<(ZFMT % A % Z % F)<<std::endl;
  return;
}

void 
Material::write(std::ostream& OX) const
  /*!
    Write out the information about the material
    in a humman readable form (same as mcnpx file)
    \param OX :: Output stream
  */
{
  typedef std::map<std::string,MXcards> MXTYPE;

  std::ostringstream cx;
  cx.precision(10);
  cx<<"m"<<Mnum<<"     ";
  if (Mnum<10) cx<<" ";
  std::vector<Zaid>::const_iterator zc;
  std::vector<std::string>::const_iterator vc;
  for(zc=zaidVec.begin();zc!=zaidVec.end();zc++)
    cx<<*zc<<" ";

  for(vc=Libs.begin();vc!=Libs.end();vc++)
    cx<<*vc<<"  ";

  StrFunc::writeMCNPX(cx.str(),OX);

  MXTYPE::const_iterator mc;
  for(mc=mxCards.begin();mc!=mxCards.end();mc++)
    mc->second.write(OX);


  std::ostringstream rx;
  if (!SQW.empty())
    {
      rx.str("");
      rx<<"mt"<<Mnum<<"    ";
      if (Mnum<10) rx<<" ";
      for(vc=SQW.begin();vc!=SQW.end();vc++)
	rx<<*vc<<" ";
      StrFunc::writeMCNPX(rx.str(),OX);
    }
  return;
} 

}  // NAMESPACE MonteCarlo
