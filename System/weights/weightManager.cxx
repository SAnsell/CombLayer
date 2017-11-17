/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/weightManager.cxx
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
#include <memory>
#include <boost/multi_array.hpp>

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
#include "particleConv.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "Mesh3D.h"
#include "WWGWeight.h"
#include "WWG.h"
#include "weightManager.h"


namespace WeightSystem
{

weightManager::weightManager() :
  WWGPtr(0)
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

WWG&
weightManager::getWWG()
  /*!
    Create/access mesh
    \return Mesh
  */
{
  ELog::RegMethod RegA("weightManager","getWWG");
  if (!WWGPtr)
    WWGPtr=new WWG;
  return *WWGPtr;
}

const WWG&
weightManager::getWWG() const
  /*!
    Create/access mesh
    \return Mesh
  */
{
  ELog::RegMethod RegA("weightManager","getWWG(const)");
  if (!WWGPtr)
    throw ColErr::EmptyValue<void>("WWGPtr not valid");
  
  return *WWGPtr;
}

  
bool
weightManager::hasParticle(const std::string& particleType) const
  /*!
    Determine if a specific particle type exists
    \param particleType :: Particle identifier
    \return true if exists
  */
{
  CtrlTYPE::const_iterator mc=WMap.find(particleType);
  return (mc==WMap.end()) ? 0 : 1;
}
  
WForm*
weightManager::getParticle(const std::string& particleType) const
  /*!
    Get a specific particle type
    \param particleType :: Particle identifier
    \return WForm pointer 
  */
{
  ELog::RegMethod RegA("weightManager","getParticle");
  CtrlTYPE::const_iterator mc=WMap.find(particleType);
  if (mc==WMap.end())
    throw ColErr::InContainerError<std::string>
      (particleType,"particle not found");
  
  return mc->second;
}

template<typename T>
void
weightManager::addParticle(const std::string& particleType)
  /*!
    Add a specific component
    \param particleType :: particle key 
   */
{
  ELog::RegMethod RegA("weightManager","addComponent");
  if (WMap.find(particleType)==WMap.end())
    {
      WMap.emplace(particleType,new T(particleType));
    }
  return;
}

void
weightManager::maskCell(const int zCell)  
  /*!
    Zero [-1] the weight of the cell
    \param zCell :: cell to be masked in all weight systems
  */
{
  ELog::RegMethod RegA("weightManager","maskCell");
  for(CtrlTYPE::value_type& wf : WMap)
    wf.second->maskCell(zCell);
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
	   std::bind(&WForm::renumberCell,
		     std::bind(&CtrlTYPE::value_type::second,
				 std::placeholders::_1),
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
  ELog::RegMethod RegA("weightManager","isMasked");
  
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
weightManager::writePHITS(std::ostream& OX) const
  /*!
    Write out the weight system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("weightManager","writePHITS");

  const particleConv& PConv=particleConv::Instance();
  if (!WMap.empty())
    {
      OX<<"[weight window]\n";
      for(const CtrlTYPE::value_type& wf : WMap)
        {
          const std::vector<double>& Evec=wf.second->getEnergy();

          OX<<"  part = "
	    <<PConv.phitsType(wf.second->getParticle())
	    <<std::endl;

          OX<<"  eng = "<<Evec.size()<<std::endl;
          for( const double& E : Evec )
            OX<<"  "<<E;
          OX<<std::endl;

          OX<<"reg  ";
          for(size_t i=1;i<=Evec.size();i++)
            OX<<"    ww"<<i;
          OX<<std::endl;

          wf.second->writePHITS(OX);
        }
    }
  
  return;
}

void
weightManager::write(std::ostream& OX) const
  /*!
    Write out the weight system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("weightManager","write");
  
  for(const CtrlTYPE::value_type& wf : WMap)
    wf.second->write(OX);

  if (WWGPtr)
    {
      WWGPtr->writeWWINP("wwinp");
      WWGPtr->write(OX);
    }
  
  return;
}

///\cond TEMPLATE

template void weightManager::addParticle<WCells>(const std::string&);

///\endcond TEMPLATE
  

  
}   // NAMESPACE WeightSystem
