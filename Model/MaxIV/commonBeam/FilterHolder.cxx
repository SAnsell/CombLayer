/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/FilterHolder.cxx
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
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "FilterHolder.h"

namespace xraySystem
{

FilterHolder::FilterHolder(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FilterHolder::FilterHolder(const FilterHolder& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  thick(A.thick),width(A.width),height(A.height),
  depth(A.depth),
  legWidth(A.legWidth),
  legHeight(A.legHeight),
  baseWidth(A.baseWidth),
  baseHeight(A.baseHeight),
  foilThick(A.foilThick),
  foilMat(A.foilMat),
  nWindows(A.nWindows),
  wWidth(A.wWidth),
  wHeight(A.wHeight),
  wDepth(A.wDepth),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: FilterHolder to copy
  */
{}

FilterHolder&
FilterHolder::operator=(const FilterHolder& A)
  /*!
    Assignment operator
    \param A :: FilterHolder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      thick=A.thick;
      width=A.width;
      height=A.height;
      depth=A.depth;
      legWidth=A.legWidth;
      legHeight=A.legHeight;
      baseWidth=A.baseWidth;
      baseHeight=A.baseHeight;
      foilThick=A.foilThick;
      foilMat=A.foilMat;
      nWindows=A.nWindows;
      wWidth=A.wWidth;
      wHeight=A.wHeight;
      wDepth=A.wDepth;
      mat=A.mat;
    }
  return *this;
}

FilterHolder*
FilterHolder::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FilterHolder(*this);
}

FilterHolder::~FilterHolder()
  /*!
    Destructor
  */
{}

void
FilterHolder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FilterHolder","populate");

  FixedOffset::populate(Control);

  thick=Control.EvalVar<double>(keyName+"Thick");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  legWidth=Control.EvalVar<double>(keyName+"LegWidth");
  legHeight=Control.EvalVar<double>(keyName+"LegHeight");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseHeight=Control.EvalVar<double>(keyName+"BaseHeight");
  foilThick=Control.EvalVar<double>(keyName+"FoilThick");
  foilMat=ModelSupport::EvalMat<int>(Control,keyName+"FoilMat");
  nWindows=Control.EvalVar<size_t>(keyName+"NWindows");
  wWidth=Control.EvalVar<double>(keyName+"WindowWidth");
  wHeight=Control.EvalVar<double>(keyName+"WindowHeight");
  wDepth=Control.EvalVar<double>(keyName+"WindowDepth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
FilterHolder::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FilterHolder","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
FilterHolder::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FilterHolder","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(thick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(thick/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height),Z);

  // leg
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(legWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(legWidth/2.0),X);

  // base
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(baseWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(depth+legHeight+baseHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin-Z*(depth+legHeight),Z);

  // filter
  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,
				  SMap.realPtr<Geometry::Plane>(buildIndex+1),
				  -foilThick);

  // windows
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*(wDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*(wHeight),Z);

  const double dividerWidth((width-wWidth*nWindows)/(nWindows+1));
  double dx(0.0);
  int SI = buildIndex+40;
  SMap.addMatch(SI-10+4,SMap.realSurf(buildIndex+3));

  for (size_t i=0; i<nWindows; i++)
    {
      dx += dividerWidth;
      ModelSupport::buildPlane(SMap,SI+3,
			       Origin-X*(width/2.0-dx),X);
      dx += wWidth;
      ModelSupport::buildPlane(SMap,SI+4,
			       Origin-X*(width/2.0-dx),X);

      SI += 10;
    }
  SMap.addMatch(SI+3,SMap.realSurf(buildIndex+4));

  return;
}

void
FilterHolder::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FilterHolder","createObjects");

  std::string Out, side;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -45 ");
  makeCell("MainLow",System,cellIndex++,mat,0.0,Out);

  // windows
  side=ModelSupport::getComposite(SMap,buildIndex," 1 -2 45 -46 ");
  int SI(buildIndex+40);
  for (size_t i=0; i<=nWindows; i++)
    {
      Out=ModelSupport::getComposite(SMap,SI-10,SI," 4 -3M ");
      System.addCell(cellIndex++,mat,0,Out+side);
      
      if (i!=nWindows)
	{
	  Out=ModelSupport::getComposite(SMap,SI," 3 -4 ");
	  System.addCell(cellIndex++,0,0,Out+side);
	}
      
      SI += 10;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 46 -6 ");
  makeCell("MainUp",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 23 -3 5 -6 ");
  makeCell("MainVoid1",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 4 -24 5 -6 ");
  makeCell("MainVoid2",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 26 -5 ");
  makeCell("Leg",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 23 -13 26 -5 ");
  makeCell("LegVoid1",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 14 -24 26 -5 ");
  makeCell("LegVoid2",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 23 -24 25 -26 ");
  makeCell("Base",System,cellIndex++,mat,0.0,Out);

  // filter
  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -1 3 -4 5 -6 ");
  makeCell("Filter",System,cellIndex++,foilMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -1 23 -3 5 -6 ");
  makeCell("FilterVoid1",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -1 4 -24 5 -6 ");
  makeCell("FilterVoid2",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -1 23 -24 25 -5 ");
  makeCell("FilterVoid2",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -2 23 -24 25 -6 ");
  addOuterSurf(Out);

  return;
}


void
FilterHolder::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FilterHolder","createLinks");

  FixedComp::setConnect(0,Origin-Y*(thick/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(thick/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(depth),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
FilterHolder::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FilterHolder","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
