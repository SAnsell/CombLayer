/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomShutter.cxx
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
#include "ZoomShutter.h"

namespace shutterSystem
{

ZoomShutter::ZoomShutter(const size_t ID,const std::string& K,
			 const std::string& ZK) :
  GeneralShutter(ID,K),zoomKey(ZK),
  colletInnerCell(0),colletOuterCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Index number of shutter
    \param K :: Key name
    \param ZK :: zoom Key name
  */
{}

ZoomShutter::ZoomShutter(const ZoomShutter& A) : 
  GeneralShutter(A),
  zoomKey(A.zoomKey),nBlock(A.nBlock),xStart(A.xStart),
  xAngle(A.xAngle),zStart(A.zStart),zAngle(A.zAngle),
  colletHGap(A.colletHGap),colletVGap(A.colletVGap),
  colletFGap(A.colletFGap),colletMat(A.colletMat),
  iBlock(A.iBlock),colletInnerCell(A.colletInnerCell),
  colletOuterCell(A.colletOuterCell)
  /*!
    Copy constructor
    \param A :: ZoomShutter to copy
  */
{}

ZoomShutter&
ZoomShutter::operator=(const ZoomShutter& A)
  /*!
    Assignment operator
    \param A :: ZoomShutter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralShutter::operator=(A);
      zoomKey=A.zoomKey;
      nBlock=A.nBlock;
      xStart=A.xStart;
      xAngle=A.xAngle;
      zStart=A.zStart;
      zAngle=A.zAngle;
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

ZoomShutter::~ZoomShutter() 
  /*!
    Destructor
  */
{}

void
ZoomShutter::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomShutter","populate");

  const FuncDataBase& Control=System.getDataBase();

  // Modification to the general shutter populated variables:
  
  nBlock=Control.EvalVar<int>(zoomKey+"NBlocks");
  zStart=Control.EvalVar<double>(zoomKey+"ZStart");
  // Note this is in mRadian
  xAngle=Control.EvalVar<double>(zoomKey+"GuideXAngle")/1000.0; 
  zAngle=Control.EvalVar<double>(zoomKey+"GuideZAngle")/1000.0; 
  
  colletHGap=Control.EvalVar<double>(zoomKey+"ColletHGap");
  colletVGap=Control.EvalVar<double>(zoomKey+"ColletVGap");
  colletFGap=Control.EvalVar<double>(zoomKey+"ColletFGap");
  colletMat=ModelSupport::EvalMat<int>(Control,zoomKey+"ColletMat");

  populated|=2;
  return;
}

void
ZoomShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("ZoomShutter","createSurfaces");

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

void
ZoomShutter::createInsert(Simulation& System)
  /*!
    Create the insert
    \param System :: Simulation to replace
  */
{
  ELog::RegMethod RegA("ZoomShutter","createInsert");
  // Create
  std::string Out;
  
  size_t cutPt(0);
  // Get Test object
  const MonteCarlo::Qhull* OuterCell=System.findQhull(colletOuterCell);
  // Create First Object:
  if (nBlock>1)
    {
      collInsertBlock ItemZB(0,surfIndex+500,"zoomShutterBlock");
      Out=ModelSupport::getComposite(SMap,surfIndex,"7 ")+divideStr();
      ItemZB.initialize(System,*this);
      ItemZB.setOrigin(frontPt+Y*colletFGap,xStart,xAngle,zStart,zAngle);
      ItemZB.createAll(System,0,Out,"");
      iBlock.push_back(ItemZB);
    }
  for(int i=1;i<nBlock-1;i++)
    {
      collInsertBlock ItemZB(i,surfIndex+1000+i*100,"zoomShutterBlock");
      ItemZB.createAll(System,iBlock.back());
      iBlock.push_back(ItemZB);
       // Nasty code to force using the system:
      if (OuterCell && OuterCell->isValid(ItemZB.getExit()))
	{
	  OuterCell=0;
	  ItemZB.insertObjects(System);
	  cutPt=iBlock.size();
	  processColletExclude(System,colletInnerCell,0,cutPt);
	  cutPt--;   // iBlocksize -1
	}

    }
  // Outer Cell
  if (nBlock>2)
    {
      collInsertBlock ItemZB(nBlock-1,surfIndex+1000+(nBlock-1)*100,
			     "zoomShutterBlock");
      const collInsertBlock& ZB=iBlock.back();
      ItemZB.initialize(System,ZB);
      Out=ModelSupport::getComposite(SMap,surfIndex,"-17 ")+divideStr();
      ItemZB.createAll(System,ZB.getLinkSurf(2),"",Out);
      iBlock.push_back(ItemZB);
    }
  processColletExclude(System,colletOuterCell,cutPt,iBlock.size());	  
  return;
}

void
ZoomShutter::processColletExclude(Simulation& System,const int cellN,
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
  ELog::RegMethod RegA("ZoomShutter","processColletExclude");
  size_t firstCell(0);
  for(size_t i=indexA+1;i<indexB;i++)
    {
      collInsertBlock& ZB=iBlock[i-1];
      if (!iBlock[i].equalExternal(ZB))
	{
	  if (firstCell)
	    ZB.addOuterSurf(iBlock[firstCell].getLinkSurf(1));
	  ZB.addOuterSurf(ZB.getLinkSurf(-2));
	  ZB.setInsertCell(cellN);
	  ZB.insertObjects(System);
	  firstCell=i;
	}
    }
  // Always add one block:
  collInsertBlock& ZOut=iBlock.back();
  if (firstCell)
    ZOut.addOuterSurf(iBlock[firstCell].getLinkSurf(-1));
  ZOut.setInsertCell(cellN);
  ZOut.insertObjects(System);
  return;
}

void
ZoomShutter::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomShutter","constructObjects");

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
ZoomShutter::getExitTrack() const
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    \return Exit direction
  */
{
  ELog::RegMethod RegA("ZoomShutter","getExitTrack");

  const int b4cMat(47);
  std::vector<collInsertBlock>::const_iterator ac=
    find_if(iBlock.begin(),iBlock.end(),
	    [b4cMat](const collInsertBlock& cb)
	    {
	      return (cb.getMat()==b4cMat);
	    });

  std::vector<collInsertBlock>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    [b4cMat](const collInsertBlock& cb)
	    {
	      return (cb.getMat()==b4cMat);
	    });
	    
  if (ac==iBlock.end() || bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endErr;
      return Y;
    }
  const masterRotate& MR=masterRotate::Instance();
  const Geometry::Vec3D PtA=bc->getWindowCentre()-
    ac->getWindowCentre();
  ELog::EM<<"Pt A == "<<MR.calcRotate(ac->getWindowCentre())<<ELog::endTrace;
  ELog::EM<<"Pt B == "<<MR.calcRotate(bc->getWindowCentre())<<ELog::endTrace;
  ELog::EM<<"Pt axis == "<<MR.calcAxisRotate(PtA.unit())<<ELog::endTrace;
  return PtA.unit();
}

Geometry::Vec3D
ZoomShutter::getExitPoint() const
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    - Use the centre of the  
    \return Exit direction
  */
{
  ELog::RegMethod RegA("ZoomShutter","getExitPoint");
  
  const int b4cMat(47);

  std::vector<collInsertBlock>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    [b4cMat](const collInsertBlock& cb)
	    {
	      return (cb.getMat()==b4cMat);
	    });

  if (bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endCrit;
      return (iBlock.empty()) ? Origin : iBlock.back().getCentre();
    }
  return bc->getWindowCentre();
}

double
ZoomShutter::processShutterDrop() const
  /*!
    Calculate the drop on the shutter, given that the 
    fall has to be such that the shutter takes neutrons with
    the correct angle for the shutter.
    \return drop value
  */
{
  ELog::RegMethod RegA("ZoomShutter","processShutterDrop");
  // Calculate the distance between the moderator/shutter enterance.
  // currently it is to the target centre

  const double drop=innerRadius*tan(zAngle);
  return drop-zStart;
} 

void
ZoomShutter::setTwinComp()
  /*!
    Determine the effective direction from the shutter exit
    - Constructed by taking the original point (centre) to the 
      centre point of the last unit
    - Use the centre of the b4c
  */
{
  ELog::RegMethod RegA("ZoomShutter","setTwinComp");
    
  const int b4cMat(47);
  std::vector<collInsertBlock>::const_iterator ac=
    find_if(iBlock.begin(),iBlock.end(),
	    [b4cMat](const collInsertBlock& cb)
	    {
	      return (cb.getMat()==b4cMat);
	    });


  std::vector<collInsertBlock>::const_reverse_iterator bc=
    find_if(iBlock.rbegin(),iBlock.rend(),
	    [b4cMat](const collInsertBlock& cb)
	    {
	      return (cb.getMat()==b4cMat);
	    });


  if (ac==iBlock.end() || bc==iBlock.rend())
    {
      ELog::EM<<"Problem finding B4C blocks"<<ELog::endErr;
      return;
    }
  
  const double zCShift=(closed % 2) ? 
    closedZShift-openZShift : 0;

  bEnter=ac->getWindowCentre()-Z*zCShift;
  bExit=bc->getWindowCentre()-Z*zCShift;
  bY=bExit-bEnter;
  bY.makeUnit();
  bZ=Z;

  Geometry::Quaternion::calcQRot(zAngle,X).rotate(bZ);
  bX=bZ*bY;
  if (X.dotProd(bX)<0)
    bX*=-1;
  // Only amount to add is closed shutter offset:
  
  return;
}

void
ZoomShutter::createAll(Simulation& System,const double,
		       const attachSystem::FixedComp* FCPtr)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("ZoomShutter","createAll");
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
