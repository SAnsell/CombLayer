/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/TwinPipe.cxx
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedOffsetUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurfMap.h"

#include "subPipeUnit.h"
#include "MultiPipe.h"

namespace tdcSystem
{

MultiPipe::MultiPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedGroup("Flange","Pipes"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

MultiPipe::~MultiPipe() 
  /*!
    Destructor
  */
{}

void
MultiPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("MultiPipe","populate");
  
  FixedRotate::populate(Control);

  const size_t NPipes=Control.EvalVar<size_t>(keyName+"NPipes");

  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");

  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  pipes.clear();
  for(size_t i=0;i<NPipes;i++)
    {
      const std::string keyA=keyName+"SubPipe"+std::to_string(i);
      subPipeUnit A(keyA);
      A.populate(Control);
      pipes.push_back(A);
    }
  
  return;
}

void
MultiPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("MultiPipe","createSurfaces");
  
  // Inner void
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  makeShiftedSurf(SMap,"front",buildIndex+11,Y,flangeLength);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeRadius);
  
  int BI(buildIndex+100);
  size_t index(1);
  for(const subPipeUnit& PU : pipes)
    {
      attachSystem::FixedOffsetUnit pipeFC(PU.keyName,Origin,Y,Z);
      pipeFC.setOffset(PU.xStep,0,PU.zStep);
      pipeFC.setRotation(PU.xyAngle,PU.zAngle);
      pipeFC.applyOffset();
      
      const Geometry::Vec3D pOrg=pipeFC.getCentre();
      const Geometry::Vec3D pY=pipeFC.getY();
      ModelSupport::buildPlane(SMap,BI+1,pOrg+pY*PU.length,pY);
      ModelSupport::buildPlane
	(SMap,BI+11,pOrg+pY*(PU.length-PU.flangeLength),pY);
      
      ModelSupport::buildCylinder(SMap,BI+7,pOrg,pY,PU.radius);
      ModelSupport::buildCylinder
	(SMap,BI+17,pOrg+pY*PU.length,pY,PU.radius+PU.thick);    
      ModelSupport::buildCylinder
	(SMap,BI+27,pOrg+pY*PU.length,pY,PU.flangeRadius);

      FixedComp::setConnect(index,pOrg+pY*PU.length,pY);
      FixedComp::setLinkSurf(index,SMap.realSurf(BI+1));

      index++;
      BI+=100;
    }
  
  return;
}

void
MultiPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MultiPipe","createObjects");

  std::string Out;
  const std::string frontStr=getRuleStr("front");
  

  HeadRule pipeExclude;
  HeadRule pipeTotal;
  int BI(buildIndex+100);
  // pipe exclude +/- other outer zone
  const int BImax(static_cast<int>(pipes.size())*100+buildIndex);
  for(size_t i=0;i<pipes.size();i++)
    {
      const subPipeUnit& pUnit(pipes[i]);
      
      const int BPrev((i) ? BI-100 : BImax);
      const int BNext((BImax==BI) ? buildIndex+100 : BI+100);
      const std::string cutOuter=
	ModelSupport::getComposite(SMap,BPrev,BNext," 17 17M ");
      
      Out=ModelSupport::getComposite(SMap,BI," -7 -1 ");
      makeCell("PipeVoid",System,cellIndex++,pUnit.voidMat,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,BI," 11 -17M 7M -1M ");
      makeCell("PipeWall",System,cellIndex++,pUnit.wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,BI," -27M 17M -1M 11M ");
      makeCell("PipeFlange",System,cellIndex++,pUnit.flangeMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,BI," -27M 17M -11M 11 ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,BI-100," 27  ");
      makeCell("PipeOuter",System,cellIndex++,0,0.0,Out+cutOuter);
      
      pipeExclude.addIntersection(SMap.realSurf(BI+7));
      Out=ModelSupport::getComposite(SMap,BI," -27 -1 ");
      pipeTotal.addUnion(Out);

      BI+=100;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -7 ");
  makeCell("Flange",System,cellIndex++,flangeMat,0.0,
	   Out+frontStr+pipeExclude.display());

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -7 ");
  addOuterSurf("Flange",Out+frontStr);

  if (!pipes.empty())
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"11 ");
      pipeTotal.addIntersection(Out);
      addOuterSurf("Pipes",pipeTotal);
    }
  
  return;
}

void
MultiPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("MultiPipe","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,-Y);
  // Note outer links done in 

  return;
}
    
void
MultiPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("MultiPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
