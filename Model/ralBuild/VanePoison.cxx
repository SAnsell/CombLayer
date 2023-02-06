/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuild/VanePoison.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Exception.h"
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
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "VanePoison.h"

namespace moderatorSystem
{

VanePoison::VanePoison(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}
  
VanePoison::VanePoison(const VanePoison& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  nBlades(A.nBlades),bWidth(A.bWidth),absThick(A.absThick),
  bGap(A.bGap),yLength(A.yLength),zLength(A.zLength),
  modTemp(A.modTemp),modMat(A.modMat),bladeMat(A.bladeMat),
  absMat(A.absMat)
  /*!
    Copy constructor
    \param A :: VanePoison to copy
  */
{}

VanePoison&
VanePoison::operator=(const VanePoison& A)
  /*!
    Assignment operator
    \param A :: VanePoison to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      nBlades=A.nBlades;
      bWidth=A.bWidth;
      absThick=A.absThick;
      bGap=A.bGap;
      yLength=A.yLength;
      zLength=A.zLength;
      modTemp=A.modTemp;
      modMat=A.modMat;
      bladeMat=A.bladeMat;
      absMat=A.absMat;
    }
  return *this;
}

VanePoison::~VanePoison() 
/*!
  Destructor
*/
{}

void
VanePoison::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DtaBase
  */
{
  ELog::RegMethod RegA("VanePoison","populate");
  
  FixedRotate::populate(Control);
  nBlades=Control.EvalVar<size_t>(keyName+"NBlades");

  bGap=Control.EvalVar<double>(keyName+"BladeGap");
  bWidth=Control.EvalVar<double>(keyName+"BladeWidth");
  absThick=Control.EvalVar<double>(keyName+"AbsThick");
  yLength=Control.EvalVar<double>(keyName+"YLength");
  zLength=Control.EvalVar<double>(keyName+"ZLength");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  absMat=ModelSupport::EvalMat<int>(Control,keyName+"AbsMat");
  
  return;
}
  
void
VanePoison::createLinks()
  /*!
    Construct FixedObject boundary
  */
{
  ELog::RegMethod RegA("VanePoison","createLinks");

  return;
}
  
void
VanePoison::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("VanePoison","createSurface");

  // Surfaces 1-6 are the FC container
  // Surfaces 11-16 are the outer blades etc

  // Each blade starts at 101
  

  // First calc half width
  const double width=(static_cast<double>(nBlades-1)*bGap+
		      static_cast<double>(nBlades)*bWidth)/2.0;
  
  if (std::abs(yStep)>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*yLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*width,X);
  if (zLength>0.0)
    {
      ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*zLength,Z);
      ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*zLength,Z);
      setCutSurf("Base",SMap.realSurf(buildIndex+15));
      setCutSurf("Top",-SMap.realSurf(buildIndex+16));
    }
  
  int surfN(buildIndex+100);
  Geometry::Vec3D BCent(Origin-X*(width-bWidth/2.0));
  const Geometry::Vec3D GCent(Origin+X*(width+bGap/2.0));
  for(size_t i=0;i<nBlades;i++)
    {
      // outer
      ModelSupport::buildPlane(SMap,surfN+1,BCent-X*bWidth/2.0,X);      
      ModelSupport::buildPlane(SMap,surfN+2,BCent+X*bWidth/2.0,X);      
      ModelSupport::buildPlane(SMap,surfN+11,BCent-X*absThick/2.0,X);      
      ModelSupport::buildPlane(SMap,surfN+12,BCent+X*absThick/2.0,X);      
      surfN+=100;
      BCent+=X*(bGap+bWidth);

    }

  return;
}

void
VanePoison::createObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int linkPt)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param FC :: Fixed Component for links
    \param linkPt :: Link direction
  */
{
  ELog::RegMethod RegA("VanePoison","createObjects");

  if (!isActive("Base") || !isActive("Top"))
    throw ColErr::InContainerError<std::string>
      ("Base/Top","ExternalCut Base/Top not set");

  const HeadRule zBase=getRule("Base");
  const HeadRule zTop=getRule("Top");
  HeadRule yFront,yBack;

  HeadRule HR;
  yFront= (std::abs(yStep)>Geometry::zeroTol) ?
    HeadRule(SMap,buildIndex,11) :  FC.getFullRule(linkPt);
  yBack=HeadRule(SMap,buildIndex,-12);

  const HeadRule combineHR=zTop*yBack*yFront*zBase;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14");
  addOuterSurf(HR*combineHR);

  int surfN(buildIndex+100);
  for(size_t i=0;i<nBlades;i++)
    {
      // inner abs:
      HR=ModelSupport::getHeadRule(SMap,surfN,"11 -12");
      System.addCell(cellIndex++,absMat,modTemp,HR*combineHR);      
      // Blade
      HR=ModelSupport::getHeadRule(SMap,surfN,"1 -2 (-11:12)");
      System.addCell(cellIndex++,bladeMat,modTemp,HR*combineHR);      
      
      // Inner moderator material
      if (i)
	{
	  HR=ModelSupport::getHeadRule(SMap,surfN-100,"2 -101");
	  System.addCell(cellIndex++,modMat,modTemp,HR*combineHR);
	}
      surfN+=100;
    }
  return;
}

  
void
VanePoison::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int linkIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: link system
    \param linkIndex :: linked index
  */
{
  ELog::RegMethod RegA("VanePoison","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,linkIndex);
  if (nBlades)
    {
      createSurfaces();
      createObjects(System,FC,linkIndex);
      createLinks();
      insertObjects(System);       
    }
  return;
}
  
}  // NAMESPACE moderatorSystem
