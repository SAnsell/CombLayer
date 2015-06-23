/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/RotaryCollimator.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "surfEqual.h"
#include "generateSurf.h"
#include "objectRegister.h"
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
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "HoleUnit.h"
#include "RotaryCollimator.h"

namespace constructSystem
{

RotaryCollimator::RotaryCollimator(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),holeIndex(0),nHole(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


RotaryCollimator::~RotaryCollimator() 
  /*!
    Destructor
  */
{}

void
RotaryCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function database
  */
{
  ELog::RegMethod RegA("RotaryCollimator","populate");

  const FuncDataBase& Control=System.getDataBase();

  nHole=Control.EvalVar<size_t>(keyName+"NHole");
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  radius=Control.EvalVar<double>(keyName+"Radius");
  depth=Control.EvalVar<double>(keyName+"Depth");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");

  // Layers
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac",depth,
				  cFrac);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"Mat",defMat,cMat);

  for(size_t i=0;i<nHole;i++)
    {
      const std::string holeName=keyName+"Hole"+
	StrFunc::makeString(i);
      Holes.push_back(HoleUnit(SMap,holeName));
      Holes.back().populate(Control);
    }

  // Rotation angle:
  const size_t HI=Control.EvalVar<size_t>(keyName+"HoleIndex");
  const double HA=Control.EvalVar<double>(keyName+"HoleAngOff");
  setHoleIndex(HI,HA);

  return;
}

void
RotaryCollimator::setHoleIndex(const size_t HIndex,const double HAngle)
  /*!
    Given a hole index determine and set the angle offset
    \param HIndex :: Index to use [ Zero :: use HAngle directly]
    \param HAngle :: Angle to use if HIndex is zero
  */
{
  ELog::RegMethod RegA("RotaryCollimator","setHoleIndex");

  if (HIndex==0)
    {
      holeIndex=0;
      holeAngOffset=HAngle;
      return;
    }
  if (HIndex>Holes.size())
    throw ColErr::IndexError<size_t>(HIndex,Holes.size(),
				     "vector<Hole> :: HIndex");
  holeIndex=HIndex;
  holeAngOffset= -Holes[HIndex-1].getAngle();
  return;
}

void
RotaryCollimator::createUnitVector(const attachSystem::FixedComp& FC
				   const long int sideIndex)
  /*!
    Create the unit vectors. The vectors are created
    relative to the exit point
    \param FC :: Centre beamline
    \param sideIndex :: link index
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(FC,sideIndex);
  mainFC.createUnitVector(FC,sideIndex);

  beamFC=mainFC;
  mainFC.applyShift(xStep,yStep,zStep-rotDepth);  
  mainRC.applyAngleRotate(xyAngle,zAngle);
  setDefault("Main");
  return;
}

Geometry::Vec3D
RotaryCollimator::getHoleCentre() const
  /*!
    Calculate the hole centre 
    \return Centre point
  */
{
  if (holeIndex>0)
    return Holes[holeIndex-1].getCentre();
  
  return Centre;
}

void
RotaryCollimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("RotaryCollimator","createSurface");
  // INNER PLANES
  
  // Front/Back
  ModelSupport::buildPlane(SMap,colIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,colIndex+1,Origin+Y*thick,Y);

  // Master Cylinder
  ModelSupport::buildCylinder(SMap,colIndex+7,Origin,Y,radius);

  return;
}

void
RotaryCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -7");
  addOuterSurf(Out);
  //
  // Sectors
  // All via centre / at Angle from normal
  //
  HeadRule HoleExclude;
  for(size_t i=0;i<nHole;i++)
    {
      HoleShape& HU=Holes[i];
      HU.setFaces(SMap.realSurf(colIndex+1),SMap.realSurf(colIndex+2));
      HU.createAll(holeAngOffset,*this,0);          // Use THESE Origins
      if (HU.getShape())
	HoleExclude.addUnion(HU.getExclude());
    }
  
  Out+=" "+HoleExclude.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));            

  return;
}

void 
RotaryCollimator::setCentre(const Geometry::Vec3D& C)
  /*!
    Sets the centre
    \param C :: Centre point
  */
{
  Centre=C;
  return;
}

void 
RotaryCollimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("RotaryCollimator","LayerProcess");
  
  return;
}

void
RotaryCollimator::createAll(Simulation& System,
			    const attachSystem::FixedComp& LC,
			    const long int index)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc [Guide]
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createAll");

  populate(System);
  createUnitVector(LC);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
