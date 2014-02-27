/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/H2Section.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "LayerInfo.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "ModBase.h"
#include "H2Section.h"

namespace ts1System
{

H2Section::H2Section(const std::string& Key)  :
 constructSystem::ModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Section::H2Section(const H2Section& A) : 
  constructSystem::ModBase(A),
  height(A.height),width(A.width),
  depth(A.depth),frontWall(A.frontWall),backWall(A.backWall),
  mainWall(A.mainWall),vacGap(A.vacGap),heGap(A.heGap),
  outGap(A.outGap),midAlThick(A.midAlThick),outAlThick(A.outAlThick),
  nSi(A.nSi),siThick(A.siThick),alMat(A.alMat),
  siMat(A.siMat),lh2Mat(A.lh2Mat),lh2Temp(A.lh2Temp)
  /*!
    Copy constructor
    \param A :: H2Section to copy
  */
{}

H2Section&
H2Section::operator=(const H2Section& A)
  /*!
    Assignment operator
    \param A :: H2Section to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ModBase::operator=(A);
      height=A.height;
      width=A.width;
      depth=A.depth;
      frontWall=A.frontWall;
      backWall=A.backWall;
      mainWall=A.mainWall;
      vacGap=A.vacGap;
      heGap=A.heGap;
      outGap=A.outGap;
      midAlThick=A.midAlThick;
      outAlThick=A.outAlThick;
      nSi=A.nSi;
      siThick=A.siThick;
      alMat=A.alMat;
      siMat=A.siMat;
      lh2Mat=A.lh2Mat;
      lh2Temp=A.lh2Temp;
    }
  return *this;
}

H2Section::~H2Section() 
 /*!
   Destructor
 */
{}

H2Section*
H2Section::clone() const
 /*!
   Clone constructor
   \return copy of (this)
 */
{
  return new H2Section(*this);
}

void
H2Section::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("H2Section","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");

  frontWall=Control.EvalVar<double>(keyName+"FrontWall");
  backWall=Control.EvalVar<double>(keyName+"BackWall");
  mainWall=Control.EvalVar<double>(keyName+"MainWall");

  vacGap=Control.EvalVar<double>(keyName+"VacGap");
  heGap=Control.EvalVar<double>(keyName+"HeGap");
  outGap=Control.EvalVar<double>(keyName+"OutGap");
  midAlThick=Control.EvalVar<double>(keyName+"MidAlThick");
  outAlThick=Control.EvalVar<double>(keyName+"OutAlThick");

  nSi=Control.EvalVar<int>(keyName+"NSiLayers");
  siThick=Control.EvalVar<double>(keyName+"SiThick");

  // Materials
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  lh2Mat=ModelSupport::EvalMat<int>(Control,keyName+"Lh2Mat");
  siMat=ModelSupport::EvalMat<int>(Control,keyName+"SiMat");
  lh2Temp=Control.EvalVar<double>(keyName+"Lh2Temp");

  return;
}
  
void
H2Section::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("H2Section","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);
  ELog::EM<<"Z == "<<Z<<ELog::endDebug;
  return;
}

void
H2Section::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Section","createSurface");
  
  if (nSi>0)
    {
//      const double hStep((depth-nSi*siThick)/(nSi+1));
      const double hStep((depth-nSi*siThick)/(nSi+3));      
      double yPos(-depth/2.0);
      int plateIndex(modIndex+100);
      for(int i=0;i<nSi;i++)
	{
	  yPos+=hStep;
	  ModelSupport::buildPlane(SMap,plateIndex+1,Origin+Y*yPos,Y);
	  yPos+=siThick;
	  ModelSupport::buildPlane(SMap,plateIndex+2,Origin+Y*yPos,Y);
	  plateIndex+=10;
	}
    }

  // H2 layer
  ModelSupport::buildPlane(SMap,modIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,modIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,modIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,modIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,modIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,modIndex+6,Origin+Z*height/2.0,Z);

  // Al layer
  ModelSupport::buildPlane(SMap,modIndex+11,
			   Origin-Y*(depth/2.0+backWall),Y);
  ModelSupport::buildPlane(SMap,modIndex+12,
			   Origin+Y*(depth/2.0+frontWall),Y);
  ModelSupport::buildPlane(SMap,modIndex+13,
			   Origin-X*(width/2.0+mainWall),X);
  ModelSupport::buildPlane(SMap,modIndex+14,
			   Origin+X*(width/2.0+mainWall),X);
  ModelSupport::buildPlane(SMap,modIndex+15,
			   Origin-Z*(height/2.0+mainWall),Z);
  ModelSupport::buildPlane(SMap,modIndex+16,
			   Origin+Z*(height/2.0+mainWall),Z);

  double offset=vacGap;
  // vac layer
  ModelSupport::buildPlane(SMap,modIndex+21,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+22,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+23,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+24,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+25,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,modIndex+26,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=midAlThick;
  // al mid layer
  ModelSupport::buildPlane(SMap,modIndex+31,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+32,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+33,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+34,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+35,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,modIndex+36,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=heGap;
  // al mid layer
  ModelSupport::buildPlane(SMap,modIndex+41,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+42,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+43,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+44,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+45,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,modIndex+46,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=outAlThick;
  // al mid layer
  ModelSupport::buildPlane(SMap,modIndex+51,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+52,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+53,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+54,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+55,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,modIndex+56,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=outGap;
  // al mid layer
  ModelSupport::buildPlane(SMap,modIndex+61,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+62,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,modIndex+63,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+64,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,modIndex+65,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,modIndex+66,
			   Origin+Z*(height/2.0+mainWall+offset),Z);


  FixedComp::setConnect(0,Origin-Y*(depth/2.0+backWall+offset),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+backWall+offset),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0+mainWall+offset),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+mainWall+offset),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+mainWall+offset),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+mainWall+offset),Z);

  return;
}

void
H2Section::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("H2Section","addToInsertChain");

  for(int i=modIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
 }


void
H2Section::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Section","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,modIndex,"61 -62 63 -64 65 -66 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  
  if (nSi>0)
    {
      int plateIndex(modIndex+100);
      // inital
      Out=ModelSupport::getComposite(SMap,modIndex,"1 -101 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,lh2Temp,Out));
      for(int i=0;i<nSi;i++)
	{
	  Out=ModelSupport::getComposite(SMap,modIndex,
					 plateIndex,"1M -2M 3 -4 5 -6");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,siMat,lh2Temp,Out));
	  if (i!=nSi-1)
	    {
	      Out=ModelSupport::getComposite(SMap,modIndex,
					     plateIndex,"2M -11M 3 -4 5 -6");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,lh2Temp,Out));
	    }
	  plateIndex+=10;
	}
      // final
      Out=ModelSupport::getComposite(SMap,modIndex,plateIndex-10,
				     "2M -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,lh2Temp,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,modIndex,"1 -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,lh2Temp,Out));
    }

  // AL

  Out=ModelSupport::getComposite(SMap,modIndex,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,lh2Temp,Out));

  // vac layer
  Out=ModelSupport::getComposite(SMap,modIndex,
		      "21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,modIndex,
		      "31 -32 33 -34 35 -36 (-21:22:-23:24:-25:26)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Ter layer
  Out=ModelSupport::getComposite(SMap,modIndex,
		      "41 -42 43 -44 45 -46 (-31:32:-33:34:-35:36)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,modIndex,
		      "51 -52 53 -54 55 -56 (-41:42:-43:44:-45:46)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Outer
  Out=ModelSupport::getComposite(SMap,modIndex,
		      "61 -62 63 -64 65 -66 (-51:52:-53:54:-55:56)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

void
H2Section::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:
  ELog::RegMethod RegA("H2Section","createLinks");
  
  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+61));
  FixedComp::setLinkSurf(1,SMap.realSurf(modIndex+62));
  FixedComp::setLinkSurf(2,-SMap.realSurf(modIndex+63));
  FixedComp::setLinkSurf(3,SMap.realSurf(modIndex+64));
  FixedComp::setLinkSurf(4,-SMap.realSurf(modIndex+65));
  FixedComp::setLinkSurf(5,SMap.realSurf(modIndex+66));

  return;
}

Geometry::Vec3D
H2Section::getSurfacePoint(const size_t layerIndex,
			  const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Section","getSurfacePoint");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>6) 
    throw ColErr::IndexError<size_t>(layerIndex,6,"layerIndex");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  LayerInfo LA(height,width,depth);

  if (!layerIndex)
    {
      const LayerInfo LX(frontWall,backWall,mainWall,
			 mainWall,mainWall,mainWall);
      LA+=LX;
      const double offset[6]={0,vacGap,midAlThick,heGap,outAlThick,outGap};
      for(size_t i=1;i!=layerIndex;i++)
	LA+=offset[i];
    }

  return Origin+XYZ[sideIndex]*LA.Item(sideIndex);
}

std::string
H2Section::getLayerString(const size_t sideIndex,
			  const size_t layerIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Section","getLayerString");

  if (layerIndex>6) 
    throw ColErr::IndexError<size_t>(layerIndex,6,"layerIndex");
  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  const int signValue((sideIndex % 2) ? 1 : -1);
  std::ostringstream cx;
  cx<<" "<<signValue*SMap.realSurf(SI)<<" ";
  return cx.str();
}

int
H2Section::getLayerSurf(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Section","getLayerSurf");

  if (layerIndex>6) 
    throw ColErr::IndexError<size_t>(layerIndex,6,"layerIndex");
  if (sideIndex>5)
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  return SMap.realSurf(SI);
}

void
H2Section::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
		      
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("H2Section","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
