/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/varList.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"

varList::varList() :
  varNum(0)
  /*!
    Default constructor
  */
{}

varList::varList(const varList& A) :
  varNum(A.varNum)
  /*!
    Standard Copy constructor.
    Makes a memory copy of the FItem*
    \param A :: varList to copy
  */
{
  std::map<std::string,FItem*>::const_iterator vc;
  for(vc=A.varName.begin();vc!=A.varName.end();vc++)
    {
      FItem* Ptr=vc->second->clone();
      varName.insert(std::pair<std::string,FItem*>(vc->first,Ptr));
      varItem.insert(std::pair<int,FItem*>(Ptr->getIndex(),Ptr));
    }
  return;
}

varList&
varList::operator=(const varList& A) 
  /*!
    Standard Copy constructor.
    Makes a memory copy of the FItem*
    \param A :: varList to copy
    \return *this
  */
{
  if (this!=&A)
    {
      varNum=A.varNum;
      deleteMem();
      std::map<std::string,FItem*>::const_iterator vc;
      for(vc=A.varName.begin();vc!=A.varName.end();vc++)
        {
	  FItem* Ptr=vc->second->clone();
	  varName.insert(std::pair<std::string,FItem*>(vc->first,Ptr));
	  varItem.insert(std::pair<int,FItem*>(Ptr->getIndex(),Ptr));
	}
    }
  return *this;
}

varList::~varList() 
  /*!
    Deletion operator removes
    points memory for FItems
  */
{
  deleteMem();
}

void
varList::resetActive()
  /*!
    Reset the active unit
  */
{
  for(varStore::value_type& VC : varName)
    VC.second->resetActive();
      
}


void
varList::deleteMem() 
  /*!
    Erase and clear the list of variables
  */
{
  std::map<int,FItem*>::iterator vc;
  for(vc=varItem.begin();vc!=varItem.end();vc++)
    delete vc->second;
  varItem.erase(varItem.begin(),varItem.end());
  varName.erase(varName.begin(),varName.end());
  return;
}

const FItem*
varList::findVar(const std::string& Key) const  
  /*!
    Returns a pointer to the FItem* instance.
    \param Key :: Name of variable
    \retval 0 :: If no such function name exists,
    \retval FItem pointer
  */
{
  std::map<std::string,FItem*>::const_iterator vc;
  vc=varName.find(Key);
  if (vc!=varName.end())
    return vc->second;

  return 0;
}

const FItem*
varList::findVar(const int Key) const  
  /*!
    Returns a pointer to the FItem* which Key is
    the same as the one given by 'F'. 
    \param Key :: opcode index to use for the search
    \retval null :: If no such function key exists,
    \retval FuncDefinition if item exists
  */
{
  std::map<int,FItem*>::const_iterator vc;
  vc=varItem.find(Key);
  if (vc!=varItem.end())
    return vc->second;
  return 0;
}

FItem*
varList::findVar(const int Key)  
  /*!
    Returns a pointer to the FITem* which Key is
    the same as the one given by 'F'. 
    \param Key :: opcode index to use for the search
    \retval null :: If no such function key exists,
    \retval FuncDefinition if item exists
  */
{
  std::map<int,FItem*>::iterator vc;
  vc=varItem.find(Key);
  if (vc!=varItem.end())
    return vc->second;
  return 0;
}

FItem* 
varList::findVar(const std::string& Key) 
  /*!
    Returns a pointer to the FItem* instance.
    \param Key :: Name of variable
    \retval 0 :: If no such function name exists,
    \retval FItem pointer
  */
{
  std::map<std::string,FItem*>::iterator vc;
  vc=varName.find(Key);
  if (vc!=varName.end())
    return vc->second;

  return 0;
}


void
varList::copyVar(const std::string& newKey,const std::string& oldKey) 
  /*!
    Copy a variable into the var system [with new number]
    \param newKey :: new variable name
    \param oldKey :: existing variable to copy [must exist]
  */
{ 
  ELog::RegMethod RegA("varList","copyVar");

  if (newKey==oldKey) return;

  std::map<std::string,FItem*>::iterator ac;
  std::map<std::string,FItem*>::const_iterator bc;

  bc=varName.find(oldKey);
  if (bc==varName.end())
    throw ColErr::InContainerError<std::string>(oldKey,"Var item not found");

  ac=varName.find(newKey);
  if (ac!=varName.end())
    {
      const int I=ac->second->getIndex();
      delete ac->second;

      std::map<int,FItem*>::iterator ic;
      ic=varItem.find(I);
      varName.erase(ac);
      varItem.erase(ic);
    }
  FItem* Ptr=bc->second->clone();
  Ptr->setIndex(varNum);
  varNum++;
    // Now insert into master lists
  varName.insert(std::pair<std::string,FItem*>(newKey,Ptr));
  varItem.insert(std::pair<int,FItem*>(Ptr->getIndex(),Ptr));

  return;
}

void
varList::copyVarSet(const std::string& oldHead,const std::string& newHead) 
  /*!
    Copy a set of variables all with "oldHead" and 
    changed to "newSet"system [with new number]
    \param oldHead :: Old head name
    \param newHead :: replacement head name
  */
{
  ELog::RegMethod RegA("varList","copyVarSet");

  if (oldHead==newHead) return;

  // NOTE: map is ORDERED
  // Thus we only need to find ONE value and then iterate through


  std::map<std::string,FItem*>::const_iterator oldMC;
  const size_t subLen=oldHead.length();
  
  oldMC=varName.lower_bound(oldHead);
  // Need to use a replacement set because each replacement
  // invalidates the iterator position:
  std::map<std::string,std::string> replaceSet;
  
  while(oldMC!=varName.end() &&
        (!subLen || oldMC->first.substr(0,subLen)==oldHead))
    {
      // Find new key
      const std::string newKey=(subLen) ?
        newHead+oldMC->first.substr(subLen) : newHead+oldMC->first;
      replaceSet.emplace(oldMC->first,newKey);
      
      oldMC++;
    }
  if (replaceSet.empty())
    throw ColErr::InContainerError<std::string>
      (oldHead,"Key part not in variable map");

  for(const std::pair<std::string,std::string>& Item : replaceSet)
    {
      copyVar(Item.second,Item.first);
    }
  
  return;
}

int 
varList::selectValue(const int Key,
		     Geometry::Vec3D& oVec,
		     double& oDbl) const
  /*!
    Simple selector
    \param Key :: Variable name
    \param oVec :: output vector
    \param oDbl :: output value [ selected]
    \retval Output Type used : 0 :: double
    \retval Output Type used : 1 :: Vector 
    \retval Output Type used : -1 :: FAILURE 
  */
{
  const FItem* FPtr=findVar(Key);
  if (!FPtr) return -1;

  if (FPtr->getValue(oVec))
    return 1;
  FPtr->getValue(oDbl);
  return 0;
}

template<typename T>
T
varList::getValue(const int Key) const
  /*!
    Get the value from an time
    \param Key :: return value
    \retval value if Key exists
    \retval 0.0 if not (should this throw)
  */
{
  const FItem* FPtr=findVar(Key);
  T Out(0);
  if (FPtr)
    FPtr->getValue(Out);
  return Out;
}

template<>
std::string
varList::getValue(const int Key) const
  /*!
    Get the value from an time
    \param Key :: return value
    \retval value if Key exists
    \retval 0.0 if not (should this throw?)
  */
{
  const FItem* FPtr=findVar(Key);
  std::string Out("NULL");
  if (FPtr)
    FPtr->getValue(Out);
  return Out;
}

template<typename T>
void
varList::setValue(const int Key,const T& Value)
  /*!
    Get the value from an time
    \param Key :: Item to set [if it doesnt exist nothing done]
    \param Value :: Value to set
  */
{
  FItem* FPtr=findVar(Key);
  if (FPtr)
    FPtr->setValue(Value);
  return;
}

void
varList::removeVar(const std::string& Name)
  /*!
    Remove a variable
    \param Name :: Name of variable
    \todo Improve varList as this is rubbish code
  */
{
  ELog::RegMethod RegA("varList","removeVar");

  varStore::iterator mc=varName.find(Name);
  if (mc==varName.end())
    throw ColErr::InContainerError<std::string>(Name,"Name");

  std::map<int,FItem*>::iterator ic=varItem.find(mc->second->getIndex());
  if (ic==varItem.end())
    throw ColErr::InContainerError<int>(mc->second->getIndex(),
                                        "NAME [INT] "+Name);

  delete mc->second;
  varItem.erase(ic);
  varName.erase(mc);
  return;
}


template<typename T>
FItem* 
varList::createFType(const int I,const T& V)
  /*!
    Creates a specific varble from an index and 
    value
    \param I :: Index value
    \param V :: Value 
    \return FItem pointer.
  */

{ 
  return new FValue<T>(this,I,V); 
}
  


template<>
FItem* 
varList::createFType<Code>(const int I,const Code& V) 
  /*!
    Create a function type		      
    \param I :: Index 
    \param V :: Code Item to copy 
    \return Fitem ptr
  */
{
  return new FFunc(this,I,V); 
}

template<>
void
varList::addVar(const std::string& Name,const Code& Value) 
  /*!
    Set the values to be V. If the variable exists
    and is not of the correct type, it is deleted
    and replaced.
    \param Name :: Name of the variable
    \param Value :: current value
  */
{
  std::map<std::string,FItem*>::iterator vc;

  vc=varName.find(Name);
  FItem* Ptr(0);
  if (vc!=varName.end())
    {
      // Note that the variable number is re-used 
      // despite the change in variable.
      const int I=vc->second->getIndex();

      delete vc->second;
      std::map<int,FItem*>::iterator ac;
      ac=varItem.find(I);
      varName.erase(vc);
      varItem.erase(ac);
      Ptr=createFType<Code>(I,Value);
    }
  else
  // Need to make a completely new item
    {
      Ptr=createFType(varNum,Value);
      varNum++;
    }
  // Now insert into master lists
  varName.emplace(Name,Ptr);
  varItem.emplace(Ptr->getIndex(),Ptr);
  return;
}

template<typename T>
void
varList::addVar(const std::string& Name,const T& Value) 
  /*!
    Set the values to be V. If the variable exists
    and is not of the correct type, it is deleted
    and replaced.
    \param Name :: Name of the variable
    \param Value :: current value
  */
{
  std::map<std::string,FItem*>::iterator vc;

  vc=varName.find(Name);
  FItem* Ptr(0);
  if (vc!=varName.end())
    {
      // Note that the variable number is re-used 
      // despite the change in variable.
      const int I=vc->second->getIndex();

      delete vc->second;
      std::map<int,FItem*>::iterator ac;
      ac=varItem.find(I);
      varName.erase(vc);
      varItem.erase(ac);
      Ptr=createFType<T>(I,Value);
    }
  else
  // Need to make a completely new item
    {
      Ptr=createFType(varNum,Value);
      varNum++;
    }
  // Now insert into master lists
  varName.insert(std::pair<std::string,FItem*>(Name,Ptr));
  varItem.insert(std::pair<int,FItem*>(Ptr->getIndex(),Ptr));
  return;
}

template<typename T>
void
varList::setVar(const std::string& Name,const T& Value) 
  /*!
    Set the values to be V and it must have the same type
    \throw InContainterError if Name is not current
    \param Name :: Name of the variable
    \param Value :: current value
  */
{
  std::map<std::string,FItem*>::iterator vc;
  vc=varName.find(Name);
  if (vc==varName.end())
    throw ColErr::InContainerError<std::string>(Name,"varList::setVar");
  try
    {
      vc->second->setValue(Value);
    }
  catch (ColErr::ExBase&)
    {
      // Wrong type?
      addVar(Name,Value);
    }
  return;
}

std::vector<std::string>
varList::getKeys() const
  /*!
    Generate the keys for the variables. Sorted
    \return sorted vector of names
  */
{
  std::vector<std::string> keyValue;
  for(const varStore::value_type& mc : varName)
    {
      keyValue.push_back(mc.first);
    }
  std::sort(keyValue.begin(),keyValue.end());
  return keyValue;
}

void
varList::writeActive(std::ostream& OX) const
  /*!
    Write out all active the variables.
    Assumes that varStore is stored alphabetically.
    \param OX :: Output stream
  */
{
  for(const varStore::value_type& mc : varName)
    {
      if (mc.second->isActive())
	{
	  OX<<mc.first<<" ";
	  mc.second->write(OX);
	  OX<<std::endl;
	}
    }
  return;
}

void
varList::writeAll(std::ostream& OX) const
  /*!
    Write out all the variables.
    \param OX :: Output stream
  */
{
  varStore::const_iterator mc;
  for(mc=varName.begin();mc!=varName.end();mc++)
    {
      OX<<mc->first<<" ";
      mc->second->write(OX);
      OX<<std::endl;
    }
  return;
}


///\cond TEMPLATE

template void varList::setValue(const int,const Geometry::Vec3D&);
template void varList::setValue(const int,const std::string&);
template void varList::setValue(const int,const double&);
template void varList::setValue(const int,const int&);
template void varList::setValue(const int,const size_t&);
template void varList::setValue(const int,const Code&);

template double varList::getValue(const int) const;
template size_t varList::getValue(const int) const;
template int varList::getValue(const int) const;

template void varList::addVar(const std::string&,const std::string&);
template void varList::addVar(const std::string&,const Geometry::Vec3D&);
template void varList::addVar(const std::string&,const double&);
template void varList::addVar(const std::string&,const int&);
template void varList::addVar(const std::string&,const long int&);
template void varList::addVar(const std::string&,const size_t&);
// template void varList::addVar(const std::string&,const Code&);

template void varList::setVar(const std::string&,const Geometry::Vec3D&);
template void varList::setVar(const std::string&,const std::string&);
template void varList::setVar(const std::string&,const double&);
template void varList::setVar(const std::string&,const int&);
template void varList::setVar(const std::string&,const long int&);
template void varList::setVar(const std::string&,const size_t&);
template void varList::setVar(const std::string&,const Code&);

template FItem* varList::createFType(const int,const Geometry::Vec3D&);
template FItem* varList::createFType(const int,const double&);
template FItem* varList::createFType(const int,const std::string&);
template FItem* varList::createFType(const int,const int&);
template FItem* varList::createFType(const int,const long int&);
template FItem* varList::createFType(const int,const size_t&);

///\endcond TEMPLATE

