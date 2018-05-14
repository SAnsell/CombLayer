/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LinkWrapper.cxx
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
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "surfExpand.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "ExcludedComp.h"
#include "LinkWrapper.h"

namespace constructSystem
{

LinkWrapper::LinkWrapper(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::ExcludedComp(),
  attachSystem::FixedComp(Key,40),InOutLinkB(20),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LinkWrapper::LinkWrapper(const LinkWrapper& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::ExcludedComp(A),
  attachSystem::FixedComp(A),InOutLinkB(A.InOutLinkB),
  refIndex(A.refIndex),cellIndex(A.cellIndex),surfNum(A.surfNum),
  surfCent(A.surfCent),surfAxis(A.surfAxis),flag(A.flag),
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
      attachSystem::ExcludedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
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
LinkWrapper::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("LinkWrapper","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  for(size_t i=0;i<nLayers;i++)
    {
      std::string kItem=keyName+"LayThick";
      std::string kMat=keyName+"LayMat";
      layerThick.push_back(Control.EvalVar<double>
			   (StrFunc::makeString(kItem,i+1)));
      layerMat.push_back(ModelSupport::EvalMat<int>
			 (Control,StrFunc::makeString(kMat,i+1)));
    }
  // Material
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
  return;
}
  
void
LinkWrapper::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("LinkWrapper","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

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
LinkWrapper::addSurface(const std::string& FCName,
			const long int linkIndex)
/*!
  Add a boundary surface
  \param FCName :: Fixed object to use
  \param linkIndex :: Link surface index 
*/
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(string,Index)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const attachSystem::FixedComp* FCptr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCName,"FixedComp");

  addSurface(*FCptr,linkIndex);
  return;
}

void
LinkWrapper::addSurface(const std::string& FCName,
			const std::string& LList)
/*!
  Add a boundary surface
  \param FCName :: Fixed object to use
  \param LList :: Link surface index 
*/
{
  ELog::RegMethod RegA("LinkWrapper","addSurface(string,string)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const attachSystem::FixedComp* FCptr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCName,"FixedComp");

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

std::string
LinkWrapper::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,refIndex,surfList);
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
LinkWrapper::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("LinkWrapper","createAll");
  populate(System);
  processMask();

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE constructSystem

