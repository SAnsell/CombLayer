/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Material.cxx
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
#include <climits>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <memory>
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
#include "RefCon.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OX,const Material& MT)
  /*!
    Write out the material 
    \param OX :: Outstream
    \param MT :: Material 
   */
{
  MT.write(OX);
  return OX;
}

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
  ELog::RegMethod RegA("Material","operator*=");

  for(Zaid& ZR : zaidVec)
    ZR.setDensity(ZR.getDensity()*V);
  
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
  ELog::RegMethod RegA("Material","operator+=");

  // Need to match off Zaids:
  for(const Zaid& ZItem : A.zaidVec)
    {
      std::vector<Zaid>::iterator lc=
        std::find(zaidVec.begin(),zaidVec.end(),ZItem);
      if (lc!=zaidVec.end())
	lc->setDensity(ZItem.getDensity()+lc->getDensity());
      else
	zaidVec.push_back(ZItem);
    }

  // SQW:
  std::set<std::string> sqSet;
  for(const std::string& SQItem : SQW)
    sqSet.insert(SQItem);

  for(const std::string& SQItem : A.SQW)
    {
      if (sqSet.find(SQItem)==sqSet.end())
        SQW.push_back(SQItem);
    }

  // MX:
  typedef std::map<std::string,MXcards> MXTYPE;
  for(MXTYPE::value_type MX : mxCards)
    {
      // find if has particle
      MXTYPE::const_iterator mc=A.mxCards.find(MX.first);
      // only add if doesn't exist
      if (mc!=A.mxCards.end())
	{
	  MXcards& mx=MX.second;
	  const MXcards& Amx=mc->second;
	  for(const Zaid& ZI : zaidVec)
	    {
	      const size_t zIndex(ZI.getZaidNum());
	      if (!mx.hasZaid(zIndex) && Amx.hasZaid(zIndex) )
		mx.setZaid(zIndex,Amx.getZaid(zIndex));
	    }
	}
    }
  // Now add All cards in A, that don't exist in *this.
  for(const MXTYPE::value_type MX : A.mxCards)
    {
      MXTYPE::const_iterator mc=mxCards.find(MX.first);
      if (mc==mxCards.end())
	mxCards.emplace(mc->first,mc->second);
    }

  // SQW
  for(const std::string& SQItem : A.SQW)
    {
      if (sqSet.find(SQItem)==sqSet.end())
        SQW.push_back(SQItem);
    }

  // LIBS:
  sqSet.clear();
  for(const std::string& LItem : Libs)
    sqSet.insert(LItem);

  for(const std::string& LItem : A.Libs)
    {
      if (sqSet.find(LItem)==sqSet.end())
	Libs.push_back(LItem);
    }
  
  calcAtomicDensity();
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
Material::getZaidIndex(const size_t ZNum,const size_t TNum,const char C) const
  /*!
    Determine if a Zaid exists
    \param ZNum :: Zaid number
    \param TNum :: Tag number 
    \param C :: Character
    \return ULONG_MAX on no match / otherwize Index
  */
{
  for(size_t i=0;i<zaidVec.size();i++)
    if (zaidVec[i].isEquivalent(ZNum,TNum,C))
      return i;
  
  return ULONG_MAX;
}

bool
Material::hasZaid(const size_t ZNum,const size_t TNum,const char C) const
  /*!
    Determine if a Zaid exists
    \param ZNum :: Zaid number [ignored if ZNum==0;]
    \param TNum :: Tag number [ignored if TNum==0;]
    \param C :: Character  [Ignored if C==0 ]
    \return ULONG_MAX on no match / otherwize Index
  */
{
  return 
    (getZaidIndex(ZNum,TNum,C)==ULONG_MAX) ? 0 : 1;
}

void
Material::setMXitem(const size_t ZNum,const size_t TNum,const char C,
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

  if (!P.empty())
    {
      const size_t NIndex=getZaidIndex(ZNum,TNum,C);
      if (NIndex!=ULONG_MAX)
	{
	  MXTYPE::iterator mc= mxCards.find(P);
	  if (mc==mxCards.end())
	    mxCards.emplace(P,MXcards(P));
	  mc= mxCards.find(P);
	  mc->second.setZaid(ZNum,Item);
	}
    }
  return;
}

void
Material::removeMX(const std::string& P)
  /*!
    Remove a given MX card by particle type
    \param P :: particle type
   */
{
  typedef std::map<std::string,MXcards> MXTYPE;

  MXTYPE::iterator mc=mxCards.find(P);
  if (mc!=mxCards.end())
    mxCards.erase(mc);
  
  return;
}

void
Material::removeLib(const std::string& lHead)
  /*!
    Remove a given lib component card 
    \param lHead :: header for removal
   */
{
  const std::string::size_type len=lHead.size();

  std::vector<std::string>::iterator vc =
    std::remove_if(Libs.begin(),Libs.end(),
                   [&len,&lHead](const std::string& Item)
                   { return (Item.substr(0,len)==lHead); });

  Libs.erase(vc,Libs.end());
  return;
}

  
int
Material::lineType(std::string& Line)
  /*!
    Determine the type of the line. Removes trailing spaces etc
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
  
  StrFunc::stripComment(Line);
  if (Line.size()<1 ||                     // comments blank line, ^c or ^c<spc> 
      (tolower(Line[0])=='c' && 
       (Line.size()==1 || isspace(Line[1]))))
    return -1;
  
  Line=StrFunc::fullBlock(Line);                
  int index;
  std::string part;
  if (!StrFunc::convert(Line,part)) return 0;
  if (part.size()<2 || (part[0]!='m' && part[0]!='M')) 
    return 0;
  part[0]=' ';
  if (StrFunc::convert(part,index))
    return index;
  // do something clever here:
  if (part[1]=='x' || part[1]=='X' ||
      part[1]=='t' || part[1]=='T' )
    part[1]=' ';
  if (StrFunc::convert(part,index))
    return index;
  

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
  ELog::RegMethod RegA("Material","getExtraType");

  std::string FItem;
  std::string LCopy(Line);
  if (!StrFunc::section(LCopy,FItem) || FItem.size()<3)
    return 0;
  StrFunc::lowerString(FItem);
  if (FItem[0]!='m') return 0;
  // get key
  std::string key;
  for(size_t i=0;i<FItem.size() && isalpha(FItem[i]);i++)
    {
      key+=FItem[i];
      FItem[i]=' ';
    }
  int index;
  if (!StrFunc::sectPartNum(FItem,index))
    return 0;
  Line=FItem+LCopy;
  if (key=="mx")
    {
      // Put tail back  now ==>   : stuff
      FItem+=LCopy;
      std::string unit;
      if (!StrFunc::section(FItem,unit) || 
	  unit[0]!=':')
	return 2;

      if (unit.size()>1)
	particles=unit.substr(1,std::string::npos);
      else
	StrFunc::section(FItem,particles);

      return 2;
    }
  if (key=="mt")
    return 1;
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
  std::vector<std::string> Items=
    StrFunc::StrParts(MLine);
  
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
  int typeFlag(0);
  // skip first item
  for(const std::string& sUnit : Items)
    {
      if (!typeFlag)
	{
	  if (!AZ.setZaid(sUnit))
	    ELog::EM<<"Failed to convert Zaid:"<<sUnit<<ELog::endErr;
	  typeFlag=1;
	}
      else
	{
	  if (!StrFunc::convert(sUnit,Dens))
	    ELog::EM<<"Failed to convert Density:"<<sUnit<<ELog::endErr;
	  AZ.setDensity(Dens);
	  zaidVec.push_back(AZ);
	  typeFlag=0;
	}
    }
  // PROCESS MT Line:
  std::vector<std::string> MTItems=StrFunc::StrParts(MTLine);
  for(const std::string& sUnit : MTItems)
    SQW.push_back(StrFunc::fullBlock(sUnit));
  
  // PROCESS LIBS
  std::vector<std::string> LibItems=StrFunc::StrParts(LibLine);
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
  ELog::RegMethod RegA("Material","setMaterial<Vec>");

  if (PVec.empty()) return -1;

  // Process First line : 
  // Divide line into zaid / density / extras:
  std::vector<std::string> Items=StrFunc::StrParts(PVec[0]);
  if (Items.size()<2) return -1;

  std::string& FItem(Items[0]);  
  const char pItem=FItem[0];
  FItem[0]=' ';
  if ((pItem!='m' && pItem!='M') || !StrFunc::convert(FItem,Mnum))
    {
      ELog::EM<<"Non material card at "<<pItem<<FItem<<ELog::endErr;
      return -2;
    }
  
  Zaid AZ;
  double Dens;                   // density

  // PROCESS : mXX zaid line
  int typeFlag(0);
  // skip first item
  Items.erase(Items.begin());
  for(const std::string& unit : Items)
    {
      if (!typeFlag)
	{
	  if (AZ.setZaid(unit))
	    typeFlag=1;
	  else              // Maybe a modifier [hopefully]
	    Libs.push_back(unit);
	}
      else
	{
	  if (StrFunc::convert(unit,Dens))
	    {
	      AZ.setDensity(Dens);
	      zaidVec.push_back(AZ);
	      typeFlag=0;
	    }
	}
    }
  // NOW PROCESS EXTRA LINES (and libs)
  std::vector<std::string>::const_iterator sc(PVec.begin());

  for(sc++;sc<PVec.end();sc++)
    {
      std::string sqwItem(*sc);
      std::string Particle;
      const int mType=getExtraType(sqwItem,Particle);
      switch (mType)
	{
	case 1:          // mt card
	  SQW.push_back(StrFunc::fullBlock(sqwItem));
	  break;
	case 2:          // mx card
	  throw ColErr::InContainerError<std::string>
	    (sqwItem,"Un supported mx card");
	}
    }
  Name="m"+std::to_string(Mnum);
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

  if (fabs(FSum)<1e-7)
    throw ColErr::NumericalAbort("Sum of zaidDensity: zero");
      
  if (D>0.0)
    {
      for(Zaid& ZC : zaidVec)
	ZC.setDensity(D*ZC.getDensity()/FSum);
      atomDensity=D;
    }
  else if (D<0.0)
    {
      double MAW(0.0);   // Mean atomic weight

      for(const Zaid& ZC : zaidVec)
	MAW+=ZC.getAtomicMass()*ZC.getDensity()/FSum;
      if (fabs(MAW)<0.5)
	throw ColErr::NumericalAbort("MAW too low: <0.5 ");
      const double aRho= -D*RefCon::avogadro/MAW;

      for(Zaid& ZC : zaidVec)
	ZC.setDensity(aRho*ZC.getDensity()/FSum);

      atomDensity=aRho;
    }
  return;
}

double
Material::getMacroDensity() const
  /*!
    Return the macroscopic density (g/cc)
   */
{
  double AW(0.0);
  double sumDens(0.0);
  for(const Zaid& ZC : zaidVec)
    {
      AW+=ZC.getAtomicMass()*ZC.getDensity();
      sumDens+=ZC.getDensity();
    }
  if (sumDens<1e-10) return 0.0;
  AW/=sumDens;
  return atomDensity*AW/RefCon::avogadro;
}


double
Material::getMeanA() const
  /*!
    Return the mean A value 
    \return mean atomic number
  */
{
  double AW(0.0);
  double sumDens(0.0);
  for(const Zaid& ZC : zaidVec)
    {
      AW+=ZC.getAtomicMass()*ZC.getDensity();
      sumDens+=ZC.getDensity();
    }
  if (sumDens<1e-10) return 0.0;
  AW/=sumDens;
  return AW;
}
  
void
Material::calcAtomicDensity() 
  /*!
    Calculate the atomic density
   */
{
  atomDensity=0.0;
  for(const Zaid& ZC : zaidVec)
    {
      if (ZC.getZ())
	atomDensity+=ZC.getDensity();
    }

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

  for(const Zaid& ZItem : zaidVec)
    std::cout<<ZItem<<std::endl;

  std::cout<<"Libs == ";
  for(const std::string& LItem : Libs)
    std::cout<<LItem<<"  ";
  std::cout<<std::endl;

  std::cout<<"S(q,w)== ";
  for(const std::string& SItem : Libs)
    std::cout<<SItem<<"  ";
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
  std::vector<size_t> cZaid;      // Cinder zaid list
  std::vector<double> cFrac;   // Fraction 

  // Construct zaid and fractions
  // normalize fractions
  double sum(0.0);
  for(const Zaid& ZItem : zaidVec)
    {
      Element::Instance().addZaid(ZItem,cZaid,cFrac);
      sum+= ZItem.getDensity();;
    }
  for(double& C : cFrac)
    C/=sum;
  
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
Material::changeLibrary(const size_t T,const char key)
  /*!
    Change the library type of all materials out
    of the 000 type.
    \param T :: New Tag
    \param key :: type-key
  */
{
  for(Zaid& ZC : zaidVec)
    if (ZC.getIso() && ZC.getKey()==key)
      ZC.setTag(T);

  return;
}

void 
Material::writeZaid(std::ostream& OX,const double F,const size_t ZD) 
  /*!
    Write the zaid file
    \param OX :: Output streamx
    \param F :: Fraction 
    \param ZD :: Zaid number
  */
{
  ELog::RegMethod RegA("Material","writeZaid");
  boost::format ZFMT("%3d%03d0  %10.7e");

  const size_t Z=ZD/1000;
  const size_t A=ZD % 1000;
  if (A)
    OX<<(ZFMT % A % Z % F)<<std::endl;
  return;
}

void 
Material::writePHITS(std::ostream& OX) const
  /*!
    Write out the information about the material
    in PHITS files format
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Material","writePHITS");
  

  const Element& EL(Element::Instance());

  std::ostringstream cx;
  OX<<"$ Material : "<<Name<<" rho="<<getMacroDensity() << " g/cc"<<std::endl;
  OX<<"mat["<<Mnum<<"]\n";

  
  cx.precision(10);
  std::vector<Zaid>::const_iterator zc;
  std::vector<std::string>::const_iterator vc;
  for(const Zaid& ZItem: zaidVec)
    {
      cx.str("");
      if (ZItem.getIso())
	{
	  cx<<"  "<<ZItem.getIso()<<EL.elmSym(ZItem.getZ())
	    <<"       "<<ZItem.getDensity();
	}
      else
	{
	  cx<<"    "<<EL.elmSym(ZItem.getZ())
	    <<"       "<<ZItem.getDensity();
	}
		  
      OX<<cx.str()<<std::endl;
    }
  
  if (!SQW.empty())
    {
      cx.str("");
      cx<<"mt"<<Mnum<<"    ";
      if (Mnum<10) cx<<" ";
      std::copy(SQW.begin(),SQW.end(),
		std::ostream_iterator<std::string>(cx," "));
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
} 

void 
Material::writeFLUKA(std::ostream& OX) const
  /*!
    Write out material definitions in the fixed FLUKA format
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Material","writeFLUKA");

  const std::string mat(" M"+std::to_string(Mnum));
  typedef std::map<std::string,MXcards> MXTYPE;
  
  std::ostringstream cx;
  cx<<"*\n* Material : "<<Name<<" rho="<<getMacroDensity()<<" g/cc";
  StrFunc::writeMCNPX(cx.str(),OX);
  cx.str("");

  cx<<"MATERIAL -  - "<<getMacroDensity()<<" -  -  - "<<mat<<std::endl;

  std::vector<Zaid>::const_iterator zc;
  std::vector<std::string>::const_iterator vc;
  cx<<"COMPOUND ";
  cx<<std::setprecision(5);
  size_t i(0);
  const size_t n(zaidVec.size());
  for(const Zaid& ZItem: zaidVec)
    {
      cx<<ZItem.getDensity()<<
	" "+ZItem.getFlukaName()+" ";
    i++;
    if (!(i%3))
      {
	cx<<mat;
	if (i!=n) cx<<" COMPOUND ";
      }
    }

  // Add additional empty WHAT cards and SDEF in the end
  // if the COMPOUND line is not complete
  if (i%3)
    {
      while (i++%3)
	  cx<< " - - ";
      cx<<mat;
    }

  StrFunc::writeFLUKA(cx.str(),OX);

  return;
} 

void 
Material::writePOVRay(std::ostream& OX) const
  /*!
    Write out the information about the material
    in the POV-Ray form
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Material","writePOVRay");

  masterWrite& MW=masterWrite::Instance();
  const int rgbScale(0xFFFFF);
  // RGB : 150 max
  const int rgb(Mnum*rgbScale/150);
  Geometry::Vec3D rgbCol(rgb/0xFFFF,(rgb % 0xFFFF)/0xFF,rgb % 0xFF);
  rgbCol.makeUnit();

  
  OX<<"#declare mat"<<MW.NameNoDot(Name)<<" = texture {"
    << " pigment{color rgb<"
    <<   MW.NumComma(rgbCol)
    <<"> } };"<<std::endl;
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
  cx<<"c\nc Material : "<<Name<<" rho="<<atomDensity<<"\n";
  cx<<"c           (rho="<<getMacroDensity()<<" g/cc)";
  StrFunc::writeMCNPXcomment(cx.str(),OX);
  cx.str("");
  
  cx.precision(10);
  cx<<"m"<<Mnum<<"     ";
  if (Mnum<10) cx<<" ";
  std::copy(zaidVec.begin(),zaidVec.end(),std::ostream_iterator<Zaid>(cx," "));

  for(const std::string& libItem : Libs)
    cx<<libItem<<"  ";
  StrFunc::writeMCNPX(cx.str(),OX);


  cx.str("");
  MXTYPE::const_iterator mc;
  for(mc=mxCards.begin();mc!=mxCards.end();mc++)
    {
      cx<<"mx"<<Mnum;
      mc->second.write(cx,zaidVec);
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  // avoid having to reset flags/precision in cx
  std::ostringstream rx;
  if (!SQW.empty())
    {
      rx.str("");
      rx<<"mt"<<Mnum<<"    ";
      if (Mnum<10) rx<<" ";
      std::copy(SQW.begin(),SQW.end(),
		std::ostream_iterator<std::string>(rx," "));
      StrFunc::writeMCNPX(rx.str(),OX);
    }
  return;
} 

}  // NAMESPACE MonteCarlo
