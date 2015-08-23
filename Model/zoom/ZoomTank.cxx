/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomTank.cxx
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
#include <numeric>
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ZoomTank.h"

namespace zoomSystem
{

ZoomTank::ZoomTank(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,0),
  tankIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tankIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ZoomTank::ZoomTank(const ZoomTank& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  tankIndex(A.tankIndex),cellIndex(A.cellIndex),
  populated(A.populated),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  nCylinder(A.nCylinder),cylThickness(A.cylThickness),
  CRadius(A.CRadius),CylDepth(A.CylDepth),CylX(A.CylX),
  CylZ(A.CylZ),cylTotalDepth(A.cylTotalDepth),
  windowThick(A.windowThick),windowRadius(A.windowRadius),
  width(A.width),height(A.height),length(A.length),
  wallThick(A.wallThick),wallMat(A.wallMat),
  windowMat(A.windowMat)
  /*!
    Copy constructor
    \param A :: ZoomTank to copy
  */
{}

ZoomTank&
ZoomTank::operator=(const ZoomTank& A)
  /*!
    Assignment operator
    \param A :: ZoomTank to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      nCylinder=A.nCylinder;
      cylThickness=A.cylThickness;
      CRadius=A.CRadius;
      CylDepth=A.CylDepth;
      CylX=A.CylX;
      CylZ=A.CylZ;
      cylTotalDepth=A.cylTotalDepth;
      windowThick=A.windowThick;
      windowRadius=A.windowRadius;
      width=A.width;
      height=A.height;
      length=A.length;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      windowMat=A.windowMat;
    }
  return *this;
}

ZoomTank::~ZoomTank() 
 /*!
   Destructor
 */
{}

void
ZoomTank::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("ZoomTank","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  nCylinder=Control.EvalVar<size_t>(keyName+"NCylinder");
  CRadius.resize(nCylinder);
  CylDepth.resize(nCylinder);
  CylX.resize(nCylinder);
  CylZ.resize(nCylinder);
  std::ostringstream cx;
  for(size_t i=0;i<nCylinder;i++)
    {
      cx.str("");
      cx<<i+1;
      CRadius[i]=Control.EvalVar<double>(keyName+"Radius"+cx.str());
      CylDepth[i]=Control.EvalVar<double>(keyName+"CDepth"+cx.str());
      CylX[i]=Control.EvalVar<double>(keyName+"CXStep"+cx.str());
      CylZ[i]=Control.EvalVar<double>(keyName+"CZStep"+cx.str());
    }
  cylTotalDepth=std::accumulate(CylDepth.begin(),CylDepth.end(),0.0);

  windowThick=Control.EvalVar<double>(keyName+"WindowThick");
  windowRadius=Control.EvalVar<double>(keyName+"WindowRadius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");

  populated |= 1;
  return;
}
  
void
ZoomTank::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param TC :: Linked object
  */
{
  ELog::RegMethod RegA("ZoomTank","createUnitVector");
  TwinComp::createUnitVector(TC);

  // // Reverse X
  // X*=-1;
  Origin=bEnter;
  Origin+=X*xStep+Y*yStep+Z*zStep;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);

  // SAMPLE Position
  // const masterRotate& MR=masterRotate::Instance();
  // ELog::EM<<"Zoom Sample postion == "
  // 	  <<MR.calcRotate(bEnter+bY*200.0)<<ELog::endDebug;
  return;
}

void
ZoomTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomTank","createSurface");

  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,tankIndex+1,
			   Origin+Y*cylTotalDepth,Y);
  ModelSupport::buildPlane(SMap,tankIndex+2,
			   Origin+Y*(cylTotalDepth+length),Y);
  ModelSupport::buildPlane(SMap,tankIndex+3,
			   Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tankIndex+4,
			   Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tankIndex+5,
			   Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,tankIndex+6,
			   Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,tankIndex+11,
			   Origin+Y*(cylTotalDepth+wallThick),Y);
  ModelSupport::buildPlane(SMap,tankIndex+12,
			   Origin+Y*(cylTotalDepth+length-wallThick),Y);
  ModelSupport::buildPlane(SMap,tankIndex+13,
			   Origin-X*(width/2.0-wallThick),X);
  ModelSupport::buildPlane(SMap,tankIndex+14,
			   Origin+X*(width/2.0-wallThick),X);
  ModelSupport::buildPlane(SMap,tankIndex+15,
			   Origin-Z*(height/2.0-wallThick),Z);
  ModelSupport::buildPlane(SMap,tankIndex+16,
			   Origin+Z*(height/2.0-wallThick),Z);


  double depthSum=0.0;
  for(size_t i=0;i<nCylinder;i++)
    {
      const int iN(static_cast<int>(i));
      // Plates across
      ModelSupport::buildPlane(SMap,tankIndex+111+iN,
			       Origin+Y*depthSum,Y);
      ModelSupport::buildPlane(SMap,tankIndex+121+iN,
			       Origin+Y*(depthSum+wallThick),Y);
      depthSum+=CylDepth[i];
      // Radii [Outer]
      ModelSupport::buildCylinder(SMap,tankIndex+171+iN,
				  Origin+X*CylX[i]+Z*CylZ[i],
				  Y,CRadius[i]);
      // Radii [Inner]
      ModelSupport::buildCylinder(SMap,tankIndex+181+iN,
				  Origin+X*CylX[i]+Z*CylZ[i],
				  Y,CRadius[i]-wallThick);
    }      

  // Window stuff
  ModelSupport::buildCylinder(SMap,tankIndex+17,
			      Origin+X*CylX[0]+Z*CylZ[0],
			      Y,windowRadius);
  ModelSupport::buildPlane(SMap,tankIndex+101,
			   Origin+Y*(wallThick-windowThick),Y);
  
  return;
}

void
ZoomTank::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZoomTank","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,tankIndex,
				 "111 -2 3 -4 5 -6 (1:-173)");
  addOuterSurf(Out);

  // Dead Volume
  Out=ModelSupport::getComposite(SMap,tankIndex,"-173 171 111 -113 (-112:172)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Inner Volume
  Out=ModelSupport::getComposite(SMap,tankIndex,"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // first Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-181 121 -122");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // skipping front window
  Out=ModelSupport::getComposite(SMap,tankIndex,"-171 181 121 -122");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // second Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-182 122 -123");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tankIndex,"-172 171 112 -123 (-122:182)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // third Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-183 123 -11");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tankIndex,"-173 172 113 -11 (-123:183)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Main bulk tank:
  Out=ModelSupport::getComposite(SMap,tankIndex,"1 -2 3 -4 5 -6 "
				 "((-11 173):12:-13:14:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  

  // window
  //   support:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-171 17 111 -121");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  //  Silicon window
  Out=ModelSupport::getComposite(SMap,tankIndex,"-17 101 -121");
  System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,Out));
  //  void
  Out=ModelSupport::getComposite(SMap,tankIndex,"-17 111 -101 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}

void
ZoomTank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // // set Links:
  // FixedComp::setConnect(0,BVec[0]+Y*(vacPosGap+alPos+terPos+
  // 				     outPos+clearNeg),Y);
  // FixedComp::setConnect(1,BVec[1]-Y*(vacNegGap+alPos+terNeg+
  // 				     outNeg+clearNeg),-Y);
  // FixedComp::setConnect(2,BVec[2]-
  // 			X*(vacSide+alSide+terSide+outSide+clearSide),-X);
  // FixedComp::setConnect(3,BVec[3]+
  // 			X*(vacSide+alSide+terSide+outSide+clearSide),X);
  // FixedComp::setConnect(4,BVec[4]-
  // 			Z*(vacBase+alBase+terBase+outBase+clearBase),-Z);
  // FixedComp::setConnect(5,BVec[5]+
  // 			Z*(vacTop+alTop+terTop+outTop+clearTop),Z);

  // // Set Connect surfaces:
  // for(int i=2;i<6;i++)
  //   FixedComp::setLinkSurf(i,SMap.realSurf(vacIndex+41+i));

  // // For Cylindrical surface must also have a divider:
  // // -- Groove:
  // FixedComp::setLinkSurf(0,SMap.realSurf(vacIndex+41));
  // FixedComp::addLinkSurf(0,SMap.realSurf(divideSurf));

  // FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+42));
  // FixedComp::addLinkSurf(1,-SMap.realSurf(divideSurf));

  return;
}

void
ZoomTank::createAll(Simulation& System,const attachSystem::TwinComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("ZoomTank","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE zoomSystem
