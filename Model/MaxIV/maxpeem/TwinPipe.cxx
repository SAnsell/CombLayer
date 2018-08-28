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
#include "stringCombine.h"
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
  attachSystem::ContainedSpace(),
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

  coJoinRadius=Control.EvalVar<double>(keyName+"CoJoinRadius");
  coJoinLength=Control.EvalVar<double>(keyName+"CoJoinLength");
  coJoinThick=Control.EvalVar<double>(keyName+"CoJoinThick");

  pipeARadius=Control.EvalVar<double>(keyName+"PipeARadius");
  pipeALength=Control.EvalVar<double>(keyName+"PipeALength");
  pipeAThick=Control.EvalVar<double>(keyName+"PipeAThick");
  pipeAXYAngle=Control.EvalVar<double>(keyName+"PipeAXYAngle");
  pipeAZAngle=Control.EvalVar<double>(keyName+"PipeAZAngle");  

  pipeBRadius=Control.EvalVar<double>(keyName+"PipeBRadius");
  pipeBLength=Control.EvalVar<double>(keyName+"PipeBLength");
  pipeBThick=Control.EvalVar<double>(keyName+"PipeBThick");
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
  
  // Inner void
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  makeShiftedSurf(SMap,"front",buildIndex+11,1,Y,flangeALength);
  makeShiftedSurf(SMap,"front",buildIndex+101,1,Y,coJoinLength);

  // Get A Pipe Vector:
  const Geometry::Quaternion QAxy=
    Geometry::Quaternion::calcQRotDeg(pipeAXYAngle,Z);
  const Geometry::Quaternion QAz=
    Geometry::Quaternion::calcQRotDeg(pipeAZAngle,X);
  const Geometry::Quaternion QBxy=
    Geometry::Quaternion::calcQRotDeg(pipeBXYAngle,Z);
  const Geometry::Quaternion QBz=
    Geometry::Quaternion::calcQRotDeg(pipeBZAngle,X);

  Geometry::Vec3D YA(Y);
  Geometry::Vec3D YB(Y);
  QAz.rotate(YA);
  QAxy.rotate(YA);

  QBz.rotate(YB);
  QBxy.rotate(YB);
  
  // Co-join 
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,coJoinRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,
			      coJoinRadius+coJoinThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeCJRadius);
  
  // Pipe A
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,YA,pipeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,YA,
			      pipeARadius+pipeAThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,YA,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+YA*pipeALength,YA);
  
  // Pipe B
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,YB,pipeBRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,
			      YB,pipeBRadius+pipeBThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,Origin,YB,flangeBRadius);
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin+YB*pipeBLength,YB);
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
  /*
  std::string Out;
  
  const std::string frontStr=frontRule();
  const std::string backStr=backRule();
  
  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  // FLANGE Front: 
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -107 7 ");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,Out+frontStr);

  // FLANGE Back: 
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -207 7 ");
  makeCell("BackFlange",System,cellIndex++,feMat,0.0,Out+backStr);

  // Inner clip if present
  if (bellowStep>Geometry::zeroTol)
    {

      if (innerLayer)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 7");
	  makeCell("MainPipe",System,cellIndex++,feMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 17");
	  makeCell("Cladding",System,cellIndex++,bellowMat,0.0,Out);
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -21 -17 7 ");
	  makeCell("FrontClip",System,cellIndex++,feMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,buildIndex," -12 22 -17 7 ");
	  makeCell("BackClip",System,cellIndex++,feMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 7");
	  makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
	}
 
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -21 -27 17");
      makeCell("FrontSpaceVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," -12 22 -27 17");
      makeCell("BackSpaceVoid",System,cellIndex++,0,0.0,Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -22 -27 7 ");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
    }
  
  
  // outer boundary [flange front]
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -107 ");
  addOuterSurf(Out+frontStr);
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 12 -207 ");
  addOuterUnionSurf(Out+backStr);

  
  // outer boundary mid tube
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 11 -12 -27");
  addOuterUnionSurf(Out);
  */
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

  // stuff for intersection
  /*
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));
  
  FixedComp::setConnect(7,Origin-Z*(radius+bellowThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+bellowThick),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+27));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+27));

  // pipe wall
  FixedComp::setConnect(9,Origin-Z*(radius+feThick),-Z);
  FixedComp::setConnect(10,Origin+Z*(radius+feThick),Z);
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+17));
  */  

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
