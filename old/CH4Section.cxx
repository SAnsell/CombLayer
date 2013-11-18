/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/CH4Section.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "ModBase.h"
#include "CH4Section.h"

namespace ts1System
{

CH4Section::CH4Section(const std::string& Key)  :
  ModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4Section::CH4Section(const CH4Section& A) : 
  ModBase(A),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  height(A.height),width(A.width),depth(A.depth),
  frontWall(A.frontWall),backWall(A.backWall),
  mainWall(A.mainWall),vacGap(A.vacGap),heGap(A.heGap),
  outGap(A.outGap),midAlThick(A.midAlThick),
  outAlThick(A.outAlThick),poisonYStep1(A.poisonYStep1),
  poisonYStep2(A.poisonYStep2),poisonThick(A.poisonThick),
  pCladThick(A.pCladThick),alMat(A.alMat),ch4Mat(A.ch4Mat),
  poisonMat(A.poisonMat),pCladMat(A.pCladMat),
  ch4Temp(A.ch4Temp)
  /*!
    Copy constructor
    \param A :: CH4Section to copy
  */
{}

CH4Section&
CH4Section::operator=(const CH4Section& A)
  /*!
    Assignment operator
    \param A :: CH4Section to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ModBase::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
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
      poisonYStep1=A.poisonYStep1;
      poisonYStep2=A.poisonYStep2;
      poisonThick=A.poisonThick;
      pCladThick=A.pCladThick;
      alMat=A.alMat;
      ch4Mat=A.ch4Mat;
      poisonMat=A.poisonMat;
      pCladMat=A.pCladMat;
      ch4Temp=A.ch4Temp;
    }
  return *this;
}

CH4Section*
CH4Section::clone() const
 /*!
   Clone constructor
 */
{
  return new CH4Section(*this);
}

CH4Section::~CH4Section() 
 /*!
   Destructor
 */
{}

void
CH4Section::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("CH4Section","populate");
  
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

// Poison
  poisonYStep1=Control.EvalVar<double>(keyName+"PoisonYStep1");
  poisonYStep2=Control.EvalVar<double>(keyName+"PoisonYStep2");  
  poisonThick=Control.EvalVar<double>(keyName+"PoisonGdThick");
  pCladThick=Control.EvalVar<double>(keyName+"PCladThick");

  // Materials
  poisonMat=Control.EvalVar<int>(keyName+"PoisonMat");
  pCladMat=Control.EvalVar<int>(keyName+"PCladMat");
  alMat=Control.EvalVar<int>(keyName+"AlMat");
  ch4Mat=Control.EvalVar<int>(keyName+"CH4Mat");
  ch4Temp=Control.EvalVar<double>(keyName+"CH4Temp");

  return;
}
  
void
CH4Section::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("CH4Section","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);
  return;
}

void
CH4Section::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CH4Section","createSurface");

  std::vector<LayerInfo> LVec;
  LayerInfo LI(depth,width,height);
  LayerInfo LA(backWall,frontWall,mainWall,
	       mainWall,mainWall,mainWall);
  LVec.push_back(LI);   // Inner
  LI+=LA;  LVec.push_back(LI);     // Al walls
  LI+=vacGap;  LVec.push_back(LI);     // Vac space
  LI+=midAlWall;  LVec.push_back(LI);     // Vac walls
  LI+=heGap;  LVec.push_back(LI);     // He space
  LI+=outerAlWall;  LVec.push_back(LI);     // Outer wall
  LI+=outGap;  LVec.push_back(LI);     // Outer wall

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  int ch4Layer(modIndex+1);
  for(size_t i=0;i<LVec.size();i++)
    {
      // 1+j+j%2 gives 1,1,3,3,5,5 as j iterates
      for(size_t j=0;j<6;j++)
	ModelSupport::buildPlane(SMap,ch4Layer+static_cast<int>(j),
				 Origin+XYZ[j]*LVec[i].Item(j),XYZ[1+j-j%2]);
      ch4Layer+=10;
    }

  // Poison layers (Gd+Al):
  ModelSupport::buildPlane(SMap,modIndex+101,Origin+
                      Y*(poisonYStep1-poisonThick/2.0),Y);
  ModelSupport::buildPlane(SMap,modIndex+102,Origin+
                      Y*(poisonYStep1+poisonThick/2.0),Y);
  ModelSupport::buildPlane(SMap,modIndex+111,Origin+
                      Y*(poisonYStep1-poisonThick/2.0-pCladThick),Y);
  ModelSupport::buildPlane(SMap,modIndex+112,Origin+
                      Y*(poisonYStep1+poisonThick/2.0+pCladThick),Y);  

  ModelSupport::buildPlane(SMap,modIndex+201,Origin+
			   Y*(poisonYStep2-poisonThick/2.0),Y);
  ModelSupport::buildPlane(SMap,modIndex+202,Origin+
			   Y*(poisonYStep2+poisonThick/2.0),Y);
  ModelSupport::buildPlane(SMap,modIndex+211,Origin+
			   Y*(poisonYStep2-poisonThick/2.0-pCladThick),Y);
  ModelSupport::buildPlane(SMap,modIndex+212,Origin+
			   Y*(poisonYStep2+poisonThick/2.0+pCladThick),Y);  


  FixedComp::setConnect(0,Origin-Y*(depth/2.0+backWall+offset),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+backWall+offset),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0+mainWall+offset),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+mainWall+offset),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+mainWall+offset),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+mainWall+offset),Z);

  return;
}

void
CH4Section::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("CH4Section","addToInsertChain");

  for(int i=modIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
 }


void
CH4Section::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4Section","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,modIndex,"61 -62 63 -64 65 -66 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  // Poison layer A
  Out=ModelSupport::getComposite(SMap,modIndex,"101 -102 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,poisonMat,ch4Temp,Out));
  
  Out=ModelSupport::getComposite(SMap,modIndex,"111 -101 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,ch4Temp,Out));
  
  Out=ModelSupport::getComposite(SMap,modIndex,"-112 102 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,ch4Temp,Out));

  // nPoisonLayers = 1
  // CH4      
  Out=ModelSupport::getComposite(SMap,modIndex,"1 -111 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,ch4Temp,Out));
  
  Out=ModelSupport::getComposite(SMap,modIndex,"112 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,ch4Temp,Out));    
  

  // Al
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,ch4Temp,Out));

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
CH4Section::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:
  ELog::RegMethod RegA("CH4Section","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+61));
  FixedComp::setLinkSurf(1,SMap.realSurf(modIndex+62));
  FixedComp::setLinkSurf(2,-SMap.realSurf(modIndex+63));
  FixedComp::setLinkSurf(3,SMap.realSurf(modIndex+64));
  FixedComp::setLinkSurf(4,-SMap.realSurf(modIndex+65));
  FixedComp::setLinkSurf(5,SMap.realSurf(modIndex+66));

  return;
}

Geometry::Vec3D
CH4Section::getSurfacePoint(const size_t layerIndex,
			  const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CH4Section","getSurfacePoint");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>=4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layerIndex");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  return Origin+XYZ[sideIndex]*LVec[layerIndex].Item(sideIndex);
}

std::string
CH4Section::getLayerString(const size_t sideIndex,
			   const size_t layerIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4Section","getLayerString");

  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");
  if (sideIndex>5)
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  std::ostringstream cx;
  cx<<" "<<-SMap.realSurf(SI)<<" ";
  return cx.str();
}

int
CH4Layer::getLayerSurf(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLayerSurf");

  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");
  if (sideIndex>5)
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  return SMap.realSurf(SI);
}

void
CH4Section::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC)
		      
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("CH4Section","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
