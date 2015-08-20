/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/DiskPre.cxx
  *
  * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "ContainedComp.h"
#include "CellMap.h"
#include "DiskPreModFlowGuide.h"

namespace essSystem
{

  DiskPreModFlowGuide::DiskPreModFlowGuide(const std::string& Key) :
    attachSystem::ContainedComp(),
    attachSystem::FixedComp(Key,6),
    insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(insIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {}

  DiskPreModFlowGuide::DiskPreModFlowGuide(const DiskPreModFlowGuide& A) : 
    attachSystem::ContainedComp(A),
    attachSystem::FixedComp(A),
    insIndex(A.insIndex),
    cellIndex(A.cellIndex),
    wallThick(A.wallThick),
    wallMat(A.wallMat),
    gapWidth(A.gapWidth),
    nBaffles(A.nBaffles)
    /*!
      Copy constructor
      \param A :: DiskPreModFlowGuide to copy
    */
  {}

  DiskPreModFlowGuide&
  DiskPreModFlowGuide::operator=(const DiskPreModFlowGuide& A)
  /*!
    Assignment operator
    \param A :: DiskPreModFlowGuide to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedComp::operator=(A);
	cellIndex=A.cellIndex;
	wallThick=A.wallThick;
	wallMat=A.wallMat;
	gapWidth=A.gapWidth;
	nBaffles=A.nBaffles;
      }
    return *this;
  }

  DiskPreModFlowGuide*
  DiskPreModFlowGuide::clone() const
  /*!
    Clone self 
    \return new (this)
  */
  {
    return new DiskPreModFlowGuide(*this);
  }

  DiskPreModFlowGuide::~DiskPreModFlowGuide()
  /*!
    Destructor
  */
  {}
  

  void
  DiskPreModFlowGuide::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("DiskPreModFlowGuide","populate");

    wallThick=Control.EvalVar<double>(keyName+"WallThick");
    wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
    gapWidth=Control.EvalVar<double>(keyName+"GapWidth");
    nBaffles=Control.EvalVar<int>(keyName+"NBaffles");

    return;
  }

  void
  DiskPreModFlowGuide::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
  */
  {
    ELog::RegMethod RegA("DiskPreModFlowGuide","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);

    return;
  }


  void
  DiskPreModFlowGuide::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create planes for the inner structure iside DiskPreMod
  */
  {
    ELog::RegMethod RegA("DiskPreModFlowGuide","createSurfaces");

    const double radius = fabs(FC.getLinkPt(6)[0]) > Geometry::zeroTol ? fabs(FC.getLinkPt(6)[0]) : fabs(FC.getLinkPt(6)[1]);
    const double dy = radius*2.0/static_cast<double>(nBaffles+1); // y-distance between plates

    ModelSupport::buildPlane(SMap, insIndex+3, Origin-X*(wallThick/2.0), X);
    ModelSupport::buildPlane(SMap, insIndex+4, Origin+X*(wallThick/2.0), X);

    ModelSupport::buildCylinder(SMap, insIndex+7, Origin, Z, radius-gapWidth);
    ModelSupport::buildPlane(SMap, insIndex+14, Origin-X*(gapWidth+wallThick/2.0), X);
    ModelSupport::buildPlane(SMap, insIndex+24, Origin+X*(gapWidth+wallThick/2.0), X);

    double y(0.0);
    int SI(insIndex);
    for (size_t i=0; i<nBaffles; i++)
      {
	y = -radius + dy*static_cast<double>(i+1);
	ModelSupport::buildPlane(SMap, SI+1, Origin+Y*(y-wallThick/2.0), Y);
	ModelSupport::buildPlane(SMap, SI+2, Origin+Y*(y+wallThick/2.0), Y);
	SI += 10;
      }

    return; 
  }

  void
  DiskPreModFlowGuide::createObjects(Simulation& System, attachSystem::FixedComp& FC)
  /*!
    Create the objects
    \param System :: Simulation to add results
    \param FC :: FC object where the inner structure is to be added
  */
  {
    ELog::RegMethod RegA("DiskPreModFlowGuide","createObjects");
    
    attachSystem::CellMap* CM = dynamic_cast<attachSystem::CellMap*>(&FC);
    MonteCarlo::Object* InnerObj(0);
    int innerCell(0);

    if (CM)
      {
	innerCell=CM->getCell("Inner");
	InnerObj=System.findQhull(innerCell);
      }
    if (!InnerObj)
      throw ColErr::InContainerError<int>(innerCell,"DiskPreMod inner cell not found");

    const int innerMat = InnerObj->getMat();
    const double innerTemp = InnerObj->getTemp();
    CM->deleteCell(System, "Inner");
    
    std::string Out;
    const std::string topBottomStr = FC.getLinkString(7) + FC.getLinkString(8);
    const std::string sideStr = FC.getLinkString(6);

    // central plate
    Out=ModelSupport::getComposite(SMap,insIndex," 3 -4 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0,Out+topBottomStr+sideStr));

    // side plates
    int SI(insIndex);
    for (size_t i=0; i<nBaffles; i++)
      {
	// Baffles
	if (i%2)
	  {
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 -14M  ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0,Out+topBottomStr+sideStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 14M -3M  ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 24M  ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0,Out+topBottomStr+sideStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 -24M 4M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 ");
	  }
	else if (i<nBaffles)
	  {
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 -3M -7M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0,Out+topBottomStr));

	    // x<0
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 7M -3M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	    // same but x>0 - divided by surf 3M to gain speed
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 7M 3M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 4M -7M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0,Out+topBottomStr));

	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " 1 -2 ");
	  }

	// Splitting of innerCell (to gain speed)
	if (i==0)
	  {
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " -1 -3M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	    Out = ModelSupport::getComposite(SMap, SI, insIndex, " -1 4M ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	  }
	else
	  {
	    Out = ModelSupport::getComposite(SMap, SI, SI-10, " -1 2M ") + ModelSupport::getComposite(SMap, insIndex, " -3 ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	    
	    Out = ModelSupport::getComposite(SMap, SI, SI-10, " -1 2M ") + ModelSupport::getComposite(SMap, insIndex, " 4 ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));

	    if (i == nBaffles-1)
	    {
	      Out = ModelSupport::getComposite(SMap, SI, insIndex, " 2 -3M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	      Out = ModelSupport::getComposite(SMap, SI, insIndex, " 2 4M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+topBottomStr+sideStr));
	    }
	  }
	
	SI += 10;
      }

    return; 
  }

  void
  DiskPreModFlowGuide::createLinks()
  /*!
    Creates a full attachment set
  */
  {  
    ELog::RegMethod RegA("DiskPreModFlowGuide","createLinks");


    return;
  }

  void
  DiskPreModFlowGuide::createAll(Simulation& System,
				 attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
  */
  {
    ELog::RegMethod RegA("DiskPreModFlowGuide","createAll");

    populate(System.getDataBase());
    createUnitVector(FC);

    createSurfaces(FC);
    createObjects(System, FC);
    createLinks();

    insertObjects(System);       
    return;
  }

}  // NAMESPACE essSystem
