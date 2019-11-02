/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/DBMaterial.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"

namespace ModelSupport
{

DBMaterial::DBMaterial() 
  /*!
    Constructor
  */
{
  initMaterial();
  initMXUnits();
}

DBMaterial&
DBMaterial::Instance() 
  /*!
    Singleton constructor
    \return DBMaterial object
  */
{
  static DBMaterial DObj;
  return DObj;
}

void
DBMaterial::checkNameIndex(const int MIndex,const std::string& MName) const
  /*!
    Check the name and index is not already in the system
    throw on error
    \param MIndex :: Index Number
    \param MName :: Material name
  */
{
  if ( MStore.find(MIndex)!=MStore.end() )
    throw ColErr::InContainerError<int>(MIndex,"MIndex");
  if ( IndexMap.find(MName)!=IndexMap.end() )
    throw ColErr::InContainerError<std::string>(MName,"MName");
  return;
}

int
DBMaterial::getFreeNumber() const
  /*!
    Get the first available free number
    \return unallocated material number
   */
{
  ELog::RegMethod RegA("DBMaterial","getFreeNumber");
  int FNum(static_cast<int>(MStore.size()));  
  FNum++;               // Avoid possible zero
  while(MStore.find(FNum)!=MStore.end())
    {
      FNum++;
    }
  return FNum;
  
}

void
DBMaterial::readFile(const std::string& FName)
  /*!
    Read a basic MCNP material file
    \param FName :: Filename
   */
{
  ELog::RegMethod RegItem("DBMaterial","readMaterial");


  if (FName.empty()) return;

  std::vector<std::string> MatLines;
  std::ifstream IX;
  IX.open(FName.c_str());
  if (!IX.good())
    throw ColErr::FileError(0,FName,"File could not be opened");
  
  int currentMat(0);          // Current material
  std::string Line = StrFunc::getLine(IX);           
  while(IX.good() && currentMat != -100)
    {
      Line = StrFunc::getLine(IX);           
      const int lineType = MonteCarlo::Material::lineType(Line);
      if (lineType==0 && !MatLines.empty())             // Continue line
	MatLines.back()+=" "+Line;
      else if (lineType>0 && lineType==currentMat)      // mt/mx etc line
	MatLines.push_back(Line);
      else if (lineType>0 || lineType==-100 || !IX.good())    // newMat / END
	{
	  if (currentMat)
	    {
	      MonteCarlo::Material MObj;
	      if (!MObj.setMaterial(MatLines))     
		setMaterial(MObj);
	      MatLines.clear();
	    }
	  currentMat=lineType;
	  MatLines.push_back(Line);
	}
    }
  return;
} 

void
DBMaterial::cloneMaterial(const std::string& oldName,
			  const std::string& extraName)
  /*!
    Clone a name for use
    \param oldName :: Existing material
    \param extraName :: new name
  */
{
  ELog::RegMethod RegA("DBMaterial","cloneMaterial");

  SCTYPE::const_iterator mc=IndexMap.find(oldName);
  if (mc==IndexMap.end())
    throw ColErr::InContainerError<std::string>
      (oldName,"OldMaterial not available in database : ");

  SCTYPE::const_iterator nx=IndexMap.find(extraName);
  if (nx!=IndexMap.end())
    throw ColErr::InContainerError<std::string>
      (extraName,"New Material already present");
  
  IndexMap.emplace(extraName,mc->second);
  return;
}

void
DBMaterial::overwriteMaterial(const std::string& original,
			      const std::string& newMaterial)
  /*!
    Clone a name for use
    \param original :: Existing material to change
    \param newMaterial :: new material to use instead
  */
{
  ELog::RegMethod RegA("DBMaterial","overwriteMaterial");

  SCTYPE::const_iterator mc=IndexMap.find(original);
  if (mc==IndexMap.end())
    throw ColErr::InContainerError<std::string>
      (original,"Original material not available");

  SCTYPE::const_iterator nx=IndexMap.find(newMaterial);
  if (nx==IndexMap.end())
    throw ColErr::InContainerError<std::string>
      (original,"New material not available");

  
  
  IndexMap.emplace(original,mc->second);
  return;
}
  
void
DBMaterial::setMaterial(const MonteCarlo::Material& MO)
  /*!
    Store are material in the data base
    \param MO :: Material object
   */
{
  ELog::RegMethod RegA("DBMaterial","setMaterial");

  const std::string& MName=MO.getName();
  const int MIndex=MO.getNumber();
  checkNameIndex(MIndex,MName);
  MStore.insert(MTYPE::value_type(MIndex,MO));
  IndexMap.insert(SCTYPE::value_type(MName,MIndex));
  return;
}
  
bool
DBMaterial::createMaterial(const std::string& MName)
  /*!
    Creates a material based on the name fraction
    If # then new density
    \param MName :: Material name based on fraction load
    \return 0 if not possible / 1 on material existing or created
  */
{
  ELog::RegMethod RegA("DBMaterial","createMaterial");
  if (hasKey(MName)) return 1;

  
  // Now key found
  // can be a new density or a new mix:
  std::string::size_type pos=MName.find('#');
  double PFrac;
  if (pos!=std::string::npos)
    {
      const std::string AKey=MName.substr(0,pos);
      const std::string BKey=MName.substr(pos+1);
      if (StrFunc::convert(BKey,PFrac))
	{
	  createNewDensity(MName,AKey,PFrac);
	  return 1;
	}
    }

  pos=MName.find('%');
  if (pos!=std::string::npos)
    {

      const std::string AKey=MName.substr(0,pos);
      const std::string BKey=MName.substr(pos+1);
      if (StrFunc::convert(BKey,PFrac))
	{
	  if (AKey=="ParaOrtho")
	    {
	      createOrthoParaMix(MName,PFrac/100.0);
	      return 1;
	    }
	  if (AKey=="UBurn")
	    {
	      createMix(MName,"U3Si2","U3Si2Burnt",PFrac/100.0);
	      return 1;
	    }
	  if (AKey=="ULowBurn")
	    {
	      createMix(MName,"U3Si2Low","U3Si2LowBurnt",PFrac/100.0);
	      return 1;
	    }
	}
      // SECOND %
      // Example : D2O%H2O%80.0   D2O at 80% atomic fraction
      pos=BKey.find('%');
	      
      if (pos!=std::string::npos)
	{
	  const std::string XKey=BKey.substr(0,pos);
	  const std::string YKey=BKey.substr(pos+1);
	  if (StrFunc::convert(YKey,PFrac))
	    {
	      createMix(MName,AKey,XKey,PFrac/100.0);
	      return 1;
	    }
	}
    }
	
  return 0;
}


int
DBMaterial::createOrthoParaMix(const std::string& Name,
			       const double PFrac)
  /*!
    Creates an ortho/Para Mixture
    \param Name :: Name of object
    \param PFrac :: fraction of Para
    \return current number
   */
{
  ELog::RegMethod RegA("DBMaterial","createOrthoParaMix");

  const int matNum(getFreeNumber());
  const double H2density(0.041975);
  if (PFrac<-1e-5 || PFrac>1.00001)
    throw ColErr::RangeError<double>(PFrac,0.0,1.0,"Para fraction");

  MonteCarlo::Material MObj;
  std::string Unit;
  std::string SQW;
  const std::string MLib="hlib=.70h pnlib=70u";
  if (PFrac>1e-5)
    {
      Unit+=" 1001.70c "+StrFunc::makeString(H2density*PFrac);
      SQW+="parah.20t ";
    }
  if (1.0-PFrac>1e-5)
    {
      Unit+=" 1004.70c "+StrFunc::makeString(H2density*(1.0-PFrac));
      SQW+="orthh.99t ";
    }
  
  MObj.setMaterial(matNum,Name,Unit,SQW,MLib);
  setMaterial(MObj);
  return matNum;
}

int
DBMaterial::createMix(const std::string& Name,
		      const std::string& MatA,
		      const std::string& MatB,
		      const double PFrac)
  /*!
    Creates an ortho/Para Mixture
    \param Name :: Name of object
    \param MatA :: Material A [PFrac]
    \param MatB :: Material B [1-PFrac]
    \param PFrac :: fraction of MatA
    \return current number
   */
{
  ELog::RegMethod RegA("DBMaterial","createMix");

  const int matNum(getFreeNumber());

  MonteCarlo::Material MA=getMaterial(MatA);
  MonteCarlo::Material MB=getMaterial(MatB);
  if (PFrac<-1e-5 || PFrac>1.00001)
    throw ColErr::RangeError<double>(PFrac,0.0,1.0,"Mix fraction [PFrac]");

  MA*=PFrac;
  MB*=(1.0-PFrac);
  MA+=MB;
  MA.setNumber(matNum);
  MA.setName(Name);

  setMaterial(MA);
  return matNum;
}

int
DBMaterial::createNewDensity(const std::string& Name,
			     const std::string& MatA,
			     const double densityFrac)
  /*!
    Creates an new material based on density
    \param Name :: Name of object
    \param MatA :: Material 
    \param densityFrac :: scale of denisty  / -ve for absolte
    
    \return current number
   */
{
  ELog::RegMethod RegA("DBMaterial","createNewDensity");

  const int matNum(getFreeNumber());

  // special case for void 
  if (std::abs(densityFrac)<1e-5)
    {
      MonteCarlo::Material MA=getMaterial("Void");
      MA.setNumber(matNum);
      MA.setName(Name);
      return matNum;
    }
  
  MonteCarlo::Material MA=getMaterial(MatA);
  MA.setNumber(matNum);
  MA.setName(Name);

	  
  if (densityFrac>0.0)
    MA.setDensity(MA.getAtomDensity()*densityFrac);
  else if (densityFrac> -0.3)          // atom fraction
    MA.setDensity(-densityFrac);
  else                                 // read density
    MA.setDensity(densityFrac);


  setMaterial(MA);
  return matNum;
}


void
DBMaterial::removeAllThermal()
  /*!
    Removes the thermal treatment from all materials
   */
{
  ELog::RegMethod RegA("DBMaterial","removeAllThermal");

  // we don't change the index card:
  for(MTYPE::value_type& mc : MStore)
    mc.second.removeSQW();
  return;
}

void
DBMaterial::removeThermal(const std::string& matName)
  /*!
    Removes the thermal treatment from a material
    \param matName :: Material name
   */
{
  ELog::RegMethod RegA("DBMaterial","removeThermal");


  // we don't change the index card:
  SCTYPE::const_iterator mIc=IndexMap.find(matName);
  if (mIc==IndexMap.end())
    throw ColErr::InContainerError<std::string>
      (matName,"No material available");

  MTYPE::iterator mc=MStore.find(mIc->second);
  mc->second.removeSQW();
  return;
}

void
DBMaterial::resetMaterial(const MonteCarlo::Material& MO)
  /*!
    Store are material in the data base
    Allows the material to already exist
    \param MO :: Material object
   */
{
  ELog::RegMethod RegA("DBMaterial","resetMaterial");

  const std::string& MName=MO.getName();
  const int MIndex=MO.getNumber();

  MTYPE::iterator mc=MStore.find(MIndex);
  if (mc!=MStore.end()) MStore.erase(mc);
  SCTYPE::iterator sc=IndexMap.find(MName);
  if (sc!=IndexMap.end()) IndexMap.erase(sc);

  MStore.insert(MTYPE::value_type(MIndex,MO));
  IndexMap.insert(SCTYPE::value_type(MName,MIndex));
  return;
}


void
DBMaterial::initMXUnits()
  /*!
    Initialize the MX options
  */
{
  ELog::RegMethod RegA("DBMaterial","initMXUnits");

  typedef std::tuple<size_t,size_t,char,std::string,
		     std::string> MXTYPE;
  std::vector<MXTYPE> mxVec=
    {
      MXTYPE(6000,70,'c',"h","6012.70h"),
      MXTYPE(4009,24,'c',"h","model"),
      MXTYPE(4009,70,'c',"h","model"),
      
      MXTYPE(4009,80,'c',"h","4009.80h"),
      MXTYPE(4010,80,'c',"h","4010.80h"),
      
      MXTYPE(78190,80,'c',"h","78190.80h"),
      MXTYPE(78192,80,'c',"h","78192.80h"),
      MXTYPE(78194,80,'c',"h","78194.80h"),
      MXTYPE(78195,80,'c',"h","78195.80h"),
      MXTYPE(78196,80,'c',"h","78196.80h"),
      MXTYPE(78198,80,'c',"h","78198.80h")
    };


  // NOTE : u is an illegal particle so how does MX work here??
  //  mxVec.push_back(MXTYPE(6000,70,'c',"u","6012.70u"));
  
  for(const MXTYPE& vc : mxVec)
    {
      MTYPE::iterator mc;  
      for(mc=MStore.begin();mc!=MStore.end();mc++)
	mc->second.setMXitem(std::get<0>(vc),std::get<1>(vc),
			     std::get<2>(vc),std::get<3>(vc),
			     std::get<4>(vc));
    }
  return;
}

const MonteCarlo::Material&
DBMaterial::getMaterial(const int MIndex) const
  /*!
    Get the material based on the index value
    \param MIndex :: Material 
    \return Material
   */
{
  ELog::RegMethod RegA("DBMaterial","getMaterial<int>");

  MTYPE::const_iterator mc=MStore.find(MIndex);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(MIndex,"MIndex in MStore");
  return mc->second;
}

const MonteCarlo::Material&
DBMaterial::getMaterial(const std::string& MName) const
  /*!
    Get the material based on the name value
    \param MName :: Material name
    \return Material
   */
{
  ELog::RegMethod RegA("DBMaterial","getMaterial<string>");

  SCTYPE::const_iterator mc=IndexMap.find(MName);
  if (mc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(MName,"IndexMap");
  return getMaterial(mc->second);
}

bool
DBMaterial::hasKey(const std::string& Key) const
  /*!
    Determine if the string exists in the data base
    \param Key :: KeyName
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","hasKey<string>");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  return (sc==IndexMap.end()) ? 0 : 1;
}

bool
DBMaterial::hasKey(const int KeyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return 1 / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","hasKey<int>");
  
  MTYPE::const_iterator mc=MStore.find(KeyNum);
  return (mc==MStore.end()) ? 0 : 1;
}

const std::string&
DBMaterial::getKey(const int KeyNum) const
  /*!
    Determine if the index exists in the data base
    \param KeyNum :: key number
    \return matNumber / 0 on fail
  */
{
  ELog::RegMethod RegA("DBMaterial","getKey");
  
  MTYPE::const_iterator mc=MStore.find(KeyNum);
  if (mc==MStore.end())
    throw ColErr::InContainerError<int>(KeyNum,"KeyNum");
  
  const std::string& OutStr=mc->second.getName();
  SCTYPE::const_iterator sc=IndexMap.find(OutStr);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(OutStr,"IndexMap");
  if (sc->second!=KeyNum)
    throw ColErr::MisMatch<int>(KeyNum,sc->second,"KeyNum/IndexMap");
  
  return OutStr;
}

int
DBMaterial::processMaterial(const std::string& matKey)
  /*!
    Produce or process the string 
    \param matKey :: String / Index name
    \return material index
  */
{
  ELog::RegMethod RegA("DBMaterial","processMaterial");


  if (createMaterial(matKey))
    return getIndex(matKey);
  
  // Note: EvalVar converts any string into a integer [best guess]
  int out;
  if (!StrFunc::convert(matKey,out))
    throw ColErr::InContainerError<std::string>(matKey,"Material not present");

  if(!hasKey(out))
    throw ColErr::InContainerError<int>(out,"Material not present");
  
  return out;
}
  
int
DBMaterial::getIndex(const std::string& Key) const
  /*!
    Determine if the index exists in the data base
    \param Key :: key name
    \return matNumber 
  */
{
  ELog::RegMethod RegA("DBMaterial","getIndex");
  
  SCTYPE::const_iterator sc=IndexMap.find(Key);
  if (sc==IndexMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key");

  return sc->second;
}


void
DBMaterial::setActive(const int M)
  /*!
    Set a item in the active list
    \param M :: Material number
   */
{
  active.insert(M);
  return;
}

void
DBMaterial::resetActive()
  /*!
    Reset the active list
  */
{
  active.erase(active.begin(),active.end());
  return;
}

bool
DBMaterial::isActive(const int matN) const
  /*!
    Determine if material is active or not
    \param matN :: Material number
    \return true is active is set.
   */
{
  return (active.find(matN)!=active.end()) ? 1 : 0;
}

void
DBMaterial::setENDF7()
  /*!
    Convert to ENDF7 format [if not .24c type]
  */
{
  ELog::RegMethod RegA("DBMaterial","setENDF7");

  MTYPE::iterator mc;
  for(mc=MStore.begin();mc!=MStore.end();mc++)
    mc->second.setENDF7();
  return;
}

void
DBMaterial::writeCinder(std::ostream& OX) const
  /*!
    Write out a material stream for Cinder
    -- Note the format is format(a6,i4,e10.0):
    mat N::index:Number density 
    There is not space between the number and the number density
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writeCinder");

  std::set<int>::const_iterator sc;
  for(sc=active.begin();sc!=active.end();sc++)
    {
      if (*sc)
	{
	  MTYPE::const_iterator mx=MStore.find(*sc);
	  if (mx==MStore.end())
	    throw ColErr::InContainerError<int>(*sc,"MStore");	  
	  mx->second.writeCinder(OX);
	}
    }
  return;
}

void
DBMaterial::deactivateParticle(const std::string& P)
  /*!
    Deactivate information for particular particle 
    - currently only h (proton)
    \param P :: Particle to deactivate
  */
{
  ELog::RegMethod RegA("DBMaterial","deactivateParticle");

  // cut hydrogen
  if (P=="h")
    {
      for(MTYPE::value_type& MT : MStore)
        {
          MT.second.removeMX(P);
          MT.second.removeLib(P);
        }
    }

  return;
}

  
void
DBMaterial::writeMCNPX(std::ostream& OX) const
  /*!
    Write everything out to the stream
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writeMCNPX");

  for(const int sActive : active)
    {
      if (sActive)
	{
	  MTYPE::const_iterator mp=MStore.find(sActive);
	  if (mp==MStore.end())
	    throw ColErr::InContainerError<int>(sActive,"MStore find(active item)");
	  mp->second.write(OX);
	}
    }
  return;
}

void
DBMaterial::writePHITS(std::ostream& OX) const
  /*!
    Write everything out to the stream
    for the phits output
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writePHITS");

  for(const int sActive : active)
    {
      if (sActive)
	{
	  MTYPE::const_iterator mp=MStore.find(sActive);
	  if (mp==MStore.end())
	    throw ColErr::InContainerError<int>
              (sActive,"MStore find(active item)");
          
	  mp->second.writePHITS(OX);
	}
    }
  return;
}

void
DBMaterial::writeFLUKA(std::ostream& OX) const
  /*!
    Write everything out to the fluka system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writeFLUKA");

  for(const int sActive : active)
    {
      if (sActive)
	{
	  MTYPE::const_iterator mp=MStore.find(sActive);
	  if (mp==MStore.end())
	    throw ColErr::InContainerError<int>(sActive,"MStore find(active item)");
	  
	  mp->second.writeFLUKA(OX);
	}
    }
  return;
}

void
DBMaterial::writePOVRay(std::ostream& OX) const
  /*!
    Write materials out to the POV-Ray system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("DBMaterial","writePOVRay");

  for(const int sActive : active)
    {
      if (sActive)
	{
	  MTYPE::const_iterator mp=MStore.find(sActive);
	  if (mp==MStore.end())
	    throw ColErr::InContainerError<int>
	      (sActive,"MStore find(active item)");
	  
	  mp->second.writePOVRay(OX);
	}
    }
  return;
}

} // NAMESPACE ModelSupport
