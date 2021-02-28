/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/portSet.cxx
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Importance.h"
#include "Object.h"

#include "portItem.h"
#include "doublePortItem.h"
#include "portSet.h"

namespace constructSystem
{

portSet::portSet(attachSystem::FixedComp& FC)  :
  FUnit(FC),
  cellPtr(dynamic_cast<attachSystem::CellMap*>(&FC)),
  outerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


portSet::~portSet() 
  /*!
    Destructor
  */
{}

void
portSet::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("portSet","populate");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const std::string keyName(FUnit.getKeyName());

  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  double L,R,W,FR,FT,CT,LExt,RB;
  int WMat,CMat,VMat;
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

      VMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"VoidMat",portBase+"VoidMat",0);
      WMat=ModelSupport::EvalMat<int>
	(Control,portName+"WallMat",portBase+"WallMat");
      CMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"CapMat",portBase+"CapMat",WMat);

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
      windowPort->setMaterial(VMat,WMat);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
      OR.addObject(windowPort);
    }					    
  return;
}
  

const portItem&
portSet::getPort(const size_t index) const
  /*!
    Accessor to ports
    \param index :: index point
    \return port
  */
{
  ELog::RegMethod RegA("portSet","getPort");

  if (index>=Ports.size())
    throw ColErr::IndexError<size_t>(index,Ports.size(),"index/Ports size");
     
  return *(Ports[index]);
}


void
portSet::addInsertPortCells(const int CN) 
  /*!
    Add a cell to the ports insert list
    \param CN :: Cell number
  */
{
  portCells.insert(CN);
  return;
}
  

void
portSet::intersectPorts(Simulation& System,
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
  ELog::RegMethod RegA("portSet","intersectPorts");

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
portSet::intersectVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("portSet","intersectPorts");

  if (aIndex==bIndex || aIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(aIndex,Ports.size(),
				     "Port does not exist");
  if (bIndex>=Ports.size())
    throw ColErr::IndexError<size_t>(bIndex,Ports.size(),
				     "Port does not exist");

  Ports[aIndex]->intersectVoidPair(System,*Ports[bIndex]);
  
  return;
}




template<typename T>
int
portSet::procSplit(Simulation& System,const std::string& splitName,
		   const int offsetCN,const int CN,
		   const T& SplitOrg,const T& SplitAxis)
  /*!
    Process split
    \param System ::Simulation
    \param splitName :: 
    \param offsetCN :: New cell numbers 
    \param CN :: Cell to split
    \param SplitOrg :: Vec3D / container of Vec3D for origin(s)
    \param SplitAxis :: Vec3D / container of Vec3D for plane-normal(s)
   */
{
  ELog::RegMethod RegA("portSet","procSplit");

  const std::vector<int> cells=
    FUnit.splitObject(System,offsetCN,CN,SplitOrg,SplitAxis);

  attachSystem::CellMap* CPtr=
    dynamic_cast<attachSystem::CellMap*>(&FUnit);
  if (CPtr)
    {
      if (!splitName.empty())
	for(const int CN : cells)
	  CPtr->addCell(splitName,CN);
    }

  return (cells.empty()) ? CN : cells.back()+1;
}

int
portSet::splitVoidPorts(Simulation& System,
			 const std::string& splitName,
			 const int offsetCN,const int CN,
			 const std::vector<size_t>& portVec)
  /*!
    Split the void cell and store division planes
    Only use those port that a close to orthogonal with Y axis
    \param System :: Simulation to use
    \param splitName :: Name for cell output  [new]
    \param offsetCN :: output offset number  [new]
    \param CN :: Cell number to split
    \param portVec :: all ports to work with
   */
{
  ELog::RegMethod RegA("portSet","splitVoidPorts<vec>");

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  for(size_t i=1;i<portVec.size();i+=2)
    {
      const size_t AIndex=portVec[i-1];
      const size_t BIndex=portVec[i];
      
      if (AIndex==BIndex || AIndex>=PCentre.size() ||
	  BIndex>=PCentre.size())
	throw ColErr::IndexError<size_t>
	  (AIndex,BIndex,"Port number too large for"+FUnit.getKeyName());
      
      const Geometry::Vec3D CPt=
	(PCentre[AIndex]+PCentre[BIndex])/2.0;
      SplitOrg.push_back(CPt);
      SplitAxis.push_back(Geometry::Vec3D(0,1,0));
    }

  return procSplit(System,splitName,offsetCN,CN,SplitOrg,SplitAxis);
}



int
portSet::splitVoidPorts(Simulation& System,
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
  ELog::RegMethod RegA("portSet","splitVoidPorts");

  const Geometry::Vec3D& X=FUnit.getX();
  const Geometry::Vec3D& Y=FUnit.getY();
  const Geometry::Vec3D& Z=FUnit.getZ();

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

  return procSplit(System,splitName,offsetCN,CN,SplitOrg,SplitAxis);  
}

int
portSet::splitVoidPorts(Simulation& System,
			 const std::string& splitName,
			 const int offsetCN,
			 const int CN)
  /*!
    Split the void cell and store division planes
    Only use those port that a close to orthogonal with Y axis
    \param System :: Simulation to use
    \param splitName :: Name for cell output
    \param offsetCN :: output offset number
    \param CN :: Cell number to split
   */
{
  ELog::RegMethod RegA("portSet","splitVoidPorts");

  std::vector<Geometry::Vec3D> SplitOrg;
  std::vector<Geometry::Vec3D> SplitAxis;

  const Geometry::Vec3D& Y=FUnit.getY();

  size_t preFlag(0);
  for(size_t i=0;i<PCentre.size();i++)
    {
      if (std::abs(Ports[i]->getY().dotProd(Y))
	  <Geometry::zeroTol)
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
  return procSplit(System,splitName,offsetCN,CN,SplitOrg,SplitAxis);
}

void
portSet::insertAllInCell(Simulation& System,
			 const int cellN)
  /*!
    Overload of containdGroup so that the ports can also 
    be inserted if needed
    \param System :: Simulation to use    
    \param cellN :: Cell for insert
  */
{
  //  ContainedGroup::insertAllInCell(System,cellN);
  for(const std::shared_ptr<portItem>& PC : Ports)
    PC->insertInCell(System,cellN);
  return;
}

void
portSet::insertAllInCell(Simulation& System,
			 const std::vector<int>& cellVec)
  /*!
    Overload of containdGroup so that the ports can also 
    be inserted if needed
    \param System :: Simulation to use    
    \param cellVec :: Cells for insert
  */
{
  
  for(const std::shared_ptr<portItem>& PC : Ports)
    PC->insertInCell(System,cellVec);
  return;
}


void
portSet::insertPortInCell(Simulation& System,
			  const std::vector<std::set<int>>& cellVec)
  /*!
    Allow ports to be intersected into arbitary cell list
    \param System :: Simulation to use    
    \param cellVec :: Sets of cellnumbers corresponding to each port (in order)
    for which the port will be inserted into.
  */
{
  ELog::RegMethod RegA("portSet","insertPortInCell");
  
  for(size_t index=0;index<Ports.size() && index<cellVec.size();index++)
    {
      const std::set<int>& cellSet=cellVec[index];
      for(const int CN : cellSet)
	Ports[index]->insertInCell(System,CN);
    }
  return;
}

void
portSet::createPorts(Simulation& System,
		     MonteCarlo::Object* insertObj,
		     const std::vector<int>& CCVec,
		     const HeadRule& innerSurf,
		     const HeadRule& outerSurf)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
    \param CC :: Insert cells [from CC.getInsertCell()]			
    \param innerSurf :: HeadRule to inner surf
    \param outerSurf :: HeadRule to outer surf
   */
{
  ELog::RegMethod RegA("portSet","createPorts");

  populate(System.getDataBase());
  for(size_t i=0;i<Ports.size();i++)
    {
      for(const int CN : CCVec)
	  Ports[i]->addOuterCell(CN);

      for(const int CN : portCells)
	Ports[i]->addOuterCell(CN);

      Ports[i]->setCentLine(FUnit,PCentre[i],PAxis[i]);
      Ports[i]->constructTrack(System,insertObj,innerSurf,outerSurf);
      // if (outerVoid && FUnit.hasCell(outerVoidName))
      //  	Ports[i]->addPortCut(CellMap::getCellObject(System,OuterVoidName));
      Ports[i]->insertObjects(System);
    }
  return;
}

void
portSet::createPorts(Simulation& System,
		     const std::string& cellName,
		     const std::vector<int>& CCVec)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
    \param cellName :: Main cell nabe

    \param CCVec :: Insert cells [from CC.getInsertCell()]			
  */
{
  ELog::RegMethod RegA("portSet","createPorts");

  populate(System.getDataBase());
  ELog::EM<<"Insert Cell ="<<cellName<<ELog::endDiag;
  MonteCarlo::Object* insertObj=
    cellPtr->getCellObject(System,cellName);
  ELog::EM<<"Insert Cell ="<<*insertObj<<ELog::endDiag;
  return;
  for(size_t i=0;i<Ports.size();i++)
    {
      for(const int CN : CCVec)
	  Ports[i]->addOuterCell(CN);

      for(const int CN : portCells)
	Ports[i]->addOuterCell(CN);

      Ports[i]->setCentLine(FUnit,PCentre[i],PAxis[i]);
      int SN=insertObj->trackSurf(PCentre[i],PAxis[i]);
      ELog::EM<<"Surf == "<<SN<<ELog::endDiag;
      
      //      Ports[i]->constructTrack(System,insertObj,innerSurf,outerSurf);
      // if (outerVoid && FUnit.hasCell(outerVoidName))
      //  	Ports[i]->addPortCut(CellMap::getCellObject(System,OuterVoidName));
      Ports[i]->insertObjects(System);
    }
  return;
}



///\cond TEMPLATE
template
int
portSet::procSplit(Simulation&,const std::string&,const int,const int,
		   const Geometry::Vec3D&,const Geometry::Vec3D&);

template
int
portSet::procSplit(Simulation&,const std::string&,const int,const int,
		   const std::vector<Geometry::Vec3D>&,
		   const std::vector<Geometry::Vec3D>&);

///\endcond TEMPLATE
  
}  // NAMESPACE constructSystem
