/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/EdgeWater.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <array>
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "AttachSupport.h"
#include "geomSupport.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "EdgeWater.h"

namespace essSystem
{

EdgeWater::EdgeWater(const std::string& key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(key,6),
  attachSystem::CellMap(),
  edgeIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(edgeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: Name for item in search

  */
{}

EdgeWater::EdgeWater(const EdgeWater& A) :
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  edgeIndex(A.edgeIndex),cellIndex(A.cellIndex),
  width(A.width),wallThick(A.wallThick),midWallThick(A.midWallThick),
  midWallLength(A.midWallLength),
  cutAngle(A.cutAngle),cutWidth(A.cutWidth),preThick(A.preThick),
  modMat(A.modMat),
  preMat(A.preMat),
  wallMat(A.wallMat),modTemp(A.modTemp),preTemp(A.preTemp),
  sideRule(A.sideRule)

  /*!
    Copy constructor
    \param A :: EdgeWater to copy
  */
{}

EdgeWater&
EdgeWater::operator=(const EdgeWater& A)
  /*!
    Assignment operator
    \param A :: EdgeWater to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      wallThick=A.wallThick;
      midWallThick=A.midWallThick;
      midWallLength=A.midWallLength;
      cutAngle=A.cutAngle;
      cutWidth=A.cutWidth;
      preThick=A.preThick;
      modMat=A.modMat;
      preMat=A.preMat;
      wallMat=A.wallMat;
      modTemp=A.modTemp;
      preTemp=A.preTemp;
      sideRule=A.sideRule;
    }
  return *this;
}

EdgeWater*
EdgeWater::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
   */
{
  return new EdgeWater(*this);
}

EdgeWater::~EdgeWater()
  /*!
    Destructor
  */
{}

void
EdgeWater::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EdgeWater","populate");

  width=Control.EvalVar<double>(keyName+"Width");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  midWallThick=Control.EvalVar<double>(keyName+"MidWallThick");
  if (midWallThick>Geometry::zeroTol)
    midWallLength=Control.EvalVar<double>(keyName+"MidWallLength");
  else
    midWallLength=0.0;

  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");
  cutWidth=Control.EvalVar<double>(keyName+"CutWidth");
  preThick=Control.EvalDefVar<double>(keyName+"PreThick", 0.0);

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  preMat=ModelSupport::EvalMat<int>(Control,keyName+"PreMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  preTemp=Control.EvalVar<double>(keyName+"PreTemp");

  return;
}

void
EdgeWater::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the EdgeWater direction
    - X Across the EdgeWater
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
    \param sideIndex :: Link direction
  */
{
  ELog::RegMethod RegA("EdgeWater","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  return;
}

void
EdgeWater::createLinks()
  /*!
    Construct links for the triangle moderator
    The normal 1-3 and 5-6 are plane,
    7,8,9 are
  */
{
  ELog::RegMethod RegA("EdgeWater","createLinks");

  // Loop over corners that are bound by convex

  return;
}



void
EdgeWater::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EdgeWater","createSurface");

  // Only x surfaces:

  ModelSupport::buildPlane(SMap,edgeIndex+1,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,edgeIndex+2,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,edgeIndex+11,
			   Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,edgeIndex+12,
			   Origin+X*(wallThick+width/2.0),X);

  // mid wall
  ModelSupport::buildPlane(SMap,edgeIndex+21,Origin-X*(midWallThick/2.0),X);
  ModelSupport::buildPlane(SMap,edgeIndex+22,Origin+X*(midWallThick/2.0),X);
  ModelSupport::buildPlane(SMap,edgeIndex+23,Origin+Y*(midWallLength),Y);


  // front dividers:
  const Geometry::Vec3D EdPtA=Origin-X*(cutWidth/2.0);
  const Geometry::Vec3D EdPtB=Origin+X*(cutWidth/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,edgeIndex+103,EdPtA,X,Z,cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,edgeIndex+104,EdPtB,X,Z,-cutAngle);

  ModelSupport::buildPlaneRotAxis(SMap,edgeIndex+203,EdPtA-X*wallThick,
				  X,Z,cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,edgeIndex+204,EdPtB+X*wallThick,
				  X,Z,-cutAngle);

  return;
}

void
EdgeWater::createObjects(Simulation& System,
			 const std::string& divider,
			 const std::string& container)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
    \param divider :: surface on lobe
    \param container string :: wing surface edge of reflector
  */
{
  ELog::RegMethod RegA("EdgeWater","createObjects");

  std::string Out;

  if (midWallThick>Geometry::zeroTol) // build the wall in the middle
    {
      Out=ModelSupport::getComposite(SMap,edgeIndex," 1 -21 103 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
				       modTemp,Out+container+divider));

      Out=ModelSupport::getComposite(SMap,edgeIndex," 21 -22 -23 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
				       modTemp,Out+container+divider));
      Out=ModelSupport::getComposite(SMap,edgeIndex," 21 -22 23 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,
				       modTemp,Out+container+divider));


      Out=ModelSupport::getComposite(SMap,edgeIndex," 22 -2 -104 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
				       modTemp,Out+container+divider));
    } else // no middle wall
    {
      if (preThick>Geometry::zeroTol)
	{
	  HeadRule HR(divider);
	  Geometry::Plane *surfDivider = dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(HR.getPrimarySurface()));
	  if (Origin.Y()>0)
	    ModelSupport::buildShiftedPlaneReversed(SMap,edgeIndex+302,surfDivider,
						    -preThick);
	  else
	    ModelSupport::buildShiftedPlane(SMap,edgeIndex+302,surfDivider,
					    -preThick);
	  Out=ModelSupport::getComposite(SMap,edgeIndex," 1 -2 103 -104 -302");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
					   modTemp,Out+container+divider));
	  Out=ModelSupport::getComposite(SMap,edgeIndex," 1 -2 103 -104 302");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,preMat,
					   preTemp,Out+container+divider));
	} else // no premoderator needed
	{
	  Out=ModelSupport::getComposite(SMap,edgeIndex," 1 -2 103 -104");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
					   modTemp,Out+container+divider));
	}
    }

  //  CellMap::setCell("Water",  cellIndex-1);
  // Two walls : otherwise divider container
  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -1 103 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out+container));
  CellMap::addCell("Wall",  cellIndex-1);
  Out=ModelSupport::getComposite(SMap,edgeIndex," 2 -12 -104 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out+container));
  CellMap::addCell("Wall",  cellIndex-1);

  // front walls
  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -103 203 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,
				   Out+container+divider));
  CellMap::addCell("Wall",  cellIndex-1);
  CellMap::setCell("InnerAlSupply",  cellIndex-1);
  Out=ModelSupport::getComposite(SMap,edgeIndex," -12 104 -204");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,
				   Out+container+divider));
  CellMap::addCell("Wall",  cellIndex-1);

  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -12 203 -204") + divider;
  addOuterSurf(Out);
  sideRule = Out;
  return;
}



Geometry::Vec3D
EdgeWater::getSurfacePoint(const size_t,
			const long int) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
EdgeWater::getLayerSurf(const size_t ,
			const long int ) const
  /*!
    Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-3]
    \param  :: Side [0-3] // mid sides
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
EdgeWater::getLayerString(const size_t,
			  const long int) const
  /*!
    Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getLayerString");

  throw ColErr::AbsObjMethod("Not implemented yet");

}


void
EdgeWater::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex,
		     const std::string& container)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param sideIndex :: link point to create on
    \param container :: bounding edge -- replace with headrule.
  */
{
  ELog::RegMethod RegA("EdgeWater","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  const std::string divider=FC.getSignedLinkString(sideIndex);
  createObjects(System,divider,container);

  createLinks();
  insertObjects(System);
  return;
}

}  // NAMESPACE essSystem
