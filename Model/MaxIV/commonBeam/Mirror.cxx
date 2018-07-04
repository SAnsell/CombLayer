/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/Mirror.cxx
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
#include "support.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "Mirror.h"


namespace xraySystem
{

Mirror::Mirror(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


Mirror::~Mirror()
  /*!
    Destructor
   */
{}

void
Mirror::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("Mirror","populate");

  FixedOffset::populate(Control);

  theta=Control.EvalVar<double>(keyName+"Theta");
  phi=Control.EvalDefVar<double>(keyName+"Phi",0.0);
  
  radius=Control.EvalDefVar<double>(keyName+"Radius",0.0);
  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  length=Control.EvalVar<double>(keyName+"Length");
  
  baseTop=Control.EvalVar<double>(keyName+"BaseTop");
  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");
  baseOutWidth=Control.EvalVar<double>(keyName+"BaseOutWidth");
  baseGap=Control.EvalVar<double>(keyName+"BaseGap");

  mirrMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}

void
Mirror::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("Mirror","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
Mirror::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("Mirror","createSurfaces");

  // main xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(-theta,X));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PY);
  QXA.rotate(PZ);

  const Geometry::Quaternion QYA
    (Geometry::Quaternion::calcQRotDeg(phi,PY));

  QYA.rotate(PX);
  QYA.rotate(PZ);
  
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-PX*(width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+PX*(width/2.0),PX);

  if (std::abs(radius)>Geometry::zeroTol)
    {
      // calc edge cut
      const double tAngle = length/(2.0*radius);  // cos(-a) == cos(a)
      const double lift = radius*(1.0-cos(tAngle));
      if (radius<0)
	ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*lift,-PZ);
      else
	ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*lift,PZ);
      ModelSupport::buildCylinder(SMap,buildIndex+107,Origin-PZ*radius,PX,std::abs(radius));
      ModelSupport::buildCylinder(SMap,buildIndex+117,Origin-PZ*radius,PX,std::abs(radius)+thick);
    }
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*thick,PZ);
      ModelSupport::buildPlane(SMap,buildIndex+106,Origin,PZ);
    }
  

  // support
  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin-PX*(baseOutWidth+width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   Origin+PX*(baseOutWidth+width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin+PZ*baseTop,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin-PZ*baseDepth,PZ);


  ModelSupport::buildPlane(SMap,buildIndex+216,Origin-PZ*(thick+baseGap),PZ);

  return; 
}

void
Mirror::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("Mirror","createObjects");

  std::string Out;
  // xstal
  if (std::abs(radius)<Geometry::zeroTol)
    Out=ModelSupport::getComposite(SMap,buildIndex,
				   " 101 -102 103 -104 105 -106 ");
  else
    Out=ModelSupport::getComposite
      (SMap,buildIndex," 103 -104 107 -117 105 ");    
  
  makeCell("Mirror",System,cellIndex++,mirrMat,0.0,Out);

  // Make sides
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -103 203 -205 206 ");
  makeCell("LeftSide",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 104 -204 -205 206 ");
  makeCell("RightSide",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 101 -102 103 -104 -216 206 ");
  makeCell("Base",System,cellIndex++,baseMat,0.0,Out);

  // vacuum units:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 -105 216" ); 
  makeCell("BaseVac",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 -205 106 " ); 
  makeCell("TopVac",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 203 -204 -205 206" ); 
  addOuterSurf(Out);
  
  return; 
}

void
Mirror::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("Mirror","createLinks");
  
  return;
}

void
Mirror::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("Mirror","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
