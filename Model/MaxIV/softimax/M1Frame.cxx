/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Frame.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "Importance.h"
#include "Object.h"
#include "M1Frame.h"

namespace xraySystem
{

M1Frame::M1Frame(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1Frame::~M1Frame()
  /*!
    Destructor
   */
{}

void
M1Frame::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Frame","populate");

  FixedRotate::populate(Control);

  bladeOutRad=Control.EvalVar<double>(keyName+"BladeInRad");
  bladeOutRad=Control.EvalVar<double>(keyName+"BladeOutRad");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeTopAngle=Control.EvalVar<double>(keyName+"BladeTopAngle");
  bladeBaseAngle=Control.EvalVar<double>(keyName+"BladeBaseAngle");
  bladeBaseWidth=Control.EvalVar<double>(keyName+"BladeBaseWidth");
  bladeBaseHeight=Control.EvalVar<double>(keyName+"BladeBaseHeight");
  bladeFullHeight=Control.EvalVar<double>(keyName+"BladeFullHeight");

  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");  

  return;
}

void
M1Frame::createSurfaces()
  /*!
    Create surfaces for the frame
  */
{
  ELog::RegMethod RegA("M1Frame","createSurfaces");


  const Geometry::Cylinder* innerCyl=
    SurfMap::realPtrThrow<Geometry::Cylinder>
    ("RingRadius","Inner Connect ring not defined");
  
  const Geometry::Vec3D Org=innerCyl->getCentre();

  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Z);

  SurfMap::makeExpandedCylinder("RingRadius",SMap,buildIndex+7,bladeInRad);
  SurfMap::makeExpandedCylinder("RingRadius",SMap,buildIndex+17,bladeOutRad);
  SurfMap::makeShiftedPlane("FSurf",SMap,buildIndex+101,Y,bladeThick);

  // Z == up (0 deg)
  const Geometry::Vec3D Axis=
    Z*std::sin(M_PI*bladeTopAngle/180.0)+
    X*std::cos(M_PI*bladeTopAngle/180.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+103,Org,Axis);
  SurfMap::makePlane("baseVoid",SMap,buildIndex+105,Org-Z*bladeFullHeight,Z);

  // contact points:
  const double R=innerCyl->getRadius()-bladeInRad;
  const double zDiff=(Origin-Org).dotProd(Z);
  const double xDiff=std::sqrt(R*R-zDiff*zDiff);
  const Geometry::Vec3D cPt=Org-X*xDiff+Z*zDiff;

  ModelSupport::buildPlane(SMap,buildIndex+113,cPt,X);
  SurfMap::makePlane("outVoid",SMap,buildIndex+114,
		     cPt-X*(bladeOutRad-bladeInRad),X);
  SurfMap::makePlane("mirrorVoid",SMap,buildIndex+124,
		     Org+X*(bladeBaseWidth/2.0),X);
  // side cut axis
  const Geometry::Vec3D BAxis=
    Z*std::sin(M_PI*bladeBaseAngle/180.0)+
    X*std::cos(M_PI*bladeBaseAngle/180.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+123,Org,BAxis);
  
  
  return;
}

MonteCarlo::Object*
M1Frame::getBoundaryCell(Simulation& System,
			 const std::string& cellName,
			 HeadRule& HR) const
  /*!
    Get the cell and make the appropiate surface substitutions
    \param System :: simuation
    \param System :: Simulation
    \return HeadRule with substitutions
   */
{
  ELog::RegMethod RegA("M1Frame","getBoundaryCell");
  
  MonteCarlo::Object* OPtr=getCellObject(System,cellName);
  HR=OPtr->getHeadRule();
  HR.substituteSurf
    (SMap,buildIndex,SurfMap::getSurf("FarEdge"),114);
  return OPtr;
}
  
void
M1Frame::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Frame","createObjects");
  
  const HeadRule flatHR=getSurfRule("FSurf");
  const int ASurf=getSurf("FSurf");
  const int BSurf=getSurf("BSurf");
  const int beamSurf=getSurf("BeamEdge");
  HeadRule HR;
  
  // First we are going to cut the main CVoids into section for the
  // frame plates

  MonteCarlo::Object* OPtr;
  
  OPtr=getBoundaryCell(System,"TopCVoid",HR);
  HR.substituteSurf(SMap,buildIndex,BSurf,101);
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"-7:17:103");
  makeCell("FRingSupport",System,cellIndex++,voidMat,0.0,HR);  

  OPtr=getBoundaryCell(System,"TopCVoid",HR);
  HR.substituteSurf(SMap,buildIndex,ASurf,101);
  OPtr->procHeadRule(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 7 -17 -103 100");
  makeCell("FRingSupport",System,cellIndex++,supportMat,0.0,HR*flatHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-124 -123 -101 7 114 105 -100");
  makeCell("FRingSupport",System,cellIndex++,supportMat,0.0,HR*flatHR);

  //  MID Void
  OPtr=getBoundaryCell(System,"BackCVoid",HR);

  HR.substituteSurf(SMap,buildIndex,BSurf,101);

  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(-7:17:-100) (-7:-114:100)");
  makeCell("BRingSupport",System,cellIndex++,voidMat,0.0,HR); 
  OPtr=getBoundaryCell(System,"BackCVoid",HR);
  HR.substituteSurf(SMap,buildIndex,ASurf,101);
  OPtr->procHeadRule(HR);

  //  LOW Void
  OPtr=getBoundaryCell(System,"LowCVoid",HR);
  HR.substituteSurf(SMap,buildIndex,BSurf,101);
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(-7:-114)");
  makeCell("BRingSupport",System,cellIndex++,voidMat,0.0,HR);
  OPtr=getBoundaryCell(System,"LowCVoid",HR);
  HR.substituteSurf(SMap,buildIndex,ASurf,101);
  OPtr->procHeadRule(HR);
  
  //  const HeadRule MainHR=removeBoundaryCell(System,"PlateVoid");
  OPtr=getBoundaryCell(System,"PlateVoid",HR);
  HR.substituteSurf(SMap,buildIndex,BSurf,101);
  HR*=HeadRule(SMap,buildIndex,-7);
  makeCell("BPlateSupport",System,cellIndex++,voidMat,0.0,HR);

  OPtr=getBoundaryCell(System,"PlateVoid",HR);
  HR.substituteSurf(SMap,buildIndex,ASurf,101);    
  OPtr->procHeadRule(HR);
  
  // PlateVoid (thin section in low plane:
  OPtr=getBoundaryCell(System,"OuterVoid",HR);
  HR.substituteSurf(SMap,buildIndex,BSurf,101);
  HR.substituteSurf(SMap,buildIndex,beamSurf,124);
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"-7:123");
  makeCell("BOuterSupport",System,cellIndex++,voidMat,0.0,HR);
  
  OPtr=getBoundaryCell(System,"OuterVoid",HR);
  HR.substituteSurf(SMap,buildIndex,ASurf,101);    
  OPtr->procHeadRule(HR);

  // face void
  OPtr=CellMap::getCellObject(System,"FaceVoid");
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101:123:-7")+
    flatHR.complement();
  OPtr->addIntersection(HR);
  

  return;
}

void
M1Frame::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Frame","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
M1Frame::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Frame","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
