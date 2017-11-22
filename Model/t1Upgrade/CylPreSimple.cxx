/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/CylPreSimple.cxx
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
#include "LayerComp.h"
#include "ContainedComp.h"
#include "CylPreSimple.h"

namespace ts1System
{

CylPreSimple::CylPreSimple(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,6),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1),nView(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylPreSimple::CylPreSimple(const CylPreSimple& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),innerRadius(A.innerRadius),
  innerHeight(A.innerHeight),innerDepth(A.innerDepth),
  radius(A.radius),height(A.height),depth(A.depth),
  mat(A.mat),temp(A.temp),nView(A.nView),viewY(A.viewY),
  FLpts(A.FLpts),FLunit(A.FLunit)
  /*!
    Copy constructor
    \param A :: CylPreSimple to copy
  */
{}

CylPreSimple&
CylPreSimple::operator=(const CylPreSimple& A)
  /*!
    Assignment operator
    \param A :: CylPreSimple to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      innerRadius=A.innerRadius;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
      nView=A.nView;
      viewY=A.viewY;
      FLpts=A.FLpts;
      FLunit=A.FLunit;
    }
  return *this;
}

CylPreSimple::~CylPreSimple()
  /*!
    Destructor
  */
{}

void
CylPreSimple::checkItems(const attachSystem::FixedComp& Mod)
  /*!
    The CylPreSimple wraps BOTH a moderator + a flightline assembly
    These two objects are prebuild. All points are calculated from that
    point
    \param Mod :: Inner item for radius
   */
{
  ELog::RegMethod RegA("CylPreSimple","checkItems");

  // Clear all the variables
  nView=0;
  FLpts.clear();
  FLunit.clear();

  const long int modLink(static_cast<long int>(Mod.NConnect())+1);
  if (modLink<5)
    throw ColErr::IndexError<long int>(4,modLink,"Moderator LU size");
  
  Geometry::Vec3D OutPt=Mod.getLinkPt(1);  // first outer surface
  innerRadius=OutPt.Distance(Origin);
  ELog::EM<<"Inner radius == "<<innerRadius<<ELog::endDiag;

  // Never figure out if we are +/- Z so decided to test
  Geometry::Vec3D ZUpPt;
  Geometry::Vec3D ZDownPt;
  double ZUpCos(-1.0);
  double ZDownCos(1.0);
  for(long int i=2;i<modLink;i++)
    {
      const Geometry::Vec3D OutAxis=Mod.getLinkAxis(i);
      const double DP=OutAxis.dotProd(Z);
      OutPt=Mod.getLinkPt(i)-Origin;
      if (DP>ZUpCos)
	{
	  ZUpCos=DP;
	  ZUpPt=OutPt;
	}
      if (DP<ZDownCos)
	{
	  ZDownCos=DP;
	  ZDownPt=OutPt;
	}
    }
  innerHeight=ZUpPt.abs();
  innerDepth=ZDownPt.abs();

  for(size_t i=0;i<nLayers;i++)
    {
      radius[i]+=innerRadius;
      height[i]+=innerHeight;
      depth[i]+=innerDepth;
    }

  return;
}
  
void
CylPreSimple::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylPreSimple","populate");

  // clear stuff 
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
      M=ModelSupport::EvalMat<int>
	(Control,StrFunc::makeString(keyName+"Material",i+1));   
      const std::string TStr=StrFunc::makeString(keyName+"Temp",i+1);
      T=(!M || !Control.hasVariable(TStr)) ?
	0.0 : Control.EvalVar<double>(TStr); 
      
      radius.push_back(R);
      height.push_back(H);
      depth.push_back(D);
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
CylPreSimple::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("CylPreSimple","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);  
  return;
}

void
CylPreSimple::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CylPreSimple","createSurfaces");

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
  for(size_t flIndex=0;flIndex<nView;flIndex++)
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
CylPreSimple::createObjects(Simulation& System,
			 const attachSystem::ContainedComp* CMod)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
    \param CMod :: Moderator fixed unit
  */
{
  ELog::RegMethod RegA("CylPreSimple","createObjects");

  std::string Out;

  int SI(modIndex);
  int CI(modIndex);  // Cut index

  // Process even number of surfaces:
  const size_t even(!(nLayers % 2 ) ? nLayers-2 : nLayers);
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");
      if (i==nLayers-1)
	addOuterSurf(Out);

      for(size_t viewIndex=0;viewIndex<nView;viewIndex++)
	{
	  Out+=ModelSupport::getComposite
	    (SMap,CI+100*static_cast<int>(viewIndex),
	     modIndex+100*static_cast<int>(viewIndex),
	     " (-101M:103:104:105:106) ");
	}
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10," (7:-5:6) ");
      else if (CMod)
	Out+=CMod->getExclude();
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
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

      for(size_t viewIndex=0;viewIndex<nView;viewIndex++)
	{	  
	  const int divideN(modIndex+100*static_cast<int>(viewIndex));
	  Out=ModelSupport::getComposite(SMap,SI,CI," 7 -7M ");
	  Out+=ModelSupport::getComposite(SMap,SI+100*static_cast<int>(viewIndex),
					  divideN," 101M (103:104:105:106) -113 -114 -115 -116");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	}
    }
  
  // Finally the void cell:  
  Out=ModelSupport::getComposite(SMap,modIndex,
				 modIndex+10*static_cast<int>(nLayers-1),"9 -7M ");

  for(size_t viewIndex=0;viewIndex<nView;viewIndex++)
    {	  
      std::string OComp(Out);
      OComp+=ModelSupport::getComposite(SMap,modIndex+100*static_cast<int>(viewIndex),
				 "101 -103 -104 -105 -106 " );
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OComp));
    }
  return; 
}

void
CylPreSimple::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("CylPreSimple","createLinks");

  if (nLayers)
    {
      const int SI(modIndex+static_cast<int>(nLayers-1)*10);
      FixedComp::setConnect(0,Origin-Y*radius[nLayers-1],-Y);
      FixedComp::setLinkSurf(0,SMap.realSurf(SI+7));
      FixedComp::setBridgeSurf(0,-SMap.realSurf(modIndex+2));

      FixedComp::setConnect(1,Origin+Y*radius[nLayers-1],Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+7));
      FixedComp::setBridgeSurf(1,SMap.realSurf(modIndex+2));
      
      FixedComp::setConnect(4,Origin-Z*(height[nLayers-1]/2.0),-Z);
      FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));

      FixedComp::setConnect(5,Origin+Z*(height[nLayers-1]/2.0),Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

      FixedComp::setConnect(2,Origin-X*radius[nLayers-1],-X);
      FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(2,-SMap.realSurf(modIndex+1));

      FixedComp::setConnect(3,Origin+X*radius[nLayers-1],X);
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(3,SMap.realSurf(modIndex+1));

    }
  else 
    ELog::EM<<"NO Layers in CylPreSimple"<<ELog::endErr;
  return;
}

Geometry::Vec3D
CylPreSimple::getSurfacePoint(const size_t layerIndex,
                              const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylPreSimple","getSurfacePoint");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  if (layerIndex>0)
    {
      switch(SI)
	{
	case 0:
	  return Origin-Y*(radius[layerIndex-1]);
	case 1:
	  return Origin+Y*(radius[layerIndex-1]);
	case 2:
	  return Origin-X*(radius[layerIndex-1]);
	case 3:
	  return Origin+X*(radius[layerIndex-1]);
	case 4:
	  return Origin-Z*(height[layerIndex-1]);
	case 5:
	  return Origin+Z*(height[layerIndex-1]);
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
	  return Origin-X*innerRadius;
	case 3:
	  return Origin+X*innerRadius;
	case 4:
	  return Origin-Z*innerDepth;
	case 5:
	  return Origin+Z*innerHeight;

	}
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}


int
CylPreSimple::getLayerSurf(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-3]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylPreSimple","getLayerSurf");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(!layerIndex ? modIndex :
	       10*static_cast<int>(layerIndex-1)+modIndex);

  const int signValue((sideIndex<0) ? -1 : 1);
  
  switch(std::abs(sideIndex))
    {
    case 1:
    case 2:    
    case 3:
    case 4:
      return (layerIndex) ? 
	SMap.realSurf(SI+7) :
	SMap.realSurf(modIndex+9);
    case 5:
      return -signValue*SMap.realSurf(SI+5);
    case 6:
      return signValue*SMap.realSurf(SI+6);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

std::string
CylPreSimple::getLayerString(const size_t layerIndex,
			     const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylPreSimple","getLayerString");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(!layerIndex ? modIndex :
	       10*static_cast<int>(layerIndex-1)+modIndex);

  std::ostringstream cx;
  switch(sideIndex)
    {
    case 1:
    case -1:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< -SMap.realSurf(modIndex+2)<<" ";
      break;
    case 2:
    case -2:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<<SMap.realSurf(modIndex+2)<<" ";
      break;
    case 3:
    case -3:
      cx<<" "<<-SMap.realSurf(SI+5)<<" ";
      break;
    case 4:
    case -4:
      cx<<" "<<SMap.realSurf(SI+6)<<" ";
      break;
    case 5:
    case -5:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< -SMap.realSurf(modIndex+1)<<" ";
      break;
    case 6:
    case -6:
      cx<<" "<<((layerIndex) ? 
		SMap.realSurf(SI+7) :
		SMap.realSurf(modIndex+9)) <<" "
	<< SMap.realSurf(modIndex+1)<<" ";
      break;
    }
  if (sideIndex<0)
    {
      HeadRule HR(cx.str());
      HR.makeComplement();
      return HR.display();
    }
  return cx.str();
}

void
CylPreSimple::createFlightPoint(const attachSystem::FixedComp& FLine)
  /*!
    Create View points for a flightline
    \param FLine :: Flight line to add
  */
  
{
  ELog::RegMethod RegA("CylPreSimple","createFlightPoint");

  if (FLine.NConnect()<6)
    throw ColErr::IndexError<size_t>
      (6,FLine.NConnect(),"FLine: "+FLine.getKeyName()+" LU size");
  
  for(long int i=3;i<=6;i++)
    {
      const Geometry::Vec3D Pt=FLine.getLinkPt(i);
      const Geometry::Vec3D Ax=FLine.getLinkAxis(i);
      FLpts.push_back(Pt);
      FLunit.push_back(Ax);
    }
  viewY.push_back(FLine.getY());
  nView++;
  return;
}

void
CylPreSimple::createAll(Simulation& System,
			const attachSystem::FixedComp& CylMod,
			const attachSystem::FixedComp& FLine)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param CylMod :: Cylindrical moderator [3 surfaces]
    \param FLine :: FLight line
   */
{
  ELog::RegMethod RegA("CylPreSimple","createAll");

  const attachSystem::ContainedComp* CModPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&CylMod);

  populate(System.getDataBase());
  createUnitVector(CylMod);
  checkItems(CylMod);
  createFlightPoint(FLine);

  createSurfaces();
  createObjects(System,CModPtr);
  createLinks();
  insertObjects(System);       
  return;
}

}  // NAMESPACE instrumentSystem
