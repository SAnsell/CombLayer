/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bnct/DiscTarget.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "DiscTarget.h"

namespace bnctSystem
{

DiscTarget::DiscTarget(const std::string& Key)  : 
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  discIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(discIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

DiscTarget::DiscTarget(const DiscTarget& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  discIndex(A.discIndex),cellIndex(A.cellIndex),
  NLayers(A.NLayers),depth(A.depth),radius(A.radius),
  mat(A.mat),maxRad(A.maxRad),maxIndex(A.maxIndex)
  /*!
    Copy constructor
    \param A :: DiscTarget to copy
  */
{}

DiscTarget&
DiscTarget::operator=(const DiscTarget& A)
  /*!
    Assignment operator
    \param A :: DiscTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      NLayers=A.NLayers;
      depth=A.depth;
      radius=A.radius;
      mat=A.mat;
      maxRad=A.maxRad;
      maxIndex=A.maxIndex;
    }
  return *this;
}

DiscTarget::~DiscTarget() 
  /*!
    Destructor
  */
{}

void
DiscTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("DiscTarget","populate");

  FixedOffset::populate(Control);

  NLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double D,R;
  int M;
  for(size_t i=0;i<NLayers;i++)
    {
      const std::string NStr=StrFunc::makeString(i);
      D=Control.EvalVar<double>(keyName+"Depth"+NStr);
      R=Control.EvalPair<double>(keyName+"Radius"+NStr,keyName+"Radius");
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);
      depth.push_back(D);
      radius.push_back(R);
      mat.push_back(M);
    }      
  return;
}

void
DiscTarget::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for origin
    \param sideIndex :: Link Point
  */
{
  ELog::RegMethod RegA("DiscTarget","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
DiscTarget::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("DiscTarget","createSurface");

  ModelSupport::buildPlane(SMap,discIndex+1,Origin,Y);
  int DI(discIndex+10);
  maxRad=0.0;
  for(size_t i=0;i<NLayers;i++)
    {
      ModelSupport::buildPlane(SMap,DI+1,Origin+Y*depth[i],Y);
      ModelSupport::buildCylinder(SMap,DI+7,Origin,Y,radius[i]);
      if (radius[i]>maxRad)
	{
	  maxRad=radius[i];
	  maxIndex=i;
	}
      DI+=10;
    }
  return;
}

void
DiscTarget::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DiscTarget","createObjects");
  
  std::string Out;
  int DI(discIndex+10);
  for(size_t i=0;i<NLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,DI-10,DI,"1 -1M -7M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],0.0,Out));
      if (maxRad-radius[i] >Geometry::zeroTol)
	{
	  double workRadius(radius[i]);
	  int WI(DI);   // work index
	  int SI(DI+10);   // search index
	  for(size_t j=i+1;j<NLayers;j++)
	    {
	      if ((radius[j]-workRadius)>Geometry::zeroTol)
		{
		  Out=ModelSupport::getComposite(SMap,DI-10,DI,"1 -1M  ");
		  Out+=ModelSupport::getComposite(SMap,SI,WI,"  ");
		  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[j],0.0,Out));
		  WI=SI;
		  workRadius=radius[j];
		}
	      SI+=10;
	    }
	  for(size_t j=i;j>0;j--)
	    {
	      if ((radius[j]-workRadius)>Geometry::zeroTol)
		{
		  Out=ModelSupport::getComposite(SMap,DI-10,DI,"1 -1M ");
		  Out+=ModelSupport::getComposite(SMap,SI,WI," 7M -7  ");
		  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[j],0.0,Out));
		  WI=SI;
		  workRadius=radius[j];
		}
	    }
	}
      
      DI+=10;
    }
  
  Out=ModelSupport::getComposite(SMap,discIndex,DI-10,"1 -1M  ");      
  DI=static_cast<int>(maxIndex)*10+discIndex;

  Out+=ModelSupport::getComposite(SMap,DI," -17 ");      

  addOuterSurf(Out);
  addBoundarySurf(Out);
  
  
  return;
}


void
DiscTarget::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("DiscTarget","createLinks");

  // FixedComp::setLinkSurf(0,-SMap.realSurf(discIndex+1));
  // FixedComp::setLinkSurf(1,SMap.realSurf(discIndex+2));
  // FixedComp::setLinkSurf(2,-SMap.realSurf(discIndex+3));
  // FixedComp::setLinkSurf(3,SMap.realSurf(discIndex+4));
  // FixedComp::setLinkSurf(4,-SMap.realSurf(discIndex+5));
  // FixedComp::setLinkSurf(5,SMap.realSurf(discIndex+6));

  return;
}

void
DiscTarget::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: FixedComp for origin
    \param sideIndex :: Link Point
  */
{
  ELog::RegMethod RegA("DiscTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE bnctSystem
