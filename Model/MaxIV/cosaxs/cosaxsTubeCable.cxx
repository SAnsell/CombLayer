/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/cosaxsTubeCable.cxx
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "cosaxsTubeCable.h"

namespace xraySystem
{

cosaxsTubeCable::cosaxsTubeCable(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

cosaxsTubeCable::cosaxsTubeCable(const cosaxsTubeCable& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  zStep(A.zStep),
  tailRadius(A.tailRadius),detYStep(A.detYStep),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: cosaxsTubeCable to copy
  */
{}

cosaxsTubeCable&
cosaxsTubeCable::operator=(const cosaxsTubeCable& A)
  /*!
    Assignment operator
    \param A :: cosaxsTubeCable to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      zStep=A.zStep;
      tailRadius=A.tailRadius;
      detYStep=A.detYStep;
      mat=A.mat;
    }
  return *this;
}

cosaxsTubeCable*
cosaxsTubeCable::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new cosaxsTubeCable(*this);
}

cosaxsTubeCable::~cosaxsTubeCable()
  /*!
    Destructor
  */
{}

void
cosaxsTubeCable::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  tailRadius=Control.EvalVar<double>(keyName+"TailRadius");
  detYStep=Control.EvalVar<double>(keyName+"DetYStep");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
cosaxsTubeCable::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
cosaxsTubeCable::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));

    }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0-zStep),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0+zStep),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+15,
   				  SMap.realPtr<Geometry::Plane>(buildIndex+5),
				  -height);

  const double tailLength(3*M_PI/4*tailRadius);
  const double bottomLength(detYStep/2.0);
  const double upLength(length-tailLength-bottomLength-detYStep);

  // centre of cylinders
  const double yTail(detYStep+upLength+tailRadius);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*yTail,Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+111,
  				  SMap.realPtr<Geometry::Plane>(buildIndex+102),
  				  -bottomLength);

  ModelSupport::buildCylinder(SMap,buildIndex+107,
  			      Origin+Y*yTail+Z*zStep,X,
  			      tailRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,
  			      Origin+Y*yTail+Z*zStep,X,
  			      tailRadius+height);

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(yTail+tailRadius+height+1),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  return;
}

void
cosaxsTubeCable::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","createObjects");

  std::string Out;
  const std::string frontStr(frontRule());
  const std::string backStr(backRule());

  std::string side(ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 "));
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -102 107 ")+side;
  makeCell("Cable1",System,cellIndex++,mat,0.0,frontStr+Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -107 ");
  makeCell("CableTailInner",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -102 6 107 -117 ");
  makeCell("CableTail1",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 102 107 -117 ");
  makeCell("CableTail2",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 15 -5 102 -117 ");
  makeCell("CableTail3",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 15 -5 -111 ");
  makeCell("CableBotVoid",System,cellIndex++,0,0.0,Out+frontStr);

  if (detYStep>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 15 -5 111 -102 ");
      makeCell("CableBotCable",System,cellIndex++,mat,0.0,Out+frontStr);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 (15 -6 (-102 : 102 -117) : 6 -117 ) ");
  addOuterSurf(Out);

  return;
}


void
cosaxsTubeCable::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0-zStep),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0+zStep),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
cosaxsTubeCable::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTubeCable","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
