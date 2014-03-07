/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/PreCollimator.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "HoleUnit.h"
#include "PreCollimator.h"

namespace hutchSystem
{

PreCollimator::PreCollimator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,2),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),
  populated(0),holeIndex(0),nHole(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PreCollimator::PreCollimator(const PreCollimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  colIndex(A.colIndex),cellIndex(A.cellIndex),
  populated(A.populated),Axis(A.Axis),
  XAxis(A.XAxis),ZAxis(A.ZAxis),xyAngle(A.xyAngle),zAngle(A.zAngle),
  fStep(A.fStep),xStep(A.xStep),zStep(A.zStep),Centre(A.Centre),
  radius(A.radius),depth(A.depth),defMat(A.defMat),innerWall(A.innerWall),
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
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      Axis=A.Axis;
      XAxis=A.XAxis;
      ZAxis=A.ZAxis;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      fStep=A.fStep;
      xStep=A.xStep;
      zStep=A.zStep;
      Centre=A.Centre;
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
  
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

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
      std::ostringstream cx;
      cx<<keyName+"Hole"<<i+1;
      Holes.push_back(HoleUnit(SMap,cx.str(),colIndex+10*i));
      Holes.back().populate(Control);
    }

  // Rotation angle:
  const size_t HI=Control.EvalVar<size_t>(keyName+"HoleIndex");
  const double HA=Control.EvalVar<double>(keyName+"HoleAngOff");
  setHoleIndex(HI,HA);

  populated |= 1;
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
PreCollimator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors. The vectors are created
    relative to the exit point
    \param FC :: TwinComp to attach to 
  */
{
  ELog::RegMethod RegA("PreCollimator","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getExit();
  Origin+= X*xStep+Y*fStep+Z*zStep;
  bEnter=Origin;

  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  // Move centre before rotation
  Centre=Origin+Y*(depth/2.0);
  Axis=Y;
  XAxis=X;
  ZAxis=Z;
  Qz.rotate(Axis);
  Qz.rotate(ZAxis);
  Qxy.rotate(Axis);
  Qxy.rotate(XAxis);
  Qxy.rotate(ZAxis);
  
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
  ModelSupport::buildPlane(SMap,colIndex+1,Centre-Axis*depth/2.0,Axis);
  ModelSupport::buildPlane(SMap,colIndex+2,Centre+Axis*depth/2.0,Axis);

  // Master Cylinder
  ModelSupport::buildCylinder(SMap,colIndex+7,Centre,Axis,radius);

  // Process exit:
  setBeamExit(colIndex+2,Origin+Axis*depth,Axis);

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
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -7");
  addOuterSurf(Out);
  //
  // Sectors
  // All via centre / at Angle from normal
  //
  for(size_t i=0;i<nHole;i++)
    {
      HoleUnit& HU=Holes[i];
      HU.setFaces(SMap.realSurf(colIndex+1),SMap.realSurf(colIndex+2));
      HU.createAll(holeAngOffset,*this);          // Use THESE Origins
      std::string OutItem=HU.createObjects();
      if (OutItem!=" ")
	{
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutItem));
	  Out+=" "+HU.getExclude();
	}
    }

  Out+=" "+getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));            
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
      DA.setOutNum(cellIndex,colIndex+201+100*static_cast<int>(i));

      DA.makePair<Geometry::Plane>(SMap.realSurf(colIndex+1),
				   SMap.realSurf(colIndex+2));
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
  window.push_back(SMap.realSurf(colIndex+3));
  window.push_back(SMap.realSurf(colIndex+4));
  window.push_back(SMap.realSurf(colIndex+5));
  window.push_back(SMap.realSurf(colIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return SMap.realSurf(colIndex+2);
}

void
PreCollimator::createPartial(Simulation& System,
			     const attachSystem::FixedComp& FC)
  /*!
    Allows the construction of points but not the explicit object
    \param System :: Simulation item
    \param FC :: Twin component to set axis etc
   */
{
  ELog::RegMethod RegA("PreCollimator","createPartial");

  populate(System);
  createUnitVector(FC);
  return;
}

void
PreCollimator::createAll(Simulation& System,
		      const attachSystem::FixedComp& LC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc [Guide]
  */
{
  ELog::RegMethod RegA("PreCollimator","createAll");

  populate(System);
  createUnitVector(LC);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
