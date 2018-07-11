/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PortTube.cxx
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
#include <utility>
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
#include "BaseMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"

#include "portItem.h"
#include "PortTube.h"

namespace constructSystem
{

PortTube::PortTube(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedSpace(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  delayPortBuild(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

PortTube::PortTube(const PortTube& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedSpace(A),
  attachSystem::CellMap(A),attachSystem::FrontBackCut(A),
  radius(A.radius),
  wallThick(A.wallThick),length(A.length),portAXStep(A.portAXStep),
  portAZStep(A.portAZStep),portARadius(A.portARadius),
  portALen(A.portALen),portAThick(A.portAThick),
  portBXStep(A.portBXStep),portBZStep(A.portBZStep),
  portBRadius(A.portBRadius),portBLen(A.portBLen),
  portBThick(A.portBThick),flangeARadius(A.flangeARadius),
  flangeALength(A.flangeALength),flangeBRadius(A.flangeBRadius),
  flangeBLength(A.flangeBLength),voidMat(A.voidMat),
  wallMat(A.wallMat),PCentre(A.PCentre),PAxis(A.PAxis),
  Ports(A.Ports)
  /*!
    Copy constructor
    \param A :: PortTube to copy
  */
{}

PortTube&
PortTube::operator=(const PortTube& A)
  /*!
    Assignment operator
    \param A :: PortTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedSpace::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      wallThick=A.wallThick;
      length=A.length;
      portAXStep=A.portAXStep;
      portAZStep=A.portAZStep;
      portARadius=A.portARadius;
      portALen=A.portALen;
      portAThick=A.portAThick;
      portBXStep=A.portBXStep;
      portBZStep=A.portBZStep;
      portBRadius=A.portBRadius;
      portBLen=A.portBLen;
      portBThick=A.portBThick;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      PCentre=A.PCentre;
      PAxis=A.PAxis;
      Ports=A.Ports;
    }
  return *this;
}


  
PortTube::~PortTube() 
  /*!
    Destructor
  */
{}

void
PortTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PortTube","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  portAXStep=Control.EvalDefVar<double>(keyName+"PortAXStep",0.0);
  portAZStep=Control.EvalDefVar<double>(keyName+"PortAZStep",0.0);
  portARadius=Control.EvalPair<double>(keyName+"PortARadius",
				       keyName+"PortRadius");
  portALen=Control.EvalPair<double>(keyName+"PortALen",
				       keyName+"PortLen");
  portAThick=Control.EvalPair<double>(keyName+"PortAThick",
				       keyName+"PortThick");

  portBXStep=Control.EvalDefVar<double>(keyName+"PortBXStep",0.0);
  portBZStep=Control.EvalDefVar<double>(keyName+"PortBZStep",0.0);
  portBRadius=Control.EvalPair<double>(keyName+"PortBRadius",
				       keyName+"PortRadius");
  portBLen=Control.EvalPair<double>(keyName+"PortBLen",
				       keyName+"PortLen");
  portBThick=Control.EvalPair<double>(keyName+"PortBThick",
				       keyName+"PortThick");


  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  double L,R,W,FR,FT,PT;
  int PMat,OFlag;
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+std::to_string(i);
      portItem windowPort(portName);
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalPair<Geometry::Vec3D>(portName,portBase,"Axis");
      
      L=Control.EvalPair<double>(portName,portBase,"Length");
      R=Control.EvalPair<double>(portName,portBase,"Radius");
      W=Control.EvalPair<double>(portName,portBase,"Wall");
      FR=Control.EvalPair<double>(portName,portBase,"FlangeRadius");
      FT=Control.EvalPair<double>(portName,portBase,"FlangeLength");
      PT=Control.EvalDefPair<double>(portName,portBase,"PlateThick",0.0);
      PMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"PlateMat",portBase+"PlateMat",wallMat);

      OFlag=Control.EvalDefVar<int>(portName+"OuterVoid",0);

      if (OFlag) windowPort.setWrapVolume();

      windowPort.setMain(L,R,W);
      windowPort.setFlange(FR,FT);
      windowPort.setCoverPlate(PT,PMat);
      windowPort.setMaterial(voidMat,wallMat);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
    }					    
  return;
}

void
PortTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("PortTube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  Origin+=
    Y*(portALen+wallThick+length/2.0)-
    X*portAXStep-
    Z*portAZStep;

  return;
}


void
PortTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(portALen+wallThick+length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+101));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+202,
			       Origin+Y*(portBLen+wallThick+length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+202));
    }

  // void space:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  // metal
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  // port
  const Geometry::Vec3D inOrg=Origin+X*portAXStep+Z*portAZStep;
  const Geometry::Vec3D outOrg=Origin+X*portBXStep+Z*portBZStep;

  ModelSupport::buildCylinder(SMap,buildIndex+107,inOrg,Y,portARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,outOrg,Y,portBRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+117,inOrg,Y,
			      portARadius+portAThick);
  ModelSupport::buildCylinder(SMap,buildIndex+217,outOrg,Y,
			      portBRadius+portBThick);

  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-
			   Y*(portALen+wallThick-flangeALength+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+212,Origin+
			   Y*(portBLen+wallThick-flangeBLength+length/2.0),Y);

  // flange:
  ModelSupport::buildCylinder(SMap,buildIndex+127,inOrg,Y,
			      flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+227,outOrg,Y,
			      flangeBRadius);
  
  return;
}

void
PortTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PortTube","createObjects");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  // main walls
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -17 7 -2 ");
  makeCell("MainCylinder",System,cellIndex++,wallMat,0.0,Out);

  // plates front/back
  if ((portARadius+portAThick-(radius+wallThick))< -Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 -17 117 ");
      makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);
    }
  if ((portBRadius+portBThick-radius-wallThick)< -Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 217 ");
      makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
    }

  // port:
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -1  -107 ");
  makeCell("FrontPortVoid",System,cellIndex++,voidMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  makeCell("BackPortVoid",System,cellIndex++,voidMat,0.0,Out+backSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 107 -117");
  makeCell("FrontPortWall",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 207 -217 ");
  makeCell("BackPortWall",System,cellIndex++,wallMat,0.0,Out+backSurf);

  // flanges
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 117 -127 ");
  makeCell("FrontPortWall",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 212 217 -227 ");
  makeCell("BackPortWall",System,cellIndex++,wallMat,0.0,Out+backSurf);



  
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"-11 -127 (-117:-111) ");
  addOuterUnionSurf(Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ( -217:212 ) ");
  addOuterUnionSurf(Out+backSurf);
  return;
}

void
PortTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createLinks");

  // port centre
  const Geometry::Vec3D inOrg=Origin+X*portAXStep+Z*portAZStep;
  const Geometry::Vec3D outOrg=Origin+X*portBXStep+Z*portBZStep;
  
  FrontBackCut::createFrontLinks(*this,inOrg,Y); 
  FrontBackCut::createBackLinks(*this,outOrg,Y);  
  
  return;
}

  
void
PortTube::createPorts(Simulation& System)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("PortTube","createPorts");

  for(size_t i=0;i<Ports.size();i++)
    {
      if (!delayPortBuild && getBuildCell())
	Ports[i].addOuterCell(getBuildCell());
      else
	{
	  for(const int CN : insertCells)
	    Ports[i].addOuterCell(CN);
	}
      
      
      for(const int CN : portCells)
	Ports[i].addOuterCell(CN);

      Ports[i].setCentLine(*this,PCentre[i],PAxis[i]);
      Ports[i].constructTrack(System);
    }
  return;
}

const portItem&
PortTube::getPort(const size_t index) const
  /*!
    Accessor to ports
    \param index :: index point
    \return port
  */
{
  ELog::RegMethod RegA("PortTube","getPort");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"index/Ports size");

      
  return Ports[index];
}


void
PortTube::addInsertPortCells(const int CN)
  /*!
    Add a cell to the ports insert list
    \param CN :: Cell number
  */
{
  portCells.insert(CN);
  return;
}

void
PortTube::splitVoidPorts(Simulation& System,
			 const std::string& splitName,
			 const int offsetCN,const int CN,
			 const std::vector<size_t>& portVec)
  /*!
    Split the void cell and store divition planes
    Only use those port that a close to orthogonal with Y axis
    \param System :: Simulation to use
    \param splitName :: Name for cell output  [new]
    \param offsetCN :: output offset number  [new]
    \param CN :: Cell number to split
    \param portPair :: Standard pair
   */
{
  ELog::RegMethod RegA("PortTube","splitVoidPorts");

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  for(size_t i=1;i<portVec.size();i+=2)
    {
      const size_t AIndex=portVec[i-1];
      const size_t BIndex=portVec[i];
      
      if (AIndex==BIndex || AIndex>=PCentre.size() ||
	  BIndex>=PCentre.size())
	throw ColErr::IndexError<size_t>
	  (AIndex,BIndex,"Port number too large for"+keyName);
      
      const Geometry::Vec3D CPt=
	(PCentre[AIndex]+PCentre[BIndex])/2.0;
      SplitOrg.push_back(CPt);
      SplitAxis.push_back(Y);
    }
 

  const std::vector<int> cells=
    FixedComp::splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);
      
  if (!splitName.empty())
  for(const int CN : cells)
    CellMap::addCell(splitName,CN);


  return;
}

void
PortTube::splitVoidPorts(Simulation& System,
			 const std::string& splitName,
			 const int offsetCN,
			 const int CN,
			 const Geometry::Vec3D& inobjAxis)
  /*!
    Split the void cell and store divition planes
    Only use those port that a close to orthogonal with Y axis
    \param System :: Simulation to use
    \param splitName :: Name for cell output
    \param offsetCN :: output offset number
    \param CN :: Cell number to split
    \param inobjAxis :: axis to split pot on
   */
{
  ELog::RegMethod RegA("PortTube","splitVoidPorts");

  const Geometry::Vec3D Axis=(X*inobjAxis[0]+
			      Y*inobjAxis[1]+
			      Z*inobjAxis[2]).unit();
  
  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  size_t preFlag(0);
  for(size_t i=0;i<PCentre.size();i++)
    {
      // within basis set
      if (Ports[i].getY().dotProd(inobjAxis)<Geometry::zeroTol)
	{
	  if (preFlag)   // test to have two ports
	    {
	      const Geometry::Vec3D CPt=
		(PCentre[preFlag-1]+PCentre[i])/2.0;
	      SplitOrg.push_back(CPt);
	      SplitAxis.push_back(Axis);
	    }
	  preFlag=i+1;
	}
    }
  
  const std::vector<int> cells=
    FixedComp::splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);

  if (!splitName.empty())
  for(const int CN : cells)
    CellMap::addCell(splitName,CN);
  
  return;
}

void
PortTube::intersectPorts(Simulation& System,
			 const size_t aIndex,
			 const size_t bIndex) const
  /*!
    Overlaps two ports if the intersect because of size
    Currently does not check that they don't intersect.
    \param System :: Simulation
    \param aIndex :: Inner port
    \param aIndex :: Outer port
   */
{
  ELog::RegMethod RegA("PortTube","intersectPorts");

  if (aIndex==bIndex || aIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(aIndex,Ports.size(),
				     "Port does not exist");
  if (bIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(bIndex,Ports.size(),
				     "Port does not exist");

  Ports[aIndex].intersectPair(System,Ports[bIndex]);
  
  return;
}
  
void
PortTube::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("PortTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);

  createLinks();
  insertObjects(System);

  if (!delayPortBuild)
    createPorts(System);
  else
    ELog::EM<<"DElay"<<ELog::endDiag;
  
  return;
}
  
}  // NAMESPACE constructSystem
