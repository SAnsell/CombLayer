/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PipeTube.cxx
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
#include "Line.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SurInter.h"
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
#include "PipeTube.h"

namespace constructSystem
{

PipeTube::PipeTube(const std::string& Key) :
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedSpace(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  delayPortBuild(0),portConnectIndex(1),
  rotAxis(0,1,0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

  
PipeTube::~PipeTube() 
  /*!
    Destructor
  */
{}

void
PipeTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PipeTube","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

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
  int PMat;
  int OFlag;
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
PipeTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("PipeTube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  applyPortRotation();

  return;
}


void
PipeTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PipeTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }
  
  
  // void space:
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0-flangeALength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(length/2.0-flangeBLength),Y);

  // flange:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);
  
  return;
}

void
PipeTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PipeTube","createObjects");

  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontSurf+backSurf);
  // main walls
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,Out+frontSurf+backSurf);


  // flanges
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 -101 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -207 102 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backSurf);

  
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -17 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -101 ");
  addOuterUnionSurf(Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," -207 102 ");
  addOuterUnionSurf(Out+backSurf);

  return;
}

void
PipeTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("PipeTube","createLinks");

  // port centre
  
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  
  return;
}

  
void
PipeTube::createPorts(Simulation& System)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("PipeTube","createPorts");

  for(size_t i=0;i<Ports.size();i++)
    {
      if (getBuildCell())
	Ports[i].addOuterCell(getBuildCell());
      else
	for(const int CN : insertCells)
	  Ports[i].addOuterCell(CN);

      for(const int CN : portCells)
	Ports[i].addOuterCell(CN);
      
      Ports[i].setCentLine(*this,PCentre[i],PAxis[i]);
      Ports[i].constructTrack(System);
    }
  return;
}

const portItem&
PipeTube::getPort(const size_t index) const
  /*!
    Accessor to ports
    \param index :: index point
    \return port
  */
{
  ELog::RegMethod RegA("PipeTube","getPort");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"index/Ports size");
     
  return Ports[index];
}


void
PipeTube::addInsertPortCells(const int CN) 
  /*!
    Add a cell to the ports insert list
    \param CN :: Cell number
  */
{
  portCells.insert(CN);
  return;
}
  

void
PipeTube::intersectPorts(Simulation& System,
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
  ELog::RegMethod RegA("PipeTube","intersectPorts");

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
PipeTube::setPortRotation(const size_t index,
			  const Geometry::Vec3D& RAxis)
  /*!
    Set Port rotation 
    \param index 
        -0 : No rotation / no shift
        - 1,2 : main ports 
        - 3-N+2 : Extra Ports
    \param RAxis :: Rotation axis expresses in local X,Y,Z
  */
{
  ELog::RegMethod RegA("PipeTube","setPortRotation");
  
  portConnectIndex=index;
  if (portConnectIndex>1)
    rotAxis=RAxis.unit();
  return;
}

void
PipeTube::applyPortRotation()
  /*!
    Apply a rotation to all the PCentre and the 
    PAxis of the ports
  */
{
  ELog::RegMethod RegA("PipeTube","applyPortRotation");

  if (!portConnectIndex) return;
  if (portConnectIndex>Ports.size()+3)
    throw ColErr::IndexError<size_t>
      (portConnectIndex,Ports.size()+3,"PI exceeds number of Ports+3");

  Geometry::Vec3D YPrime(0,-1,0);
  if (portConnectIndex<3)
    {
      Origin+=Y*(length/2.0);
      if (portConnectIndex==2)
	{
	  Y*=1;
	  X*=-1;
	}
      return;
    }
  else
    {
      const size_t pIndex=portConnectIndex-3;
      YPrime=PAxis[pIndex].unit();
      const Geometry::Quaternion QV=
	Geometry::Quaternion::calcQVRot(Geometry::Vec3D(0,1,0),YPrime,rotAxis);
      
      // Now move QV into the main basis set origin:
      const Geometry::Vec3D& QVvec=QV.getVec();
      const Geometry::Vec3D QAxis=X*QVvec.X()+
	Y*QVvec.Y()+Z*QVvec.Z();
      
      const Geometry::Quaternion QVmain(QV[0],QAxis);  
      QVmain.rotate(X);
      QVmain.rotate(Y);
      QVmain.rotate(Z);
      const Geometry::Vec3D offset=calcCylinderDistance(pIndex);
      Origin+=offset;
    }

  return;
}

Geometry::Vec3D
PipeTube::calcCylinderDistance(const size_t pIndex) const
  /*!
    Calculate the shift vector
    \param pIndex :: Port index [0-NPorts]
   */
{
  ELog::RegMethod RegA("PipeTube","calcCylinderDistance");
  
  if (pIndex>Ports.size())
    throw ColErr::IndexError<size_t>
      (pIndex,Ports.size(),"PI exceeds number of Ports");

  const Geometry::Vec3D PC=
    X*PCentre[pIndex].X()+Y*PCentre[pIndex].Y()+Z*PCentre[pIndex].Z();
  const Geometry::Vec3D PA=
    X*PAxis[pIndex].X()+Y*PAxis[pIndex].Y()+Z*PAxis[pIndex].Z();

  const Geometry::Line CylLine(Geometry::Vec3D(0,0,0),Y);
  const Geometry::Line PortLine(PC,PA);

  Geometry::Vec3D CPoint;
  std::tie(CPoint,std::ignore)=CylLine.closestPoints(PortLine);
  // calc external impact point:

  const double R=radius+wallThick;
  const double ELen=Ports[pIndex].getExternalLength();
  const Geometry::Cylinder mainC(0,Geometry::Vec3D(0,0,0),Y,R);
  
  const Geometry::Vec3D RPoint=
    SurInter::getLinePoint(PC,PA,&mainC,CPoint-PA*ELen);
  
  return RPoint-PA*ELen;
}


int
PipeTube::splitVoidPorts(Simulation& System,
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
    \param portVec :: all ports to work with
   */
{
  ELog::RegMethod RegA("PipeTube","splitVoidPorts<vec>");

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
      SplitAxis.push_back(Geometry::Vec3D(0,1,0));
    }

  const std::vector<int> cells=
    FixedComp::splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);
  
  if (!splitName.empty())
    for(const int CN : cells)
      CellMap::addCell(splitName,CN);


  return (cells.empty()) ? CN : cells.back()+1;
}

int
PipeTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("PipeTube","splitVoidPorts");

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
  
  return (cells.empty()) ? CN : cells.back()+1;
}

int
PipeTube::splitVoidPorts(Simulation& System,
			 const std::string& splitName,
			 const int offsetCN,
			 const int CN)
  /*!
    Split the void cell and store divition planes
    Only use those port that a close to orthogonal with Y axis
    \param System :: Simulation to use
    \param splitName :: Name for cell output
    \param offsetCN :: output offset number
    \param CN :: Cell number to split
   */
{
  ELog::RegMethod RegA("PipeTube","splitVoidPorts");

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  size_t preFlag(0);
  for(size_t i=0;i<PCentre.size();i++)
    {
      if (Ports[i].getY().dotProd(Y)<Geometry::zeroTol)
	{
	  if (preFlag)
	    {
	      const Geometry::Vec3D CPt=
		(PCentre[preFlag-1]+PCentre[i])/2.0;
	      SplitOrg.push_back(CPt);
	      SplitAxis.push_back(Geometry::Vec3D(0,1,0));
	    }
	  preFlag=i+1;
	}
    }
  
  const std::vector<int> cells=
    FixedComp::splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);

  if (!splitName.empty())
    for(const int CN : cells)
      CellMap::addCell(splitName,CN);
  
  return (cells.empty()) ? CN : cells.back()+1;
}

void
PipeTube::insertInCell(Simulation& System,const int cellN)
  /*!
    Overload of containdComp so that the ports can also 
    be inserted if needed
  */
{
  ContainedComp::insertInCell(System,cellN);
  if (!delayPortBuild)
    {
      for(const portItem& PC : Ports)
	PC.insertInCell(System,cellN);
    }
  return;
}
  
  
void
PipeTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("PipeTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  
  insertObjects(System);
  if (!delayPortBuild)
    createPorts(System);

  return;
}
  
}  // NAMESPACE constructSystem
