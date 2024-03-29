/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LinkWrapper.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinkWrapper.h"

namespace constructSystem
{

LinkWrapper::LinkWrapper(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,40),
  InOutLinkB(20),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LinkWrapper::LinkWrapper(const LinkWrapper& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  InOutLinkB(A.InOutLinkB),
  surfNum(A.surfNum),surfCent(A.surfCent),
  surfAxis(A.surfAxis),flag(A.flag),
  surfEntryOrder(A.surfEntryOrder),nLayers(A.nLayers),
  layerThick(A.layerThick),layerMat(A.layerMat),
  defMat(A.defMat),mask(A.mask)
  /*!
    Copy constructor
    \param A :: LinkWrapper to copy
  */
{}

LinkWrapper&
LinkWrapper::operator=(const LinkWrapper& A)
  /*!
    Assignment operator
    \param A :: LinkWrapper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      surfNum=A.surfNum;
      surfCent=A.surfCent;
      surfAxis=A.surfAxis;
      flag=A.flag;
      surfEntryOrder=A.surfEntryOrder;
      nLayers=A.nLayers;
      layerThick=A.layerThick;
      layerMat=A.layerMat;
      defMat=A.defMat;
      mask=A.mask;
    }
  return *this;
}

LinkWrapper::~LinkWrapper() 
 /*!
   Destructor
 */
{}

void
LinkWrapper::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("LinkWrapper","populate");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  for(size_t i=0;i<nLayers;i++)
    {
      std::string kItem=keyName+"LayThick";
      std::string kMat=keyName+"LayMat";
      layerThick.push_back(Control.EvalVar<double>
			   (kItem+std::to_string(i+1)));
      layerMat.push_back(ModelSupport::EvalMat<int>
			 (Control,kMat+std::to_string(i+1)));
    }
  // Material
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
  return;
}
  
void
LinkWrapper::addSurface(const attachSystem::FixedComp& FC,
			std::string LList)
/*!
    Add a set of boundary surfaces
    \param FC :: Fixed object to use
    \param LList :: Link surface index (as a deliminated string)
  */
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(FC,string)");

  int exitFlag(0);
  std::string LX;  // Item 
  long int NX;
  while( StrFunc::section(LList,NX) )
    {
      addSurface(FC,NX);
      exitFlag++;
    }
  if (!exitFlag)
    ELog::EM<<"Failed to process any surface Line: "
	    <<LList<<ELog::endErr;
  return;
}

void
LinkWrapper::addSurface(const objectGroups& OGrp,
			const std::string& FCName,
			const long int linkIndex)
/*!
  Add a boundary surface
  \param FCName :: Fixed object to use
  \param linkIndex :: Link surface index 
*/
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(string,Index)");

  
  const attachSystem::FixedComp* FCptr=
    OGrp.getObjectThrow<attachSystem::FixedComp>(FCName,"FixedComp");
  addSurface(*FCptr,linkIndex);
  return;
}

void
LinkWrapper::addSurface(const objectGroups& OGrp,
			const std::string& FCName,
			const std::string& LList)
/*!
  Add a boundary surface
  \param OGrp :: outer group
  \param FCName :: Fixed object to use
  \param LList :: Link surface index 
*/
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(string,string)");

  
  const attachSystem::FixedComp* FCptr=
    OGrp.getObjectThrow<attachSystem::FixedComp>(FCName,"FixedComp");

  addSurface(*FCptr,LList);
  return;
}

void
LinkWrapper::addSurface(const attachSystem::FixedComp& FC,
			const long int LIndex)
/*!
  Add a boundary surface
  \param FC :: Fixed object to use
  \param LIndex :: Link surface index 
*/
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(FC,Index)");

  // Surfaces on links point outwards (hence swap of sign)
  surfNum.push_back(FC.getLinkSurf(LIndex));
  surfEntryOrder.push_back(static_cast<int>(surfNum.size()));
  return;
}

void
LinkWrapper::addSurface(const Geometry::Vec3D& Cent,
			const Geometry::Vec3D& Axis)
/*!
    Add a boundary surface
    \param Cent :: Centre Point
    \param Axis :: Axis point
  */
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(Vec3D)");

  surfCent.push_back(Cent);
  surfAxis.push_back(Axis.unit());
  surfEntryOrder.push_back(-static_cast<int>(surfAxis.size()));
  return;
}


bool
LinkWrapper::sectorFlag(const size_t LN,const size_t SN) const
  /*!
    Determine if a sector is used
    \param LN :: Layer number
    \param SN :: Sector number
    \return true if the sector is to be built
  */
{
  ELog::RegMethod RegA("LinkWrapper","sectorFlag");
  return (! (flag[SN] & (1UL << LN)) );
}


void
LinkWrapper::processMask()
  /*!
    Internal function to process mask [AFTER populate]
  */
{
  ELog::RegMethod RegA("LinkWrapper","processMask");

  flag.resize(surfEntryOrder.size());
  fill(flag.begin(),flag.end(),0);
  std::map<size_t,size_t>::const_iterator mc;
  for(mc=mask.begin();mc!=mask.end();mc++)
    {
      if (mc->first>=surfEntryOrder.size())
	throw ColErr::IndexError<size_t>(mc->first,surfEntryOrder.size(),
					 "surfEntryOrder size");
	
      const int indexNum=surfEntryOrder[mc->first];  // sector number

      size_t SN(0);
      if (indexNum<0)  // That is a constructed item
	SN=surfNum.size()+static_cast<size_t>(-indexNum)-1;
      else
	SN=static_cast<size_t>(indexNum)-1;

      flag[SN]= (mc->second==0) ? (1 << nLayers)-1 : mc->second;
    }
  
  return;
}

void
LinkWrapper::maskSection(const size_t sectNumber)
  /*!
    Mask the full section
    \param sectNumber :: Sector number
  */
{
  ELog::RegMethod RegA("LinkWrapper","maskSection");

  mask[sectNumber]=0;
  return;
}

void
LinkWrapper::maskSection(std::string sectList)
  /*!
    Mask the full section
    \param sectList :: section numbers
  */
{
  ELog::RegMethod RegA("LinkWrapper","maskSection(string)");

  size_t Index;
  while(StrFunc::section(sectList,Index))
    mask[Index]=0;

  if (!StrFunc::isEmpty(sectList))
    throw ColErr::InvalidLine(sectList,"sectList not empty");
    
  return;
}

void
LinkWrapper::addExcludeObj(const int SN)
  /*!
    Add a single surface to the exclude space (union)
    \param SN ::  Surf number
  */
{
  ELog::RegMethod RegA("LinkWrapper","addExcludeObj(int)");

  excludeSpace.addUnion(HeadRule(SN)); 
  return;
}

void
LinkWrapper::addExcludeObj(const objectGroups& OGrp,
			   const std::string& objName,
			   const std::string& grpName) 
  /*!
    Add a set of surfaces to the output
    \param ObjName ::  ContainedGroup Object Name [to use outer cell]
    \param grpName ::  Group in CG
  */
{
  ELog::RegMethod RegA("LinkWrapper","addExcludeObj(str,str)");

  const attachSystem::ContainedGroup* CCPtr=
    OGrp.getObjectThrow<attachSystem::ContainedGroup>
    (objName,"objectHame");
  
  const std::string OutStr=CCPtr->getCompExclude(grpName);
  excludeSpace.addUnion(CCPtr->getOuterSurf(grpName)); 
 return;
}

void
LinkWrapper::addExcludeObj(const objectGroups& OGrp,
			   const std::string& objName)
  /*!
    Add a set of surfaces to the output
    \param ObjName ::  ContainedComp/Group Object Name [to use outer cell]
  */
{
  ELog::RegMethod RegA("LinkWrapper","addExcludeObj(str,str)");

  const attachSystem::ContainedGroup* CCPtr=
    OGrp.getObject<attachSystem::ContainedGroup>(objName);
  if (CCPtr)
    excludeSpace.addUnion(CCPtr->getAllExclude());
  else
    {
      const ContainedComp* CCPtr=
	OGrp.getObjectThrow<ContainedComp>(objName,"Object Name");
      excludeSpace.addUnion(CCPtr->getOuterSurf());      
    }
  return;
}

void
LinkWrapper::addExcludeObj(const HeadRule& HR)
  /*!
    Add a set of surfaces to the output
    \param HR ::  HeadRule to add
  */
{
  ELog::RegMethod RegA("LinkWrapper","addExcludeObj(HR)");

  excludeSpace.addUnion(HR);      
  return;
}



void
LinkWrapper::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("LinkWrapper","createAll");

  populate(System.getDataBase());
  processMask();

  FixedComp::createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  
  return;
}

  
}  // NAMESPACE constructSystem

