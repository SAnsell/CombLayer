/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/GuideBox.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"

#include "GuideBox.h"


namespace bibSystem
{

GuideBox::GuideBox(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  guideIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(guideIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

GuideBox::GuideBox(const GuideBox& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  guideIndex(A.guideIndex),cellIndex(A.cellIndex),
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
      cellIndex=A.cellIndex;
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
GuideBox::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Point for link
  */
{
  ELog::RegMethod RegA("GuideBox","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyOffset();

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
  ModelSupport::buildPlane(SMap,guideIndex+1,Origin,Y);     
  ModelSupport::buildPlane(SMap,guideIndex+2,Origin+Y*length,Y);    
  ModelSupport::buildPlane(SMap,guideIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,guideIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,guideIndex+5,Origin-Z*width/2.0,Z);
  ModelSupport::buildPlane(SMap,guideIndex+6,Origin+Z*width/2.0,Z);

  ModelSupport::buildCylinder(SMap,guideIndex+7,Origin,Z,NiRadius);    

  ModelSupport::buildPlane(SMap,guideIndex+13,Origin-
			   X*(width/2.0-NiThickness),X);
  ModelSupport::buildPlane(SMap,guideIndex+14,Origin+
			   X*(width/2.0-NiThickness),X);
  ModelSupport::buildPlane(SMap,guideIndex+15,Origin-
			   Z*(width/2.0-NiThickness),Z);
  ModelSupport::buildPlane(SMap,guideIndex+16,Origin+
			   Z*(width/2.0-NiThickness),Z);

  return; 
}

void
GuideBox::createObjects(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
    \param FC :: FixedComb for boundary surface
    \param sideIndex :: side for boundar
   */
{
  ELog::RegMethod RegA("GuideBox","createObjects");
  
  std::string Out;

  const std::string boundSurf=FC.getLinkString(sideIndex);
  
  Out=ModelSupport::getComposite(SMap,guideIndex," -7 3 -4 5 -6 ");
  Out+=boundSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addBoundarySurf(Out);

  Out=ModelSupport::getComposite(SMap,guideIndex,"1 -2 7 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addBoundaryUnionSurf(Out);

  // Inner boundary surface [For insert devices]


  Out=ModelSupport::getComposite(SMap,guideIndex,
				 "-2 7 3 -4 5 -6 (-13:14:-15:16)");
  Out+=boundSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,guideIndex," -2 3 -4 5 -6 ");
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
  FixedComp::setLinkSurf(0,-SMap.realSurf(guideIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(guideIndex+2));

  FixedComp::setConnect(2,Origin-X*width/2.0+Y*10.0,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(guideIndex+3));

  FixedComp::setConnect(3,Origin+X*width/2.0+Y*10.0,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(guideIndex+4));

  FixedComp::setConnect(4,Origin-Z*height/2.0,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(guideIndex+5));

  FixedComp::setConnect(5,Origin+Z*height/2.0,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(guideIndex+6));

  
  return;
}

void
GuideBox::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int orgIndex,
		    const attachSystem::FixedComp& LimitFC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param orgIndex :: linkPoint for origin
    \param LimitFC :: FixedComponent for hard Limit
    \param sideIndex :: FixedComponent for hard Limit
   */
{
  ELog::RegMethod RegA("GuideBox","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,orgIndex); 
  createSurfaces();
  createObjects(System,LimitFC,sideIndex);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
