/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LWInner.cxx
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "generateSurf.h"
#include "surfExpand.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LinkWrapper.h"
#include "LWInner.h"

namespace constructSystem
{

LWInner::LWInner(const std::string& Key) : 
  LinkWrapper(Key)
  /*!
    Constructor
    \param Key :: Name for item 
   */
{}

LWInner::LWInner(const LWInner& A) : 
  LinkWrapper(A)
  /*!
    Copy constructor
    \param A :: LWInner to copy
  */
{}

LWInner&
LWInner::operator=(const LWInner& A)
  /*!
    Assignment operator
    \param A :: LWInner to copy
    \return *this
  */
{
  if (this!=&A)
    {
      LinkWrapper::operator=(A);
    }
  return *this;
}

LWInner::~LWInner()
  /*!
    Destructor
   */
{}					   


void
LWInner::createObjects(Simulation& System)
  /*!
    Creates the inner view Wrapping object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LWInner","createObjects");

  const HeadRule boundaryComp(excludeSpace.complement());
  HeadRule HR;
  for(const int SN : surfNum)
    HR.addIntersection(SN);

  addOuterSurf(HR);

  HeadRule OuterHR(HR);
  for(size_t i=0;i<nLayers;i++)
    {
      HeadRule CX;
      int lSurf(buildIndex+101+100*static_cast<int>(i));
      for(size_t j=0;j<surfNum.size();j++)
	{
	  const int signV((surfNum[j]>0) ? 1 : -1);
	  if (sectorFlag(i,j))
	    CX.addIntersection(SMap.realSurf(signV*lSurf));
	  else
	    CX.addIntersection(SMap.realSurf(surfNum[j]));
	  lSurf++;
	}
      System.addCell(cellIndex++,layerMat[i],0.0,
		     OuterHR*boundaryComp*CX.complement());
      OuterHR=CX;
    }
  // Special object : the inner:
  // Add boundary exclude:
  if (defMat>=0)
    {
      const HeadRule preOuter(OuterHR);
      OuterHR*=boundaryComp;
      System.addCell(cellIndex++,defMat,0.0,OuterHR);
    }
  return;
}

void
LWInner::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LWInner","createSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  // Create outer surfaces [if needed]

  size_t linkIndex(0);
      std::vector<int>::const_iterator vc;
  for(vc=surfNum.begin();vc!=surfNum.end();vc++)
    FixedComp::setLinkSurf(linkIndex++,SMap.realSurf(*vc));

  int nSurf(buildIndex+1);      
  for(size_t i=0;i<surfCent.size();i++)
    {
      const Geometry::Vec3D CP=Origin+surfCent[i];
      const Geometry::Vec3D Axis=X*surfAxis[i].X()+
	Y*surfAxis[i].Y()+Z*surfAxis[i].Z();
  
      ModelSupport::buildPlane(SMap,nSurf,CP,Axis);
      surfNum.push_back(SMap.realSurf(nSurf));
      FixedComp::setLinkSurf(linkIndex++,-surfNum.back());
      nSurf++;
    }

  std::vector<Geometry::Surface*> SList;
  if (nLayers)
    {
      std::vector<int>::const_iterator vc;
      for(vc=surfNum.begin();vc!=surfNum.end();vc++)
	SList.push_back(SurI.getSurf(abs(*vc)));
    }      
  
  // Layer surfaces:
  linkIndex=20;
  double lThick(0.0);
  for(size_t i=0;i<nLayers;i++)
    { 
      lThick+=layerThick[i];
      int lSurf(buildIndex+101+100*static_cast<int>(i));
      for(size_t j=0;j<SList.size();j++)
	{
	  const int signV((surfNum[j]>0) ? 1 : -1);
	  Geometry::Surface* SurOut=
	    ModelSupport::surfaceCreateExpand(SList[j],signV*lThick);
	  SurOut->setName(lSurf);
	  SMap.registerSurf(lSurf,SurOut);

	  if (i==nLayers-1)
	    FixedComp::setLinkSurf(linkIndex++,
				     SMap.realSurf(signV*lSurf));
	  lSurf++;
	}
    }
  
  return;
}
  
}  // NAMESPACE constructSystem
