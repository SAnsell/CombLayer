/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/ConeModerator.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "virtualMod.h"
#include "ConeModerator.h"

namespace delftSystem
{

ConeModerator::ConeModerator(const std::string& Key)  :
  virtualMod(Key),
  coneIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(coneIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ConeModerator::ConeModerator(const ConeModerator& A) : 
  virtualMod(A),
  coneIndex(A.coneIndex),cellIndex(A.cellIndex),
  depth(A.depth),length(A.length),innerAngle(A.innerAngle),
  outerAngle(A.outerAngle),fCut(A.fCut),fDepth(A.fDepth),
  alView(A.alView),alBack(A.alBack),faceThick(A.faceThick),
  modTemp(A.modTemp),modMat(A.modMat),alMat(A.alMat),
  HCell(A.HCell)
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
      cellIndex=A.cellIndex;
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
      HCell=A.HCell;
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


  FixedOffset::populate(Control);
  
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
ConeModerator::createUnitVector(const attachSystem::FixedComp& CUnit,
				const long int sideIndex)
  /*!
    Create the unit vectors
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("ConeModerator","createUnitVector");
  
  FixedComp::createUnitVector(CUnit,sideIndex);
  yStep-=fCut;
  applyOffset();
  return;
}

  
void
ConeModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConeModerator","createSurfaces");

  ModelSupport::buildCone(SMap,coneIndex+7,Origin-Y*alBack,
			  Y,outerAngle);
  ModelSupport::buildCone(SMap,coneIndex+17,Origin,Y,outerAngle);
  ModelSupport::buildCone(SMap,coneIndex+27,Origin+Y*depth,Y,innerAngle);
  ModelSupport::buildCone(SMap,coneIndex+37,Origin+Y*(depth+alView),
			  Y,innerAngle);
  // Corresponding cut planes
  ModelSupport::buildPlane(SMap,coneIndex+1,Origin+Y*(fCut-alBack),Y);
  ModelSupport::buildPlane(SMap,coneIndex+11,Origin+Y*fCut,Y);

  const double bCut((fCut+fDepth>depth) ? fCut+fDepth : depth);
  ModelSupport::buildPlane(SMap,coneIndex+21,Origin+Y*bCut,Y);
  ModelSupport::buildPlane(SMap,coneIndex+31,Origin+Y*(bCut+alView),Y);

  // End surfaces:
  ModelSupport::buildPlane(SMap,coneIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,coneIndex+12,Origin+Y*(length+faceThick),Y);

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

  std::string Out;  


  Out=ModelSupport::getComposite(SMap,coneIndex," -7 1 -2 (17:-11) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  
  // Hydrogne
  Out=ModelSupport::getComposite(SMap,coneIndex," -17 11 -2 (27:-21) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // AL layer 
  Out=ModelSupport::getComposite(SMap,coneIndex," -27 21 -2 (37:-31) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Mid Void
  Out=ModelSupport::getComposite(SMap,coneIndex," -37 31 -12 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Cap :
  Out=ModelSupport::getComposite(SMap,coneIndex," -7 2 -12 37");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,coneIndex," -7 1 -12 ");
  addOuterSurf(Out);

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

  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE delftSystem
