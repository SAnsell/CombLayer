/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/matIndex.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "matIndex.h"

namespace ModelSupport
{

matIndex::matIndex() 
  /*!
    Constructor
  */
{}

matIndex& 
matIndex::Instance() 
  /*!
    Effective this object			
    \return matIndex object
  */
{
  static matIndex A;
  return A;
}

matIndex::~matIndex()
  /*!
    Destructor
  */
{
  MTYPE::iterator mc;
  for(mc=MMap.begin();mc!=MMap.end();mc++)
    delete mc->second;
}

MonteCarlo::Material*
matIndex::addMaterial(MonteCarlo::Material* MPtr)
  /*!
    Adds and manages a material
    \param MPtr :: material to add
    \return Material Ptr
  */
{
  ELog::RegMethod RegA("matIndex","addMaterial");
  // First off make the surface a uniq
  if (!MPtr)
    throw ColErr::EmptyValue<MonteCarlo::Material*>("matIndex::addMaterial");
  MTYPE::iterator vc=MMap.find(MPtr->getName());
  if (vc!=MMap.end())
    {
      ELog::EM<<"Material overwriting existing mat : "
	      <<MPtr->getName()<<ELog::endWarn;
      delete vc->second;
      MMap.erase(vc);
    }
  MMap.insert(MTYPE::value_type(MPtr->getName(),MPtr));
  return MPtr;
}

MonteCarlo::Material*
matIndex::cloneMaterial(const MonteCarlo::Material* MPtr)
  /*!
    Adds a clone of a material
    \param MPtr :: material to add
    \return Material Ptr
  */
{
  return (MPtr) ? addMaterial(MPtr->clone()) : 0;
}

MonteCarlo::Material*
matIndex::createMaterial(const std::string& matName)
  /*!
    Helper function to determine to create a material
    \param matName : Number of the Material to create
    \returns Material pointer 
  */
{
  ELog::RegMethod RegA("matIndex","createMateiral");
  
  MonteCarlo::Material* outPtr=new MonteCarlo::Material;
  outPtr->setName(matName);
  return addMaterial(outPtr);
}


MonteCarlo::Material*
matIndex::getMat(const std::string& MName) const
  /*!
    Get a material based on the name
    \param MName :: Search name
    \return Material Ptr / 0
   */
{
  MTYPE::const_iterator mc=MMap.find(MName);
  return (mc==MMap.end()) ? 0 : mc->second;
}


void
matIndex::createUniqID()
  /*!
    Create a unique number of all the materials
    This can be done anytime since names and pointers are
    used not the number. This is just for the MCNPX output
  */
{
  int N(1);
  MTYPE::iterator sp;
  for(sp=MMap.begin();sp!=MMap.end();sp++)
    {
      if (sp->second)
        {
	  sp->second->setNumber(N);
	  N++;
	}
    }
  return;  
}

void
matIndex::writeMCNPX(std::ostream& OX) const
  /*!
    Write out the materials for an MCNPX file
    \param OX :: Output stream
  */
{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- MATERIAL CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  MTYPE::const_iterator sp;
  for(sp=MMap.begin();sp!=MMap.end();sp++)
    {
      if (sp->second)
        {
	  sp->second->write(OX);
	}
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}


} // NAMESPACE ModelSupport
