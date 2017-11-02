/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/BeamStop.cxx
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
#include <functional>
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "beamBlock.h"
#include "BeamStop.h"

namespace hutchSystem
{

BeamStop::BeamStop(const std::string& Key)  :
  attachSystem::FixedComp(Key,2),
  stopIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(stopIndex+1),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamStop::BeamStop(const BeamStop& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  stopIndex(A.stopIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),innerWidth(A.innerWidth),innerHeight(A.innerHeight),
  innerLength(A.innerLength),steelWidth(A.steelWidth),
  steelHeight(A.steelHeight),steelLength(A.steelLength),
  steelDepth(A.steelDepth),voidWidth(A.voidWidth),
  voidHeight(A.voidHeight),voidDepth(A.voidDepth),
  voidLength(A.voidLength),concWidth(A.concWidth),
  concHeight(A.concHeight),concLength(A.concLength),
  concDepth(A.concDepth),defInnerMat(A.defInnerMat),
  steelMat(A.steelMat),concMat(A.concMat),BBlock(A.BBlock),
  nLayers(A.nLayers),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: BeamStop to copy
  */
{}

BeamStop&
BeamStop::operator=(const BeamStop& A)
  /*!
    Assignment operator
    \param A :: BeamStop to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      innerWidth=A.innerWidth;
      innerHeight=A.innerHeight;
      innerLength=A.innerLength;
      steelWidth=A.steelWidth;
      steelHeight=A.steelHeight;
      steelLength=A.steelLength;
      steelDepth=A.steelDepth;
      voidWidth=A.voidWidth;
      voidHeight=A.voidHeight;
      voidDepth=A.voidDepth;
      voidLength=A.voidLength;
      concWidth=A.concWidth;
      concHeight=A.concHeight;
      concLength=A.concLength;
      concDepth=A.concDepth;
      defInnerMat=A.defInnerMat;
      steelMat=A.steelMat;
      concMat=A.concMat;
      BBlock=A.BBlock;
      nLayers=A.nLayers;
      CDivideList=A.CDivideList;
    }
  return *this;
}

BeamStop::~BeamStop() 
  /*!
    Destructor
  */
{}

void
BeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BeamStop","populate");

  beamBlock Item;
  size_t flag(0);
  size_t itemNum(0);
  do 
    {
      if (flag==beamBlock::Size)
	{
	  BBlock.push_back(Item);
	  itemNum++;
	  if (itemNum>20)
	    {
	      throw ColErr::IndexError<size_t>
		(itemNum,20,"All variables in a block totally generic");
	    }
	}
      for(flag=0;flag<beamBlock::Size && 
	    Item.setFromControl(Control,keyName+"Inner",itemNum,flag);
	  flag++) ;
    } while (flag==beamBlock::Size);

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");


  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");
  

  steelWidth=Control.EvalVar<double>(keyName+"SteelWidth");
  steelDepth=Control.EvalVar<double>(keyName+"SteelDepth");
  steelLength=Control.EvalVar<double>(keyName+"SteelLength");
  steelHeight=Control.EvalVar<double>(keyName+"SteelHeight");

  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");

  concWidth=Control.EvalVar<double>(keyName+"ConcWidth");
  concDepth=Control.EvalVar<double>(keyName+"ConcDepth");
  concLength=Control.EvalVar<double>(keyName+"ConcLength");
  concHeight=Control.EvalVar<double>(keyName+"ConcHeight");

  defInnerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");
  
  return;
}

void
BeamStop::createUnitVector(const attachSystem::FixedComp& LC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param LC :: LinearComponent the beamstop is attached to.
    \param sideIndex :: Side to conect to
  */
{
  ELog::RegMethod RegA("BeamStop","createUnitVector");

//  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  FixedComp::createUnitVector(LC,sideIndex);
  applyShift(xStep,yStep,zStep);
  
//  CS.setENum(chipIRDatum::collExit,MR.calcRotate(ExitPoint));  
  return;
}

void
BeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamStop","createSurface");

  // INNER BOX:
  // OUTSIDE Front Face:  
  ModelSupport::buildPlane(SMap,stopIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,stopIndex+2,Origin+Y*innerLength,Y);
  ModelSupport::buildPlane(SMap,stopIndex+3,Origin-X*innerWidth/2.0,X);
  ModelSupport::buildPlane(SMap,stopIndex+4,Origin+X*innerWidth/2.0,X);
  ModelSupport::buildPlane(SMap,stopIndex+5,Origin-Z*innerHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,stopIndex+6,Origin+Z*innerHeight/2.0,Z);

  // STEEL BOX:
  ModelSupport::buildPlane(SMap,stopIndex+12,
			   Origin+Y*(innerLength+steelLength),Y);
  ModelSupport::buildPlane(SMap,stopIndex+13,
			   Origin-X*(innerWidth/2.0+steelWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+14,
			   Origin+X*(innerWidth/2.0+steelWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+15,
			   Origin-Z*(innerHeight/2.0+steelHeight),Z);
  ModelSupport::buildPlane(SMap,stopIndex+16,
			   Origin+Z*(innerHeight/2.0+steelDepth),Z);

  // Void BOX:
  ModelSupport::buildPlane(SMap,stopIndex+22,
			   Origin+Y*(innerLength+steelLength+voidLength),Y);
  ModelSupport::buildPlane(SMap,stopIndex+23,
			   Origin-X*(innerWidth/2.0+steelWidth+voidWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+24,
			   Origin+X*(innerWidth/2.0+steelWidth+voidWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+25,
			   Origin-Z*(innerHeight/2.0+steelHeight+voidHeight),Z);
  ModelSupport::buildPlane(SMap,stopIndex+26,
			   Origin+Z*(innerHeight/2.0+steelDepth+voidDepth),Z);

  // Concrete BOX:
  ModelSupport::buildPlane(SMap,stopIndex+32,
	   Origin+Y*(innerLength+steelLength+voidLength+concLength),Y);
  ModelSupport::buildPlane(SMap,stopIndex+33,
	   Origin-X*(innerWidth/2.0+steelWidth+voidWidth+concWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+34,
	   Origin+X*(innerWidth/2.0+steelWidth+voidWidth+concWidth),X);
  ModelSupport::buildPlane(SMap,stopIndex+35,
	   Origin-Z*(innerHeight/2.0+steelHeight+voidHeight+concHeight),Z);
  ModelSupport::buildPlane(SMap,stopIndex+36,
	   Origin+Z*(innerHeight/2.0+steelDepth+voidDepth+concDepth),Z);

  return;
}

void
BeamStop::createDefSurfaces()
  /*!
    Create a surface stack in the default [n
  */
{
  ELog::RegMethod RegA("BeamStop","createDefSurfaces");

  double totalThick(0.0);
  for(int i=0;i<static_cast<int>(BBlock.size());i++)
    {
      const beamBlock& BB=BBlock[static_cast<size_t>(i)];
      totalThick+=BB.thickness;
      Geometry::Vec3D RAxis;
      if (std::abs(BB.angle)>1e-3)
	{
	  RAxis= Y;
	  Geometry::Quaternion::calcQRotDeg(BB.angle,Z).rotate(RAxis);
	  ModelSupport::buildPlane(SMap,stopIndex+10*i+101,
				   Origin+Y*totalThick,RAxis);

	  RAxis= Y;
	  Geometry::Quaternion::calcQRotDeg(-BB.angle,Z).rotate(RAxis);
	  ModelSupport::buildPlane(SMap,stopIndex+10*i+102,
				   Origin+Y*totalThick,RAxis);
	}
      else
	{
	  ModelSupport::buildPlane(SMap,stopIndex+10*i+101,
				   Origin+Y*totalThick,Y);
	}
    }
  return;
}

void
BeamStop::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components : only called if 
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("BeamStop","createObjects");

  std::string Out;

  std::string frontSurf=" 1 ";
  // Full OUTER EXclude
  Out=ModelSupport::getComposite(SMap,stopIndex,"1 -32 33 -34 35 -36");
  addOuterSurf(Out);

  // Create Inner:
  
  // CREATE INNER [Note need 1 and 2 to cope with angled items]
  std::string Base=
    ModelSupport::getComposite(SMap,stopIndex,"1 -2 3 -4 5 -6 ");
  std::string Next;
  std::string Prev;

  std::ostringstream dx;
  double totalThick(0.0);
  for(int i=0;i<static_cast<int>(BBlock.size());i++)
    {
      const beamBlock& BB=BBlock[static_cast<size_t>(i)];
      totalThick+=BB.thickness;
      if (std::abs(BB.angle)<1e-3)
	{
	  Out=Base+Prev+
	    ModelSupport::getComposite(SMap,stopIndex+i*10," -101 ");
	  Prev=ModelSupport::getComposite(SMap,stopIndex+i*10," 101 ");
	}
      else
	{
	  Out=Base+Prev+
	    ModelSupport::getComposite(SMap,stopIndex+i*10," -101 -102 ");
	  Prev=ModelSupport::getComposite(SMap,stopIndex+i*10," (101:102) ");
	}
      System.addCell(MonteCarlo::Qhull(cellIndex++,BB.matN,0.0,Out));
    }
  // REAR SPACER:
  if (innerLength-totalThick>0.0)
    {

      Out=Base+Prev;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
    }
  
  // STEEL LAYER
  Out=ModelSupport::getComposite(SMap,stopIndex,
		  "1 -12 13 -14 15 -16 (2:-3:4:-5:6)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
  // VOID LAYER
  Out=ModelSupport::getComposite(SMap,stopIndex,
		  "1 -22 23 -24 25 -26 (12:-13:14:-15:16)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // Concrete LAYER
  Out=ModelSupport::getComposite(SMap,stopIndex,
		  "1 -32 33 -34 35 -36 (22:-23:24:-25:26)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  return;
}

void 
BeamStop::layerProcess(Simulation& )
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
  */
{
  ELog::RegMethod RegA("BeamStop","LayerProcess");
  
  
  return;
}

  
void
BeamStop::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("BeamStop","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*
			(innerLength+steelLength+voidLength+concLength),Y);
  

  FixedComp::setLinkSurf(0,-SMap.realSurf(1));
  FixedComp::setLinkSurf(1,SMap.realSurf(32));

  
  return;
}

  
void
BeamStop::createAll(Simulation& System,
		    const attachSystem::FixedComp& LC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComponent to use
    \param sideIndex :: index point [signed]
  */
{
  ELog::RegMethod RegA("BeamStop","createAll");

  populate(System.getDataBase());
  createUnitVector(LC,sideIndex);
  createSurfaces();  

  createDefSurfaces();
  createObjects(System);
  layerProcess(System);
  createLinks();
  insertObjects(System);
 
  return;
}
  
}  // NAMESPACE shutterSystem
