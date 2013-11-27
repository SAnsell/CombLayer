/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/Collimator.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "generateSurf.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "Collimator.h"

namespace hutchSystem
{

Collimator::Collimator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,2),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),populated(0),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Collimator::Collimator(const Collimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  colIndex(A.colIndex),cellIndex(A.cellIndex),
  populated(A.populated),insertCell(A.insertCell),
  gap(A.gap),offset(A.offset),xyAngle(A.xyAngle),
  zAngle(A.zAngle),Centre(A.Centre),width(A.width),depth(A.depth),
  height(A.height),defMat(A.defMat),innerWall(A.innerWall),
  innerWallMat(A.innerWallMat),nLayers(A.nLayers),cFrac(A.cFrac),
  cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: Collimator to copy
  */
{}

Collimator&
Collimator::operator=(const Collimator& A)
  /*!
    Assignment operator
    \param A :: Collimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      insertCell=A.insertCell;
      gap=A.gap;
      offset=A.offset;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      Centre=A.Centre;
      width=A.width;
      depth=A.depth;
      height=A.height;
      defMat=A.defMat;
      innerWall=A.innerWall;
      innerWallMat=A.innerWallMat;
      nLayers=A.nLayers;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}

Collimator::~Collimator() 
  /*!
    Destructor
  */
{}

void
Collimator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  gap=Control.EvalVar<double>(keyName+"Gap");
  offset=Control.EvalVar<double>(keyName+"Offset");
  xStep=Control.EvalVar<double>(keyName+"XStep");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
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

  populated |= 1;
  return;
}

void
Collimator::setAxis(const Geometry::Vec3D& A)
  /*!
    Set the movement axis			
    \param A :: Axis to move						
  */
{
  populated |=2;
  bZ=A.unit();
  return;
}

void
Collimator::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
    \param TC :: Twin Component to attach to
  */
{
  ELog::RegMethod RegA("Collimator","createUnitVector");


  FixedComp::createUnitVector(TC.getBeamExit(),TC.getBY(),TC.getZ());

  if (populated & 8)        // Flat mid face
    Origin=Centre+Y*fStep+X*xStep+Z*zStep;
  else
    Origin=Centre-Y*(depth/2.0-fStep)+X*xStep+Z*zStep;
  bEnter=Origin;

  // Handle Angles [AFTER ORIGIN]:
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  Qz.rotate(Y);
  Qz.rotate(Z);
  Qz.rotate(bZ);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);
  Qxy.rotate(bZ);

  bY=Y;
  bX=bY*bZ;
  setExit(Origin+Y*depth,bY);  
  
  return;
}


void
Collimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("Collimator","createSurfaces");
  
  // Front

  ModelSupport::buildPlane(SMap,colIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,colIndex+2,Origin+Y*depth,Y);
  setBeamExit(colIndex+2,bEnter,bY);

  // down
  ModelSupport::buildPlane(SMap,colIndex+5,
			   Origin-bX*height/2.0,bX);
  // up [dependent on Axis]
  ModelSupport::buildPlane(SMap,colIndex+6,
			   Origin+bX*(height/2.0),bX);

  // left/Right [Dept. on Axis]
  ModelSupport::buildPlane(SMap,colIndex+3,
			   Origin-bZ*(width-offset+gap/2.0),bZ);
  ModelSupport::buildPlane(SMap,colIndex+4,
			   Origin+bZ*(width-offset+gap/2.0),bZ);

  // INNER LEFT/RIGHT:
  ModelSupport::buildPlane(SMap,colIndex+13,
			   Origin-bZ*(-offset+gap/2.0),bZ);
  ModelSupport::buildPlane(SMap,colIndex+14,
			   Origin+bZ*(offset+gap/2.0),bZ);

  // INNER LEFT/RIGHT Wall:
  ModelSupport::buildPlane(SMap,colIndex+23,
			   Origin-bZ*(-offset+gap/2.0+innerWall),bZ);
  ModelSupport::buildPlane(SMap,colIndex+24,
			   Origin+bZ*(offset+gap/2.0+innerWall),bZ);

  return;
}

void
Collimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Collimator","createObjects");

  std::string Out;
  // LEFT box: [virtual]  
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);

  // Mid Vacuum
  if (gap>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 13 -14 5 -6 ");
      Out+=getContainer();
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
    }

  // LEFT box: [Plate Cutter]
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 23 -13 5 -6 ");
  Out+=getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,0.0,Out));

  // LEFT box: [Real]
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 3 -23 5 -6 ");
  Out+=getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  CDivideList.push_back(cellIndex-1);

  // RIGHT box: [Plate]
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -24 14 5 -6 ");
  Out+=getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,0.0,Out));

  // RIGHT box: [real]
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -4 24 5 -6 ");
  Out+=getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  CDivideList.push_back(cellIndex-1);
    
  return;
}

void 
Collimator::setCentre(const Geometry::Vec3D& C)
  /*!
    Sets the centre
    \param C :: Centre point
  */
{
  populated |= 4;
  populated &= (~8);
  Centre=C;
  return;
}

void 
Collimator::setMidFace(const Geometry::Vec3D& fC)
  /*!
    Sets the centre
    \param fC :: Centre point (Mid Face)
  */
{
  populated |= 8;
  populated &= (~4);
  Centre=fC;
  return;
}

void 
Collimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("Collimator","LayerProcess");
  
  if (nLayers<1) return;

  ModelSupport::surfDivide DA;
  // Generic
  for(size_t i=0;i<static_cast<size_t>(nLayers-1);i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[static_cast<size_t>(nLayers-1)]);

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
Collimator::exitWindow(const double Dist,
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
Collimator::createPartial(Simulation& System,
			  const attachSystem::TwinComp& TC)
  /*!
    Generic function to create just sufficient to get datum points
    \param System :: Simulation item
    \param TC :: Twin component to set axis etc
  */
{
  ELog::RegMethod RegA("Collimator","createPartail");
  populate(System);
  if (populated !=7 && populated!=11)

    {
      ELog::EM<<"ERROR Collimator not populated:"<<populated<<ELog::endErr;
      throw ColErr::ExitAbort(ELog::RegMethod::getFull());
    }
  createUnitVector(TC);  
  return;
}
  
void
Collimator::createAll(Simulation& System,
		      const attachSystem::TwinComp& TC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param TC :: Hutch component to set axis etc
  */
{
  ELog::RegMethod RegA("Collimator","createAll");
  populate(System);
  if (populated !=7 && populated!=11)

    {
      ELog::EM<<"ERROR Collimator not populated:"<<populated<<ELog::endErr;
      throw ColErr::ExitAbort(ELog::RegMethod::getFull());
    }

  createUnitVector(TC);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
