/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/PreCollimator.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "HoleUnit.h"
#include "PreCollimator.h"

namespace hutchSystem
{

PreCollimator::PreCollimator(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffsetGroup(Key,"Main",2,"Beam",2),
  holeIndex(0),nHole(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PreCollimator::PreCollimator(const PreCollimator& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffsetGroup(A),
  Centre(A.Centre),radius(A.radius),depth(A.depth),
  defMat(A.defMat),innerWall(A.innerWall),
  innerWallMat(A.innerWallMat),nHole(A.nHole),nLayers(A.nLayers),
  Holes(A.Holes),cFrac(A.cFrac),cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: PreCollimator to copy
  */
{}

PreCollimator&
PreCollimator::operator=(const PreCollimator& A)
  /*!
    Assignment operator
    \param A :: PreCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedGroup::operator=(A);
      radius=A.radius;
      depth=A.depth;
      defMat=A.defMat;
      innerWall=A.innerWall;
      innerWallMat=A.innerWallMat;
      nHole=A.nHole;
      nLayers=A.nLayers;
      Holes=A.Holes;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}

PreCollimator::~PreCollimator() 
  /*!
    Destructor
  */
{}

void
PreCollimator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("PreCollimator","populate");

  const FuncDataBase& Control=System.getDataBase();

  nHole=Control.EvalVar<size_t>(keyName+"NHole");
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  depth=Control.EvalVar<double>(keyName+"Depth");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");

  // Layers
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",depth,
				  cFrac);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"Mat_",defMat,cMat);

  for(int i=0;i<static_cast<int>(nHole);i++)
    {
      Holes.push_back(HoleUnit(keyName+"Hole"+std::to_string(i+1)));
      Holes.back().populate(Control);
    }

  // Rotation angle:
  const size_t HI=Control.EvalVar<size_t>(keyName+"HoleIndex");
  const double HA=Control.EvalVar<double>(keyName+"HoleAngOff");
  setHoleIndex(HI,HA);

  return;
}

void
PreCollimator::setHoleIndex(const size_t HIndex,const double HAngle)
  /*!
    Given a hole index determine and set the angle offset
    \param HIndex :: Index to use [ Zero :: use HAngle directly]
    \param HAngle :: Angle to use if HIndex is zero
  */
{
  ELog::RegMethod RegA("PreCollimator","setHoleIndex");
  if (HIndex==0)
    {
      holeIndex=0;
      holeAngOffset=HAngle;
      return;
    }
  if (HIndex>Holes.size())
  throw ColErr::IndexError<size_t>(HIndex,Holes.size(),
				   "vector<Hole>::"+RegA.getFull());
  holeIndex=HIndex;
  holeAngOffset= -Holes[HIndex-1].getAngle();
  return;
}

void
PreCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Create the unit vectors. The vectors are created
    relative to the exit point
    \param FC :: FixedComp to attach to 
  */
{
  ELog::RegMethod RegA("PreCollimator","createUnitVector");

  FixedOffsetGroup::createUnitVector(FC,sideIndex);

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  beamFC.applyShift(0,depth/2.0,0);
  
  setDefault("Main","Beam");
  return;
}

Geometry::Vec3D
PreCollimator::getHoleCentre() const
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
PreCollimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("PreCollimator","createSurface");
  // INNER PLANES
  
  // Front/Back
  ModelSupport::buildPlane(SMap,buildIndex+1,bOrigin-bY*depth/2.0,bY);
  ModelSupport::buildPlane(SMap,buildIndex+2,bOrigin+bY*depth/2.0,bY);

  // Master Cylinder
  ModelSupport::buildCylinder(SMap,buildIndex+7,bOrigin,bY,radius);

  // Process exit:
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  beamFC.setExit(buildIndex+2,Origin+bY*depth,bY);

  return;
}

void
PreCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreCollimator","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7");
  addOuterSurf(Out);
  //
  // Sectors
  // All via centre / at Angle from normal
  //
  for(size_t i=0;i<nHole;i++)
    {
      HoleUnit& HU=Holes[i];
      HU.setFaces(SMap.realSurf(buildIndex+1),SMap.realSurf(buildIndex+2));
      HU.build(holeAngOffset,*this);          // Use THESE Origins
      std::string OutItem=HU.createObjects();
      if (OutItem!=" ")
	{
	  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,OutItem));
	  Out+=" "+HU.getExclude();
	}
    }

  Out+=" "+getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,defMat,0.0,Out));            
  CDivideList.push_back(cellIndex-1);

  return;
}

void 
PreCollimator::setCentre(const Geometry::Vec3D& C)
  /*!
    Sets the centre
    \param C :: Centre point
  */
{
  Centre=C;
  return;
}

void 
PreCollimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("PreCollimator","LayerProcess");
  
  if (nLayers<1) return;
  ModelSupport::surfDivide DA;
  // Generic
  const size_t sNL(static_cast<size_t>(nLayers-1));
  for(size_t i=0;i<sNL;i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[sNL]);

  const size_t CSize=CDivideList.size();
  for(size_t i=0;i<CSize;i++)
    {
      DA.init();
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,buildIndex+201+100*static_cast<int>(i));

      DA.makeTemplate<Geometry::Plane>(SMap.realSurf(buildIndex+1),
				       SMap.realSurf(buildIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
PreCollimator::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(buildIndex+3));
  window.push_back(SMap.realSurf(buildIndex+4));
  window.push_back(SMap.realSurf(buildIndex+5));
  window.push_back(SMap.realSurf(buildIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return SMap.realSurf(buildIndex+2);
}

void
PreCollimator::createPartial(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Allows the construction of points but not the explicit object
    \param System :: Simulation item
    \param FC :: Twin component to set axis etc
   */
{
  ELog::RegMethod RegA("PreCollimator","createPartial");

  populate(System);
  createUnitVector(FC,sideIndex);
  return;
}

void
PreCollimator::createAll(Simulation& System,
			 const attachSystem::FixedComp& LC,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc [Guide]
  */
{
  ELog::RegMethod RegA("PreCollimator","createAll");

  populate(System);
  createUnitVector(LC,sideIndex);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
