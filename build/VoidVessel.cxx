/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/VoidVessel.cxx
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
#include <iterator>
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
#include "PointOperation.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "chipDataStore.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "VoidVessel.h"


namespace shutterSystem
{

VoidVessel::VoidVessel(const std::string& Key)  : 
  attachSystem::FixedComp(Key,0),attachSystem::ContainedComp(),
  voidIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(voidIndex+1),populated(0),steelCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

VoidVessel::VoidVessel(const VoidVessel& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  voidIndex(A.voidIndex),cellIndex(A.cellIndex),
  populated(A.populated),voidOrigin(A.voidOrigin),vXoffset(A.vXoffset),
  vThick(A.vThick),vGap(A.vGap),vSide(A.vSide),vBase(A.vBase),
  vTop(A.vTop),vBack(A.vBack),vFront(A.vFront),vFDepth(A.vFDepth),
  vForwardAngle(A.vForwardAngle),vWindowThick(A.vWindowThick),
  vWindowView(A.vWindowView),vWindowWTopLen(A.vWindowWTopLen),
  vWindowETopLen(A.vWindowETopLen),vWindowWLowLen(A.vWindowWLowLen),
  vWindowELowLen(A.vWindowELowLen),vWindowWTopOff(A.vWindowWTopOff),
  vWindowETopOff(A.vWindowETopOff),vWindowWLowOff(A.vWindowWLowOff),
  vWindowELowOff(A.vWindowELowOff),vWindowWTopZ(A.vWindowWTopZ),
  vWindowETopZ(A.vWindowETopZ),vWindowWLowZ(A.vWindowWLowZ),
  vWindowELowZ(A.vWindowELowZ),vWindowAngleLen(A.vWindowAngleLen),
  vWindowAngleROff(A.vWindowAngleROff),vWindowAngleLOff(A.vWindowAngleLOff),
  vWindowMat(A.vWindowMat),vMat(A.vMat),steelCell(A.steelCell)
  /*!
    Copy constructor
    \param A :: VoidVessel to copy
  */
{}

VoidVessel&
VoidVessel::operator=(const VoidVessel& A)
  /*!
    Assignment operator
    \param A :: VoidVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      voidOrigin=A.voidOrigin;
      vXoffset=A.vXoffset;
      vThick=A.vThick;
      vGap=A.vGap;
      vSide=A.vSide;
      vBase=A.vBase;
      vTop=A.vTop;
      vBack=A.vBack;
      vFront=A.vFront;
      vFDepth=A.vFDepth;
      vForwardAngle=A.vForwardAngle;
      vWindowThick=A.vWindowThick;
      vWindowView=A.vWindowView;
      vWindowWTopLen=A.vWindowWTopLen;
      vWindowETopLen=A.vWindowETopLen;
      vWindowWLowLen=A.vWindowWLowLen;
      vWindowELowLen=A.vWindowELowLen;
      vWindowWTopOff=A.vWindowWTopOff;
      vWindowETopOff=A.vWindowETopOff;
      vWindowWLowOff=A.vWindowWLowOff;
      vWindowELowOff=A.vWindowELowOff;
      vWindowWTopZ=A.vWindowWTopZ;
      vWindowETopZ=A.vWindowETopZ;
      vWindowWLowZ=A.vWindowWLowZ;
      vWindowELowZ=A.vWindowELowZ;
      vWindowAngleLen=A.vWindowAngleLen;
      vWindowAngleROff=A.vWindowAngleROff;
      vWindowAngleLOff=A.vWindowAngleLOff;
      vWindowMat=A.vWindowMat;
      vMat=A.vMat;
      steelCell=A.steelCell;
    }
  return *this;
}


VoidVessel::~VoidVessel() 
  /*!
    Destructor
  */
{}

void
VoidVessel::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("VoidVessel","populate");

  const FuncDataBase& Control=System.getDataBase();
   
  vThick=Control.EvalVar<double>(keyName+"Thick");
  vGap=Control.EvalVar<double>(keyName+"Gap");
  vSide=Control.EvalVar<double>(keyName+"Side");
  vBase=Control.EvalVar<double>(keyName+"Base");
  vTop=Control.EvalVar<double>(keyName+"Top");
  vBack=Control.EvalVar<double>(keyName+"Back");
  vFront=Control.EvalVar<double>(keyName+"Front");
  vXoffset=Control.EvalVar<double>(keyName+"Xoffset");

  vFDepth=Control.EvalVar<double>(keyName+"FDepth");
  vForwardAngle=Control.EvalVar<double>(keyName+"FAngle");

  vWindowView=Control.EvalVar<double>(keyName+"WindowView"); 
  vWindowThick=Control.EvalVar<double>(keyName+"WindowThick");

  vWindowWTopOff=Control.EvalVar<double>(keyName+"WindowWTopOff");
  vWindowETopOff=Control.EvalVar<double>(keyName+"WindowETopOff");
  vWindowWLowOff=Control.EvalVar<double>(keyName+"WindowWLowOff");
  vWindowELowOff=Control.EvalVar<double>(keyName+"WindowELowOff");

  vWindowWTopLen=Control.EvalVar<double>(keyName+"WindowWTopLen");
  vWindowETopLen=Control.EvalVar<double>(keyName+"WindowETopLen");
  vWindowWLowLen=Control.EvalVar<double>(keyName+"WindowWLowLen");
  vWindowELowLen=Control.EvalVar<double>(keyName+"WindowELowLen");

  vWindowWTopZ=Control.EvalVar<double>(keyName+"WindowWTopZ");
  vWindowETopZ=Control.EvalVar<double>(keyName+"WindowETopZ");
  vWindowWLowZ=Control.EvalVar<double>(keyName+"WindowWLowZ");
  vWindowELowZ=Control.EvalVar<double>(keyName+"WindowELowZ");

  vWindowAngleLen=Control.EvalVar<double>(keyName+"WindowAngleLen");
  vWindowAngleROff=Control.EvalVar<double>(keyName+"WindowAngleROffset");
  vWindowAngleLOff=Control.EvalVar<double>(keyName+"WindowAngleLOffset");

  vMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  vWindowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");
      
  populated = 1;
  return;
}

void
VoidVessel::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("VoidVessel","createUnitVector");
  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  attachSystem::FixedComp::createUnitVector(FC);
  Origin+=Y*vXoffset;

  CS.setCNum(chipIRDatum::voidCentre,MR.calcRotate(Origin));
  return;
}

void
VoidVessel::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("VoidVessel","createSurface");
  //
  // OUTER VOID
  //
  ModelSupport::buildPlane(SMap,voidIndex+1,Origin-Y*(vFront+vGap),Y);
  ModelSupport::buildPlane(SMap,voidIndex+2,Origin+Y*(vBack+vGap),Y);
  ModelSupport::buildPlane(SMap,voidIndex+3,Origin-X*(vSide+vGap),X);
  ModelSupport::buildPlane(SMap,voidIndex+4,Origin+X*(vSide+vGap),X);
  ModelSupport::buildPlane(SMap,voidIndex+5,Origin-Z*(vBase+vGap),Z);
  ModelSupport::buildPlane(SMap,voidIndex+6,Origin+Z*(vTop+vGap),Z);

  Geometry::Vec3D EdgeAxis(Y);
  Geometry::Quaternion::calcQRotDeg(-vForwardAngle,Z).rotate(EdgeAxis);
  Geometry::Vec3D OP=Origin-X*vSide-Y*(vFront-vFDepth)-EdgeAxis*vGap;
  ModelSupport::buildPlane(SMap,voidIndex+13,OP,EdgeAxis);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(vForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin+X*vSide-Y*(vFront-vFDepth)-EdgeAxis*vGap;
  ModelSupport::buildPlane(SMap,voidIndex+14,OP,EdgeAxis);
  //
  // Outer METAL
  //
  ModelSupport::buildPlane(SMap,voidIndex+21,Origin-Y*vFront,Y);
  ModelSupport::buildPlane(SMap,voidIndex+22,Origin+Y*vBack,Y);
  ModelSupport::buildPlane(SMap,voidIndex+23,Origin-X*vSide,X);
  ModelSupport::buildPlane(SMap,voidIndex+24,Origin+X*vSide,X);
  ModelSupport::buildPlane(SMap,voidIndex+25,Origin-Z*vBase,Z);
  ModelSupport::buildPlane(SMap,voidIndex+26,Origin+Z*vTop,Z);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(-vForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin-X*vSide-Y*(vFront-vFDepth);
  ModelSupport::buildPlane(SMap,voidIndex+33,OP,EdgeAxis);
  ModelSupport::buildPlane(SMap,voidIndex+53,OP+EdgeAxis*vThick,EdgeAxis);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(vForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin+X*vSide-Y*(vFront-vFDepth);
  ModelSupport::buildPlane(SMap,voidIndex+34,OP,EdgeAxis);
  ModelSupport::buildPlane(SMap,voidIndex+54,OP+EdgeAxis*vThick,EdgeAxis);


  //
  // Inner METAL
  //
  ModelSupport::buildPlane(SMap,voidIndex+41,Origin-Y*(vFront-vThick),Y);
  ModelSupport::buildPlane(SMap,voidIndex+42,Origin+Y*(vBack-vThick),Y);
  ModelSupport::buildPlane(SMap,voidIndex+43,Origin-X*(vSide-vThick),X);
  ModelSupport::buildPlane(SMap,voidIndex+44,Origin+X*(vSide-vThick),X);
  ModelSupport::buildPlane(SMap,voidIndex+45,Origin-Z*(vBase-vThick),Z);
  ModelSupport::buildPlane(SMap,voidIndex+46,Origin+Z*(vTop-vThick),Z);
  
  // WINDOWS:
  // Inner metal:
  ModelSupport::buildPlane(SMap,voidIndex+103,Origin-X*(vSide-vWindowThick),X);
  ModelSupport::buildPlane(SMap,voidIndex+104,Origin+X*(vSide-vWindowThick),X);

  // West side [TOP]:
  ModelSupport::buildPlane(SMap,voidIndex+105,Origin+Z*(vWindowWTopZ-vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+106,Origin+Z*(vWindowWTopZ+vWindowView),Z);

  ModelSupport::buildPlane(SMap,voidIndex+101,Origin+Y*(vWindowWTopOff-vWindowWTopLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+102,Origin+Y*(vWindowWTopOff+vWindowWTopLen/2.0),Y);
  
  // West Side [LOW]:

  ModelSupport::buildPlane(SMap,voidIndex+115,Origin+Z*(vWindowWLowZ-vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+116,Origin+Z*(vWindowWLowZ+vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+111,Origin+Y*(vWindowWLowOff-vWindowWLowLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+112,Origin+Y*(vWindowWLowOff+vWindowWLowLen/2.0),Y);

  // East Side [HIGH]:
  ModelSupport::buildPlane(SMap,voidIndex+125,Origin+Z*(vWindowETopZ-vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+126,Origin+Z*(vWindowETopZ+vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+121,Origin+Y*(vWindowETopOff-vWindowETopLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+122,Origin+Y*(vWindowETopOff+vWindowETopLen/2.0),Y);
  
  // East Side [LOW]:
  ModelSupport::buildPlane(SMap,voidIndex+135,Origin+Z*(vWindowELowZ-vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+136,Origin+Z*(vWindowELowZ+vWindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+131,Origin+Y*(vWindowELowOff-vWindowELowLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+132,Origin+Y*(vWindowELowOff+vWindowELowLen/2.0),Y);

  // Side windows : Complex:
  const Geometry::Plane* sRPlane=SMap.realPtr<Geometry::Plane>(voidIndex+53);
  const Geometry::Plane* sLPlane=SMap.realPtr<Geometry::Plane>(voidIndex+54);
  const Geometry::Surface* sRSidePlane=SMap.realSurfPtr(voidIndex+43);
  const Geometry::Surface* sLSidePlane=SMap.realSurfPtr(voidIndex+44);
  const Geometry::Surface* sFlat=SMap.realSurfPtr(voidIndex+41);
  const Geometry::Surface* sRoof=SMap.realSurfPtr(voidIndex+46);
  const Geometry::Surface* sFloor=SMap.realSurfPtr(voidIndex+45);

  // Right Window:
  Geometry::Vec3D VPts[4];
  VPts[0]=SurInter::getPoint(sRPlane,sRSidePlane,sFloor);
  VPts[1]=SurInter::getPoint(sRPlane,sRSidePlane,sRoof);
  VPts[2]=SurInter::getPoint(sRPlane,sFlat,sRoof);

  // calculate normals:
  Geometry::Vec3D Centre=(VPts[0]+VPts[2])/2.0;
  Geometry::Vec3D Axis=sRPlane->getNormal();
  Geometry::Vec3D CrossVec=(VPts[2]-VPts[1]).unit();
  // Remove Z
  Centre-=Z*Centre.dotProd(Z);

  // West side (right) [LOW] -- HYDROGEN:
  ModelSupport::buildPlane(SMap,voidIndex+201,
			   Centre-CrossVec*(vWindowAngleLen-vWindowAngleROff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+202,
			   Centre+CrossVec*(vWindowAngleLen+vWindowAngleROff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+203,Centre-Axis*(vThick-vWindowThick),Axis);

  // LEFT WINDOW
  VPts[0]=SurInter::getPoint(sLPlane,sLSidePlane,sFloor);
  VPts[1]=SurInter::getPoint(sLPlane,sLSidePlane,sRoof);
  VPts[2]=SurInter::getPoint(sLPlane,sFlat,sRoof);

  // calculate normals:
  Centre=(VPts[0]+VPts[2])/2.0;
  Axis=sLPlane->getNormal();
  CrossVec=(VPts[2]-VPts[1]).unit();
  // Remove Z
  Centre-=Z*Centre.dotProd(Z);
  
  // West side (right) [LOW] -- HYDROGEN:
  ModelSupport::buildPlane(SMap,voidIndex+221,
			   Centre-CrossVec*(vWindowAngleLen-vWindowAngleLOff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+222,
			   Centre+CrossVec*(vWindowAngleLen+vWindowAngleLOff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+223,Centre-Axis*(vThick-vWindowThick),Axis);
  return;
}

void
VoidVessel::createObjects(Simulation& System,
			  const attachSystem::ContainedComp* Reflector)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param Reflector :: Explicit reflector component
   */
{
  ELog::RegMethod RegA("VoidVessel","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,voidIndex,"1 -2 3 -4 5 -6 13 14 "
                         "(-21 : 22 : -23 : 24 : -25 : 26 : -33 : -34)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"21 -22 23 -24 25 -26 33 34 "
                               "(-41 : 42 : -43 : 44 : -45 : 46 : -53 : -54)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vMat,0.0,Out));
  steelCell=cellIndex-1;

  if (!Reflector)
  // Add inner object + Intersection 
    Out=ModelSupport::getComposite(SMap,voidIndex,"41 -42 43 -44 45 -46 53 54 "
				   "#78000T ");
  else
    {
      Out=ModelSupport::getComposite(SMap,voidIndex,"41 -42 43 -44 45 -46 53 54 ")+
	Reflector->getExclude();
    }

  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Now add Outer surface:
  Out=ModelSupport::getComposite(SMap,voidIndex,"1 -2 3 -4 5 -6 13 14");
  addOuterSurf(Out);
  
  return;
}

void
VoidVessel::createWindows(Simulation& System)
  /*!
    Adds the windows to the system
    \param System :: Simulation object ot add
  */
{
  ELog::RegMethod RegA("VoidVessel","createWindows");

  MonteCarlo::Qhull* steelObj=System.findQhull(steelCell);
  if (!steelObj)
    throw ColErr::InContainerError<int>(steelCell,RegA.getBase());
  std::string Out,outerInclude;
  
  // West Top
  Out=ModelSupport::getComposite(SMap,voidIndex,"-103 23 101 -102 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,voidIndex,"103 -43 101 -102 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( 43 : -101 : 102 : -105 : 106 ) ");
  steelObj->addSurfString(outerInclude);

  // West Low
  Out=ModelSupport::getComposite(SMap,voidIndex,"-103 23 111 -112 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"103 -43 111 -112 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( 43 : -111 : 112 : -115 : 116 )");
  steelObj->addSurfString(outerInclude);

  // East Top:
  Out=ModelSupport::getComposite(SMap,voidIndex,"104 -24 121 -122 125 -126");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"-104 44 121 -122 125 -126");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( -44 : -121 : 122 : -125 : 126 )");
  steelObj->addSurfString(outerInclude);

  // East Low:
  Out=ModelSupport::getComposite(SMap,voidIndex,"104 -24 131 -132 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"-104 44 131 -132 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( -44 : -131 : 132 : -135 : 136 )");
  steelObj->addSurfString(outerInclude);

  // East Angle Low:
  Out=ModelSupport::getComposite(SMap,voidIndex,"33 -203 201 -202 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"203 (-53:-43) -111 201 -202 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,"((53 43) : -201 : 202 : -135 : 136)");
  steelObj->addSurfString(outerInclude);

  // West Angle High:
  Out=ModelSupport::getComposite(SMap,voidIndex,"34 -223 221 -222 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,vWindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"223 (-54:44) -121 221 -222 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,"((54 -44) : -221 : 222 : -105 : 106)");
  steelObj->addSurfString(outerInclude);
  

  
  return;
}

void
VoidVessel::processVoid(Simulation& System)
  /*!
    Adds this as the outside layer of the void
    \param System :: Simulation object ot add
  */
{
  // Add void
  MonteCarlo::Qhull* Obj=System.findQhull(74123);
  if (Obj)
    Obj->procString("-1 "+getExclude());
  else
    System.addCell(MonteCarlo::Qhull(74123,0,0.0,"-1 "+getExclude()));

  return;
}

void
VoidVessel::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp* Reflector)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param Reflector :: Explicit reflector component
  */
{
  ELog::RegMethod RegA("VoidVessel","createAll");
  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System,Reflector);
  createWindows(System);
  return;
}
  
}  // NAMESPACE shutterSystem
