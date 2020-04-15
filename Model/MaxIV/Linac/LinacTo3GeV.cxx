/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/LinacTo3GeV.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

#include "LinacTo3GeV.h"

namespace xraySystem
{

// Note currently uncopied:
  
LinacTo3GeV::LinacTo3GeV(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
}
  
LinacTo3GeV::~LinacTo3GeV()
  /*!
    Destructor
   */
{}

void
LinacTo3GeV::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("LinacTo3GeV","populate");
  FixedOffset::populate(Control);

  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  const int voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  buildZone.setInnerMat(voidMat);

  return;
}


void
LinacTo3GeV::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("LinacTo3GeV","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    {

      if (isActive("floor"))
	{
	  std::string Out;
	  ModelSupport::buildPlane
	    (SMap,buildIndex+3,Origin-X*outerRadius,X);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+4,Origin+X*outerRadius,X);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+6,Origin+Z*outerRadius,Z);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
	  const HeadRule HR(Out+getRuleStr("floor"));
	  buildZone.setSurround(HR);
	}
      else
	{
	  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
	  buildZone.setSurround(HeadRule(-SMap.realSurf(buildIndex+7)));
	}
    }

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*100.0,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  return;
}




void
LinacTo3GeV::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("LinacTo3GeV","buildObjects");

  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));
  MonteCarlo::Object* masterCellA=
    buildZone.constructMasterCell(System,*this);


  return;
}

void
LinacTo3GeV::createLinks()
  /*!
    Create a front/back link
   */
{
  //  setLinkSignedCopy(0,*bellowA,1);
  //  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  


void 
LinacTo3GeV::createAll(Simulation& System,
				 const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("LinacTo3GeV","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  ELog::EM<<"Populate  == "<<Origin<<ELog::endDiag;

  createSurfaces();
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

