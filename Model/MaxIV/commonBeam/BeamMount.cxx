/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BeamMount.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BeamMount.h"


namespace xraySystem
{

BeamMount::BeamMount(const std::string& Key) :
  attachSystem::ContainedGroup("Block","Support"),
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


BeamMount::~BeamMount()
  /*!
    Destructor
   */
{}

void
BeamMount::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeamMount","populate");

  FixedOffsetGroup::populate(Control);

  blockFlag=Control.EvalDefVar<int>(keyName+"BlockFlag",0);
  
  upFlag=Control.EvalDefVar<int>(keyName+"UpFlag",1);
  outLift=Control.EvalVar<double>(keyName+"OutLift");
  beamLift=Control.EvalVar<double>(keyName+"BeamLift");


  if (blockFlag)
    {
      blockXYAngle=Control.EvalDefVar<double>(keyName+"BlockXYAngle",0.0);
      width=Control.EvalVar<double>(keyName+"Width");
      height=Control.EvalVar<double>(keyName+"Height");
      length=Control.EvalVar<double>(keyName+"Length");
      blockMat=ModelSupport::EvalMat<int>(Control,keyName+"BlockMat");
    }

  supportRadius=Control.EvalVar<double>(keyName+"SupportRadius");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");

  
  return;
}

void
BeamMount::createUnitVector(const attachSystem::FixedComp& centreFC,
			   const long int cIndex,
			   const attachSystem::FixedComp& flangeFC,
			   const long int fIndex)
  /*!
    Create the unit vectors.
    The first flangeFC is to set the X,Y,Z relative to the axis
    and the origin at the beam centre position.
    
    The beamFC axis are set so (a) Y ==> cIndex axis
                               (b) Z ==> mount axis
                               (c) X ==> correct other

    \param centreFC :: FixedComp for origin
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange centre
    \param fIndex :: direction for links
  */
{
  ELog::RegMethod RegA("BeamMount","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  mainFC.createUnitVector(flangeFC,fIndex);
  const Geometry::Vec3D& ZBeam=mainFC.getY();
  const Geometry::Vec3D YBeam=centreFC.getLinkAxis(cIndex);
  const Geometry::Vec3D XBeam=ZBeam*YBeam;
  Geometry::Vec3D BC(mainFC.getCentre());

  // need to remove directoin in Y of flange
  
  const double BY=BC.dotProd(ZBeam);
  const double MY=centreFC.getLinkPt(cIndex).dotProd(ZBeam);
  BC += ZBeam * (MY-BY);
  if (XBeam.abs()>0.5)
    {
      beamFC.createUnitVector(BC,XBeam,YBeam,ZBeam);
    }
  else
    {
      beamFC.createUnitVector(flangeFC,fIndex);
    }
  applyOffset();

  if (upFlag)
    beamFC.applyShift(0,0,-outLift);  // only lift offset
  else
    beamFC.applyShift(0,0,-beamLift);  // only beam offset

  setDefault("Main","Beam");

  return;
}

void
BeamMount::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("BeamMount","createSurfaces");


  const attachSystem::FixedComp& beamFC=getKey("Beam");

  // Not can have a local rotation of the beam component
  // construct support
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,supportRadius);

  if (!isActive("mountSurf"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
      setCutSurf("mountSurf",SMap.realSurf(buildIndex+101));
    }

  if (blockFlag==0)
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,bOrigin,Y);
    }
  else             // make centre block
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       bOrigin-bY*(length/2.0),bY);
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       bOrigin+bY*(length/2.0),bY);
      ModelSupport::buildPlane(SMap,buildIndex+3,
			       bOrigin-bX*(width/2.0),bX);
      ModelSupport::buildPlane(SMap,buildIndex+4,
			       bOrigin+bX*(width/2.0),bX);
      
      if (blockFlag==1)  // make centre
	{
	  ModelSupport::buildPlane(SMap,buildIndex+5,
				   bOrigin-bZ*(height/2.0),bZ);

	  ModelSupport::buildPlane(SMap,buildIndex+6,
				   bOrigin+bZ*(height/2.0),bZ);
	}
      else             // make on lower edge
	{
	  ModelSupport::buildPlane(SMap,buildIndex+5,
				   bOrigin-bZ*height,bZ);
	  ModelSupport::buildPlane(SMap,buildIndex+6,bOrigin,bZ);
	}
    }
  return; 
}

void
BeamMount::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BeamMount","createObjects");

  const std::string mountSurf(ExternalCut::getRuleStr("mountSurf"));

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -5 " );
  makeCell("Support",System,cellIndex++,supportMat,0.0,Out+mountSurf);

  if (blockFlag)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 " );
      makeCell("Block",System,cellIndex++,blockMat,0.0,Out);
      addOuterSurf("Block",Out);
    }
    
  
  // final exclude:
  //  Out=ModelSupport::getComposite(SMap,buildIndex,"-117 -6");
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -5 " );
  addOuterSurf("Support",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 " );  
  addOuterSurf("Block",Out);
  
  return; 
}

void
BeamMount::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("BeamMount","createLinks");
  
  return;
}

void
BeamFlange::createAll(Simulation& ,
		      const attachSystem::FixedComp&,
		      const long int)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param portFC :: FixedComp
    \param portIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BeamFlange","createAll(FC)");

  throw ColErr::AbsObjMethod("Single value createAll");
  return;
}

void
BeamMount::createAll(Simulation& System,
		    const attachSystem::FixedComp& centreFC,
		    const long int cIndex,
		    const attachSystem::FixedComp& flangeFC,
		    const long int fIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param centreFC :: FixedComp for beam origin
    \param cIndex :: link point of centre [and axis]
    \param flangeFC :: link point of flange center
    \param fIndex :: direction for links
   */
{
  ELog::RegMethod RegA("BeamMount","createAll");
  populate(System.getDataBase());

  createUnitVector(centreFC,cIndex,flangeFC,fIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
