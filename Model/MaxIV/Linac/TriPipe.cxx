/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/TriPipe.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <array>

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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "TriPipe.h"

namespace tdcSystem
{

TriPipe::TriPipe(const std::string& Key) : 
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Pipe","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


TriPipe::~TriPipe() 
  /*!
    Destructor
  */
{}

void
TriPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TriPipe","populate");
  
  FixedRotate::populate(Control);

  axisXStep=Control.EvalVar<double>(keyName+"AxisXStep");
  axisZStep=Control.EvalVar<double>(keyName+"AxisZStep");
  axisXYAngle=Control.EvalVar<double>(keyName+"AxisXYAngle");
  axisZAngle=Control.EvalVar<double>(keyName+"AxisZAngle");
  
  // Void + Wall:
  frontWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  backWidth=Control.EvalVar<double>(keyName+"BackWidth");  
  frontHeight=Control.EvalVar<double>(keyName+"FrontHeight");
  backHeight=Control.EvalVar<double>(keyName+"BackHeight");

  length=Control.EvalVar<double>(keyName+"Length");
  
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");


  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}
  
void
TriPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TriPipe","createSurfaces");
  
  // Inner void
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y); 
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  // use this so angled fronts correctly make
  FrontBackCut::getShiftedFront(SMap,buildIndex+11,1,Y,flangeALength);


  
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  FrontBackCut::getShiftedBack(SMap,buildIndex+12,-1,Y,flangeBLength);

  attachSystem::FixedOffsetUnit PipeFC("Axis",*this,0);
  PipeFC.setRotation(axisXYAngle,axisZAngle);
  PipeFC.applyOffset();

  const Geometry::Vec3D& Org=PipeFC.getCentre();
  const Geometry::Vec3D& AX=PipeFC.getX();
  const Geometry::Vec3D& AZ=PipeFC.getZ();
  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Org-AX*(frontWidth/2.0),
			   Org-AX*(backWidth/2.0),
			   Org-AX*(backWidth/2.0)+AZ,
			   AX);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Org+AX*(frontWidth/2.0),
			   Org+AX*(backWidth/2.0),
			   Org+AX*(backWidth/2.0)+AZ,
			   AX);
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Org-AZ*(frontHeight/2.0),
			   Org-AZ*(backHeight/2.0),
			   Org-AZ*(backHeight/2.0)+AX,
			   AZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Org+AZ*(frontHeight/2.0),
			   Org+AZ*(backHeight/2.0),
			   Org+AZ*(backHeight/2.0)+AX,
			   AZ);

  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Org-AX*(wallThick+frontWidth/2.0),
			   Org-AX*(wallThick+backWidth/2.0),
			   Org-AX*(wallThick+backWidth/2.0)+AZ,
			   AX);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Org+AX*(wallThick+frontWidth/2.0),
			   Org+AX*(wallThick+backWidth/2.0),
			   Org+AX*(wallThick+backWidth/2.0)+AZ,
			   AX);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Org-AZ*(wallThick+frontHeight/2.0),
			   Org-AZ*(wallThick+backHeight/2.0),
			   Org-AZ*(wallThick+backHeight/2.0)+AX,
			   AZ);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Org+AZ*(wallThick+frontHeight/2.0),
			   Org+AZ*(wallThick+backHeight/2.0),
			   Org+AZ*(wallThick+backHeight/2.0)+AX,
			   AZ);
  
  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
TriPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum system
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("TriPipe","createObjects");

  std::string Out;

  const std::string frontStr=frontRule();
  const std::string backStr=backRule();
  
  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"3 -4  5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"13 -14  15 -16 (-3:4:-5:6)");
  makeCell("Walls",System,cellIndex++,wallMat,0.0,Out+frontStr+backStr);

  // FLANGE Front: 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -11 -107 (-13 : 14 : -15 : 16) ");
  makeCell("FrontFlange",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  // FLANGE Back:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 12 -207 (-13 : 14 : -15 : 16) ");
  makeCell("BackFlange",System,cellIndex++,flangeMat,0.0,Out+backStr);

  // outer boundary [flange front/back]
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -107 ");
  addOuterSurf("FlangeA",Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -207 ");
  addOuterSurf("FlangeB",Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf("Pipe",Out);
  return;
}
  
void
TriPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TriPipe","createLinks");
  
  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  const double H((frontHeight+backHeight)/2.0);
  const double W((frontWidth+backWidth)/2.0);
  FixedComp::setConnect(2,Origin-X*(wallThick+H+W),-X);
  FixedComp::setConnect(3,Origin-X*(wallThick+H+W),X);
  FixedComp::setConnect(4,Origin-Z*(wallThick+H+W),-Z);
  FixedComp::setConnect(5,Origin+Z*(wallThick+H+W),Z);
  
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+18));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));


  // top lift point : Out is an complemnt of the volume
  std::string Out;
  FixedComp::setConnect(7,Origin+Z*(wallThick+H),Z);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-15 : 16 : (-3 17) : (4 18))");
  FixedComp::setLinkSurf(7,Out);

  FixedComp::nameSideIndex(7,"outerPipe");
  
  return;
}
  
  
void
TriPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TriPipe","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
