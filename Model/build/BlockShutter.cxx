/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/BlockShutter.cxx
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
#include <numeric>
#include <iterator>
#include <memory>
#include <functional>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfDIter.h"
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
#include "shutterBlock.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "collInsertCyl.h"
#include "BlockShutter.h"

namespace shutterSystem
{

BlockShutter::BlockShutter(const size_t ID,const std::string& K,
			 const std::string& ZK) :
  GeneralShutter(ID,K),b4cMat(47),
  blockKey(ZK)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Index number of shutter
    \param K :: Key name
    \param ZK :: zoom Key name
  */
{}

BlockShutter::BlockShutter(const BlockShutter& A) : 
  GeneralShutter(A),b4cMat(A.b4cMat),
  blockKey(A.blockKey),nBlock(A.nBlock),colletHGap(A.colletHGap),
  colletVGap(A.colletVGap),colletFGap(A.colletFGap),
  colletMat(A.colletMat),iBlock(A.iBlock),
  colletInnerCell(A.colletInnerCell),
  colletOuterCell(A.colletOuterCell)
  /*!
    Copy constructor
    \param A :: BlockShutter to copy
  */
{}

BlockShutter&
BlockShutter::operator=(const BlockShutter& A)
  /*!
    Assignment operator
    \param A :: BlockShutter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralShutter::operator=(A);
      blockKey=A.blockKey;
      nBlock=A.nBlock;
      colletHGap=A.colletHGap;
      colletVGap=A.colletVGap;
      colletFGap=A.colletFGap;
      colletMat=A.colletMat;
      iBlock=A.iBlock;
      colletInnerCell=A.colletInnerCell;
      colletOuterCell=A.colletOuterCell;
    }
  return *this;
}

BlockShutter::~BlockShutter() 
  /*!
    Destructor
  */
{}

void
BlockShutter::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BlockShutter","populate");

  const FuncDataBase& Control=System.getDataBase();

  // Modification to the general shutter populated variables:
  
  nBlock=Control.EvalVar<int>(blockKey+"NBlocks");
  zStart=Control.EvalVar<double>(blockKey+"ZStart");
  // Note this is in mRadian
  xAngle=Control.EvalVar<double>(blockKey+"GuideXAngle")*M_PI/180.0;
  zAngle=Control.EvalVar<double>(blockKey+"GuideZAngle")*M_PI/180.0;
  xStep=Control.EvalVar<double>(blockKey+"GuideXStep");
  
  colletHGap=Control.EvalVar<double>(blockKey+"ColletHGap");
  colletVGap=Control.EvalVar<double>(blockKey+"ColletVGap");
  colletFGap=Control.EvalVar<double>(blockKey+"ColletFGap");
  colletMat=ModelSupport::EvalMat<int>(Control,blockKey+"ColletMat");

  populated|=2;
  return;
}

void
BlockShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("BlockShutter","createSurfaces");

  // Inner cut [on flightline]
  ModelSupport::buildPlane
    (SMap,surfIndex+325,
     frontPt+Z*(-colletVGap+voidHeightInner/2.0+centZOffset),zSlope);
  
  // Inner cut [on flightline]
  ModelSupport::buildPlane
    (SMap,surfIndex+326,
     frontPt-Z*(-colletVGap+voidHeightInner/2.0+centZOffset),zSlope);

  // Outer cut [on flightline]
  ModelSupport::buildPlane
    (SMap,surfIndex+425,
     frontPt+Z*(-colletVGap+voidHeightOuter/2.0+centZOffset),zSlope);
  
  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+426,
      frontPt-Z*(-colletVGap+voidHeightOuter/2.0-centZOffset),zSlope);

  // HORRIZONTAL
  ModelSupport::buildPlane(SMap,surfIndex+313,
	  Origin-X*(-colletHGap+voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+314,
		   Origin+X*(-colletHGap+voidWidthInner/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+413,
         Origin-X*(-colletHGap+voidWidthOuter/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+414,
	    Origin+X*(-colletHGap+voidWidthOuter/2.0),X);
  
  // Forward gap
  ModelSupport::buildPlane(SMap,surfIndex+401,
	   frontPt+Y*(voidDivide+colletFGap),Y);

  return;
}  

BlockShutter::zbTYPE
BlockShutter::makeBlockUnit(const FuncDataBase& Control,
			    const int index) const
  /*!
    Create the correct block unit
    \param Control :: Control object
    \param index :: Index of block
    \return shared_ptr to block type
  */
{
  ELog::RegMethod RegA("BlockShutter","makeBlockUnit");

  const std::string tKey=
    StrFunc::makeString(blockKey+"Block",index+1)+"TYPE";
  const int cylFlag=Control.EvalDefVar<int>(tKey,0);

  const int SI(surfIndex+1000+100*index);
  return (!cylFlag) ?
    zbTYPE(new collInsertBlock(index,SI,blockKey+"Block")) 
    :  zbTYPE(new collInsertCyl(index,SI,blockKey+"Block"));

}

void
BlockShutter::createInsert(Simulation& System)
  /*!
    Create the insert
    \param System :: Simulation to replace
  */
{
  ELog::RegMethod RegA("BlockShutter","createInsert");
  const FuncDataBase& Control=System.getDataBase();
  // Create
  std::string Out;

  
  size_t cutPt(0);
  // Get Test object
  const MonteCarlo::Qhull* OuterCell=System.findQhull(colletOuterCell);
  // Create First Object: (or only object)
   if (nBlock>1)
    {
      zbTYPE ItemZB=makeBlockUnit(Control,0);
      Out=ModelSupport::getComposite(SMap,surfIndex,"7 ")+divideStr();
      ItemZB->initialize(System,*this);
      ItemZB->setOrigin(frontPt+Y*0.01,xStep,xAngle,zStart,zAngle);
      ItemZB->createAll(System,0,Out,"");
      iBlock.push_back(ItemZB);
    }
  for(int i=1;i<nBlock-1;i++)
    {
      zbTYPE ItemZB=makeBlockUnit(Control,i);
      ItemZB->createAll(System,*iBlock.back());
      
      iBlock.push_back(ItemZB);
       // Nasty code to force using the system:
      if (OuterCell && OuterCell->isValid(ItemZB->getExit()))
	{
	  OuterCell=0;
	  ItemZB->insertObjects(System);
	  cutPt=iBlock.size();
	  processColletExclude(System,colletInnerCell,0,cutPt);
	  cutPt--;   // iBlocksize -1
	}
    }
  // Outer Cell
  if (nBlock>2)
    {
      zbTYPE ItemZB=makeBlockUnit(Control,nBlock-1);
      const zbTYPE ZB= iBlock.back();  // previous block
      ItemZB->initialize(System,*ZB);
      Out=ModelSupport::getComposite(SMap,surfIndex,"-17 ")+divideStr();
      ItemZB->createAll(System,ZB->getLinkSurf(2),"",Out);
      iBlock.push_back(ItemZB);
    }
  processColletExclude(System,colletOuterCell,cutPt,iBlock.size());	  
  return;
}


void
BlockShutter::processColletExclude(Simulation& System,const int cellN,
				  const size_t indexA,const size_t indexB)
  /*!
    Builds and handles the collet holder, finding the minium number 
    of surfaces that need to be tracked in
    \param System :: Simulation to use
    \param cellN :: shutterVoid cell 
    \param indexA :: initial block number to check
    \param indexB :: final block number to check
   */
{
  // Get first point
  ELog::RegMethod RegA("BlockShutter","processColletExclude");
  size_t firstCell(0);
  for(size_t i=indexA+1;i<indexB;i++)
    {
      const zbTYPE ZB=iBlock[i-1];
      if (!iBlock[i]->equalExternal(*ZB))
	{
	  if (firstCell)
	    ZB->addOuterSurf(iBlock[firstCell]->getLinkSurf(1));
	  ZB->addOuterSurf(ZB->getLinkSurf(-2));
	  ZB->setInsertCell(cellN);
	  ZB->insertObjects(System);
	  firstCell=i;
	}
    }
  // Always add one block:
  zbTYPE ZOut=iBlock.back();
  if (firstCell)
    ZOut->addOuterSurf(iBlock[firstCell]->getLinkSurf(-1));
  ZOut->setInsertCell(cellN);
  ZOut->insertObjects(System);
  return;
}

void
BlockShutter::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BlockShutter","constructObjects");

  std::string Out,OutB;
  // Create divide string

  const std::string dSurf=divideStr();  
  // Flightline

  if (voidDivide>0.0)
    {
      // exclude from flight line
      Out=ModelSupport::getComposite
	(SMap,surfIndex," (-313:314:325:-326) ");
      OutB=ModelSupport::getComposite
	(SMap,surfIndex," (-413:414:425:-426) ");
      MonteCarlo::Qhull* VObjA=System.findQhull(innerVoidCell);
      MonteCarlo::Qhull* VObjB=System.findQhull(innerVoidCell+1);

      if (!VObjA || !VObjB)
	{
	  ELog::EM<<"Failed to find innerObject: "<<innerVoidCell
		  <<ELog::endErr;
	  return;
	}
      
      VObjA->addSurfString(Out);
      VObjB->addSurfString(OutB);
      // Inner Collet
      colletInnerCell=cellIndex;
      Out=ModelSupport::getComposite
	(SMap,surfIndex,"313 -314 -325 326 7 -401")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,colletMat,0.0,Out));
      // OuterCollet
      colletOuterCell=cellIndex;
      Out=ModelSupport::getComposite
	(SMap,surfIndex,"413 -414 -425 426 -17 401");
      System.addCell(MonteCarlo::Qhull(cellIndex++,colletMat,0.0,Out));
      // SPACER:
      Out=ModelSupport::getComposite
	(SMap,surfIndex,"413 -414 -425 426 100 -401 (-313:314:325:-326)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
    }
  else
    {
      // single shutter object [worth doing?]  
    }
  return;
}

Geometry::Vec3D
BlockShutter::getExitTrack() const
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    \return Exit direction
  */
{
  ELog::RegMethod RegA("BlockShutter","getExitTrack");

  std::vector<zbTYPE>::const_iterator ac=
    find_if(iBlock.begin(),iBlock.end(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));

  
  std::vector<zbTYPE>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));

  if (ac==iBlock.end() || bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endErr;
      return Y;
    }
  const masterRotate& MR=masterRotate::Instance();
  const Geometry::Vec3D PtA=(*bc)->getWindowCentre()-
    (*ac)->getWindowCentre();
  ELog::EM<<"Pt A == "<<MR.calcRotate((*ac)->getWindowCentre())<<ELog::endTrace;
  ELog::EM<<"Pt B == "<<MR.calcRotate((*bc)->getWindowCentre())<<ELog::endTrace;
  ELog::EM<<"Pt axis == "<<MR.calcAxisRotate(PtA.unit())<<ELog::endTrace;
  return PtA.unit();
}

Geometry::Vec3D
BlockShutter::getExitPoint() const
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    - Use the centre of the  
    \return Exit direction
  */
{
  ELog::RegMethod RegA("BlockShutter","getExitPoint");
  
  std::vector<zbTYPE>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));
  if (bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endCrit;
      return (iBlock.empty()) ? Origin : iBlock.back()->getCentre();
    }
  return (*bc)->getWindowCentre();
}

double
BlockShutter::processShutterDrop() const
  /*!
    Calculate the drop on the shutter, given that the 
    fall has to be such that the shutter takes neutrons with
    the correct angle for the shutter.
    \return drop value
  */
{
  ELog::RegMethod RegA("BlockShutter","processShutterDrop");
  // Calculate the distance between the moderator/shutter enterance.
  // currently it is to the target centre

  const double drop=innerRadius*tan(zAngle);
  return drop-zStart;
} 

void
BlockShutter::setTwinComp()
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    - Use the centre of the b4c
  */
{
  ELog::RegMethod RegA("BlockShutter","setTwinComp");
    
  std::vector<zbTYPE>::const_iterator ac=
    find_if(iBlock.begin(),iBlock.end(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));

  std::vector<zbTYPE>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));

  if (ac==iBlock.end() || bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endCrit;
      return;
    }
  
  const double zCShift=(closed % 2) ? 
    closedZShift-openZShift : 0;

  bEnter=(*ac)->getWindowCentre()-Z*zCShift;
  bExit=(*bc)->getWindowCentre()-Z*zCShift;
  bY=bExit-bEnter;
  bY.makeUnit();
  bZ=Z;

  ELog::EM<<"Axis == "<<bY<<ELog::endDebug;

  Geometry::Quaternion::calcQRot(zAngle,X).rotate(bZ);
  bX=bZ*bY;
  if (X.dotProd(bX)<0)
    bX*=-1;
  // Only amount to add is closed shutter offset:
  
  return;
}

std::vector<Geometry::Vec3D>
BlockShutter::createFrontViewPoints() const
  /*!
    Given a point, make the back projection of the shutter view
    \return String of surfaces [ADDED to SMAP]
  */
{
  ELog::RegMethod RegA("BlockShutter","createViewSurf");

  std::vector<Geometry::Vec3D> Opts;

  std::vector<zbTYPE>::const_iterator ba=
    find_if(iBlock.begin(),iBlock.end(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));
  if (ba!=iBlock.end())
    {
      std::vector<zbTYPE>::const_iterator bb=
	find_if(ba+1,iBlock.end(),
		std::bind(std::equal_to<int>(),
		    std::bind<int>(&collInsertBase::getMat,
				   std::placeholders::_1),b4cMat));
      if (bb!=iBlock.end())
	Opts=(*ba)->viewWindow(bb->get());
      ELog::EM<<"Front window "<<ELog::endDebug;
      return Opts;
    }

  ELog::EM<<"Problem finding B4C blocks"<<ELog::endErr;
  return Opts;
}

std::vector<Geometry::Vec3D>
BlockShutter::createBackViewPoints() const
  /*!
    Given a point, make the back projection of the shutter view
    \return String of surfaces [ADDED to SMAP]
  */
{
  ELog::RegMethod RegA("BlockShutter","createViewSurf");

  std::vector<Geometry::Vec3D> Opts;

  std::vector<zbTYPE>::const_reverse_iterator ba=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    std::bind(std::equal_to<int>(),
			std::bind<int>(&collInsertBase::getMat,
				       std::placeholders::_1),b4cMat));
  if (ba!=iBlock.rend())
    {
      std::vector<zbTYPE>::const_reverse_iterator bb=
	find_if(ba+1,iBlock.rend(),
		std::bind(std::equal_to<int>(),
		    std::bind<int>(&collInsertBase::getMat,
				   std::placeholders::_1),b4cMat));
      if (bb!=iBlock.rend())
	Opts=(*ba)->viewWindow(bb->get());
      ELog::EM<<"Back window "<<ELog::endDebug;
      return Opts;
    }

  ELog::EM<<"Problem finding B4C blocks"<<ELog::endErr;
  return Opts;
}

  
void
BlockShutter::createAll(Simulation& System,const double,
		       const attachSystem::FixedComp* FCPtr)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FCPtr :: Fixed pointer for shutter origin [void centre]
  */
{
  ELog::RegMethod RegA("BlockShutter","createAll");
  GeneralShutter::populate(System);
  populate(System);  
  GeneralShutter::createAll(System,processShutterDrop(),FCPtr);

  createSurfaces();
  createObjects(System);  
  createInsert(System);
  setTwinComp();

  return;
}
  
}  // NAMESPACE shutterSystem
