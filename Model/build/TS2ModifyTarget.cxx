/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/TS2ModifyTarget.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "surfEqual.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "TargetBase.h"
#include "TS2ModifyTarget.h"

namespace TMRSystem
{

TS2ModifyTarget::TS2ModifyTarget(const std::string& MKey) :
  attachSystem::FixedComp(MKey,0),attachSystem::ContainedComp(),
  molyIndex(ModelSupport::objectRegister::Instance().cell(MKey)),
  cellIndex(molyIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param MKey :: Name for Moly changers
  */
{}

TS2ModifyTarget::TS2ModifyTarget(const TS2ModifyTarget& A) :  
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  molyIndex(A.molyIndex),cellIndex(A.cellIndex),
  PCut(A.PCut),SCent(A.SCent),Radius(A.Radius),
  SCut(A.SCut),CCut(A.CCut)
  /*!
    Copy constructor
    \param A :: TS2ModifyTarget to copy
  */
{}

TS2ModifyTarget&
TS2ModifyTarget::operator=(const TS2ModifyTarget& A)
  /*!
    Assignment operator
    \param A :: TS2ModifyTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      PCut=A.PCut;
      SCent=A.SCent;
      Radius=A.Radius;
      SCut=A.SCut;
      CCut=A.CCut;
    }
  return *this;
}
  
TS2ModifyTarget::~TS2ModifyTarget() 
  /*!
    Destructor
  */
{}

void
TS2ModifyTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database for variables
  */
{
  ELog::RegMethod RegA("TS2ModifyTarget","populate");

  const size_t nPlates=Control.EvalVar<size_t>(keyName+"NPlates");
  for(size_t i=0;i<nPlates;i++)
    {
      plateCut Item;
      const std::string keyIndex(StrFunc::makeString(keyName+"P",i+1));
      const double PY=
	Control.EvalPair<double>(keyIndex,keyName+"P","Dist");
      Item.centre=Y*PY;
      Item.axis=Y;
      Item.thick=
	Control.EvalPair<double>(keyIndex,keyName+"P","Thick");
      
      Item.mat=ModelSupport::EvalMat<int>
	(Control,keyIndex+"Mat",keyName+"PMat");
      Item.layerMat=ModelSupport::EvalMat<int>
	(Control,keyIndex+"LayreMat",keyName+"PLayerMat");
      if (Item.layerMat>=0)
	Item.layerThick=Control.EvalPair<double>
	  (keyIndex,keyName+"P","LayerThick");
      PCut.push_back(Item);
    }
  // Sphere:
  const size_t nSphere=Control.EvalVar<size_t>(keyName+"NCutSph");
  for(size_t i=0;i<nSphere;i++)
    {
      sphereCut Item;
      
      const std::string keyIndex(StrFunc::makeString(keyName+"CutSph",i+1));
      Item.centre=Control.EvalPair<Geometry::Vec3D>
	(keyIndex,keyName+"CutSph","Cent");
      Item.axis=Control.EvalPair<Geometry::Vec3D>
	(keyIndex,keyName+"CutSph","Axis");
      Item.axis.makeUnit();
      Item.radius=Control.EvalPair<double>(keyIndex,keyName+"CutSph","Radius");
      Item.dist=Control.EvalPair<double>(keyIndex,keyName+"CutSph","Dist");
      Item.mat=ModelSupport::EvalMat<int>
	(Control,keyIndex+"Mat",keyName+"CutSphMat");
      Item.defCutPlane();
      SCut.push_back(Item);
    }

  // Cones:
  const int nCone=Control.EvalVar<int>(keyName+"NCone");
  for(int i=0;i<nCone;i++)
    {
      coneCut Item;
      
      const std::string keyIndex(StrFunc::makeString(keyName+"Cone",i+1));
      Item.centre=Control.EvalPair<Geometry::Vec3D>
	(keyIndex,keyName+"Cone","Cent");
      Item.axis=Control.EvalPair<Geometry::Vec3D>
	(keyIndex,keyName+"Cone","Axis");
      Item.axis.makeUnit();
      Item.angleA=Control.EvalPair<double>(keyIndex,keyName+"Cone","AngleA");
      Item.angleB=Control.EvalPair<double>(keyIndex,keyName+"Cone","AngleB");
      Item.dist=Control.EvalPair<double>(keyIndex,keyName+"Cone","Dist");
      Item.mat=ModelSupport::EvalMat<int>
	(Control,keyIndex+"Mat",keyName+"ConeMat");
      Item.layerMat=ModelSupport::EvalMat<int>
	(Control,keyIndex+"LayerMat",keyName+"ConeLayerMat");
      Item.layerThick=
	Control.EvalPair<double>(keyIndex,keyName+"Cone","LayerThick");
      Item.layerThick*=cos(M_PI*std::abs(Item.angleA)/180.0);
      //      Item.defCutPlane();
      CCut.push_back(Item);
    }
  return;
}

void
TS2ModifyTarget::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Target component
  */
{
  ELog::RegMethod RegA("TS2ModifyTarget","createUnitVector");

  FixedComp::createUnitVector(FC);
  return;
}

void
TS2ModifyTarget::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("TS2ModifyTarget","createSurface");

  // Plates at 0 index offset:
  int offset(molyIndex);
  for(size_t i=0;i<PCut.size();i++)
    {
      const plateCut& Item=PCut[i];
      const Geometry::Vec3D Pt=Origin+Item.centre;

      ModelSupport::buildPlane(SMap,offset+1,Pt,Item.axis);
      ModelSupport::buildPlane(SMap,offset+2,
			       Pt+Item.axis*Item.thick,Item.axis);
      if (Item.layerMat>=0)
	{
	  ModelSupport::buildPlane(SMap,offset+11,
				   Pt+Item.axis*Item.layerThick,Item.axis);
	  ModelSupport::buildPlane(SMap,offset+12,
				   Pt+Item.axis*(Item.thick-Item.layerThick),
				   Item.axis);
	}
      offset+=100;
    }

  // SpherCuts at 500 index offset:
  offset=molyIndex+2000;
  for(size_t i=0;i<SCut.size();i++)
    {
      sphereCut& Item=SCut[i];
      Item.axisCalc(X,Y,Z);
      const Geometry::Vec3D CP(Origin+Item.centre);
      ModelSupport::buildPlane
	(SMap,offset+1, CP-
	 Item.axis*(Item.dist/2.0+Item.negCutPlane),
	 Item.axis);			       
      ModelSupport::buildPlane
	(SMap,offset+2,CP+
	 Item.axis*(Item.dist/2.0+Item.posCutPlane),
	 Item.axis);	
      ModelSupport::buildSphere
	(SMap,offset+7,CP-Item.axis*
	 (Item.dist+Item.radius),Item.radius);
      ModelSupport::buildSphere
	(SMap,offset+8,CP+Item.axis*
	 (Item.dist+Item.radius),Item.radius);
      offset+=100;
    }

  // Cones:
  offset=molyIndex+3000;
  for(size_t i=0;i<CCut.size();i++)
    {
      coneCut& Item=CCut[i];
      Item.axisCalc(X,Y,Z);
      const Geometry::Vec3D CP(Origin+Item.centre);
      ModelSupport::buildCone(SMap,offset+7,CP,Item.axis,Item.angleA,
			      Item.cutFlagA());
      ModelSupport::buildCone(SMap,offset+8,CP+Item.axis*Item.dist,
			      Item.axis,Item.angleB,Item.cutFlagB());
      ModelSupport::buildCone(SMap,offset+17,CP+Item.axis*0.3,
			      Item.axis,Item.angleA,Item.cutFlagA());
      ModelSupport::buildCone(SMap,offset+18,CP+Item.axis*(Item.dist-0.3),
			      Item.axis,Item.angleB,Item.cutFlagB());
      offset+=100;
    }
  
  return;
}

void
TS2ModifyTarget::createObjects(Simulation& System,
			       const int mainBody,const int skinBody)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param mainBody :: Main target surface
    \param skinBody :: Sub-layer activation study
  */
{
  ELog::RegMethod RegA("TS2ModifyTarget","createObjects");

  std::string Out;
  int offset;

  MonteCarlo::Qhull* QPtrA=System.findQhull(mainBody);
  MonteCarlo::Qhull* QPtrB=System.findQhull(skinBody);
  if (!QPtrA || !QPtrB)
    {
      ELog::EM<<"Failed on QHull for main/skin body "<<
	mainBody<<":"<<skinBody<<ELog::endCrit;
      if (!QPtrA)
	throw ColErr::InContainerError<int>(mainBody,"MainBody");
      else
	throw ColErr::InContainerError<int>(skinBody,"SkinBody");
    }
  std::vector<HeadRule> PlateUnits;
  std::vector<HeadRule> ConeUnits;

  if (!SCut.empty())
    {
      // Sphere:
      offset=molyIndex+2000;
      for(size_t i=0;i<SCut.size();i++)
	{
	  Out=ModelSupport::getComposite(SMap,offset,"1 -2 7 8 ");
	  addOuterUnionSurf(Out);
	  Out+=getContainer();
	  System.addCell(MonteCarlo::Qhull(cellIndex++,SCut[i].mat,0.0,Out));
	  offset+=100;
	}
    }

  // Cone:
  if (!CCut.empty())
    {
      HeadRule ExCone;
      HeadRule ConeItem;
      offset=molyIndex+3000;
      for(size_t i=0;i<CCut.size();i++)
	{
	  // Ta layer
	  std::ostringstream cx;
	  cx<<" "<< -CCut[i].cutFlagA()*SMap.realSurf(offset+7)
	    <<" "<< CCut[i].cutFlagA()*SMap.realSurf(offset+17)<<" ";
	  if (CCut[i].dist<0)
	    cx<< CCut[i].cutFlagB()*SMap.realSurf(offset+18)<<" ";
	  Out=cx.str()+getContainer();
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,CCut[i].layerMat,0.0,Out));
	  
	  cx.str("");
	  cx<<" "<< -CCut[i].cutFlagA()*SMap.realSurf(offset+17)
	    <<" "<< CCut[i].cutFlagB()*SMap.realSurf(offset+18)<<" ";
	  Out=cx.str()+getContainer();
	  System.addCell(MonteCarlo::Qhull(cellIndex++,CCut[i].mat,0.0,Out));

	  cx.str("");
	  cx<<" "<< CCut[i].cutFlagB()*SMap.realSurf(offset+8)
	    <<" "<< -CCut[i].cutFlagB()*SMap.realSurf(offset+18)<<" ";
	  if (CCut[i].dist<0)
	    cx<< -CCut[i].cutFlagA()*SMap.realSurf(offset+7)<<" ";
	  Out=cx.str()+getContainer();
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,CCut[i].layerMat,0.0,Out));

	  cx.str("");
	  cx<<" "<< -CCut[i].cutFlagA()*SMap.realSurf(offset+7)
	    <<" "<< CCut[i].cutFlagB()*SMap.realSurf(offset+8)<<" ";
	  ExCone.addUnion(cx.str());      
	  ConeItem.procString(cx.str()+getContainer());
	  ConeItem.populateSurf();
	  ConeUnits.push_back(ConeItem);
	  offset+=100;
	}
      ExCone.makeComplement();
      QPtrA->addSurfString(ExCone.display());
      QPtrB->addSurfString(ExCone.display());
    }

  if (!PCut.empty())
    {
      HeadRule ExPlate;
      std::string cutConeStr;
      offset=molyIndex;
      for(size_t i=0;i<PCut.size();i++)
	{
	  Out=ModelSupport::getComposite(SMap,offset,"1 -2 ");    
	  ExPlate.addUnion(Out);
	  const size_t cUnit=calcConeIntersect(ConeUnits,i);
	  if (cUnit)
	    {
	      HeadRule cCut=ConeUnits[cUnit-1];
	      cCut.makeComplement();
	      cutConeStr=cCut.display();
	    }
	  if (PCut[i].layerMat<0)
	    {
	      Out+=getContainer()+cutConeStr;
	      System.addCell(MonteCarlo::Qhull
			     (cellIndex++,PCut[i].mat,0.0,Out));
	    }
	  else
	    {
	      Out=ModelSupport::getComposite(SMap,offset,"1 -11 ");    
	      Out+=getContainer()+cutConeStr;
	      System.addCell(MonteCarlo::Qhull(cellIndex++,
					       PCut[i].layerMat,0.0,Out));
	      Out=ModelSupport::getComposite(SMap,offset,"12 -2 ");    
	      Out+=getContainer()+cutConeStr;
	      System.addCell(MonteCarlo::Qhull(cellIndex++,
					       PCut[i].layerMat,0.0,Out));
	      Out=ModelSupport::getComposite(SMap,offset,"11 -12 ");    ;
	      Out+=getContainer()+cutConeStr;
	      System.addCell(MonteCarlo::Qhull(cellIndex++,
					       PCut[i].mat,0.0,Out));
	    }
	  offset+=100;
	}
      ExPlate.makeComplement();
      QPtrA->addSurfString(ExPlate.display());
      QPtrB->addSurfString(ExPlate.display());
    }

  return;
}

size_t
TS2ModifyTarget::calcConeIntersect(const std::vector<HeadRule>& ConeUnits,
				   const size_t Index) const
  /*!
    Determine the cone/plane cut intersect
    \param ConeUnits :: Cone units to use
    \param Index :: Plate to test
    \retval coneUnits index -1 / 0 on no intersect
  */
{
  ELog::RegMethod RegA("TS2ModifyTarget","calcConeIntersect");
  // Now check cones dont intersect plates:
  const plateCut& Item=PCut[Index];
  const Geometry::Vec3D Pt=Origin+Item.centre;
  const Geometry::Vec3D AxisX(Item.axis.crossNormal());
  // Construct Simple line:
  MonteCarlo::LineIntersectVisit LI(Pt,AxisX);
  for(size_t cIndex=0;cIndex<ConeUnits.size();cIndex++)
    {
      const HeadRule& CItem(ConeUnits[cIndex]);
      const std::vector<const Geometry::Surface*> 
	SVec=CItem.getSurfaces();
      for(const Geometry::Surface* SPtr : SVec)
	{
	  const std::vector<Geometry::Vec3D> Out=
	    LI.getPoints(SPtr);

	  for(const Geometry::Vec3D ImpactPt : Out)
	    {
	      if (CItem.isValid(ImpactPt,SPtr->getName()))
		return 1+cIndex;
	    }
	}
    }
  return 0;
}
 

  
void
TS2ModifyTarget::createAll(Simulation& System,
			   const constructSystem::TargetBase& TB)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param TB :: Target Base
  */
{
  ELog::RegMethod RegA("TS2ModifyTarget","createAll");
  populate(System.getDataBase());
  createUnitVector(TB);
  createSurfaces();
  addBoundarySurf(TB.getContainer());
  createObjects(System,TB.getMainBody(),TB.getSkinBody());

  return;
}
  
}  // NAMESPACE TMRsystem
