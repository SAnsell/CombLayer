/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/VirtualTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Line.h"
#include "Cylinder.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "portItem.h"
#include "anglePortItem.h"
#include "doublePortItem.h"
#include "portBuilder.h"
#include "VirtualTube.h"

namespace constructSystem
{

VirtualTube::VirtualTube(const std::string& Key) :
  attachSystem::FixedRotate(Key,15),
  attachSystem::ContainedGroup("Main"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),

  outerVoid(0),delayPortBuild(0),portConnectIndex(1),
  rotAxis(0,1,0),zNorm(0),postZAxis(0,0,1),
  postYRotation(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


VirtualTube::~VirtualTube()
  /*!
    Destructor
  */
{}

void
VirtualTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VirtualTube","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  capMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeCapMat",wallMat);

  populatePort(Control,keyName,PCentre,PAxis,Ports);
  return;
}

void
VirtualTube::createPorts(Simulation& System,
			 MonteCarlo::Object* insertObj,
			 const HeadRule& innerSurf,
			 const HeadRule& outerSurf)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
    \param insertObj :: Object to insert port cut into
    \param innerSurf :: HeadRule to inner surf [outward]
    \param outerSurf :: HeadRule to outer surf [outward]
   */
{
  ELog::RegMethod RegA("VirtualTube","createPorts(Obj,HR,HR)");

  for(size_t i=0;i<Ports.size();i++)
    {
      const attachSystem::ContainedComp& CC=getCC("Main");
      for(const int CN : CC.getInsertCells())
	Ports[i]->addInsertCell(CN);

      for(const int CN : portCells)
	Ports[i]->addInsertCell(CN);

      Ports[i]->setCentLine(*this,PCentre[i],PAxis[i]);
      Ports[i]->constructTrack(System,insertObj,innerSurf,outerSurf);

      if (zNorm)
	Ports[i]->reNormZ(postZAxis);
  
      if (outerVoid && CellMap::hasCell("OuterVoid"))
       	Ports[i]->addPortCut(CellMap::getCellObject(System,"OuterVoid"));
      Ports[i]->insertObjects(System);
    }

  return;
}

const portItem&
VirtualTube::getPort(const size_t index) const
  /*!
    Accessor to ports
    \param index :: index point
    \return port
  */
{
  ELog::RegMethod RegA("VirtualTube","getPort");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"index/Ports size");

  return *(Ports[index]);
}


void
VirtualTube::addInsertPortCells(const int CN)
  /*!
    Add a cell to the ports insert list
    \param CN :: Cell number
  */
{
  portCells.insert(CN);
  return;
}


void
VirtualTube::intersectPorts(Simulation& System,
			 const size_t aIndex,
			 const size_t bIndex) const
  /*!
    Overlaps two ports if the intersect because of size
    Currently does not check that they don't intersect.
    \param System :: Simulation
    \param aIndex :: Inner port
    \param bIndex :: Outer port
   */
{
  ELog::RegMethod RegA("VirtualTube","intersectPorts");

  if (aIndex==bIndex || aIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(aIndex,Ports.size(),
				     "Port does not exist");
  if (bIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(bIndex,Ports.size(),
				     "Port does not exist");

  Ports[aIndex]->intersectPair(System,*Ports[bIndex]);

  return;
}

void
VirtualTube::intersectVoidPorts(Simulation& System,
			     const size_t aIndex,
			     const size_t bIndex) const
/*!
    Overlaps two ports if the intersect because of size
    Currently does not check that they don't intersect.
    \param System :: Simulation
    \param aIndex :: Inner port
    \param bIndex :: Outer port
   */
{
  ELog::RegMethod RegA("VirtualTube","intersectPorts");

  if (aIndex==bIndex || aIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(aIndex,Ports.size(),
				     "Port does not exist");
  if (bIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(bIndex,Ports.size(),
				     "Port does not exist");

  Ports[aIndex]->intersectVoidPair(System,*Ports[bIndex]);

  return;
}


void
VirtualTube::setPortRotation(const size_t index,
			     const Geometry::Vec3D& RAxis,
			     const double postAngle)
  /*!
    Set Port rotation
    \param index
        -0 : No rotation / no shift
        - 1,2 : main ports
        - 3-N+2 : Extra Ports
    \param RAxis :: Rotation axis expresses in local X,Y,Z
    \param postAngle :: Rotation on Y to correct fractional turn
  */
{
  ELog::RegMethod RegA("VirtualTube","setPortRotation");

  portConnectIndex=index;
  if (portConnectIndex>1)
    rotAxis=RAxis.unit();

  postYRotation=postAngle;
  return;
}

void
VirtualTube::setPortRotation(const size_t index,
			     const Geometry::Vec3D& RAxis,
			     const Geometry::Vec3D& postZVec)
  /*!
    Set Port rotation
    \param index
        -0 : No rotation / no shift
        - 1,2 : main ports
        - 3-N+2 : Extra Ports
    \param RAxis :: Rotation axis expresses in local X,Y,Z
    \param postAngle :: Rotation on Y to correct fractional turn
  */
{
  ELog::RegMethod RegA("VirtualTube","setPortRotation");

  portConnectIndex=index;
  if (portConnectIndex>1)
    rotAxis=RAxis.unit();

  zNorm=1;
  postZAxis=postZVec;
  return;
}

void
VirtualTube::applyPortRotation()
  /*!
    Apply a rotation to all the PCentre and the
    PAxis of the ports
  */
{
  ELog::RegMethod RegA("VirtualTube","applyPortRotation");

  // create extra link:
  nameSideIndex(7,"OrgOrigin");
  FixedComp::setConnect(7,Origin,Y);

  if (!portConnectIndex) return;
  if (portConnectIndex>Ports.size()+3)
    throw ColErr::IndexError<size_t>
      (portConnectIndex,Ports.size()+3,"PI exceeds number of Ports+3");


  if (portConnectIndex<3)
    {
      if (portConnectIndex==2)
	{
	  Y*=-1;
	  X*=-1;
	}
      return;
    }
  
  rotAxis=rotAxis.getInBasis(X,Y,Z);
  postZAxis=postZAxis.getInBasis(X,Y,Z);
  
  // ALL of these point are NOT in the FixedComp referecne basis
  // They are to be multiplied by X,Y,Z and shifed by Origin.
  const size_t pIndex=portConnectIndex-3;
  const portItem& PI=getPort(pIndex);
  const Geometry::Vec3D portAxis=PAxis[pIndex].unit();
  const Geometry::Vec3D portCentre=PCentre[pIndex];
  const double pLen=PI.getExternalLength();

  // old Y basis unit
  const Geometry::Vec3D frontShift=-Y*(length/2.0);

  if (std::abs(postYRotation)>Geometry::zeroTol)
    {
      const Geometry::Quaternion QVpost=
	Geometry::Quaternion::calcQRotDeg(postYRotation,Y);
      QVpost.rotate(X);
      QVpost.rotate(Z);
    }

  // retrack y to the port axis
  const Geometry::Quaternion QV=
    Geometry::Quaternion::calcQVRot(Geometry::Vec3D(0,1,0),portAxis,rotAxis);

  // Now move QV into the main basis set origin,X,Y,Z:
  const Geometry::Vec3D& QVvec=QV.getVec();
  const Geometry::Vec3D QAxis=QVvec.getInBasis(X,Y,Z);

  // Move X,Y,Z to the main rotation direction:
  const Geometry::Quaternion QVmain(QV[0],QAxis);
  QVmain.rotateBasis(X,Y,Z);

  const Geometry::Vec3D portOriginB=
    portCentre.getInBasis(X,Y,Z)+
    portAxis.getInBasis(X,Y,Z)*pLen;

  const Geometry::Vec3D diffB=frontShift-portOriginB;
  Origin+=diffB;


  return;
}

int
VirtualTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("VirtualTube","splitVoidPorts<vec>");

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
    {
      for(const int CN : cells)
	CellMap::addCell(splitName,CN);
    }

  return (cells.empty()) ? CN : cells.back()+1;
}

int
VirtualTube::splitVoidPorts(Simulation& System,
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
    \return last cell
   */
{
  ELog::RegMethod RegA("VirtualTube","splitVoidPorts");

  const Geometry::Vec3D Axis=(X*inobjAxis[0]+
			      Y*inobjAxis[1]+
			      Z*inobjAxis[2]).unit();

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  size_t preFlag(0);
  for(size_t i=0;i<PCentre.size();i++)
    {
      // within basis set
      if (Ports[i]->getY().dotProd(Axis)<Geometry::zeroTol)
	{
	  if (preFlag)   // test to have two ports
	    {
	      const Geometry::Vec3D CPt=
		(PCentre[preFlag-1]+PCentre[i])/2.0;
	      SplitOrg.push_back(CPt);
	      SplitAxis.push_back(inobjAxis.unit());
	    }

	  preFlag=i+1;
	}
    }
  const std::vector<int> cells=
    FixedComp::splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);

  if (!splitName.empty())
    for(const int CN : cells)
      CellMap::addCell(splitName,CN);


  return (cells.empty()) ? 0 : cells.back();
}

int
VirtualTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("VirtualTube","splitVoidPorts");

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  size_t preFlag(0);
  for(size_t i=0;i<PCentre.size();i++)
    {
      if (Ports[i]->getY().dotProd(Y)<Geometry::zeroTol)
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
VirtualTube::insertAllInCell(Simulation& System,const int cellN) const
  /*!
    Overload of containdGroup so that the ports can also
    be inserted if needed
    \param System :: Simulation to use
    \param cellN :: Cell for insert
  */
{
  ContainedGroup::insertAllInCell(System,cellN);
  if (!delayPortBuild)
    {
      for(const std::shared_ptr<portItem>& PC : Ports)
	PC->insertInCell(System,cellN);
    }
  return;
}

void
VirtualTube::insertAllInCell(Simulation& System,
			  const std::vector<int>& cellVec) const
  /*!
    Overload of containdGroup so that the ports can also
    be inserted if needed
    \param System :: Simulation to use
    \param cellVec :: Cells for insert
  */
{
  ContainedGroup::insertAllInCell(System,cellVec);

  if (!delayPortBuild)
    {
      for(const std::shared_ptr<portItem>& PC : Ports)
	PC->insertInCell(System,cellVec);
    }
  return;
}

void
VirtualTube::insertMainInCell(Simulation& System,const int cellN) const
  /*!
    Fix of insertInAllCells to only do main body without ports
    \param System :: Simulation to use
    \param cellN :: Cell of insert
  */
{
  ContainedGroup::insertAllInCell(System,cellN);
  return;
}

void
VirtualTube::insertMainInCell(Simulation& System,
			   const std::vector<int>& cellVec) const
  /*!
    Fix of insertInAllCells to only do main body without ports
    \param System :: Simulation to use
    \param cellVec :: Cells of insert
  */
{
  ContainedGroup::insertAllInCell(System,cellVec);
  return;
}

void
VirtualTube::insertPortInCell(Simulation& System,
			      const size_t index,
			      const int cellN) const
  /*!
    Allow ports to be intersected into arbitary cell list
    \param System :: Simulation to use
    \param cellN :: Cell for insert
  */
{
  ELog::RegMethod RegA("VirtualTube","insertPortInCell(cellN)");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"Port index");

  Ports[index]->insertInCell(System,cellN);

  return;
}

void
VirtualTube::insertPortsInCell(Simulation& System,const int cellN) const
  /*!
    Allow ports to be intersected into arbitary cell list
    \param System :: Simulation to use
    \param cellN :: Cell for insert
  */
{
  ELog::RegMethod RegA("VirtualTube","insertPortInCell(cellN)");

  for(const std::shared_ptr<portItem>& PC : Ports)
    PC->insertInCell(System,cellN);

  return;
}

void
VirtualTube::insertPortsInCell(Simulation& System,
			   const std::vector<std::set<int>>& cellVec) const
  /*!
    Allow ports to be intersected into arbitary cell list
    \param System :: Simulation to use
    \param cellVec :: Sets of cellnumbers corresponding to each port (in order)
    for which the port will be inserted into.
  */
{
  ELog::RegMethod RegA("VirtualTube","insertPortInCell");

  for(size_t index=0;index<Ports.size() && index<cellVec.size();index++)
    {
      const std::set<int>& cellSet=cellVec[index];
      for(const int CN : cellSet)
	Ports[index]->insertInCell(System,CN);
    }
  return;
}


void
VirtualTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VirtualTube","createAll(FC)");

  
  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length/2.0);
  applyPortRotation();
  createSurfaces();
  createObjects(System);

  createLinks();
  createPorts(System);
  insertObjects(System);
    
  return;
}

}  // NAMESPACE constructSystem
