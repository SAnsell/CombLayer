/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DiskLayerMod.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "DiskLayerMod.h"

namespace essSystem
{

DiskLayerMod::DiskLayerMod(const std::string& Key) :
  attachSystem::FixedRotate(Key,9),
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

DiskLayerMod::DiskLayerMod(const DiskLayerMod& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  midIndex(A.midIndex),midZ(A.midZ),
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
      attachSystem::FixedRotate::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);

      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      
      midIndex=A.midIndex;
      midZ=A.midZ;
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
DiskLayerMod::populate(const FuncDataBase& Control)
 /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("DiskLayerMod","populate");

  FixedRotate::populate(Control);

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
      const std::string LStr(std::to_string(i));
      T+=Control.EvalVar<double>(keyName+"Thick"+LStr);
      thick.push_back(T);
      size_t j(0);
      bool flag(0);
      double R(0.0);
      do
        {
          const std::string RStr(std::to_string(j));
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
DiskLayerMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("DiskLayerMod","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  SurfMap::setSurf("Layer0",SMap.realSurf(buildIndex+5));
  int SI(buildIndex+200);
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
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  SurfMap::setSurf("OuterRad",SMap.realSurf(buildIndex+7));
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

  int SI(buildIndex);
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
              System.addCell(MonteCarlo::Object(cellIndex++,mat[i][j],temp[i][j],
                                               layerStr+innerOut+outerOut));
              CellMap::addCell("Layer"+std::to_string(i),cellIndex-1);
            }
	  innerOut=ModelSupport::getComposite(SMap,TI," 207 ");
	  TI+=10;
	}
      outerOut=ModelSupport::getComposite(SMap,buildIndex," -7 ");
      if (mat[i][j]!=-1)
        {
          System.addCell(MonteCarlo::Object(cellIndex++,mat[i][j],temp[i][j],
                                           layerStr+innerOut+outerOut));
          CellMap::addCell("Layer"+std::to_string(i),cellIndex-1);
        }
      SI+=200;
    }
  //  int SI(buildIndex);

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

  const int SI(buildIndex+static_cast<int>(thick.size())*200);

  FixedComp::setConnect(4,Origin,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
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

HeadRule 
DiskLayerMod::getLayerHR(const size_t layerIndex,
			 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface HeadRule
  */
{
  ELog::RegMethod RegA("DiskLayerMod","getLayerHR");

  HeadRule HR;
  const size_t SI(static_cast<size_t>(std::abs(sideIndex)));
  const int signValue((sideIndex>0) ? 1 : -1);

  // Z direction is nLayer
  if (SI==5 || SI==6)
    {
      if (layerIndex>midIndex) 
	throw ColErr::IndexError<size_t>(layerIndex,midIndex,
					 "layerIndex in Z");
      const int LI((SI==5) ? static_cast<int>(midIndex-layerIndex)
                   : static_cast<int>(midIndex+layerIndex+1));

      
      HR= (SI==5) ?
	HeadRule(SMap,buildIndex+LI*200,-signValue*5) :
	HeadRule(SMap,buildIndex+LI*200,signValue*5);
    }
  else
    {
      // HORRIZONTAL CUT
      const size_t NSideLayer(radius[midIndex].size());
      if (layerIndex>NSideLayer) 
	throw ColErr::IndexError<size_t>(layerIndex,NSideLayer,
					 "layerIndex in XY");
      const int LI(static_cast<int>(layerIndex+1));
      if (SI<=4)
	HR=HeadRule(SMap,buildIndex+LI*200,signValue*7);
      
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }      
  
  if (sideIndex<0)
    HR.makeComplement();

  return HR;
}


double
DiskLayerMod::getHeight() const
  /*!
    Special calculation for the distance between two
    link point for the effective vertical height
    \return Full-Height [linkPoint 5-6]
  */
{
  return (FixedComp::getSignedRefLU(5).hasConnectPt() &&
	  FixedComp::getSignedRefLU(6).hasConnectPt() ) ?
    FixedComp::getLinkDistance(5,6) : 0.0;
}

void
DiskLayerMod::setLayout(const bool zRotate,
			const double VOffset,
			const double ORad)
  /*!
    Set basic rotation and radius value
  */
{
  if (zRotate) preYAngle=180.0;
  zStep=VOffset;
  outerRadius=ORad;
  return;
}
  
void
DiskLayerMod::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
    \param sideIndex :: side of object
   */
{
  ELog::RegMethod RegA("DiskLayerMod","createAll");

  populate(System.getDataBase());
  FixedRotate::createUnitVector(FC,sideIndex,0);

  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);  

  return;
}

}  // NAMESPACE essSystem 
