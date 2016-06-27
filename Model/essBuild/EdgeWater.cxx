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
#include "SurInter.h"

namespace essSystem
{

EdgeWater::EdgeWater(const std::string& key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(key,6),
  edgeIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(edgeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base Name for item in search
    \param extraKey :: extra [specialized] Name for item in search
  */
{}

EdgeWater::EdgeWater(const EdgeWater& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),attachSystem::FixedComp(A),
  edgeIndex(A.edgeIndex),cellIndex(A.cellIndex),
  width(A.width),wallThick(A.wallThick),
  cutAngle(A.cutAngle),cutThick(A.cutThick),
  modMat(A.modMat),wallMat(A.wallMat),modTemp(A.modTemp)
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
      cellIndex=A.cellIndex;
      width=A.width;
      wallThick=A.wallThick;
      cutAngle=A.cutAngle;
      cutThick=A.cutThick;
      modMat=A.modMat;
      wallMat=A.wallMat;
      modTemp=A.modTemp;
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

  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");
  cutThick=Control.EvalVar<double>(keyName+"CutThick");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  
  return;
}
  
void
EdgeWater::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points down the EdgeWater direction
    - X Across the EdgeWater
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
  */
{
  ELog::RegMethod RegA("EdgeWater","createUnitVector");

  FixedComp::createUnitVector(FC);
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
EdgeWater::createSurfaces(const std::string &divider)
  /*!
    Create All the surfaces
    \param :: divider - Lobe side surface
  */
{
  ELog::RegMethod RegA("EdgeWater","createSurface");

  // Only Y surfaces:

  ModelSupport::buildPlane(SMap,edgeIndex+1,Origin-Y*(width/2.0),Y);
  ModelSupport::buildPlane(SMap,edgeIndex+2,Origin+Y*(width/2.0),Y);

  ModelSupport::buildPlane(SMap,edgeIndex+11,
			   Origin-Y*(wallThick+width/2.0),Y);
  ModelSupport::buildPlane(SMap,edgeIndex+12,
			   Origin+Y*(wallThick+width/2.0),Y);
  
  // Auxiliary surfaces for the cut area
  Geometry::Plane *pz = ModelSupport::buildPlane(SMap,edgeIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,edgeIndex+101,Origin,Y);

  HeadRule HR(divider);
  const Geometry::Plane *sDiv = SMap.realPtr<Geometry::Plane>(HR.getSurfaceNumbers().front());
  const Geometry::Vec3D nDiv = sDiv->getNormal();
  const int nSign = nDiv.dotProd(X) > 0 ? 1 : -1;

  Geometry::Vec3D pt11 = SurInter::getPoint(sDiv, SMap.realPtr<Geometry::Plane>(edgeIndex+101), pz)-Y*cutThick/2.0;
  Geometry::Vec3D pt12 = SurInter::getPoint(sDiv, SMap.realPtr<Geometry::Plane>(edgeIndex+101), pz)+Y*cutThick/2.0;

  Geometry::Plane *p13 = ModelSupport::buildPlaneRotAxis
    (SMap, edgeIndex+13, pt11, nDiv, Z*nSign, cutAngle);

  Geometry::Plane *p14 = ModelSupport::buildPlaneRotAxis
    (SMap, edgeIndex+14, pt12, nDiv, Z*nSign, -cutAngle);

  ModelSupport::buildShiftedPlane(SMap, edgeIndex+23, p13, -wallThick);
  ModelSupport::buildShiftedPlane(SMap, edgeIndex+24, p14, -wallThick);
  
  return;
}
 
void
EdgeWater::createObjects(Simulation& System,
			 const std::string& divider,
			 const std::string& container)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
    \param :: divider - Lobe side surface
    \param container string :: wing surface ege
  */
{
  ELog::RegMethod RegA("EdgeWater","createObjects");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,edgeIndex," 1 -2 13 14");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,
  				   modTemp,Out+container+divider));
  
  // Two walls : otherwise divider container
  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -1 13");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,
				   modTemp,Out+container)); // \todo: remove outer BeRef cylinder from this rule
  // inclined wall
  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -13 23");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,
				   modTemp,Out+container+divider));

  Out=ModelSupport::getComposite(SMap,edgeIndex," 2 -12 14");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,
				   modTemp,Out+container)); // \todo: remove outer BeRef cylinder from this rule
  // inclined wall
  Out=ModelSupport::getComposite(SMap,edgeIndex," -12 -14 24");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,
				   modTemp,Out+container+divider));
  
  Out=ModelSupport::getComposite(SMap,edgeIndex," 11 -12 23 24");
  addOuterSurf(Out+divider);
  sideRule = Out+divider;
  return;
}


  
Geometry::Vec3D
EdgeWater::getSurfacePoint(const size_t,
			   const size_t) const
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
			const size_t ) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-3]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
EdgeWater::getLayerString(const size_t,
			  const size_t) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getLayerString");

  throw ColErr::AbsObjMethod("Not implemented yet");

}


void
EdgeWater::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const std::string& divider,
		     const std::string& container)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param cutString :: external surface cut
  */
{
  ELog::RegMethod RegA("EdgeWater","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces(divider);
  createObjects(System,divider,container);

  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
