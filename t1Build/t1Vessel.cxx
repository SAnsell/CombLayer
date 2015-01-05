/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/t1Vessel.cxx
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
#include "Tensor.h"
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
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "t1Vessel.h"

namespace shutterSystem
{

t1Vessel::t1Vessel(const std::string& Key)  : 
  attachSystem::FixedComp(Key,1),attachSystem::ContainedComp(),
  voidIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(voidIndex+1),populated(0),steelCell(0),voidCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

t1Vessel::t1Vessel(const t1Vessel& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  voidIndex(A.voidIndex),cellIndex(A.cellIndex),
  populated(A.populated),voidOrigin(A.voidOrigin),
  vXoffset(A.vXoffset),Thick(A.Thick),Gap(A.Gap),Side(A.Side),
  Base(A.Base),Top(A.Top),Back(A.Back),Front(A.Front),
  FDepth(A.FDepth),ForwardAngle(A.ForwardAngle),
  WindowThick(A.WindowThick),WindowView(A.WindowView),
  WindowWTopLen(A.WindowWTopLen),
  WindowETopLen(A.WindowETopLen),
  WindowWLowLen(A.WindowWLowLen),
  WindowELowLen(A.WindowELowLen),
  WindowWTopOff(A.WindowWTopOff),
  WindowETopOff(A.WindowETopOff),
  WindowWLowOff(A.WindowWLowOff),
  WindowELowOff(A.WindowELowOff),WindowWTopZ(A.WindowWTopZ),
  WindowETopZ(A.WindowETopZ),WindowWLowZ(A.WindowWLowZ),
  WindowELowZ(A.WindowELowZ),WindowAngleLen(A.WindowAngleLen),
  WindowAngleROff(A.WindowAngleROff),
  WindowAngleLOff(A.WindowAngleLOff),WindowMat(A.WindowMat),
  Mat(A.Mat),steelCell(A.steelCell),voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: t1Vessel to copy
  */
{}

t1Vessel&
t1Vessel::operator=(const t1Vessel& A)
  /*!
    Assignment operator
    \param A :: t1Vessel to copy
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
      Thick=A.Thick;
      Gap=A.Gap;
      Side=A.Side;
      Base=A.Base;
      Top=A.Top;
      Back=A.Back;
      Front=A.Front;
      FDepth=A.FDepth;
      ForwardAngle=A.ForwardAngle;
      WindowThick=A.WindowThick;
      WindowView=A.WindowView;
      WindowWTopLen=A.WindowWTopLen;
      WindowETopLen=A.WindowETopLen;
      WindowWLowLen=A.WindowWLowLen;
      WindowELowLen=A.WindowELowLen;
      WindowWTopOff=A.WindowWTopOff;
      WindowETopOff=A.WindowETopOff;
      WindowWLowOff=A.WindowWLowOff;
      WindowELowOff=A.WindowELowOff;
      WindowWTopZ=A.WindowWTopZ;
      WindowETopZ=A.WindowETopZ;
      WindowWLowZ=A.WindowWLowZ;
      WindowELowZ=A.WindowELowZ;
      WindowAngleLen=A.WindowAngleLen;
      WindowAngleROff=A.WindowAngleROff;
      WindowAngleLOff=A.WindowAngleLOff;
      WindowMat=A.WindowMat;
      Mat=A.Mat;
      steelCell=A.steelCell;
      voidCell=A.voidCell;
    }
  return *this;
}

t1Vessel::~t1Vessel() 
  /*!
    Destructor
  */
{}

void
t1Vessel::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("t1Vessel","populate");

  const FuncDataBase& Control=System.getDataBase();
   
  Thick=Control.EvalVar<double>(keyName+"Thick");
  Gap=Control.EvalVar<double>(keyName+"Gap");
  Side=Control.EvalVar<double>(keyName+"Side");
  Base=Control.EvalVar<double>(keyName+"Base");
  Top=Control.EvalVar<double>(keyName+"Top");
  Back=Control.EvalVar<double>(keyName+"Back");
  Front=Control.EvalVar<double>(keyName+"Front");

  FDepth=Control.EvalVar<double>(keyName+"FDepth");
  ForwardAngle=Control.EvalVar<double>(keyName+"FAngle");

  WindowView=Control.EvalVar<double>(keyName+"WindowView"); 
  WindowThick=Control.EvalVar<double>(keyName+"WindowThick");

  WindowWTopOff=Control.EvalVar<double>(keyName+"WindowWTopOff");
  WindowETopOff=Control.EvalVar<double>(keyName+"WindowETopOff");
  WindowWLowOff=Control.EvalVar<double>(keyName+"WindowWLowOff");
  WindowELowOff=Control.EvalVar<double>(keyName+"WindowELowOff");

  WindowWTopLen=Control.EvalVar<double>(keyName+"WindowWTopLen");
  WindowETopLen=Control.EvalVar<double>(keyName+"WindowETopLen");
  WindowWLowLen=Control.EvalVar<double>(keyName+"WindowWLowLen");
  WindowELowLen=Control.EvalVar<double>(keyName+"WindowELowLen");

  WindowWTopZ=Control.EvalVar<double>(keyName+"WindowWTopZ");
  WindowETopZ=Control.EvalVar<double>(keyName+"WindowETopZ");
  WindowWLowZ=Control.EvalVar<double>(keyName+"WindowWLowZ");
  WindowELowZ=Control.EvalVar<double>(keyName+"WindowELowZ");

  WindowAngleLen=Control.EvalVar<double>(keyName+"WindowAngleLen");
  WindowAngleROff=Control.EvalVar<double>(keyName+"WindowAngleROffset");
  WindowAngleLOff=Control.EvalVar<double>(keyName+"WindowAngleLOffset");

  Mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  WindowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");
      
  populated = 1;
  return;
}

void
t1Vessel::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("t1Vessel","createUnitVector");

  attachSystem::FixedComp::createUnitVector();

  Z=Geometry::Vec3D(0,0,1);          // Gravity axis [up]
  Y=Geometry::Vec3D(0,1,0);
  X=Geometry::Vec3D(1,0,0);
  Origin=Geometry::Vec3D(0,0,0);
  beamAxis=Y;

  return;
}

void
t1Vessel::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("t1Vessel","createSurface");
  //
  // OUTER VOID
  //
  ModelSupport::buildPlane(SMap,voidIndex+1,Origin-Y*(Front+Gap),Y);
  ModelSupport::buildPlane(SMap,voidIndex+2,Origin+Y*(Back+Gap),Y);
  ModelSupport::buildPlane(SMap,voidIndex+3,Origin-X*(Side+Gap),X);
  ModelSupport::buildPlane(SMap,voidIndex+4,Origin+X*(Side+Gap),X);
  ModelSupport::buildPlane(SMap,voidIndex+5,Origin-Z*(Base+Gap),Z);
  ModelSupport::buildPlane(SMap,voidIndex+6,Origin+Z*(Top+Gap),Z);

  Geometry::Vec3D EdgeAxis(Y);
  Geometry::Quaternion::calcQRotDeg(-ForwardAngle,Z).rotate(EdgeAxis);
  Geometry::Vec3D OP=Origin-X*Side-Y*(Front-FDepth)-EdgeAxis*Gap;
  ModelSupport::buildPlane(SMap,voidIndex+13,OP,EdgeAxis);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(ForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin+X*Side-Y*(Front-FDepth)-EdgeAxis*Gap;
  ModelSupport::buildPlane(SMap,voidIndex+14,OP,EdgeAxis);
  //
  // Outer METAL
  //
  ModelSupport::buildPlane(SMap,voidIndex+21,Origin-Y*Front,Y);
  ModelSupport::buildPlane(SMap,voidIndex+22,Origin+Y*Back,Y);
  ModelSupport::buildPlane(SMap,voidIndex+23,Origin-X*Side,X);
  ModelSupport::buildPlane(SMap,voidIndex+24,Origin+X*Side,X);
  ModelSupport::buildPlane(SMap,voidIndex+25,Origin-Z*Base,Z);
  ModelSupport::buildPlane(SMap,voidIndex+26,Origin+Z*Top,Z);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(-ForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin-X*Side-Y*(Front-FDepth);
  ModelSupport::buildPlane(SMap,voidIndex+33,OP,EdgeAxis);
  ModelSupport::buildPlane(SMap,voidIndex+53,OP+EdgeAxis*Thick,EdgeAxis);

  EdgeAxis=Y;
  Geometry::Quaternion::calcQRotDeg(ForwardAngle,Z).rotate(EdgeAxis);
  OP=Origin+X*Side-Y*(Front-FDepth);
  ModelSupport::buildPlane(SMap,voidIndex+34,OP,EdgeAxis);
  ModelSupport::buildPlane(SMap,voidIndex+54,OP+EdgeAxis*Thick,EdgeAxis);


  //
  // Inner METAL
  //
  ModelSupport::buildPlane(SMap,voidIndex+41,Origin-Y*(Front-Thick),Y);
  ModelSupport::buildPlane(SMap,voidIndex+42,Origin+Y*(Back-Thick),Y);
  ModelSupport::buildPlane(SMap,voidIndex+43,Origin-X*(Side-Thick),X);
  ModelSupport::buildPlane(SMap,voidIndex+44,Origin+X*(Side-Thick),X);
  ModelSupport::buildPlane(SMap,voidIndex+45,Origin-Z*(Base-Thick),Z);
  ModelSupport::buildPlane(SMap,voidIndex+46,Origin+Z*(Top-Thick),Z);
  
  // WINDOWS:
  // Inner metal:
  ModelSupport::buildPlane(SMap,voidIndex+103,Origin-X*(Side-WindowThick),X);
  ModelSupport::buildPlane(SMap,voidIndex+104,Origin+X*(Side-WindowThick),X);

  // West side [TOP]:
  ModelSupport::buildPlane(SMap,voidIndex+105,Origin+Z*(WindowWTopZ-WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+106,Origin+Z*(WindowWTopZ+WindowView),Z);

  ModelSupport::buildPlane(SMap,voidIndex+101,Origin+Y*(WindowWTopOff-WindowWTopLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+102,Origin+Y*(WindowWTopOff+WindowWTopLen/2.0),Y);
  
  // West Side [LOW]:

  ModelSupport::buildPlane(SMap,voidIndex+115,Origin+Z*(WindowWLowZ-WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+116,Origin+Z*(WindowWLowZ+WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+111,Origin+Y*(WindowWLowOff-WindowWLowLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+112,Origin+Y*(WindowWLowOff+WindowWLowLen/2.0),Y);

  // East Side [HIGH]:
  ModelSupport::buildPlane(SMap,voidIndex+125,Origin+Z*(WindowETopZ-WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+126,Origin+Z*(WindowETopZ+WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+121,Origin+Y*(WindowETopOff-WindowETopLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+122,Origin+Y*(WindowETopOff+WindowETopLen/2.0),Y);
  
  // East Side [LOW]:
  ModelSupport::buildPlane(SMap,voidIndex+135,Origin+Z*(WindowELowZ-WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+136,Origin+Z*(WindowELowZ+WindowView),Z);
  ModelSupport::buildPlane(SMap,voidIndex+131,Origin+Y*(WindowELowOff-WindowELowLen/2.0),Y);
  ModelSupport::buildPlane(SMap,voidIndex+132,Origin+Y*(WindowELowOff+WindowELowLen/2.0),Y);

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
			   Centre-CrossVec*(WindowAngleLen-WindowAngleROff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+202,
			   Centre+CrossVec*(WindowAngleLen+WindowAngleROff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+203,Centre-Axis*(Thick-WindowThick),Axis);

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
			   Centre-CrossVec*(WindowAngleLen-WindowAngleLOff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+222,
			   Centre+CrossVec*(WindowAngleLen+WindowAngleLOff),CrossVec);
  ModelSupport::buildPlane(SMap,voidIndex+223,Centre-Axis*(Thick-WindowThick),Axis);
  return;
}

void
t1Vessel::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param Reflector :: Explicit reflector component
   */
{
  ELog::RegMethod RegA("t1Vessel","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,voidIndex,"1 -2 3 -4 5 -6 13 14 "
                         "(-21 : 22 : -23 : 24 : -25 : 26 : -33 : -34)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"21 -22 23 -24 25 -26 33 34 "
                               "(-41 : 42 : -43 : 44 : -45 : 46 : -53 : -54)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,Out));
  steelCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,voidIndex,"41 -42 43 -44 45 -46 53 54 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;

  // Now add Outer surface:
  Out=ModelSupport::getComposite(SMap,voidIndex,"1 -2 3 -4 5 -6 13 14");
  addOuterSurf(Out);
  
  return;
}

void
t1Vessel::createWindows(Simulation& System)
  /*!
    Adds the windows to the system
    \param System :: Simulation object ot add
  */
{
  ELog::RegMethod RegA("t1Vessel","createWindows");

  MonteCarlo::Qhull* steelObj=System.findQhull(steelCell);
  if (!steelObj)
    throw ColErr::InContainerError<int>(steelCell,RegA.getBase());
  std::string Out,outerInclude;
  
  // West Top
  Out=ModelSupport::getComposite(SMap,voidIndex,"-103 23 101 -102 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,voidIndex,"103 -43 101 -102 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( 43 : -101 : 102 : -105 : 106 ) ");
  steelObj->addSurfString(outerInclude);

  // West Low
  Out=ModelSupport::getComposite(SMap,voidIndex,"-103 23 111 -112 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"103 -43 111 -112 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( 43 : -111 : 112 : -115 : 116 )");
  steelObj->addSurfString(outerInclude);

  // East Top:
  Out=ModelSupport::getComposite(SMap,voidIndex,"104 -24 121 -122 125 -126");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"-104 44 121 -122 125 -126");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( -44 : -121 : 122 : -125 : 126 )");
  steelObj->addSurfString(outerInclude);

  // East Low:
  Out=ModelSupport::getComposite(SMap,voidIndex,"104 -24 131 -132 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"-104 44 131 -132 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,
				  "( -44 : -131 : 132 : -135 : 136 )");
  steelObj->addSurfString(outerInclude);

  // East Angle Low:
  Out=ModelSupport::getComposite(SMap,voidIndex,"33 -203 201 -202 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"203 (-53:-43) -111 201 -202 135 -136");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,"((53 43) : -201 : 202 : -135 : 136)");
  steelObj->addSurfString(outerInclude);

  // West Angle High:
  Out=ModelSupport::getComposite(SMap,voidIndex,"34 -223 221 -222 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,WindowMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex,"223 (-54:44) -121 221 -222 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerInclude=ModelSupport::getComposite(SMap,voidIndex,"((54 -44) : -221 : 222 : -105 : 106)");
  steelObj->addSurfString(outerInclude);
  

  
  return;
}

void
t1Vessel::processVoid(Simulation& System)
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
t1Vessel::createAll(Simulation& System)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("t1Vessel","createAll");
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createWindows(System);
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
