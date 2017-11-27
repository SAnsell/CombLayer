/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/GuideShield.cxx
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
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "ReadFunctions.h"
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

#include "GuideShield.h"

namespace bibSystem
{

GuideShield::GuideShield(const std::string& Key,const size_t Index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key+std::to_string(Index),4),
  baseName(Key),
  shieldIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(shieldIndex+1),innerWidth(0.0),innerHeight(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index number of shield unit
  */
{}

GuideShield::GuideShield(const GuideShield& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  baseName(A.baseName),shieldIndex(A.shieldIndex),
  cellIndex(A.cellIndex),innerWidth(A.innerWidth),
  innerHeight(A.innerHeight),nLayers(A.nLayers),
  Height(A.Height),Width(A.Width),Mat(A.Mat)
  /*!
    Copy constructor
    \param A :: GuideShield to copy
  */
{}

GuideShield&
GuideShield::operator=(const GuideShield& A)
  /*!
    Assignment operator
    \param A :: GuideShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      innerWidth=A.innerWidth;
      innerHeight=A.innerHeight;
      nLayers=A.nLayers;
      Height=A.Height;
      Width=A.Width;
      Mat=A.Mat;
    }
  return *this;
}


GuideShield::~GuideShield()
  /*!
    Destructor
  */
{}

void
GuideShield::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("GuideShield","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  Height.clear();
  Width.clear();
  nLayers=Control.EvalPair<size_t>(keyName,baseName,"NLayer");
  double W(innerWidth),H(innerHeight);
  int M;
  for(size_t i=0;i<nLayers;i++)
    {
      W+=Control.EvalPair<double>(keyName,baseName,
				 StrFunc::makeString("Width",i+1));
      H+=Control.EvalPair<double>(keyName,baseName,
				 StrFunc::makeString("Height",i+1));
      M=ModelSupport::EvalMat<int>
	(Control,keyName+StrFunc::makeString("Mat",i+1),
	 baseName+StrFunc::makeString("Mat",i+1));

      Height.push_back(H);
      Width.push_back(W);
      Mat.push_back(M);
    }
  return;
}
  
void
GuideShield::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("protonpipe","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  return;
}
  
void
GuideShield::createSurfaces()
 /*!
   Create All the surfaces
 */
{
  ELog::RegMethod RegA("GuideShield","createSurface");

  // Divider plane
  ModelSupport::buildPlane(SMap,shieldIndex+1,Origin,Y);
  int SI(shieldIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildPlane(SMap,SI+3,Origin-X*(Width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,SI+4,Origin+X*(Width[i]/2.0),X);
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*(Height[i]/2.0),Z);
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*(Height[i]/2.0),Z);
      SI+=10;
    }
  return;
}

void
GuideShield::createObjects(Simulation& System,
			   const attachSystem::ContainedComp* CC,
			   const attachSystem::FixedComp& InnerFC,
			   const long int innerSide,
			   const attachSystem::FixedComp& OuterFC,
			   const long int outerSide)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param CC :: Container for this object
    \param InnerFC :: Inner connection
    \param innerSide :: Index of link point
    \param OuterFC :: Inner connection
    \param outerSide :: Index of link point
  */
{
  ELog::RegMethod RegA("GuideShield","createObjects");

  std::string Out;

  int SI(shieldIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,shieldIndex,SI,
				     "1 3M -4M 5M -6M ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10,"(-3:4:-5:6)");
      else if (CC)
	Out+=CC->getExclude();

      Out+=InnerFC.getLinkString(innerSide);
      Out+=OuterFC.getLinkString(outerSide);

      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat[i],0.0,Out));
      SI+=10;
    }
  if (nLayers)
    {
      Out=ModelSupport::getComposite(SMap,shieldIndex,SI-10,"1 3M -4M 5M -6M");
      addOuterSurf(Out);
      addBoundarySurf(Out);
    }
  
  return;
}

void
GuideShield::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("GuideShield","createLinks");
  if (nLayers)
    {
      const int SI((static_cast<int>(nLayers)-1)*10);
      
      FixedComp::setConnect(0,Origin-X*Width[nLayers-1],-X);
      FixedComp::setLinkSurf(0,-SMap.realSurf(SI+3));
      
      FixedComp::setConnect(1,Origin+X*Width[nLayers-1],X);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+4));

      FixedComp::setConnect(2,Origin-Y*Height[nLayers-1],-Z);
      FixedComp::setLinkSurf(2,-SMap.realSurf(SI+5));
      
      FixedComp::setConnect(3,Origin+Y*Height[nLayers-1],Z);
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+6));
    }

  return;
}

void 
GuideShield::calcInnerDimensions(const attachSystem::FixedComp& GO)
 /*!
   Calculate the inner dimensions of the object 
   \param GO :: Guide object [needs at lease 6 link points]
 */
{
  ELog::RegMethod RegA("GuideShield","calcInnerDimensions");

  innerWidth=GO.getLinkDistance(3,4);
  innerHeight=GO.getLinkDistance(5,6);
  return;
}

void
GuideShield::createAll(Simulation& System,
		       const attachSystem::FixedComp& GO,
		       const attachSystem::FixedComp& InnerFC,
		       const long int innerIndex,
		       const attachSystem::FixedComp& OuterFC,
		       const long int outerIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param GO :: Guide object to wrap
    \param InnerFC :: Inner boundary layer
    \param innerIndex :: Inner Side index
    \param OuterFC :: Outer boundary layer
    \param outerIndex :: Outer Side index
   */
{
  ELog::RegMethod RegA("GuideShield","createAll");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  calcInnerDimensions(GO);
  populate(System);

  createUnitVector(GO);
  const attachSystem::ContainedComp* CC=
    OR.getObject<attachSystem::ContainedComp>(GO.getKeyName());
    
  createSurfaces();
  createObjects(System,CC,InnerFC,innerIndex,
		OuterFC,outerIndex);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE bibSystem
