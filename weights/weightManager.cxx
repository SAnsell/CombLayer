/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weights/weightManager.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "weightManager.h"


namespace WeightSystem
{

weightManager::weightManager()
  /*!
    Constructor
  */
{}

weightManager::~weightManager()
  /*!
    Destructor 
  */
{
  CtrlTYPE::iterator mc;
  for(mc=WMap.begin();mc!=WMap.end();mc++)
    delete mc->second;
}

weightManager&
weightManager::Instance() 
  /*!
    Effective this object			
    \return weightManager object
  */
{
  static weightManager A;
  return A;
}

WForm*
weightManager::getParticle(const char c)
  /*!
    Get a specific particle type
    \param c :: Particle identifier
    \return WForm pointer 
  */
{
  ELog::RegMethod RegA("weightManager","getItem");
  CtrlTYPE::iterator mc=WMap.find(c);
  if (mc==WMap.end())
    throw ColErr::InContainerError<char>(c,RegA.getBase());
  return mc->second;
}

template<typename T>
void
weightManager::addParticle(const char c)
  /*!
    Add a specific component
    \param c :: particle key [should this be a string?]
   */
{
  ELog::RegMethod RegA("weightManager","addComponent");
  if (WMap.find(c)==WMap.end())
    {
      WMap.insert(CtrlTYPE::value_type(c,new T(c)));
    }
  return;
}

void
weightManager::maskCell(const int zCell)  
  /*!
    Zero [-1] the weight of all cells
    \param zCell :: Original Cell
  */
{
  ELog::RegMethod RegA("weightManager","zeroCell");
  for_each(WMap.begin(),WMap.end(),
	   boost::bind(&WForm::maskCell,
		       boost::bind(&CtrlTYPE::value_type::second,_1),zCell));
  return;
}
  

void
weightManager::renumberCell(const int OCell,const int NCell)  
  /*!
    Renumber cells [if required]
    \param OCell :: Original Cell
    \param NCell :: New Cell number
  */
{
  ELog::RegMethod RegA("weightManager","renumberCell");
  for_each(WMap.begin(),WMap.end(),
	   boost::bind(&WForm::renumberCell,
		       boost::bind(&CtrlTYPE::value_type::second,_1),
		       OCell,NCell));
  return;
}


bool
weightManager::isMasked(const int cellN) const
  /*!
    Determine if a cell is fully masked
    \param cellN :: Cell Number
    \return true/false
  */
{
  ELog::RegMethod RegA("weightManager","renumberCell");
  
  CtrlTYPE::const_iterator mc;
  for(mc=WMap.begin();mc!=WMap.end();mc++)
    {
      const WCells* NC=dynamic_cast<const WCells*>(mc->second);
      if (NC)
	return NC->isMasked(cellN);
    }
  return false;
}
  
void
weightManager::write(std::ostream& OX) const
  /*!
    Write out the weight system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("weightManager","write");
  for_each(WMap.begin(),WMap.end(),
	   boost::bind(&WForm::write,
		       boost::bind(&CtrlTYPE::value_type::second,_1),
		       boost::ref(OX)));
  return;
}

///\cond TEMPLATE

template void weightManager::addParticle<WCells>(const char);

///\endcond TEMPLATE
  

  
}   // NAMESPACE WeightSystem
