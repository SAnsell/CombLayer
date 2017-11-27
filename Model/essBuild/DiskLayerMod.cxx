/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DiskLayerMod.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "DiskLayerMod.h"

namespace essSystem
{

DiskLayerMod::DiskLayerMod(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,9),
  attachSystem::CellMap(),attachSystem::SurfMap(),  
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

DiskLayerMod::DiskLayerMod(const DiskLayerMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),
  midIndex(A.midIndex),midZ(A.midZ),zStep(A.zStep),
  outerRadius(A.outerRadius),thick(A.thick),radius(A.radius),
  mat(A.mat),temp(A.temp)
  /*!
    Copy constructor
    \param A :: DiskLayerMod to copy
  */
{}

DiskLayerMod&
DiskLayerMod::operator=(const DiskLayerMod& A)
  /*!
    Assignment operator
    \param A :: DiskLayerMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      
      cellIndex=A.cellIndex;
      midIndex=A.midIndex;
      midZ=A.midZ;
      zStep=A.zStep;
      outerRadius=A.outerRadius;
      thick=A.thick;
      radius=A.radius;
      mat=A.mat;
      temp=A.temp;
      
    }
  return *this;
}

  
  
DiskLayerMod*
DiskLayerMod::clone() const
  /*!
    Clone self 
    \return new (this)
   */
{
  return new DiskLayerMod(*this);
}

DiskLayerMod::~DiskLayerMod()
  /*!
    Destructor
  */
{}
  

void
DiskLayerMod::populate(const FuncDataBase& Control,
		     const double zShift,
		     const double outRadius)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
    \param zShift :: Default offset height a
    \param outRadius :: Outer radius of reflector [-ve to ignore]
  */
{
  ELog::RegMethod RegA("DiskLayerMod","populate");

  zStep=Control.EvalDefVar<double>(keyName+"ZStep",zShift);
  outerRadius=outRadius;

  // clear stuff 
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  thick.clear();
  radius.resize(nLayers);
  mat.resize(nLayers);
  temp.resize(nLayers);

  double tmp;
  int M;
  double T(0.0);
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string LStr(StrFunc::makeString(i));
      T+=Control.EvalVar<double>(keyName+"Thick"+LStr);
      thick.push_back(T);
      size_t j(0);
      bool flag(0);
      double R(0.0);
      do
        {
          const std::string RStr(StrFunc::makeString(j));
          const std::string Num=LStr+"x"+RStr;
          if (Control.hasVariable(keyName+"Radius"+Num))
            {
              const double RItem=
                Control.EvalVar<double>(keyName+"Radius"+Num);
              R=(RItem>0.0) ? RItem : R+RItem;
              radius[i].push_back(R);
            }
          else
            flag=1;
          
          M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+Num);
          tmp=Control.EvalDefVar<double>(keyName+"Temp"+Num,0.0);
          
          mat[i].push_back(M);
          temp[i].push_back(tmp);
          j++;
        } while (!flag);
    }
  midIndex=nLayers/2;
  return;
}

void
DiskLayerMod::createUnitVector(const attachSystem::FixedComp& refCentre,
                               const long int sideIndex,const bool zRotate)
  /*!
    Create the unit vectors
    \param refCentre :: Centre for object
    \param sideIndex :: index for link
    \param zRotate :: rotate Zaxis
  */
{
  ELog::RegMethod RegA("DiskLayerMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(refCentre);
  Origin=refCentre.getLinkPt(sideIndex);
  if (zRotate)
    {
      X*=-1;
      Z*=-1;
    }
  applyShift(0,0,zStep);

  return;
}


void
DiskLayerMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("DiskLayerMod","createSurfaces");

  ModelSupport::buildPlane(SMap,modIndex+5,Origin,Z);
  SurfMap::setSurf("Layer0",SMap.realSurf(modIndex+5));
  int SI(modIndex+200);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildPlane(SMap,SI+5,Origin+Z*thick[i],Z);
      SurfMap::setSurf("Layer"+std::to_string(i+1),SMap.realSurf(SI+5));
      // builds inner  cylinders:
      int TI(SI);
      for(size_t j=0;j<radius[i].size();j++)
        {
          ModelSupport::buildCylinder(SMap,TI+7,Origin,Z,radius[i][j]);
          TI+=10;
        }
      SI+=200;
    }
  ModelSupport::buildCylinder(SMap,modIndex+7,Origin,Z,outerRadius);
  SurfMap::setSurf("OuterRad",SMap.realSurf(modIndex+7));
  return; 
}

void
DiskLayerMod::createObjects(Simulation& System)
  /*!
    Create the disc component
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("DiskLayerMod","createObjects");

  std::string Out,outerOut;

  int SI(modIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      size_t j;
      int TI(SI);
      std::string innerOut;  // empty string
      const std::string layerStr=
	ModelSupport::getComposite(SMap,SI," 5 -205 ");
      for(j=0;j<radius[i].size();j++)
	{
	  outerOut=ModelSupport::getComposite(SMap,TI," -207 ");
          if (mat[i][j]!=-1)
            {
              System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i][j],temp[i][j],
                                               layerStr+innerOut+outerOut));
              CellMap::addCell("Layer"+std::to_string(i),cellIndex-1);
            }
	  innerOut=ModelSupport::getComposite(SMap,TI," 207 ");
	  TI+=10;
	}
      outerOut=ModelSupport::getComposite(SMap,modIndex," -7 ");
      if (mat[i][j]!=-1)
        {
          System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i][j],temp[i][j],
                                           layerStr+innerOut+outerOut));
          CellMap::addCell("Layer"+std::to_string(i),cellIndex-1);
        }
      SI+=200;
    }
  //  int SI(modIndex);

  //=ModelSupport::getComposite(SMap,,TI," 5 -205 - ");

  addOuterSurf(outerOut);
  return; 
}

void
DiskLayerMod::createLinks()
  /*!
    Creates a full attachment setx
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("DiskLayerMod","createLinks");

  const int SI(modIndex+static_cast<int>(thick.size())*200);

  FixedComp::setConnect(4,Origin,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(modIndex+5));
  FixedComp::setConnect(5,Origin+Z*thick[nLayers-1],Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+5));

  return;
}

Geometry::Vec3D
DiskLayerMod::getSurfacePoint(const size_t layerIndex,
			      const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    [note surfaces 1-4 are -Y/Y/-X/X]
    We need to revese the simple layer system to build a centre-outer 
    system [e.g. getting layers from NLayer/2+1
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [1-6] 
    \return Surface point
  */
{
  ELog::RegMethod RegA("DiskLayerMod","getSurfacePoint");

  if (!sideIndex) return Origin;

  const size_t SI(static_cast<size_t>(std::abs(sideIndex)));
  
  // Z direction is nLayer
  if (SI==5 || SI==6)
    {
      if (layerIndex>midIndex) 
	throw ColErr::IndexError<size_t>(layerIndex,midIndex,
					 "layerIndex in Z");
      const size_t LI((SI==5) ? midIndex-layerIndex : midIndex+layerIndex+1);
      return (LI) ? Origin+Z*thick[layerIndex-1] : Origin;
    }
  // HORRIZONTAL CUT

  const size_t NSideLayer(radius[midIndex].size());
  if (layerIndex>NSideLayer) 
	throw ColErr::IndexError<size_t>(layerIndex,NSideLayer,
					 "layerIndex in XY");

  const double R((layerIndex==NSideLayer) ? outerRadius :
		 radius[midIndex][layerIndex]);

  switch(SI)
    {
    case 0:
      return Origin-Y*R;
    case 1:
      return Origin+Y*R;
    case 2:
      return Origin-X*R;
    case 3:
      return Origin+X*R;
    }
  
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

int
DiskLayerMod::getLayerSurf(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLinkSurf");

  const size_t SI(static_cast<size_t>(std::abs(sideIndex)));
  const int signValue((SI>0) ? 1 : -1);

  // Z direction is nLayer
  if (SI==5 || SI==6)
    {
      if (layerIndex>midIndex) 
	throw ColErr::IndexError<size_t>(layerIndex,midIndex,
					 "layerIndex in Z");
      const int LI((SI==5) ? static_cast<int>(midIndex-layerIndex)
                   : static_cast<int>(midIndex+layerIndex+1));
      
      return (SI==5) ? -signValue*SMap.realSurf(modIndex+LI*200+5) :
	signValue*SMap.realSurf(modIndex+LI*200+5);
    }
  // HORRIZONTAL CUT


  const size_t NSideLayer(radius[midIndex].size());
  if (layerIndex>NSideLayer) 
	throw ColErr::IndexError<size_t>(layerIndex,NSideLayer,
					 "layerIndex in XY");
  const int LI(static_cast<int>(layerIndex+1));
  if (SI<=4)
    return signValue*SMap.realSurf(modIndex+LI*200+7);
  
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

}

std::string
DiskLayerMod::getLayerString(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface string
  */
{
  ELog::RegMethod RegA("DiskLayerMod","getLayerString");

  std::string Out;
  Out=" "+StrFunc::makeString(getLayerSurf(layerIndex,sideIndex))+" ";
  if (sideIndex<0)
    {
      HeadRule HR(Out);
      HR.makeComplement();
      return HR.display();
    }
  return Out;
}


double
DiskLayerMod::getHeight() const
  /*!
    Special calculation for the distance between two
    link point for the effective vertical height
    \return Full-Height [linkPoint 5-6]
  */
{
  return (getSignedLU(5).hasConnectPt() &&
	  getSignedLU(6).hasConnectPt() ) ?
       getLinkDistance(5,6) : 0.0;
}

void
DiskLayerMod::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex,
		      const bool zRotate,
		      const double VOffset,
		      const double ORad)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
    \param sideIndex :: side of object
    \param zRotate :: Rotate to -ve Z
    \param VOffset :: Vertical offset from target
    \param ORad :: Outer radius of zone
   */
{
  ELog::RegMethod RegA("DiskLayerMod","createAll");

  populate(System.getDataBase(),VOffset,ORad);
  createUnitVector(FC,sideIndex,zRotate);

  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);  

  return;
}

}  // NAMESPACE essSystem 
