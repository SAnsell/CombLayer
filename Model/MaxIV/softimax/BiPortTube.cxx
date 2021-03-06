/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/BiPortTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <memory>

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
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
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
#include "doublePortItem.h"
#include "BiPortTube.h"

namespace constructSystem
{

BiPortTube::BiPortTube(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),

  delayPortBuild(0),portConnectIndex(1),
  rotAxis(0,1,0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"FlangeA");
  nameSideIndex(3,"FlangeB");
}


BiPortTube::~BiPortTube()
  /*!
    Destructor
  */
{}

void
BiPortTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BiPortTube","populate");

  FixedRotate::populate(Control);

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

  flangeACapThick=Control.EvalDefPair<double>(keyName+"FlangeACapThick",
					 keyName+"FlangeCapThick",0.0);
  flangeBCapThick=Control.EvalDefPair<double>(keyName+"FlangeBCapThick",
					 keyName+"FlangeCapThick",0.0);

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  capMat=ModelSupport::EvalDefMat<int>(Control,keyName+"FlangeCapMat",wallMat);

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  double L,R,W,FR,FT,CT,LExt,RB;
  int CMat;
  int OFlag;
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+std::to_string(i);
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalTail<Geometry::Vec3D>(portName,portBase,"Axis");

      L=Control.EvalTail<double>(portName,portBase,"Length");
      R=Control.EvalTail<double>(portName,portBase,"Radius");
      W=Control.EvalTail<double>(portName,portBase,"Wall");
      FR=Control.EvalTail<double>(portName,portBase,"FlangeRadius");
      FT=Control.EvalTail<double>(portName,portBase,"FlangeLength");
      CT=Control.EvalDefTail<double>(portName,portBase,"CapThick",0.0);
      CMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"CapMat",portBase+"CapMat",capMat);

      OFlag=Control.EvalDefVar<int>(portName+"OuterVoid",0);
      // Key two variables to get a DoublePort:
      LExt=Control.EvalDefTail<double>
	(portName,portBase,"ExternPartLength",-1.0);
      RB=Control.EvalDefTail<double>
	(portName,portBase,"RadiusB",-1.0);

      std::shared_ptr<portItem> windowPort;

      if (LExt>Geometry::zeroTol && RB>R+Geometry::zeroTol)
	{
	  std::shared_ptr<doublePortItem> doublePort
	    =std::make_shared<doublePortItem>(portName);
	  doublePort->setLarge(LExt,RB);
	  windowPort=doublePort;
	}
      else
	windowPort=std::make_shared<portItem>(portName);

      if (OFlag) windowPort->setWrapVolume();
      windowPort->setMain(L,R,W);
      windowPort->setFlange(FR,FT);
      windowPort->setCoverPlate(CT,CMat);
      windowPort->setMaterial(voidMat,wallMat);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
      OR.addObject(windowPort);
    }
  return;
}

void
BiPortTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("BiPortTube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  applyPortRotation();

  return;
}

void
BiPortTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BiPortTube","createSurfaces");

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
  SurfMap::addSurf("VoidCyl",-SMap.realSurf(buildIndex+7));

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  SurfMap::addSurf("OuterCyl",SMap.realSurf(buildIndex+17));

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0-(flangeALength+flangeACapThick)),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(length/2.0-(flangeBLength+flangeBCapThick)),Y);

  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin-Y*(length/2.0-flangeACapThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(length/2.0-flangeBCapThick),Y);

  // flange:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
BiPortTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BiPortTube","createObjects");

  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  const std::string frontVoidSurf=
    (flangeACapThick<Geometry::zeroTol) ? frontSurf :
    ModelSupport::getComposite(SMap,buildIndex," 201 ");
  const std::string backVoidSurf=
    (flangeBCapThick<Geometry::zeroTol) ? backSurf :
    ModelSupport::getComposite(SMap,buildIndex," -202 ");


  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+
	   frontVoidSurf+backVoidSurf);
  // main walls
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,
	   Out+frontVoidSurf+backVoidSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 -101 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontVoidSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -207 102 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backVoidSurf);


  if (flangeACapThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -201 -107 ");
      makeCell("FrontCap",System,cellIndex++,capMat,0.0,Out+frontSurf);
    }

  if (flangeBCapThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 202 -207 ");
      makeCell("BackCap",System,cellIndex++,capMat,0.0,Out+backSurf);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -17 ");
  addOuterSurf("Main",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -101 ");
  addOuterSurf("FlangeA",Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -207 102 ");
  addOuterSurf("FlangeB",Out+backSurf);

  return;
}

void
BiPortTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("BiPortTube","createLinks");

  // port centre
  FrontBackCut::createFrontLinks(*this,Origin,Y);
  FrontBackCut::createBackLinks(*this,Origin,Y);
  // getlinke points
  FixedComp::setConnect(2,FixedComp::getLinkPt(1),-Y);
  FixedComp::setConnect(3,FixedComp::getLinkPt(2),Y);

  // make a composite flange
  std::string Out;
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -107 ");
  FixedComp::setLinkComp(2,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -207 ");
  FixedComp::setLinkComp(3,Out+backSurf);

  // inner links
  int innerFrontSurf, innerBackSurf;
  Geometry::Vec3D innerFrontVec, innerBackVec;
  if (flangeACapThick<Geometry::zeroTol)
    {
      innerFrontSurf = getFrontRule().getPrimarySurface();
      innerFrontVec = Origin-Y*(length/2.0);
    } else
    {
      innerFrontSurf = buildIndex+201;
      innerFrontVec = Origin-Y*(length/2.0-flangeACapThick);
    }

  if (flangeBCapThick<Geometry::zeroTol)
    {
      innerBackSurf  = getBackRule().getPrimarySurface();
      innerBackVec  = Origin+Y*(length/2.0);
    } else
    {
      innerBackSurf  = buildIndex+202;
      innerBackVec  = Origin+Y*(length/2.0-flangeBCapThick);
    }

  FixedComp::setConnect(4,innerFrontVec,Y);
  FixedComp::setLinkSurf(4,innerFrontSurf);
  nameSideIndex(4,"InnerFront");

  FixedComp::setConnect(5,innerBackVec,Y);
  FixedComp::setLinkSurf(5,-innerBackSurf);
  nameSideIndex(5,"InnerBack");

  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+7));
  nameSideIndex(6,"InnerSide");


  return;
}


void
BiPortTube::createPorts(Simulation& System)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("BiPortTube","createPorts");

  for(size_t i=0;i<Ports.size();i++)
    {
      const attachSystem::ContainedComp& CC=getCC("Main");
      for(const int CN : CC.getInsertCells())
	  Ports[i]->addOuterCell(CN);

      for(const int CN : portCells)
	Ports[i]->addOuterCell(CN);

      Ports[i]->setCentLine(*this,PCentre[i],PAxis[i]);
      ELog::EM<<"CAlL to PORTS construct :: OLD METHOD"<<ELog::endWarn;
      //      Ports[i]->constructTrack(System);
    }
  return;
}

const portItem&
BiPortTube::getPort(const size_t index) const
  /*!
    Accessor to ports
    \param index :: index point
    \return port
  */
{
  ELog::RegMethod RegA("BiPortTube","getPort");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"index/Ports size");

  return *(Ports[index]);
}


void
BiPortTube::addInsertPortCells(const int CN)
  /*!
    Add a cell to the ports insert list
    \param CN :: Cell number
  */
{
  portCells.insert(CN);
  return;
}


void
BiPortTube::intersectPorts(Simulation& System,
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
  ELog::RegMethod RegA("BiPortTube","intersectPorts");

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
BiPortTube::intersectVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("BiPortTube","intersectPorts");

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
BiPortTube::setPortRotation(const size_t index,
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
  ELog::RegMethod RegA("BiPortTube","setPortRotation");

  portConnectIndex=index;
  if (portConnectIndex>1)
    rotAxis=RAxis.unit();

  return;
}

void
BiPortTube::applyPortRotation()
  /*!
    Apply a rotation to all the PCentre and the
    PAxis of the ports
  */
{
  ELog::RegMethod RegA("BiPortTube","applyPortRotation");

  if (!portConnectIndex) return;
  if (portConnectIndex>Ports.size()+3)
    throw ColErr::IndexError<size_t>
      (portConnectIndex,Ports.size()+3,"PI exceeds number of Ports+3");

  // create extra link:
  nameSideIndex(7,"OrgOrigin");
  const Geometry::Vec3D YOriginal=Y;


  Geometry::Vec3D YPrime(0,-1,0);
  if (portConnectIndex<3)
    {
      Origin+=Y*(length/2.0);
      if (portConnectIndex==2)
	{
	  Y*=1;
	  X*=-1;
	}
      FixedComp::setConnect(7,Origin,YOriginal);

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

      // This moves in the new Y direction
      const Geometry::Vec3D offset=calcCylinderDistance(pIndex);
      Origin+=offset;
      FixedComp::setConnect(7,Origin,YOriginal);
    }

  return;
}

Geometry::Vec3D
BiPortTube::calcCylinderDistance(const size_t pIndex) const
  /*!
    Calculate the shift vector
    \param pIndex :: Port index [0-NPorts]
    \return the directional vector from the port origin
    to the pipetube surface
   */
{
  ELog::RegMethod RegA("BiPortTube","calcCylinderDistance");

  if (pIndex>Ports.size())
    throw ColErr::IndexError<size_t>
      (pIndex,Ports.size(),"PI exceeds number of Ports");

  // No Y point so no displacement
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
  const double ELen=Ports[pIndex]->getExternalLength();
  const Geometry::Cylinder mainC(0,Geometry::Vec3D(0,0,0),Y,R);

  const Geometry::Vec3D RPoint=
    SurInter::getLinePoint(PC,PA,&mainC,CPoint-PA*ELen);

  return RPoint-PA*ELen - PC*2.0;
}


int
BiPortTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("BiPortTube","splitVoidPorts<vec>");

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
BiPortTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("BiPortTube","splitVoidPorts");

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


  return (cells.empty()) ? CN : cells.back()+1;
}

int
BiPortTube::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("BiPortTube","splitVoidPorts");

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
BiPortTube::insertAllInCell(Simulation& System,const int cellN)
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
BiPortTube::insertAllInCell(Simulation& System,
			  const std::vector<int>& cellVec)
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
BiPortTube::insertMainInCell(Simulation& System,const int cellN)
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
BiPortTube::insertMainInCell(Simulation& System,
			   const std::vector<int>& cellVec)
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
BiPortTube::insertPortInCell(Simulation& System,
			   const std::vector<std::set<int>>& cellVec)
  /*!
    Allow ports to be intersected into arbitary cell list
    \param System :: Simulation to use
    \param cellVec :: Sets of cellnumbers corresponding to each port (in order)
    for which the port will be inserted into.
  */
{
  ELog::RegMethod RegA("BiPortTube","insertPortInCell");

  for(size_t index=0;index<Ports.size() && index<cellVec.size();index++)
    {
      const std::set<int>& cellSet=cellVec[index];
      for(const int CN : cellSet)
	Ports[index]->insertInCell(System,CN);
    }
  return;
}


void
BiPortTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BiPortTube","createAll(FC)");

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
