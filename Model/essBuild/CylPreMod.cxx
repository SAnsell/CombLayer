/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CylPreMod.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BlockAddition.h"
#include "CylPreMod.h"

namespace essSystem
{

CylPreMod::CylPreMod(const std::string& Key) :
  attachSystem::ContainedGroup("Main","BlockA","BlockB"),
  attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,6),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1),ExtAObj(new BlockAddition(Key+"ABlock")),
  ExtBObj(new BlockAddition(Key+"BBlock"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ExtAObj);
  OR.addObject(ExtBObj);
}


CylPreMod::CylPreMod(const CylPreMod& A) : 
  attachSystem::ContainedGroup(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),  
  modIndex(A.modIndex),cellIndex(A.cellIndex),ExtAObj(A.ExtAObj),
  ExtBObj(A.ExtBObj),blockActiveA(A.blockActiveA),
  blockActiveB(A.blockActiveB),aSide(A.aSide),bSide(A.bSide),
  innerRadius(A.innerRadius),innerHeight(A.innerHeight),
  innerDepth(A.innerDepth),radius(A.radius),height(A.height),
  depth(A.depth),mat(A.mat),temp(A.temp),viewY(A.viewY),
  viewAngle(A.viewAngle),viewOpenAngle(A.viewOpenAngle),
  viewHeight(A.viewHeight),viewWidth(A.viewWidth),
  FLpts(A.FLpts),FLunit(A.FLunit),layerCells(A.layerCells)
  /*!
    Copy constructor
    \param A :: CylPreMod to copy
  */
{}

CylPreMod&
CylPreMod::operator=(const CylPreMod& A)
  /*!
    Assignment operator
    \param A :: CylPreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      ExtAObj=A.ExtAObj;
      ExtBObj=A.ExtBObj;
      blockActiveA=A.blockActiveA;
      blockActiveB=A.blockActiveB;
      aSide=A.aSide;
      bSide=A.bSide;
      innerRadius=A.innerRadius;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
      viewY=A.viewY;
      viewAngle=A.viewAngle;
      viewOpenAngle=A.viewOpenAngle;
      viewHeight=A.viewHeight;
      viewWidth=A.viewWidth;
      FLpts=A.FLpts;
      FLunit=A.FLunit;
      layerCells=A.layerCells;
    }
  return *this;
}

CylPreMod::~CylPreMod()
  /*!
    Destructor
  */
{}

const std::shared_ptr<BlockAddition>& 
CylPreMod::getBox(const char F) const
  /*!
    Simple accessor 
    \param F :: Flag (A/B)
    \return Object
   */
{
  return (F=='A') ? ExtAObj : ExtBObj;
}

std::string
CylPreMod::getBoxCut(const char F) const
  /*!
    Simple accessor 
    \param F :: Flag (A/B)
    \return Object
   */
{
  if (F=='A' && blockActiveA)
    return ExtAObj->getLinkString(-4); 
  if (F=='B' && blockActiveB)
    return ExtBObj->getLinkString(-4); 
  return "";
}


void
CylPreMod::checkItems(const attachSystem::FixedComp& Mod)
  /*!
    The CylPreModerator wraps BOTH a moderator + a flightline assembly
    These two objects are prebuild. All points are calculated from that
    point
    \param Mod :: Inner item for radius
   */
{
  ELog::RegMethod RegA("CylPreMod","checkItems");

  // Clear all the variables
  viewY.clear();
  FLpts.clear();
  FLunit.clear();

  if (Mod.NConnect()<6)
    throw ColErr::IndexError<size_t>(6,Mod.NConnect(),"Moderator LU size");
  
  Geometry::Vec3D OutPt=Mod.getCentre();
  innerRadius=OutPt.Distance(Origin);
  // Never figure out if we are +/- Z so decided to test
  for(long int i=5;i<=6;i++)
    {
      OutPt=Mod.getLinkPt(i)-Origin;
      if (OutPt.dotProd(Z)>0.0)
	innerHeight=OutPt.abs();
      else
	innerDepth=OutPt.abs();
    }
  for(size_t i=0;i<nLayers;i++)
    {
      radius[i]+=innerRadius;
      height[i]+=innerHeight;
      depth[i]+=innerDepth;
    }
  for(size_t i=0;i<viewAngle.size();i++)
    {
      Geometry::Vec3D OVec(Y);
      Geometry::Vec3D WVec(X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(viewAngle[i],Z);
      Qxy.rotate(OVec);
      Qxy.rotate(WVec);

      Geometry::Vec3D leftView(-WVec);
      Geometry::Vec3D rightView(WVec);
      const Geometry::Quaternion Qview=
	Geometry::Quaternion::calcQRotDeg(viewOpenAngle[i],Z);

      Qview.rotate(leftView);
      Qview.invRotate(rightView);
      
      viewY.push_back(OVec);
      FLpts.push_back(Origin+OVec*innerRadius-WVec*(viewWidth[i]/2.0));
      FLpts.push_back(Origin+OVec*innerRadius+WVec*(viewWidth[i]/2.0));
      FLpts.push_back(Origin+OVec*innerRadius-Z*(viewHeight[i]/2.0));
      FLpts.push_back(Origin+OVec*innerRadius+Z*(viewHeight[i]/2.0));
      
      FLunit.push_back(leftView);      
      FLunit.push_back(rightView);
      FLunit.push_back(-Z);      
      FLunit.push_back(Z);
    }
  return;
}

void
CylPreMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylPreMod","populate");

  // clear stuff 
  viewAngle.clear();
  viewOpenAngle.clear();  
  viewHeight.clear();
  viewWidth.clear();

  double R(0.0);
  double H(0.0);
  double D(0.0);
  double T;
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  for(size_t i=0;i<nLayers;i++)
    {
      H+=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Height",i+1));   
      D+=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Depth",i+1));   
      R+=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Thick",i+1));   
      M=ModelSupport::EvalMat<int>(Control,
	StrFunc::makeString(keyName+"Material",i+1));   
      const std::string TStr=StrFunc::makeString(keyName+"Temp",i+1);
      T=(!M || !Control.hasVariable(TStr)) ?
	0.0 : Control.EvalVar<double>(TStr); 
      
      radius.push_back(R);
      height.push_back(H);
      depth.push_back(D);
      mat.push_back(M);
      temp.push_back(T);
    }


  const size_t nView=Control.EvalVar<size_t>(keyName+"NView");   
  double vH,vW,vA,vO;
  for(size_t i=0;i<nView;i++)
    {
      vH=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"ViewHeight",i+1));   
      vA=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"ViewAngle",i+1));   
      vW=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"ViewWidth",i+1));   
      vO=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"ViewOpenAngle",i+1));   
      viewAngle.push_back(vA);
      viewWidth.push_back(vW);
      viewOpenAngle.push_back(vO);
      viewHeight.push_back(vH);
    }
  blockActiveA=Control.EvalVar<int>(keyName+"ABlockActive");   
  blockActiveB=Control.EvalVar<int>(keyName+"BBlockActive");   
  aSide=Control.EvalVar<long int>(keyName+"ABlockSide");   
  bSide=Control.EvalVar<long int>(keyName+"BBlockSide");   

  return;
}

void
CylPreMod::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("CylPreMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);  
  return;
}

void
CylPreMod::createSurfaces()
  /*!
    Create surfaces for the model
  */
{
  ELog::RegMethod RegA("CylPreMod","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin,Y);  
  // First surface when needed:

  ModelSupport::buildCylinder(SMap,modIndex+9,Origin,Z,innerRadius);  

  int SI(modIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);  
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*depth[i],Z);  
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*height[i],Z);  
      SI+=10;
    }
  // For flight line 100
  for(size_t flIndex=0;flIndex<viewAngle.size();flIndex++)
    {
      const int baseIndex(modIndex+static_cast<int>(100*(flIndex+1)));
      // create mid divider:
      ModelSupport::buildPlane(SMap,baseIndex+1,Origin,viewY[flIndex]);  
      
      double step[]={0.0,0.0,0.0,0.0};
      for(size_t i=0;i<(nLayers+1)/2;i++)
	{
	  SI=baseIndex+10*static_cast<int>(i);
	  // SPECIAL moves inner layer in by thickness
	  for(size_t jIndex=0;jIndex<4;jIndex++)
	    {
	      const size_t j(jIndex+flIndex*4);
	      ModelSupport::buildPlane(SMap,SI+3,
				       FLpts[j]+FLunit[j]*step[jIndex],
				       FLunit[j]);  
	      SI++;
	      if (jIndex<2)
		step[jIndex]+=(i) ? radius[i]-radius[i-1] : radius[i]-innerRadius;
	      else if (jIndex==2)
		step[jIndex]+=(i) ? depth[i]-depth[i-1] : depth[i]-innerDepth;
	      else if (jIndex==3)
		step[jIndex]+=(i) ? height[i]-height[i-1] : height[i]-innerHeight;
	    }
	}
    }

  return; 
}

void
CylPreMod::createObjects(Simulation& System,
			 const attachSystem::ContainedComp* CMod)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
    \param CMod :: Moderator fixed unit
  */
{
  ELog::RegMethod RegA("CylPreMod","createObjects");

  std::string Out;
  layerCells.clear();

  int SI(modIndex);
  int CI(modIndex);  // Cut index

  // Process even number of surfaces:
  const size_t even(!(nLayers % 2 ) ? nLayers-2 : nLayers);
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");
      if (i==nLayers-1)
	{
	  addOuterSurf("Main",Out);
	}
      for(size_t viewIndex=0;viewIndex<viewAngle.size();viewIndex++)
	Out+=ModelSupport::getComposite(SMap,CI+100*static_cast<int>(viewIndex),
					modIndex+100*static_cast<int>(viewIndex),
					" (-101M:103:104:105:106) ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10," (7:-5:6) ");
      else if (CMod)
	Out+=CMod->getExclude();
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      layerCells.push_back(cellIndex-1);
      SI+=10;
      if (i!=even)
	CI+= (i+1>=(nLayers+1)/2) ? -10 : 10;
    }
  // Interstical end caps:
  const size_t evenCut(1-(nLayers%2));
  for(size_t i=0;i<(nLayers-1)/2;i++)
    {
      // Layer is between i+1 and nLayers-1-evenCut-i
      const int SI(modIndex+10*static_cast<int>(i));
      const int CI(modIndex+10*static_cast<int>(nLayers-2-evenCut-i));

      for(size_t viewIndex=0;viewIndex<viewAngle.size();viewIndex++)
	{	  
	  const int divideN(modIndex+100*static_cast<int>(viewIndex));
	  Out=ModelSupport::getComposite(SMap,SI,CI," 7 -7M ");
	  Out+=ModelSupport::getComposite
	    (SMap,SI+100*static_cast<int>(viewIndex),
	     divideN," 101M (103:104:105:106) -113 -114 -115 -116");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	  layerCells.push_back(cellIndex-1);
	}
    }
  
  // Finally the void cell:
  Out=ModelSupport::getComposite(SMap,modIndex,
				 modIndex+10*static_cast<int>(nLayers-1),"9 -7M ");

  for(size_t viewIndex=0;viewIndex<viewAngle.size();viewIndex++)
    {	  
      std::string OComp(Out);
      OComp+=ModelSupport::getComposite(SMap,modIndex+100*static_cast<int>(viewIndex),
				 "101 -103 -104 -105 -106 " );
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OComp));
    }
  return; 
}

void
CylPreMod::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("CylPreMod","createLinks");

  if (nLayers)
    {
      const int SI(modIndex+static_cast<int>(nLayers-1)*10);
      FixedComp::setConnect(0,Origin-Y*radius[nLayers-1],-Y);
      FixedComp::setLinkSurf(0,SMap.realSurf(SI+7));
      FixedComp::addBridgeSurf(0,-SMap.realSurf(modIndex+2));

      FixedComp::setConnect(1,Origin+Y*radius[nLayers-1],Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+7));
      FixedComp::addBridgeSurf(1,SMap.realSurf(modIndex+2));
      
      FixedComp::setConnect(2,Origin-Z*(height[nLayers-1]/2.0),-Z);
      FixedComp::setBridgeSurf(2,-SMap.realSurf(SI+5));

      FixedComp::setConnect(3,Origin+Z*(height[nLayers-1]/2.0),Z);
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+6));

      FixedComp::setConnect(4,Origin-X*radius[nLayers-1],-X);
      FixedComp::setLinkSurf(4,SMap.realSurf(SI+7));
      FixedComp::addBridgeSurf(4,-SMap.realSurf(modIndex+1));

      FixedComp::setConnect(5,Origin+X*radius[nLayers-1],X);
      FixedComp::setLinkSurf(5,SMap.realSurf(SI+7));
      FixedComp::addBridgeSurf(5,SMap.realSurf(modIndex+1));

    }
  else 
    ELog::EM<<"NO Layers in CylPreMod"<<ELog::endErr;
  return;
}

Geometry::Vec3D
CylPreMod::getSurfacePoint(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylPreMod","getSurfacePoint");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  if (layerIndex>0)
    {
      switch(SI)
	{
	case 0:
	  return Origin-Y*(radius[layerIndex-1]);
	case 1:
	  return Origin+Y*(radius[layerIndex-1]);
	case 2:
	  return Origin-Z*(height[layerIndex-1]);
	case 3:
	  return Origin+Z*(height[layerIndex-1]);
	case 4:
	  return Origin-X*(radius[layerIndex-1]);
	case 5:
	  return Origin+X*(radius[layerIndex-1]);
	}
    }
  else
    {
      switch(SI)
	{
	case 0:
	  return Origin-Y*innerRadius;
	case 1:
	  return Origin+Y*innerRadius;
	case 2:
	  return Origin-Z*innerDepth;
	case 3:
	  return Origin+Z*innerHeight;
	case 4:
	  return Origin-X*innerRadius;
	case 5:
	  return Origin+X*innerRadius;
	}
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}


int
CylPreMod::getLayerSurf(const size_t layerIndex,
			const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-6]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylPreMod","getLinkSurf");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(!layerIndex ? modIndex :
	       10*static_cast<int>(layerIndex-1)+modIndex);

  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
    case 2:    
    case 5:
    case 6:
      return (layerIndex) ? 
	SMap.realSurf(SI+7) :
	SMap.realSurf(modIndex+9);
    case 3:
      return -SMap.realSurf(SI+5);
    case 4:
      return SMap.realSurf(SI+6);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

std::string
CylPreMod::getLayerString(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layerstring [outlooking]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-5]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylPreMod","getLayerString");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(!layerIndex ? modIndex :
	       10*static_cast<int>(layerIndex-1)+modIndex);

  std::ostringstream cx;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< -SMap.realSurf(modIndex+1)<<" ";
      break;
    case 2:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<<SMap.realSurf(modIndex+1)<<" ";
      break;
    case 3:
      cx<<" "<<-SMap.realSurf(SI+5)<<" ";
      break;
    case 4:
      cx<<" "<<SMap.realSurf(SI+6)<<" ";
      break;
    case 5:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< -SMap.realSurf(modIndex+1)<<" ";
      break;
    case 6:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< SMap.realSurf(modIndex+1)<<" ";
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
    }
  if (sideIndex<0)
    {
      HeadRule HR(cx.str());
      HR.makeComplement();
      return HR.display();
    }
  return cx.str();
}

Geometry::Vec3D 
CylPreMod::calcViewIntercept(const size_t viewIndex,
			     const long int sideIndex) const
  /*!
    Calculate the intercept point on the view edge
    \param viewIndex :: View number
    \param sideIndex :: side Index 1/2
  */
{
  ELog::RegMethod RegA("CylPreMod","calcViewIntercept");
  
  if (viewIndex>=viewY.size())
    throw ColErr::IndexError<size_t>(viewIndex,viewY.size(),"viewIndex");

  if (!sideIndex || std::abs(sideIndex)>2)
    throw ColErr::IndexError<long int>(sideIndex,2,"sideIndex");

  const size_t SI(static_cast<size_t>(std::abs(sideIndex)-1));
  const int VI(modIndex+100*static_cast<int>(viewIndex) 
	       +static_cast<int>(SI));

  const Geometry::Vec3D BulkPtA=
    SurInter::getPoint(SMap.realSurfPtr(modIndex+17),
		       SMap.realSurfPtr(VI+113),
		       SMap.realSurfPtr(modIndex+15),
		       FLpts[4*viewIndex+SI]);

  const Geometry::Vec3D BulkPtB=
    SurInter::getPoint(SMap.realSurfPtr(modIndex+17),
		       SMap.realSurfPtr(VI+113),
		       SMap.realSurfPtr(modIndex+16),
		       FLpts[4*viewIndex+SI]);

  return (BulkPtA+BulkPtB)/2.0;
}

void
CylPreMod::updateLayers(Simulation& System,
			const char flag,
			const size_t layerN,
			const size_t levelN) const
  /*!
    If an item intersects the layered pre-moderator. Allows all 
    particular layer to have the ExtA or ExtB object intersect it
    \param System :: Simulation
    \param flag :: A / B object
    \param layerN :: layer number
    \param levelN :: level from item number
  */
{
  ELog::RegMethod RegA("CylPreMod","updateLayers");
  
  MonteCarlo::Qhull* OPtr=System.findQhull(layerCells[layerN]);
  if (!OPtr)
    throw ColErr::InContainerError<int>(layerCells[layerN],"layerCells");
  if (flag=='A')
    OPtr->addSurfString(ExtAObj->createCut(levelN));
  else
    OPtr->addSurfString(ExtBObj->createCut(levelN));

  return;  
}

void
CylPreMod::createAll(Simulation& System,
		     const attachSystem::FixedComp& CylMod)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param CylMod :: Cylindrical moderator [3 surfaces]
   */
{
  ELog::RegMethod RegA("CylPreMod","createAll");

  const attachSystem::ContainedComp* CModPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&CylMod);

  populate(System.getDataBase());
  createUnitVector(CylMod);
  checkItems(CylMod);

  createSurfaces();
  createObjects(System,CModPtr);
  createLinks();

  // CREATE BLOCK ADDITION

  Geometry::Vec3D IPt=calcViewIntercept(0,aSide);
  ExtAObj->setActive(blockActiveA);
  ExtAObj->setCentRotate(Origin);
  ExtAObj->setEdgeSurf(SMap.realSurf(modIndex+102+aSide));  // 103/104

  ExtAObj->copyInterObj(this->getCC("Main"));
  ExtAObj->createAll(System,IPt,*this,nLayers-2,4+aSide);   //5/6
  addOuterSurf("BlockA",ExtAObj->getCompExclude());

  // CREATE BLOCK ADDITION
  IPt=calcViewIntercept(1,3-bSide);   // view : side (2/1 from 1/2)
  ExtBObj->setActive(blockActiveB);
  ExtBObj->setEdgeSurf(SMap.realSurf(modIndex+205-bSide)); // 204/203

  ExtBObj->setCentRotate(Origin);
  ExtBObj->copyInterObj(this->getCC("Main"));

  ExtBObj->createAll(System,IPt,*this,nLayers-2,4+bSide);    // 5/6

  addOuterSurf("BlockB",ExtBObj->getCompExclude());  
  for(size_t i=nLayers-2;i<nLayers;i++)
    {
      updateLayers(System,'A',i,1);
      updateLayers(System,'B',i,1);
    }
  insertObjects(System);       
  return;
}

}  // NAMESPACE instrumentSystem
