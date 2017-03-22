/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/basicConstruct.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"


#include "TallySelector.h" 
#include "basicConstruct.h" 

namespace tallySystem
{

basicConstruct::basicConstruct() 
  /// Constructor
{}

basicConstruct::basicConstruct(const basicConstruct&) 
  /// Copy Constructor
{}

basicConstruct&
basicConstruct::operator=(const basicConstruct&) 
  /// Assignment operator
{
  return *this;
}

template<>
Geometry::Vec3D
basicConstruct::inputItem<Geometry::Vec3D>
(const mainSystem::inputParam& IParam,const size_t Index,
 const size_t INum,const std::string& ErrMessage) const
  /*!
    Takes a particular component out of inputParam from the tally 
    key name 
    \param IParam :: Main input parameters
    \param Index :: -T option
    \param INum :: Offset to start from
    \param ErrMessage to display
    \return Value
  */
{
  ELog::RegMethod RegA("basicConstruct","inputItem");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (INum+3>NItems)
    throw ColErr::IndexError<size_t>(NItems,INum,
				     "Insufficient items for tally");

  Geometry::Vec3D Out;
  double V(0.0);
  for(size_t i=0;i<3;i++)
    {
      const std::string& OutItem=
	IParam.getValue<std::string>("tally",Index,INum+i);
      if (!StrFunc::convert(OutItem,V))
	ELog::EM<<ErrMessage<<ELog::endErr;
      Out[i]=V;
    }
  return Out;
}

template<typename T>
T 
basicConstruct::inputItem(const mainSystem::inputParam& IParam,
			  const size_t Index,const size_t INum,
			  const std::string& ErrMessage) const
  /*!
    Takes a particular component out of inputParam from the tally 
    key name 
    \param IParam :: Main input parameters
    \param Index :: -T option
    \param INum :: Offset to start from
    \param ErrMessage to display
    \return Value
  */
{
  ELog::RegMethod RegA("basicConstruct","inputItem");

  const size_t NItems=IParam.itemCnt("tally",Index);

  if (INum>=NItems)
    throw ColErr::IndexError<size_t>(NItems,INum+1,
				     "Insufficient items for tally: "
				     +ErrMessage);

  T Out;
  const std::string& OutItem=
    IParam.getValue<std::string>("tally",Index,INum);
  if (!StrFunc::convert(OutItem,Out))
    ELog::EM<<"Convert error:"<<ErrMessage<<ELog::endErr;

  return Out;
}

template<typename T>
int 
basicConstruct::checkItem(const mainSystem::inputParam& IParam,
			  const size_t Index,const size_t INum,
			  T& Out) const
/*!
    Takes a particular component out of inputParam from the tally
    key name
    \param IParam :: Main input parameters
    \param Index :: -T flag option
    \param INum :: Item number
    \param Out :: Output value
    \return 0 if failed and 1 if good
*/
{
  ELog::RegMethod RegA("basicConstruct","checkItem");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (INum>=NItems)
    return 0;

  const std::string& OutItem=
    IParam.getValue<std::string>("tally",Index,INum);

  return StrFunc::convert(OutItem,Out);
}

long int
basicConstruct::getLinkIndex(const mainSystem::inputParam& IParam,
			     const size_t Index,const size_t Offset) const
  /*!
    Determine a link point based on the object:
    \param IParam :: input parameters
    \param Index :: Particular tally 
    \param Offset :: position offset [typcially 2]
    \return +ve number of normal link point and -ve 
    for beam link pt : 0 on failure
   */
{
  ELog::RegMethod RegA("tallyConstructor","getLinkIndex");

  std::string Snd;
  if (!checkItem<std::string>(IParam,Index,Offset,Snd))
    {
      ELog::EM<<"Tally must give a direction/LinkPt"<<ELog::endErr;
      return 0;
    }
  return attachSystem::getLinkIndex(Snd);
}


int
basicConstruct::convertRange(const std::string& Word,int& RA,int &RB)
  /*!
    Convert a pair range into two numbers:
    Range given as X - Y
    \param Word :: Unit to convert
    \param RA :: First nubmer
    \param RB :: Second number
    \return true/false
  */
{
  ELog::RegMethod RegA("basicConstruct","convertRange");

  int A,B;
  size_t ALen=StrFunc::convPartNum(Word,A);
  if (!ALen) return 0;
  for(;ALen<Word.size() && Word[ALen]!='-';ALen++) ;
  if (ALen==Word.size()) return 0;
  
  if (!StrFunc::convert(Word.substr(ALen),B))
    return 0;
  RA=A;
  RB=B;
  return 1;
}

int
basicConstruct::convertRegion(const mainSystem::inputParam& IParam,
			      const std::string& keyName,
			      const size_t Index,const size_t Offset, 
			      int& RA,int& RB) const
  /*!
    Convert the input parameter into either a region or a number 
    \param IParam :: Input stream
    \param keyName :: input item key
    \param Index :: Number of NameCard
    \param Offset :: Offset on card
    \param RA :: Region 1
    \param RB :: Region 2
    \retval 1 :: conversion is good 
    \retval 0 :: Failed
    \retval -1 :: convertion good but requires renumber
  */
{
  ELog::RegMethod RegA("basicConstruct","convertRegion");

  const ModelSupport::objectRegister& OR= 
    ModelSupport::objectRegister::Instance();

  const size_t NItems=IParam.itemCnt(keyName,Index);
  if (Offset > NItems)
    throw ColErr::IndexError<size_t>
      (NItems,Offset,"Insufficient items for key:"+
       keyName+"["+StrFunc::makeString(Index)+"]");

  RA=0;
  int outFlag(1);
  // First determine if a region name is used:
  const std::string region
    (IParam.getValue<std::string>(keyName,Index,Offset));
  RA=OR.getCell(region);
  if (RA) 
    {
      RB=OR.getLast(region);
      return outFlag;
    }

  if (Offset+1 >= NItems)
    {
      ELog::EM<<"Region a == "<<RA<<" [not a named region]"<<ELog::endCrit;
      throw ColErr::IndexError<size_t>
	(NItems,Offset,"Items for offset+1 key:"+
	 keyName+"["+StrFunc::makeString(Index)+"]");
    }

  const std::string regionB
    (IParam.getValue<std::string>(keyName,Index,Offset+1));

  if (!region.empty() && tolower(region[0])=='r' && 
      !regionB.empty() && tolower(regionB[0]=='r') &&
      StrFunc::convert(region.substr(1),RA) && 
      StrFunc::convert(regionB.substr(1),RB) )
    {
      outFlag=-1;
    }
  else if (!StrFunc::convert(region,RA) || 
	   !StrFunc::convert(regionB,RB))
    {
      RB=RA-1;
    }
  // Process:
  if (!RA || !RB || RA>RB)
    {
      ELog::EM<<"RA == "<<region.substr(1)<<ELog::endTrace;
      ELog::EM<<"Failed to convert for tally group "
	      <<Index/3+1<<ELog::endCrit;
      ELog::EM<<"Region (beg) == "<<region<<"\n";
      ELog::EM<<"Region (end) == "<<regionB
	      <<ELog::endErr;
    }
  return outFlag;
}


  
std::vector<int>
basicConstruct::getCellSelection(const Simulation& System,
                                 const int matN,
                                 const std::string& keyName) const 

  /*!
    Extract all the cells with a material based on matN and keyName
    \param System :: Simulation for build [needed for nonVoidcells ] 
    \param matN :: Material number
           -1 : all materials 
	   -2 : all non zero materials
	   > 1000 : materials containing zaid
    \param keyName :: keyName
    \return vector of cell indexes
   */
{
  ELog::RegMethod RegA("basicConstruct","getCellSelection");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::vector<int> cells;
  // NOTE that getting all the cells from OR is insane
  if (keyName=="allNonVoid" || keyName=="AllNonVoid")
    cells=System.getNonVoidCellVector();
  else if (keyName=="All" || keyName=="all")
    cells=System.getCellVector();
  else
    cells=OR.getObjectRange(keyName);

  // PROCESS mat:
  std::vector<int> Out;
  std::vector<int> matCell;
  
  if (matN>1000)
    matCell=System.getCellWithZaid(matN);
  else  
    matCell=System.getCellWithMaterial(matN);

  if (!cells.size())
    throw ColErr::InContainerError<std::string>
      (keyName,"cell emepty for mat:"+StrFunc::makeString(matN));

  std::set_intersection(cells.begin(),cells.end(),
                        matCell.begin(),matCell.end(),
                        std::back_inserter(Out));
  return Out;
}


///\cond TEMPLATE INSTANCES:
template int 
basicConstruct::inputItem(const mainSystem::inputParam&,
      const size_t,const size_t,const std::string&) const;

template long int 
basicConstruct::inputItem(const mainSystem::inputParam&,
      const size_t,const size_t,const std::string&) const;

template size_t 
basicConstruct::inputItem(const mainSystem::inputParam&,
      const size_t,const size_t,const std::string&) const;

template double
basicConstruct::inputItem(const mainSystem::inputParam&,
      const size_t,const size_t,const std::string&) const;

template std::string
basicConstruct::inputItem(const mainSystem::inputParam&,
      const size_t,const size_t,const std::string&) const;

template int
basicConstruct::checkItem(const mainSystem::inputParam&,
      const size_t,const size_t,std::string&) const;

template int
basicConstruct::checkItem(const mainSystem::inputParam&,
      const size_t,const size_t,double&) const;

template int
basicConstruct::checkItem(const mainSystem::inputParam&,
      const size_t,const size_t,int&) const;

template int
basicConstruct::checkItem(const mainSystem::inputParam&,
      const size_t,const size_t,size_t&) const;

///\endcond TEMPLATE INSTANCES
  
}  // NAMESPACE tallySystem
