/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/VanePoison.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "VanePoison.h"

namespace moderatorSystem
{

VanePoison::VanePoison(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  vaneIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vaneIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}
  
VanePoison::VanePoison(const VanePoison& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  vaneIndex(A.vaneIndex),cellIndex(A.cellIndex),
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
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
  
  FixedOffset::populate(Control);
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
VanePoison::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int linkPt)
  /*!
    Create the unit vectors
    - Y Points down the VanePoison direction
    - X Across the VanePoison
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
    \param linkPt :: Link point
  */
{
  ELog::RegMethod RegA("VanePoison","createUnitVector");
  
  FixedComp::createUnitVector(FC,linkPt);
  applyOffset();
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
    ModelSupport::buildPlane(SMap,vaneIndex+11,Origin,Y);

  ModelSupport::buildPlane(SMap,vaneIndex+12,Origin+Y*yLength,Y);
  ModelSupport::buildPlane(SMap,vaneIndex+13,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,vaneIndex+14,Origin+X*width,X);
  if (zLength>0.0)
    {
      ModelSupport::buildPlane(SMap,vaneIndex+15,Origin-Z*zLength,Z);
      ModelSupport::buildPlane(SMap,vaneIndex+16,Origin+Z*zLength,Z);
    }
  
  int surfN(vaneIndex+100);
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
  std::string yFront,yBack,zBase,zTop;

  yFront= (std::abs(yStep)>Geometry::zeroTol) ?
    ModelSupport::getComposite(SMap,vaneIndex," 11 ") :
    FC.getLinkString(linkPt);
  yBack=ModelSupport::getComposite(SMap,vaneIndex," -12 ");

  if (zLength>Geometry::zeroTol)
    {
      zBase=ModelSupport::getComposite(SMap,vaneIndex," 15 ");
      zTop=ModelSupport::getComposite(SMap,vaneIndex," -16 ");
    }
  else
    {
      long int zUp(static_cast<long int>(FC.findLinkAxis(Z)));
      long int zDown(static_cast<long int>(FC.findLinkAxis(-Z)));
      if (zUp<6) zUp+=6;
      if (zDown<6) zDown+=6;
      if (zUp>5)                 // Inner points are reversed
	std::swap(zUp,zDown);      

      zBase=FC.getLinkString(zDown+1);
      zTop=FC.getLinkString(zUp+1);
    }

  std::string Out;
  Out=ModelSupport::getComposite(SMap,vaneIndex," 13 -14 ");
  Out+=zBase+zTop+yBack+yFront;
  addOuterSurf(Out);

  int surfN(vaneIndex+100);
  for(size_t i=0;i<nBlades;i++)
    {
      // inner abs:
      Out=ModelSupport::getComposite(SMap,surfN," 11 -12 ");
      Out+=zBase+zTop+yBack+yFront;
      System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,modTemp,Out));      
      // Blade
      Out=ModelSupport::getComposite(SMap,surfN," 1 -2 (-11:12) ");
      Out+=zBase+zTop+yBack+yFront;
      System.addCell(MonteCarlo::Qhull(cellIndex++,bladeMat,modTemp,Out));      
      
      // Inner moderator material
      if (i)
	{
	  Out=ModelSupport::getComposite(SMap,surfN-100," 2 -101 ");
	  Out+=zBase+zTop+yBack+yFront;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
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
