/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/H2Moderator.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "LayerComp.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "H2Moderator.h"

namespace ts1System
{

H2Moderator::H2Moderator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::LayerComp(6),
  attachSystem::FixedComp(Key,6),
  h2Index(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(h2Index+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


H2Moderator::H2Moderator(const H2Moderator& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),h2Index(A.h2Index),
  cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  width(A.width),height(A.height),depth(A.depth),
  viewSphere(A.viewSphere),innerThick(A.innerThick),
  vacThick(A.vacThick),midThick(A.midThick),
  terThick(A.terThick),outerThick(A.outerThick),
  clearThick(A.clearThick),vacTop(A.vacTop),
  outerView(A.outerView),clearTop(A.clearTop),alMat(A.alMat),
  lh2Mat(A.lh2Mat),voidMat(A.voidMat),h2Temp(A.h2Temp)
  /*!
    Copy constructor
    \param A :: H2Moderator to copy
  */
{}

H2Moderator&
H2Moderator::operator=(const H2Moderator& A)
  /*!
    Assignment operator
    \param A :: H2Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      width=A.width;
      height=A.height;
      depth=A.depth;
      viewSphere=A.viewSphere;
      innerThick=A.innerThick;
      vacThick=A.vacThick;
      midThick=A.midThick;
      terThick=A.terThick;
      outerThick=A.outerThick;
      clearThick=A.clearThick;
      vacTop=A.vacTop;
      outerView=A.outerView;
      clearTop=A.clearTop;
      alMat=A.alMat;
      lh2Mat=A.lh2Mat;
      voidMat=A.voidMat;
      h2Temp=A.h2Temp;
    }
  return *this;
}

H2Moderator::~H2Moderator() 
 /*!
   Destructor
 */
{}

void
H2Moderator::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("H2Moderator","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  viewSphere=Control.EvalVar<double>(keyName+"ViewSphere");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");
  midThick=Control.EvalVar<double>(keyName+"MidThick");
  terThick=Control.EvalVar<double>(keyName+"TerThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");
  clearThick=Control.EvalVar<double>(keyName+"ClearThick");

  vacTop=Control.EvalVar<double>(keyName+"VacTop");
  outerView=Control.EvalVar<double>(keyName+"OuterView");
  clearTop=Control.EvalVar<double>(keyName+"ClearTop");

  // Materials
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  lh2Mat=ModelSupport::EvalMat<int>(Control,keyName+"Lh2Mat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  h2Temp=Control.EvalVar<double>(keyName+"Lh2Temp");
  applyModification();
  return;
}

void
H2Moderator::applyModification() 
  /*!
    Create the modified layer set
    layers are numbered in set of 10 going from the inner all
    (no modification) to 50 at the outer. Then in -/+ (Y,X,Z).
  */
{
  modLayer[56]=clearTop;
  modLayer[41]=outerView;
  modLayer[42]=outerView;
  modLayer[46]=vacTop;
  return;
}
  
void
H2Moderator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("H2Moderator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);
  return;
}

void
H2Moderator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Moderator","createSurface");

  // Inner surfaces
  ModelSupport::buildPlane(SMap,h2Index+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,h2Index+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,h2Index+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,h2Index+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,h2Index+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,h2Index+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildSphere(SMap,h2Index+7,
   			    Origin-Y*(depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,h2Index+8,
   			    Origin+Y*(depth/2.0-viewSphere),
   			    viewSphere);

  // Modification map:
  std::map<size_t,double>::const_iterator mc;
  const double layer[]={innerThick,vacThick,midThick,
			terThick,outerThick,clearThick};

  std::vector<double> T(6);  // Zero size
  int HI(h2Index+10);
  for(size_t i=0;i<6;i++)
    {
      // Add the new layer +/- the modification
      for(size_t surfIndex=0;surfIndex<6;surfIndex++)
	{
	  mc=modLayer.find(i*10+surfIndex+1);
	  T[surfIndex]+=(mc!=modLayer.end()) ? mc->second : layer[i];
	}

      ModelSupport::buildPlane(SMap,HI+1,
			       Origin-Y*(depth/2.0+T[0]),Y);
      ModelSupport::buildPlane(SMap,HI+2,
			       Origin+Y*(depth/2.0+T[1]),Y);
      ModelSupport::buildPlane(SMap,HI+3,
			       Origin-X*(width/2.0+T[2]),X);
      ModelSupport::buildPlane(SMap,HI+4,
			       Origin+X*(width/2.0+T[3]),X);
      ModelSupport::buildPlane(SMap,HI+5,
			       Origin-Z*(height/2.0+T[4]),Z);
      ModelSupport::buildPlane(SMap,HI+6,
			       Origin+Z*(height/2.0+T[5]),Z);
      HI+=10;
    }
  // Special 
  ModelSupport::buildSphere(SMap,h2Index+17,
   			    Origin-Y*(innerThick+depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,h2Index+18,
   			    Origin+Y*(innerThick+depth/2.0-viewSphere),
   			    viewSphere);

  return;
}

void
H2Moderator::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  for(int i=h2Index+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
}

void
H2Moderator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Moderator","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,h2Index,"-7 -8 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,h2Temp,Out));

  // Inner al
  Out=ModelSupport::getComposite(SMap,h2Index,"-17 -18 13 -14 15 -16 "
				 " (7:8:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,h2Temp,Out));

  // Vac layer
  Out=ModelSupport::getComposite(SMap,h2Index,"21 -22 23 -24 25 -26 "
				 " (17:18:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Mid al
  Out=ModelSupport::getComposite(SMap,h2Index,"31 -32 33 -34 35 -36 "
				 " (-21:22:-23:24:-25:26) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Tertiary Layer
  Out=ModelSupport::getComposite(SMap,h2Index,"41 -42 43 -44 45 -46 "
				 " (-31:32:-33:34:-35:36) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Outer Al
  Out=ModelSupport::getComposite(SMap,h2Index,"51 -52 53 -54 55 -56 "
				 " (-41:42:-43:44:-45:46) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Outer Al
  Out=ModelSupport::getComposite(SMap,h2Index,"61 -62 63 -64 65 -66 "
				 " (-51:52:-53:54:-55:56) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,h2Index," 61 -62 63 -64 65 -66 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  return;
}

void
H2Moderator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("H2Moderator","createLinks");

  // set Links:
  FixedComp::setConnect(0,getSurfacePoint(5,0),-Y);
  FixedComp::setConnect(1,getSurfacePoint(5,1),Y);
  FixedComp::setConnect(2,getSurfacePoint(5,2),-X);
  FixedComp::setConnect(3,getSurfacePoint(5,3),X);
  ELog::EM<<"H2 Point == "<<getSurfacePoint(5,4)<<ELog::endDebug;
  FixedComp::setConnect(4,getSurfacePoint(5,4),-Z);
  FixedComp::setConnect(5,getSurfacePoint(5,5),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(h2Index+61));
  FixedComp::setLinkSurf(1,SMap.realSurf(h2Index+62));
  FixedComp::setLinkSurf(2,-SMap.realSurf(h2Index+63));
  FixedComp::setLinkSurf(3,SMap.realSurf(h2Index+64));
  FixedComp::setLinkSurf(4,-SMap.realSurf(h2Index+65));
  FixedComp::setLinkSurf(5,SMap.realSurf(h2Index+66));

  return;
}

Geometry::Vec3D
H2Moderator::getSurfacePoint(const size_t layerIndex,
			     const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Moderator","getSurfacePoint");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  // Modification map:
  std::map<size_t,double>::const_iterator mc;
  const double layer[]={innerThick,vacThick,midThick,
			terThick,outerThick,clearThick};
  double T(0.0);
  for(size_t i=0;i<layerIndex;i++)
    {
      mc=modLayer.find(i*10+sideIndex+1);
      T+=(mc!=modLayer.end()) ? mc->second : layer[i];
    }

  switch(sideIndex)
    {
    case 0:
      return Origin-Y*(depth/2.0+T);
    case 1:
      return Origin+Y*(depth/2.0+T);
    case 2:
      return Origin-X*(width/2.0+T);
    case 3:
      return Origin+X*(width/2.0+T);
    case 4:
      return Origin-Z*(height/2.0+T);
    case 5:
      return Origin+Z*(height/2.0+T);
    }
  throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
}

std::string
H2Moderator::getLayerString(const size_t sideIndex,
			   const size_t layerIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLayerString");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>5) 
    throw ColErr::IndexError<size_t>(layerIndex,5,"layer");

  std::ostringstream cx;
  const int sign=(sideIndex % 2 ) ? 1 : -1;
  if (sideIndex>2 || layerIndex>2)
    {
      const int surfN(h2Index+
		      static_cast<int>(10*layerIndex+sideIndex+1));
      cx<<" "<<sign*SMap.realSurf(surfN)<<" ";

    }
  else
    {
      const int surfN(h2Index+
		      static_cast<int>(10*layerIndex+sideIndex+7));
      cx<<" "<<SMap.realSurf(surfN)<<" ";
    }
  return cx.str();
}

int
H2Moderator::getLayerSurf(const size_t layerIndex,
			 const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLayerSurf");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>5) 
    throw ColErr::IndexError<size_t>(layerIndex,5,"layer");

  const int sign=(sideIndex % 2 ) ? 1 : -1;
  if (sideIndex>2 || layerIndex>2)
    {
      const int surfN(h2Index+
		      static_cast<int>(10*layerIndex+sideIndex+1));
      return sign*SMap.realSurf(surfN);
    }
  const int surfN(h2Index+
		  static_cast<int>(10*layerIndex+sideIndex+7));
  return SMap.realSurf(surfN);
}


std::string
H2Moderator::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,h2Index,surfList);
}

void
H2Moderator::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("H2Moderator","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
//  createObjects(System);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
