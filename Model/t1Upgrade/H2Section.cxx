/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/H2Section.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
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
H2Section::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("H2Section","populate");
  
  ModBase::populate(Control);

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  if (nLayers==0) nLayers=1;  // Layers include middle

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
      int plateIndex(buildIndex+100);
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
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  // Al layer
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(depth/2.0+backWall),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(depth/2.0+frontWall),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width/2.0+mainWall),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width/2.0+mainWall),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height/2.0+mainWall),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0+mainWall),Z);

  double offset=vacGap;
  // vac layer
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=midAlThick;
  // al mid layer
  ModelSupport::buildPlane(SMap,buildIndex+31,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=heGap;
  // al mid layer
  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+43,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+45,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=outAlThick;
  // al mid layer
  ModelSupport::buildPlane(SMap,buildIndex+51,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+52,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+53,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+54,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+55,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+56,
			   Origin+Z*(height/2.0+mainWall+offset),Z);

  offset+=outGap;
  // al mid layer
  ModelSupport::buildPlane(SMap,buildIndex+61,
			   Origin-Y*(depth/2.0+backWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+62,
			   Origin+Y*(depth/2.0+frontWall+offset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+63,
			   Origin-X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+64,
			   Origin+X*(width/2.0+mainWall+offset),X);
  ModelSupport::buildPlane(SMap,buildIndex+65,
			   Origin-Z*(height/2.0+mainWall+offset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+66,
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

  for(int i=buildIndex+1;i<cellIndex;i++)
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

  Out=ModelSupport::getComposite(SMap,buildIndex,"61 -62 63 -64 65 -66 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  
  if (nSi>0)
    {
      int plateIndex(buildIndex+100);
      // inital
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -101 3 -4 5 -6");
      System.addCell(MonteCarlo::Object(cellIndex++,lh2Mat,lh2Temp,Out));
      for(int i=0;i<nSi;i++)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex,
					 plateIndex,"1M -2M 3 -4 5 -6");
	  System.addCell(MonteCarlo::Object(cellIndex++,siMat,lh2Temp,Out));
	  if (i!=nSi-1)
	    {
	      Out=ModelSupport::getComposite(SMap,buildIndex,
					     plateIndex,"2M -11M 3 -4 5 -6");
	      System.addCell(MonteCarlo::Object(cellIndex++,lh2Mat,lh2Temp,Out));
	    }
	  plateIndex+=10;
	}
      // final
      Out=ModelSupport::getComposite(SMap,buildIndex,plateIndex-10,
				     "2M -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Object(cellIndex++,lh2Mat,lh2Temp,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
      System.addCell(MonteCarlo::Object(cellIndex++,lh2Mat,lh2Temp,Out));
    }

  // AL

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  System.addCell(MonteCarlo::Object(cellIndex++,alMat,lh2Temp,Out));

  // vac layer
  Out=ModelSupport::getComposite(SMap,buildIndex,
		      "21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
		      "31 -32 33 -34 35 -36 (-21:22:-23:24:-25:26)");
  System.addCell(MonteCarlo::Object(cellIndex++,alMat,0.0,Out));

  // Ter layer
  Out=ModelSupport::getComposite(SMap,buildIndex,
		      "41 -42 43 -44 45 -46 (-31:32:-33:34:-35:36)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
		      "51 -52 53 -54 55 -56 (-41:42:-43:44:-45:46)");
  System.addCell(MonteCarlo::Object(cellIndex++,alMat,0.0,Out));

  // Outer
  Out=ModelSupport::getComposite(SMap,buildIndex,
		      "61 -62 63 -64 65 -66 (-51:52:-53:54:-55:56)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

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
  
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+61));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+62));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+63));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+64));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+65));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+66));

  return;
}

Geometry::Vec3D
H2Section::getSurfacePoint(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Section","getSurfacePoint");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  if (SI>5) 
    throw ColErr::IndexError<long int>(sideIndex,5,"sideIndex");
  if (layerIndex>6) 
    throw ColErr::IndexError<size_t>(layerIndex,6,"layerIndex");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  LayerInfo LA(height,width,depth);

  if (layerIndex)
    {
      const LayerInfo LX(frontWall,backWall,mainWall,
			 mainWall,mainWall,mainWall);
      LA+=LX;
      const double offset[6]={0,vacGap,midAlThick,heGap,outAlThick,outGap};
      for(size_t i=1;i!=layerIndex;i++)
	LA+=offset[i];
    }

  return Origin+XYZ[SI]*LA.Item(SI);
}

std::string
H2Section::getLayerString(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Section","getLayerString");
  return std::to_string(getLayerSurf(layerIndex,sideIndex));
}

int
H2Section::getLayerSurf(const size_t layerIndex,
			const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface indexxs
  */
{
  ELog::RegMethod RegA("H2Section","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");
  if (!sideIndex || std::abs(sideIndex)>6)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");

  const size_t uSIndex(static_cast<size_t>(std::abs(sideIndex)));
  const int SI(buildIndex+static_cast<int>(layerIndex*10+uSIndex));
  int signValue((sideIndex<0) ? -1 : 1);
  signValue *= (sideIndex % 2) ? -1 : 1;
  return signValue*SMap.realSurf(SI);
}

void
H2Section::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("H2Section","createAll");
  createAll(System,FC,sideIndex,FC,sideIndex);
  return;
}

void
H2Section::createAll(Simulation& System,
		     const attachSystem::FixedComp& orgFC,
		     const long int orgIndex,
		     const attachSystem::FixedComp& axisFC,
		     const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("H2Section","createAll");
  
  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
