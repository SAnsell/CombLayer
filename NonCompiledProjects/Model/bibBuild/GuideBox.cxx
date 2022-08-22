/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/GuideBox.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "ExternalCut.h"
#include "ContainedComp.h"

#include "GuideBox.h"


namespace bibSystem
{

GuideBox::GuideBox(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::ExternalCut()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

GuideBox::GuideBox(const GuideBox& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::ExternalCut(A),
  width(A.width),height(A.height),length(A.length),
  NiRadius(A.NiRadius),NiThickness(A.NiThickness),mat(A.mat)
  /*!
    Copy constructor
    \param A :: GuideBox to copy
  */
{}

GuideBox&
GuideBox::operator=(const GuideBox& A)
  /*!
    Assignment operator
    \param A :: GuideBox to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      width=A.width;
      height=A.height;
      length=A.length;
      NiRadius=A.NiRadius;
      NiThickness=A.NiThickness;
      mat=A.mat;
    }
  return *this;
}

GuideBox::~GuideBox()
  /*!
    Destructor
  */
{}

void
GuideBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GuideBox","populate");

  attachSystem::FixedOffset::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  length=Control.EvalVar<double>(keyName+"Length");

  NiRadius=Control.EvalVar<double>(keyName+"NiRadius");
  NiThickness=Control.EvalVar<double>(keyName+"NiThickness");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
GuideBox::createSurfaces()
  /*!
    Create surface for the object
  */
  
  /** Creamos las superficies de cada cuerpo*/
{
  ELog::RegMethod RegA("GuideBox","createSurfaces");


  // Match cannot be used since multi-component system [To be fixed]

  // divider surface
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);     
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);    
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*width/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*width/2.0,Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,NiRadius);    

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-
			   X*(width/2.0-NiThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+
			   X*(width/2.0-NiThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-
			   Z*(width/2.0-NiThickness),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+
			   Z*(width/2.0-NiThickness),Z);

  return; 
}

void
GuideBox::createObjects(Simulation& System)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
    \param FC :: FixedComb for boundary surface
    \param sideIndex :: side for boundar
   */
{
  ELog::RegMethod RegA("GuideBox","createObjects");
  
  std::string Out;

  
  const std::string boundSurf=ExternalCut::getRuleStr("Limit");
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 3 -4 5 -6 ");
  Out+=boundSurf;
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addBoundarySurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 7 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addBoundaryUnionSurf(Out);

  // Inner boundary surface [For insert devices]


  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-2 7 3 -4 5 -6 (-13:14:-15:16)");
  Out+=boundSurf;
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6 ");
  Out+=boundSurf;
  addOuterSurf(Out);

  return; 
}

void
GuideBox::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("GuideBox","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin,-Y); 
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*width/2.0+Y*10.0,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*width/2.0+Y*10.0,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*height/2.0,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*height/2.0,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  
  return;
}

void
GuideBox::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param orgIndex :: linkPoint for origin
   */
{
  ELog::RegMethod RegA("GuideBox","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex); 
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
