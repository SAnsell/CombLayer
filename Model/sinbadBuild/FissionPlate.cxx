/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sinbadBuild/FissionPlate.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <memory>
#include <boost/multi_array.hpp>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"

#include "LayerPlate.h"
#include "FissionPlate.h"

namespace sinbadSystem
{

FissionPlate::FissionPlate(const std::string& Key) : 
  LayerPlate(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FissionPlate::FissionPlate(const FissionPlate& A) : 
  LayerPlate(A),
  DIndex(A.DIndex),nDivide(A.nDivide),nXSpace(A.nXSpace),
  nZSpace(A.nZSpace),XPts(A.XPts),ZPts(A.ZPts),
  matIndex(A.matIndex),matTemp(A.matTemp)
  /*!
    Copy constructor
    \param A :: FissionPlate to copy
  */
{}

FissionPlate&
FissionPlate::operator=(const FissionPlate& A)
  /*!
    Assignment operator
    \param A :: FissionPlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      LayerPlate::operator=(A);
      DIndex=A.DIndex;
      nDivide=A.nDivide;
      nXSpace=A.nXSpace;
      nZSpace=A.nZSpace;
      XPts=A.XPts;
      ZPts=A.ZPts;
      matIndex=A.matIndex;
      matTemp=A.matTemp;
    }
  return *this;
}

FissionPlate*
FissionPlate::clone() const
  /*!
    Simple clone funciton
    \return new(this)
   */
{
  return new FissionPlate(*this);
}


FissionPlate::~FissionPlate() 
  /*!
    Destructor
  */
{}

std::string
FissionPlate::getXSurf(const long int Index) const
  /*!
    Calculate the side +/- surfaces
    \param Index :: Index array
    \return String
  */
{
  ELog::RegMethod RegA("FissionPlate","getXSurf");

  const int SI(buildIndex+1000+(static_cast<int>(Index)-1)*10);
  if (Index>0 && Index<static_cast<long int>(nXSpace)-1)
    return ModelSupport::getComposite(SMap,SI," 3 -13 ");
  if (Index==0)
    return ModelSupport::getComposite(SMap,buildIndex,SI," 3 -13M ");
  // right hand edge:
  return ModelSupport::getComposite(SMap,buildIndex,SI," 3M -4 ");
}


std::string
FissionPlate::getZSurf(const long int Index) const
  /*!
    Calculate the top/bottom +/- surfaces
    \param Index :: Index array
    \return String
  */
{
  ELog::RegMethod RegA("FissionPlate","getZSurf");
  
  const int SI(buildIndex+1000+(static_cast<int>(Index)-1)*10);
  if (Index>0 && Index<static_cast<long int>(nZSpace)-1)
    return ModelSupport::getComposite(SMap,SI," 5 -15 ");
  if (Index==0)
    return ModelSupport::getComposite(SMap,buildIndex,SI," 5 -15M ");
  // top edge:
  return ModelSupport::getComposite(SMap,buildIndex,SI," 5M -6 ");
}

template<typename T>
T
FissionPlate::getXZ(const FuncDataBase& Control,
		    const std::string& SubKey,const long int dLayer,
		    const long int I,const long int J) const
  /*!
    Get a database entry based on a key system:
    -  Naming for Individual : L3X12Z45
    -  Naming for Row : L3X12 
    -  Naming for Column : L3Z45 
    -  Naming for DEFAULT : L3           
    Note that layer has NO default
    \param Control :: DataBase of variables
    \param SubKey :: SubKey after master keyName
    \param I :: First index 
    \param J :: Second index 
    \return Database entry
   */
{
  ELog::RegMethod RegA("FissionPlate","getXZ");

  const std::string LN="L"+std::to_string(dLayer);
  const std::string XN="X"+std::to_string(I);
  const std::string ZN="Z"+std::to_string(J);
  const std::string PreName=keyName+SubKey+LN;
  if (Control.hasVariable(PreName+XN+ZN))
    return Control.EvalVar<T>(PreName+XN+ZN);
  if (Control.hasVariable(PreName+XN))
    return Control.EvalVar<T>(PreName+XN);
  if (Control.hasVariable(PreName+ZN))
    return Control.EvalVar<T>(PreName+ZN);

  // If this fails :: Exception thrown
  return Control.EvalVar<T>(PreName);
}
    
void
FissionPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FissionPlate","populate");

  LayerPlate::populate(Control);

  nDivide=0;

  // First determin if we have any index layers to process:
  const std::string KN=keyName+"DivIndex";
  for(size_t i=0;i<nSlab && 
	Control.hasVariable(KN+std::to_string(i));i++)
    DIndex.push_back(Control.EvalVar<size_t>(KN+std::to_string(i)));

  if (DIndex.empty() || 
      (*std::max_element(DIndex.begin(),DIndex.end())>=nSlab))
    {
      ELog::EM<<"Fission Plate not divided "<<ELog::endWarn;
      return;
    }

  // Division of layers :
  nXSpace=Control.EvalVar<size_t>(keyName+"NXSpace");  
  nZSpace=Control.EvalVar<size_t>(keyName+"NZSpace");  
  
  if (nXSpace*nZSpace==0)
    {
      ELog::EM<<"Fission Plate not divided :"
	      <<nXSpace<<" "<<nZSpace<<ELog::endWarn;
      return;
    }
    
  // READ XPTS/ZPTS [sort -- but can check for uniq etc]
  XPts.clear();
  ZPts.clear();
  double XV,ZV;
  for(size_t i=0;i<nXSpace-1;i++)
    {
      XV=Control.EvalVar<double>(keyName+"XPt"+std::to_string(i));
      XPts.push_back(XV);
    }
  std::sort(XPts.begin(),XPts.end());
  
  for(size_t i=0;i<nZSpace-1;i++)
    {
      ZV=Control.EvalVar<double>(keyName+"ZPt"+std::to_string(i));
      ZPts.push_back(ZV);
    }
  std::sort(ZPts.begin(),ZPts.end());

  //
  // MASTER Loop for Mat/Mat Temp:
  //
  nDivide=DIndex.size();
  matIndex.resize(boost::extents
		  [static_cast<long int>(nDivide)]
		  [static_cast<long int>(nXSpace)]
		  [static_cast<long int>(nZSpace)]);
  matTemp.resize(boost::extents
		 [static_cast<long int>(nDivide)]
		 [static_cast<long int>(nXSpace)]
		 [static_cast<long int>(nZSpace)]);

  for(long int dLayer=0;dLayer < static_cast<long int>(nDivide);dLayer++)
    {
      for(long int i=0;i<static_cast<long int>(nXSpace);i++)
	for(long int j=0;j<static_cast<long int>(nZSpace);j++)
	  {
	    const std::string matName=
	      getXZ<std::string>(Control,"Mat",dLayer,i,j);
	    matIndex[dLayer][i][j]=ModelSupport::EvalMatName(matName);
	    matTemp[dLayer][i][j]=getXZ<double>(Control,"MatTemp",dLayer,i,j);
	  }
    }
  return;
}

void
FissionPlate::createSurfaces()
  /*!
    Create All the extra surfaces for the divided cell
  */
{
  ELog::RegMethod RegA("FissionPlate","createSurface");


  LayerPlate::createSurfaces();
  if (!nDivide) return;

  int SI(buildIndex+1000);  
  std::vector<double>::const_iterator vc;
  for(size_t i=0;i<XPts.size();i++)    
    {
      ModelSupport::buildPlane(SMap,SI+3,Origin+X*XPts[i],X);
      SI+=10;
    }
  SI=(buildIndex+1000);
  for(size_t i=0;i<ZPts.size();i++)   
    {
      ModelSupport::buildPlane(SMap,SI+5,Origin+Z*ZPts[i],Z);
      SI+=10;
    }
  return;
}


void
FissionPlate::createObjects(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)
/*!
    Create all the objects
    \param System :: Simulation to create objects in
    \param FC :: FixedComp
    \param sideIndex :: SIGNED sideIndex offset by 1 [0 base origin]
  */
{
  ELog::RegMethod RegA("FissionPlate","createObjects");
  
  LayerPlate::createObjects(System,FC,sideIndex);
  // No work check
  if (nDivide==0)
    return;

  for(size_t dLayer=0;dLayer<nDivide;dLayer++)
    {
      const long int DI(static_cast<long int>(dLayer));

      const int CIndex=getCellIndex(DIndex[dLayer]);
      System.removeCell(CIndex);

      // This should be to a virtual function:
      const std::string fbSurf=getFrontSurface(DIndex[dLayer],FC,sideIndex)+
	getBackSurface(DIndex[dLayer],FC,sideIndex);

      for(long int i=0;i<static_cast<long int>(nXSpace);i++)
	{
	  const std::string sides=getXSurf(i);
	  for(long int j=0;j<static_cast<long int>(nZSpace);j++)
	    {
	      const std::string Out=
		fbSurf+sides+getZSurf(j);

	      System.addCell
		(MonteCarlo::Object
		 (cellIndex++,matIndex[DI][i][j],matTemp[DI][i][j],Out));
	      
	    }
	}   
    }   
  return;
}


void
FissionPlate::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("FissionPlate","createAll");

  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC,sideIndex);
  insertObjects(System);

  return;
}
  
} 
