/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/varBlock.cxx
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
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "varBlock.h"

/// Effective template convertion to index
template<> int varBlock::getTypeNum<double>() { return 1; }
/// Effective template convertion to index
template<> int varBlock::getTypeNum<int>() { return 2; }
/// Effective template convertion to index
template<> int varBlock::getTypeNum<Geometry::Vec3D>() { return 3; }

/// Effective template convertion to name
template<> std::string varBlock::getTypeString<double>() { return "double"; }
/// Effective template convertion to name
template<> std::string varBlock::getTypeString<int>() { return "int"; }
/// Effective template convertion to name
template<> std::string varBlock::getTypeString<Geometry::Vec3D>() 
  { return "Vec3D"; }


std::string
varBlock::getIndexString(const int Key)
  /*!
    Get Type string
    \param Key :: key index
    \return name of the type
   */
{
  ELog::RegMethod RegA("varBlock","getTypeString");

  switch (Key)
    {
    case 1:
      return std::string("double");
    case 2:
      return std::string("int");
    case 3:
      return std::string("Vec3D");
    }
  throw ColErr::InContainerError<int>(Key,RegA.getFull());
}

varBlock::varBlock() :
  populated(0),tCnt(0)
  /*!
    Constructor BUT ALL variables are left unpopulated.
  */
{}

varBlock::varBlock(const size_t ND,const std::string* DItems,
		   const size_t NI,const std::string* IItems) :
  populated(0),tCnt(0)
  /*!
    Constructor BUT ALL variables are left unpopulated.
    \param ND :: Number of double values
    \param DItems :: Names of double items
    \param NI :: Number of int values
    \param IItems :: Names of integer
  */
{
  typedef std::map<std::string,size_t> MapTYPE;

  for(size_t i=0;i<ND;i++)
    {
      indexVec.push_back(VTYPE(1,dV.size()));
      dataMap.insert(MapTYPE::value_type(DItems[i],tCnt));
      dV.push_back(0.0);
      flag.push_back(0);
      tCnt++;
    }
  for(size_t i=0;i<NI;i++)
    {
      indexVec.push_back(VTYPE(2,iV.size()));
      dataMap.insert(MapTYPE::value_type(IItems[i],tCnt));
      iV.push_back(0);
      flag.push_back(0);
      tCnt++;
    }
}

varBlock::varBlock(const varBlock& A) : 
  populated(A.populated),tCnt(A.tCnt),dV(A.dV),
  iV(A.iV),vV(A.vV),dataMap(A.dataMap),
  indexVec(A.indexVec),flag(A.flag)
  /*!
    Copy constructor
    \param A :: varBlock to copy
  */
{}

varBlock&
varBlock::operator=(const varBlock& A)
  /*!
    Assignment operator
    \param A :: varBlock to copy
    \return *this
  */
{
  if (this!=&A)
    {
      populated=A.populated;
      tCnt=A.tCnt;
      dV=A.dV;
      iV=A.iV;
      vV=A.vV;
      dataMap=A.dataMap;
      indexVec=A.indexVec;
      flag=A.flag;
    }
  return *this;
}

/// Double accessor to item
template<> 
double& varBlock::getItem<double>(const size_t I) { return dV[I]; }

template<> 
int& varBlock::getItem<int>(const size_t I) 
  /*!
    Template for access to item
    \param I :: Index
    \return int item
  */
{ 
  return iV[I]; 
}

template<> 
Geometry::Vec3D& 
varBlock::getItem<Geometry::Vec3D>(const size_t I)
  /*!
    Template for access to item
    \param I :: Index
    \return Geometry::Vec3D item
  */
{ 
  return vV[I]; 
}


template<>
const double& 
varBlock::getItem<double>(const size_t I) const
  /*!
    Template for to return item
    \param I :: Index
    \return double item
  */
{
  return dV[I];
}

template<>
const int& 
varBlock::getItem<int>(const size_t I) const 
  /*!
    Template for to return item
    \param I :: Index
    \return int item
  */
{
  return iV[I];
}

template<>
const Geometry::Vec3D&
varBlock::getItem<Geometry::Vec3D>(const size_t I) const
  /*!
    Get a vec3D object
    \param I :: Index [not range checked]
    \return Vec3D Item
   */
{
  return vV[I];
}

template<typename T>
T
varBlock::getVar(const size_t Item) const
  /*!
    Given a value set the item
    \param Item :: Index value to variable
    \return value
  */
{
  ELog::RegMethod RegA("varBlock","getVar<T>(size_t)");

  if (Item>=tCnt)
    throw ColErr::IndexError<size_t>(Item,tCnt,RegA.getFull());
  const VTYPE& vItem(indexVec[Item]);
  
  // Check type:
  if (vItem.first!=getTypeNum<T>())
    throw ColErr::TypeMatch(getTypeString<T>(),
			    getIndexString(vItem.first),
			    RegA.getFull());
  // check set:
  if (!flag[Item])
    ELog::EM<<"Using unset variable "<<Item<<ELog::endErr;

  return getItem<T>(vItem.second);
}

template<typename T>
T
varBlock::getVar(const std::string& Item) const
  /*!
    Given a value set the item
    \param Item :: Index value to variable
    \return value
  */
{
  ELog::RegMethod RegA("varBlock","getVar<T>(string)");

  std::map<std::string,size_t>::const_iterator mc=
    dataMap.find(Item);
  if (mc==dataMap.end())
    throw ColErr::InContainerError<std::string>(Item,RegA.getFull());
  const VTYPE& vItem(indexVec[mc->second]);
  
  // Check type:
  if (vItem.first!=getTypeNum<T>())
    throw ColErr::TypeMatch(getTypeString<T>(),
			    getIndexString(vItem.first),
			    RegA.getFull());
  // check set:
  if (!flag[mc->second])
    ELog::EM<<"Using unset variable "<<mc->second
	    <<" from "<<Item<<ELog::endErr;

  return getItem<T>(vItem.second);
}

template<typename T>
void
varBlock::setVar(const std::string& Item,const T& Value) 
  /*!
    Set a given variable
    \param Item :: Index value to variable
    \param Value :: Value to set
  */
{
  ELog::RegMethod RegA("varBlock","setVar<T>(string,T)");

  std::map<std::string,size_t>::const_iterator mc=
    dataMap.find(Item);
  if (mc==dataMap.end())
    throw ColErr::InContainerError<std::string>(Item,RegA.getFull());
  const VTYPE& vItem(indexVec[mc->second]);
  
  // Check type:
  if (vItem.first!=getTypeNum<T>())
    throw ColErr::TypeMatch(getTypeString<T>(),
			    getIndexString(vItem.first),
			    "varItem");
  flag[mc->second]=1;
  getItem<T>(vItem.second)=Value;
  return;
}

template<>
void
varBlock::setVar(const std::string& Item,const varBlock& VB)
  /*!
    Given a value set the item
    \param Item :: Index value to variable
    \param VB :: VarBlock unit
  */
{
  ELog::RegMethod RegA("varBlock","setVar<varBlock>");

  std::map<std::string,size_t>::const_iterator amc=dataMap.find(Item);
  std::map<std::string,size_t>::const_iterator bmc=VB.dataMap.find(Item);
  if (amc==dataMap.end() || bmc==VB.dataMap.end())
    throw ColErr::InContainerError<std::string>(Item,RegA.getFull());
  
  const VTYPE& AIndex(indexVec[amc->second]);
  const VTYPE& BIndex(VB.indexVec[bmc->second]);

  if (AIndex.first != BIndex.first)
    throw ColErr::TypeMatch(getIndexString(AIndex.first),
			    getIndexString(BIndex.first),
			    RegA.getFull());

  flag[amc->second]=1;
  switch(AIndex.first)
    {
    case 1:
      dV[AIndex.second]=VB.dV[BIndex.second];
      return;
    case 2:
      iV[AIndex.second]=VB.iV[BIndex.second];
      return;
    case 3:
      vV[AIndex.second]=VB.vV[BIndex.second];
      return;
    }
  throw ColErr::InContainerError<int>(AIndex.first,RegA.getFull());
}

///\cond TEMPLATE 

template void varBlock::setVar(const std::string&,const double&);
template void varBlock::setVar(const std::string&,const int&);
template void varBlock::setVar(const std::string&,const Geometry::Vec3D&);

template double varBlock::getVar(const std::string&) const;
template int varBlock::getVar(const std::string&) const;

///\endcond TEMPLATE 

