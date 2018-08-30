/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/TwinPipe.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurfMap.h"

#include "TwinPipe.h"

namespace xraySystem
{

TwinPipe::TwinPipe(const std::string& Key) :
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedGroup("Flange","PipeA","PipeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

TwinPipe::~TwinPipe() 
  /*!
    Destructor
  */
{}

void
TwinPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TwinPipe","populate");
  
  FixedOffset::populate(Control);

  pipeARadius=Control.EvalVar<double>(keyName+"PipeARadius");
  pipeALength=Control.EvalVar<double>(keyName+"PipeALength");
  pipeAThick=Control.EvalVar<double>(keyName+"PipeAThick");
  pipeAXStep=Control.EvalVar<double>(keyName+"PipeAXStep");
  pipeAZStep=Control.EvalVar<double>(keyName+"PipeAZStep");
  pipeAXYAngle=Control.EvalVar<double>(keyName+"PipeAXYAngle");
  pipeAZAngle=Control.EvalVar<double>(keyName+"PipeAZAngle");  

  pipeBRadius=Control.EvalVar<double>(keyName+"PipeBRadius");
  pipeBLength=Control.EvalVar<double>(keyName+"PipeBLength");
  pipeBThick=Control.EvalVar<double>(keyName+"PipeBThick");
  pipeBXStep=Control.EvalVar<double>(keyName+"PipeBXStep");
  pipeBZStep=Control.EvalVar<double>(keyName+"PipeBZStep");
  pipeBXYAngle=Control.EvalVar<double>(keyName+"PipeBXYAngle");
  pipeBZAngle=Control.EvalVar<double>(keyName+"PipeBZAngle");  

  flangeCJRadius=Control.EvalPair<double>(keyName+"FlangeCJRadius",
					  keyName+"FlangeRadius");
  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeCJLength=Control.EvalPair<double>(keyName+"FlangeCJLength",
					  keyName+"FlangeLength");
  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");

  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  
  return;
}

void
TwinPipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TwinPipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}



void
TwinPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TwinPipe","createSurfaces");
  

  // Get A Pipe Vector:
  const Geometry::Quaternion QAxy=
    Geometry::Quaternion::calcQRotDeg(pipeAXYAngle,Z);
  const Geometry::Quaternion QAz=
    Geometry::Quaternion::calcQRotDeg(pipeAZAngle,X);
  const Geometry::Quaternion QBxy=
    Geometry::Quaternion::calcQRotDeg(pipeBXYAngle,Z);
  const Geometry::Quaternion QBz=
    Geometry::Quaternion::calcQRotDeg(pipeBZAngle,X);

  AYAxis=Y;
  BYAxis=Y;
  QAz.rotate(AYAxis);
  QAxy.rotate(AYAxis);

  QBz.rotate(BYAxis);
  QBxy.rotate(BYAxis);

  // Inner void
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  makeShiftedSurf(SMap,"front",buildIndex+11,1,Y,flangeCJLength);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeCJRadius);
  
  // Pipe A
  const Geometry::Vec3D ACent=Origin+X*pipeAXStep+Z*pipeAZStep;
  ModelSupport::buildCylinder(SMap,buildIndex+107,ACent,AYAxis,pipeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,ACent,AYAxis,
			      pipeARadius+pipeAThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,ACent,AYAxis,flangeARadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+101,ACent+AYAxis*pipeALength,AYAxis);
  ModelSupport::buildPlane(SMap,buildIndex+111,
			   ACent+AYAxis*(pipeALength-flangeALength),AYAxis);
  
  // Pipe B
  const Geometry::Vec3D BCent=Origin+X*pipeBXStep+Z*pipeBZStep;
  ELog::EM<<"Origin == "<<Origin<<" :: "<<ACent<<" :: "<<BCent<<ELog::endDiag;
  ELog::EM<<"Dist == "<<ACent.Distance(BCent)<<ELog::endDiag;
  
  ModelSupport::buildCylinder(SMap,buildIndex+207,BCent,BYAxis,pipeBRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,BCent,
			      BYAxis,pipeBRadius+pipeBThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,BCent,BYAxis,flangeBRadius);

  ModelSupport::buildPlane(SMap,buildIndex+201,BCent+BYAxis*pipeBLength,BYAxis);
  ModelSupport::buildPlane(SMap,buildIndex+211,
			   BCent+BYAxis*(pipeBLength-flangeBLength),BYAxis);
  return;
}

void
TwinPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TwinPipe","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");

  // front flange
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -11 107 207 ");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,Out+frontStr);
  
  // pipeA 
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -107 ");
  makeCell("PipeAVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -101 107 -117 ");
  makeCell("PipeAWall",System,cellIndex++,feMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -101 117 -127 ");
  makeCell("PipeAFlange",System,cellIndex++,feMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -111 -127 117 ");
  makeCell("PipeAOutVoid",System,cellIndex++,voidMat,0.0,Out);

  // pipe B
  Out=ModelSupport::getComposite(SMap,buildIndex," -201 -207 ");
  makeCell("PipeBVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);
    
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -201 207 -217 ");
  makeCell("PipeBWall",System,cellIndex++,feMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 -201 217 -227 ");
  makeCell("PipeBFlange",System,cellIndex++,feMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -211 217 -227 ");
  makeCell("PipeBOutVoid",System,cellIndex++,voidMat,0.0,Out);

  // outer boundary [flange front]
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -7 ");
  addOuterSurf("Flange",Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -101 -127 ");
  addOuterSurf("PipeA",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -201 -227 ");
  addOuterUnionSurf("PipeB",Out);

  return;
}

void
TwinPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TwinPipe","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,-Y);

  const Geometry::Vec3D ACent=Origin+X*pipeAXStep+Z*pipeAZStep;
  const Geometry::Vec3D BCent=Origin+X*pipeBXStep+Z*pipeBZStep;
    
  FixedComp::setConnect(1,ACent+AYAxis*pipeALength,AYAxis);
  FixedComp::setConnect(2,BCent+BYAxis*pipeBLength,BYAxis);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+101));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+201));
  
  return;
}
    
void
TwinPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TwinPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
