/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/GuideShield.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "ExternalCut.h"
#include "ContainedComp.h"

#include "GuideShield.h"

namespace bibSystem
{

GuideShield::GuideShield(const std::string& Key,const size_t Index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key+std::to_string(Index),4),
  attachSystem::ExternalCut(),
  baseName(Key),innerWidth(0.0),innerHeight(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index number of shield unit
  */
{}

GuideShield::GuideShield(const GuideShield& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  attachSystem::ExternalCut(A),
  baseName(A.baseName),innerWidth(A.innerWidth),
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
      attachSystem::ExternalCut::operator=(A);
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
GuideShield::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("GuideShield","populate");

  Height.clear();
  Width.clear();
  nLayers=Control.EvalTail<size_t>(keyName,baseName,"NLayer");
  double W(innerWidth),H(innerHeight);
  int M;
  for(size_t i=0;i<nLayers;i++)
    {
      W+=Control.EvalTail<double>
	(keyName,baseName,"Width"+std::to_string(i+1));
      H+=Control.EvalTail<double>
	(keyName,baseName,"Height"+std::to_string(i+1));
      M=ModelSupport::EvalMat<int>
	(Control,keyName+"Mat"+std::to_string(i+1),
	 baseName+"Mat"+std::to_string(i+1));

      Height.push_back(H);
      Width.push_back(W);
      Mat.push_back(M);
    }
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
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  int SI(buildIndex);
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
GuideShield::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GuideShield","createObjects");

  std::string Out;

  const std::string inner=ExternalCut::getRuleStr("Inner");
  const std::string front=ExternalCut::getRuleStr("Front");
  const std::string back=ExternalCut::getRuleStr("Back");
  
  int SI(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,SI,
				     "1 3M -4M 5M -6M ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10,"(-3:4:-5:6)");
      else 
	Out+=inner;  // CC->getExclude();

      Out+=front; // InnerFC.getLinkString(innerSide);
      Out+=back; // OuterFC.getLinkString(outerSide);

      System.addCell(MonteCarlo::Object(cellIndex++,Mat[i],0.0,Out));
      SI+=10;
    }
  if (nLayers)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,SI-10,"1 3M -4M 5M -6M");
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
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param GO :: Guide object to wrap
   */
{
  ELog::RegMethod RegA("GuideShield","createAll");

  calcInnerDimensions(GO);
  populate(System.getDataBase());

  createUnitVector(GO,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE bibSystem
