/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/ConeModerator.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "virtualMod.h"
#include "ConeModerator.h"

namespace delftSystem
{

ConeModerator::ConeModerator(const std::string& Key)  :
  virtualMod(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ConeModerator::ConeModerator(const ConeModerator& A) : 
  virtualMod(A),
  depth(A.depth),length(A.length),innerAngle(A.innerAngle),
  outerAngle(A.outerAngle),fCut(A.fCut),fDepth(A.fDepth),
  alView(A.alView),alBack(A.alBack),faceThick(A.faceThick),
  modTemp(A.modTemp),modMat(A.modMat),alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: ConeModerator to copy
  */
{}

ConeModerator&
ConeModerator::operator=(const ConeModerator& A)
  /*!
    Assignment operator
    \param A :: ConeModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      virtualMod::operator=(A);
      depth=A.depth;
      length=A.length;
      innerAngle=A.innerAngle;
      outerAngle=A.outerAngle;
      fCut=A.fCut;
      fDepth=A.fDepth;
      alView=A.alView;
      alBack=A.alBack;
      faceThick=A.faceThick;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

ConeModerator*
ConeModerator::clone() const
  /*!
    Clone copy constructor
    \return new this
  */
{
  return new ConeModerator(*this); 
}

ConeModerator::~ConeModerator() 
  /*!
    Destructor
  */
{}

void
ConeModerator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database
  */
{
  ELog::RegMethod RegA("ConeModerator","populate");

  FixedRotate::populate(Control);
  
  depth=Control.EvalVar<double>(keyName+"Depth");
  length=Control.EvalVar<double>(keyName+"Length");
  innerAngle=Control.EvalVar<double>(keyName+"InnerAngle");
  outerAngle=Control.EvalVar<double>(keyName+"OuterAngle");
  fCut=Control.EvalVar<double>(keyName+"FrontCut");
  fDepth=Control.EvalVar<double>(keyName+"FrontCut");

  alView=Control.EvalVar<double>(keyName+"AlView");
  alBack=Control.EvalVar<double>(keyName+"AlBack");
  faceThick=Control.EvalVar<double>(keyName+"FaceThick");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  return;
}
    
void
ConeModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConeModerator","createSurfaces");

  ModelSupport::buildCone(SMap,buildIndex+7,Origin-Y*alBack,
			  Y,outerAngle);
  ModelSupport::buildCone(SMap,buildIndex+17,Origin,Y,outerAngle);
  ModelSupport::buildCone(SMap,buildIndex+27,Origin+Y*depth,Y,innerAngle);
  ModelSupport::buildCone(SMap,buildIndex+37,Origin+Y*(depth+alView),
			  Y,innerAngle);
  // Corresponding cut planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(fCut-alBack),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*fCut,Y);

  const double bCut((fCut+fDepth>depth) ? fCut+fDepth : depth);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*bCut,Y);
  ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*(bCut+alView),Y);

  // End surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+faceThick),Y);

  return;
}

void
ConeModerator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("ConeModerator","createLinks");


  return;
}
  
void
ConeModerator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ConeModerator","createObjects");

  HeadRule HR;


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 1 -2 (17:-11)");
  System.addCell(cellIndex++,alMat,modTemp,HR);
  
  // Hydrogne
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 11 -2 (27:-21)");
  System.addCell(cellIndex++,modMat,modTemp,HR);

  // AL layer 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 21 -2 (37:-31)");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  // Mid Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-37 31 -12");
  System.addCell(cellIndex++,0,0.0,HR);

  // Cap :
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 2 -12 37");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 1 -12");
  addOuterSurf(HR);

  return;
}

void
ConeModerator::postCreateWork(Simulation&)
  /*!
    Add pipework
   */
{
  return;
}
  
void
ConeModerator::createAll(Simulation& System,
			 const attachSystem::FixedComp& FUnit,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("ConeModerator","createAll");
  populate(System.getDataBase());

  createCentredUnitVector(FUnit,sideIndex,-fCut);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE delftSystem
