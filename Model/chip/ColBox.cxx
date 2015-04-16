/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/ColBox.cxx
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
#include "generateSurf.h"
#include "surfEqual.h"
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
#include "ColBox.h"

namespace hutchSystem
{

ColBox::ColBox(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,2),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ColBox::ColBox(const ColBox& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  colIndex(A.colIndex),cellIndex(A.cellIndex),
  populated(A.populated),insertCell(A.insertCell),
  XAxis(A.XAxis),YAxis(A.YAxis),ZAxis(A.ZAxis),
  Centre(A.Centre),xStep(A.xStep),fStep(A.fStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  width(A.width),depth(A.depth),height(A.height),
  roofThick(A.roofThick),floorThick(A.floorThick),
  frontThick(A.frontThick),backThick(A.backThick),
  viewX(A.viewX),viewZ(A.viewZ),outMat(A.outMat),
  defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: ColBox to copy
  */
{}

ColBox&
ColBox::operator=(const ColBox& A)
  /*!
    Assignment operator
    \param A :: ColBox to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      insertCell=A.insertCell;
      XAxis=A.XAxis;
      YAxis=A.YAxis;
      ZAxis=A.ZAxis;
      Centre=A.Centre;
      xStep=A.xStep;
      fStep=A.fStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      depth=A.depth;
      height=A.height;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      frontThick=A.frontThick;
      backThick=A.backThick;
      viewX=A.viewX;
      viewZ=A.viewZ;
      outMat=A.outMat;
      defMat=A.defMat;
    }
  return *this;
}

ColBox::~ColBox() 
  /*!
    Destructor
  */
{}

void
ColBox::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");

  viewX=Control.EvalVar<double>(keyName+"ViewX");
  viewZ=Control.EvalVar<double>(keyName+"ViewZ");

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  populated |= 1;
  return;
}

void
ColBox::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
    \param TC :: TwinComponent to attach to
  */
{
  ELog::RegMethod RegA("ColBox","createUnitVector");

  X=TC.getBX();  
  Y=TC.getBY();
  Z=TC.getBZ();
  Origin=Centre+Y*fStep+X*xStep+Z*zStep;

  

  // Handle Angles [AFTER ORIGIN]:
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  YAxis=Y;
  XAxis=X;
  ZAxis=Z;
  Qz.rotate(YAxis);
  Qz.rotate(ZAxis);
  Qxy.rotate(YAxis);
  Qxy.rotate(XAxis);
  Qxy.rotate(ZAxis);
    
  return;
}

void
ColBox::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("ColBox","createSurface");
  // Back

  ModelSupport::buildPlane(SMap,colIndex+1,Origin,YAxis);
  ModelSupport::buildPlane(SMap,colIndex+2,Origin+YAxis*depth,YAxis);
  ModelSupport::buildPlane(SMap,colIndex+3,Origin-XAxis*(width/2.0),XAxis);
  ModelSupport::buildPlane(SMap,colIndex+4,Origin+XAxis*(width/2.0),XAxis);
  ModelSupport::buildPlane(SMap,colIndex+5,Origin-ZAxis*(depth/2.0),ZAxis);
  ModelSupport::buildPlane(SMap,colIndex+6,Origin+ZAxis*(depth/2.0),ZAxis);

  // Front / Roof and Back planes
  ModelSupport::buildPlane(SMap,colIndex+11,
			   Origin+YAxis*backThick,YAxis);
  ModelSupport::buildPlane(SMap,colIndex+12,
			   Origin+YAxis*(depth-frontThick),YAxis);
  ModelSupport::buildPlane(SMap,colIndex+15,
			   Origin-ZAxis*(depth/2.0-floorThick),ZAxis);
  ModelSupport::buildPlane(SMap,colIndex+16,
			   Origin+ZAxis*(depth/2.0-roofThick),ZAxis);

  // View Ports:
  ModelSupport::buildPlane(SMap,colIndex+103,
			   Origin-XAxis*(viewX/2.0),XAxis);
  ModelSupport::buildPlane(SMap,colIndex+104,
			   Origin+XAxis*(viewX/2.0),XAxis);
  ModelSupport::buildPlane(SMap,colIndex+105,
			   Origin-ZAxis*(viewZ/2.0),ZAxis);
  ModelSupport::buildPlane(SMap,colIndex+106,
			   Origin-ZAxis*(viewZ/2.0),ZAxis);
  

  return;
}

void
ColBox::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ColBox","createObjects");

  std::string Out;
  // LEFT box: [virtual]  
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  
  // Front plate
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -11 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outMat,0.0,Out));
  
  
  return;
}

void 
ColBox::setMidFace(const Geometry::Vec3D& fC)
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

int
ColBox::exitWindow(const double Dist,
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
  ELog::RegMethod RegA("ColBox","exitWindow");

  window.clear();
  window.push_back(SMap.realSurf(colIndex+3));
  window.push_back(SMap.realSurf(colIndex+4));
  window.push_back(SMap.realSurf(colIndex+5));
  window.push_back(SMap.realSurf(colIndex+6));
  Pt=Origin+YAxis*(depth+Dist);  
  return SMap.realSurf(colIndex+2);
}

void
ColBox::createPartial(Simulation& System,
		      const attachSystem::TwinComp& TC)
  /*!
    Generic function to create just sufficient to get datum points
    \param System :: Simulation item
    \param TC :: Twin component to set axis etc
  */
{
  ELog::RegMethod RegA("ColBox","createPartial");
  populate(System);
  if (populated!=9)
    {
      ELog::EM<<"ERROR ColBox not populated:"<<populated<<ELog::endErr;
      throw ColErr::ExitAbort(ELog::RegMethod::getFull());
    }
  createUnitVector(TC);  
  return;
}
  
void
ColBox::createAll(Simulation& System,
		      const attachSystem::TwinComp& TC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("ColBox","createAll");

  createPartial(System,TC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
