/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/GuideUnit.cxx 
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "GuideUnit.h"

namespace beamlineSystem
{

GuideUnit::GuideUnit(const std::string& key)  :
  attachSystem::FixedRotate(key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  resetYRotation(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: keyName
  */
{}

GuideUnit::GuideUnit(const GuideUnit& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::FrontBackCut(A),
  resetYRotation(A.resetYRotation),
  begPt(A.begPt),endPt(A.endPt),length(A.length),
  nShapeLayers(A.nShapeLayers),layerThick(A.layerThick),
  layerMat(A.layerMat)
  /*!
    Copy constructor
    \param A :: GuideUnit to copy
  */
{}

GuideUnit&
GuideUnit::operator=(const GuideUnit& A)
  /*!
    Assignment operator
    \param A :: GuideUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      resetYRotation=A.resetYRotation;
      begPt=A.begPt;
      endPt=A.endPt;
      length=A.length;
      nShapeLayers=A.nShapeLayers;
      layerThick=A.layerThick;
      layerMat=A.layerMat;
    }
  return *this;
}

GuideUnit::~GuideUnit() 
  /*!
    Destructor
   */
{}

void
GuideUnit::populate(const FuncDataBase& Control)
  /*!
    Population of variables
    \param Control :: Database for variables
  */
{
  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  nShapeLayers=Control.EvalVar<size_t>(keyName+"NShapeLayers");
  
  double T(0.0);
  int M;
  for(size_t i=0;i<nShapeLayers;i++)
    {
      const std::string NStr=std::to_string(i);
      // Always get material
      M=ModelSupport::EvalMat<int>(Control,keyName+"LayerMat"+NStr);
      layerMat.push_back(M);
      if (i)
	{
	  T=Control.EvalVar<double>(keyName+"LayerThick"+NStr);
	  layerThick.push_back(T);
	}
    }
  layerThick.push_back(0.0);

  // set frontcut based on offset:
  //  const FixedRotate& get
  //  if (!frontActive())
  //    beamFrontCut=(std::abs<double>(beamYStep)>Geometry::zeroTol) ? 1 : 0;

  return;
}
  
void
GuideUnit::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("VacuumBox","createLinks");

  if (resetYRotation)
    applyAngleRotate(0,-yAngle,0);

  FrontBackCut::createFrontLinks(*this,Origin,-Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  

  return;
}
  
void
GuideUnit::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GuideUnit","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);
  
  return;
}
 
}  // NAMESPACE beamlineSystem
