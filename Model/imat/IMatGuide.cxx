/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   imat/IMatGuide.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Line.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "IMatGuide.h"

namespace imatSystem
{

IMatGuide::IMatGuide(const std::string& Key)  :
  attachSystem::ContainedGroup("Inner","Steel","Wall"),
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

IMatGuide::IMatGuide(const IMatGuide& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedGroup(A),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),length(A.length),
  height(A.height),width(A.width),glassThick(A.glassThick),
  boxThick(A.boxThick),voidSide(A.voidSide),voidBase(A.voidBase),
  voidTop(A.voidTop),feSide(A.feSide),feBase(A.feBase),
  feTop(A.feTop),wallSide(A.wallSide),wallBase(A.wallBase),
  wallTop(A.wallTop),glassMat(A.glassMat),boxMat(A.boxMat),
  feMat(A.feMat),wallMat(A.wallMat),innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: IMatGuide to copy
  */
{}

IMatGuide&
IMatGuide::operator=(const IMatGuide& A)
  /*!
    Assignment operator
    \param A :: IMatGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedGroup::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      length=A.length;
      height=A.height;
      width=A.width;
      glassThick=A.glassThick;
      boxThick=A.boxThick;
      voidSide=A.voidSide;
      voidBase=A.voidBase;
      voidTop=A.voidTop;
      feSide=A.feSide;
      feBase=A.feBase;
      feTop=A.feTop;
      wallSide=A.wallSide;
      wallBase=A.wallBase;
      wallTop=A.wallTop;
      glassMat=A.glassMat;
      boxMat=A.boxMat;
      feMat=A.feMat;
      wallMat=A.wallMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}


IMatGuide::~IMatGuide() 
 /*!
   Destructor
 */
{}

void
IMatGuide::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("IMatGuide","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  glassThick=Control.EvalVar<double>(keyName+"GlassThick");
  boxThick=Control.EvalVar<double>(keyName+"BoxThick");

  voidSide=Control.EvalVar<double>(keyName+"VoidSide");
  voidBase=Control.EvalVar<double>(keyName+"VoidBase");
  voidTop=Control.EvalVar<double>(keyName+"VoidTop");

  feSide=Control.EvalVar<double>(keyName+"FeSide");
  feBase=Control.EvalVar<double>(keyName+"FeBase");
  feTop=Control.EvalVar<double>(keyName+"FeTop");

  wallSide=Control.EvalVar<double>(keyName+"WallSide");
  wallBase=Control.EvalVar<double>(keyName+"WallBase");
  wallTop=Control.EvalVar<double>(keyName+"WallTop");
 
  glassMat=ModelSupport::EvalMat<int>(Control,keyName+"GlassMat");
  boxMat=ModelSupport::EvalMat<int>(Control,keyName+"BoxMat");
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
IMatGuide::createUnitVector(const attachSystem::FixedGroup& TC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param TC :: A second track to use the beam direction as basis
  */
{
  ELog::RegMethod RegA("IMatGuide","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(TC.getKey("Main"));
  beamFC.createUnitVector(TC.getKey("Beam"));

  mainFC.setCentre(beamFC.getCentre());
  mainFC.applyShift(xStep,yStep,zStep);
  beamFC.applyShift(xStep,yStep,zStep);
  beamFC.applyAngleRotate(xyAngle,zAngle);

  return;
}

void
IMatGuide::createSurfaces()
  /*!
    Create All the surfaces
    -- Outer plane is going 
  */
{
  ELog::RegMethod RegA("IMatGuide","createSurfaces");

  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  setSecondary("Beam");

  // Inner void layers
  double xside(width/2.0);
  double zup(height/2.0);
  double zdown(height/2.0);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,bY);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+bY*length,bY);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+bZ*zup,bZ);

  // Glass layers
  xside+=glassThick;
  zup+=glassThick;
  zdown+=glassThick;
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+bZ*zup,bZ);

  // Box layers
  xside+=boxThick;
  zup+=boxThick;
  zdown+=boxThick;
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+bZ*zup,bZ);

  // Not change
  xside=voidSide;
  zup=voidTop;
  zdown=voidBase;
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+bZ*zup,bZ);

  xside+=feSide;
  zup+=feTop;
  zdown+=feBase;
  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+bZ*zup,bZ);

  xside+=wallSide;
  zup+=wallTop;
  zdown+=wallBase;
  ModelSupport::buildPlane(SMap,buildIndex+53,Origin-bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+54,Origin+bX*xside,bX);
  ModelSupport::buildPlane(SMap,buildIndex+55,Origin-bZ*zdown,bZ);
  ModelSupport::buildPlane(SMap,buildIndex+56,Origin+bZ*zup,bZ);

  return;
}

void
IMatGuide::createObjects(Simulation& System,
			 const attachSystem::FixedComp& FC)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
    \param FC :: Fixed component (insert boundary)
  */
{
  ELog::RegMethod RegA("IMatGuide","createObjects");
  
  const std::string insertEdge=FC.getLinkString(2);
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 33 -34 35 -36 ");
  addOuterSurf("Inner",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 53 -54 55 -56 ");
  addOuterSurf("Wall",Out+insertEdge);

  // Inner void cell:
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6 ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Glass layer:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-2 13 -14 15 -16 (-3:4:-5:6) ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,glassMat,0.0,Out));
  // Box layer:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-2 23 -24 25 -26 "
				 "(-13:14:-15:16) ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,boxMat,0.0,Out));

  // Void layer:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-2 33 -34 35 -36 "
				 "(-23:24:-25:26) ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Fe layer:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-2 43 -44 45 -46 "
				 "(-33:34:-35:36) ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out));

  // Wall layer:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-2  53 -54 55 -56 "
				 "(-43:44:-45:46) ")+insertEdge;
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out));


  
  return;
}

void
IMatGuide::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("IMatGuide","createLinks");

  
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  const Geometry::Vec3D bY(beamFC.getY());

  mainFC.setConnect(1,Origin+bY*length,bY);
  beamFC.setConnect(1,Origin+bY*length,bY);

  mainFC.setConnect(0,Origin,-bY);      // Note always to the moderator
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
IMatGuide::createAll(Simulation& System,const attachSystem::FixedGroup& TC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param TC :: BulkInsert/IMatInsert
  */
{
  ELog::RegMethod RegA("IMatGuide","createAll");
  populate(System);

  createUnitVector(TC);
  createSurfaces();
  createObjects(System,TC);
  insertObjects(System); 
  createLinks();

  return;
}

  
}  // NAMESPACE imatSystem
