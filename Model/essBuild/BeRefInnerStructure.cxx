/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/DiskPreSimple.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "BeRefInnerStructure.h"
// for layerProcess:
#include "surfDIter.h"
#include "surfDivide.h"
#include "surfDBase.h"
#include "mergeTemplate.h"


namespace essSystem
{

  BeRefInnerStructure::BeRefInnerStructure(const std::string& Key) :
    attachSystem::ContainedComp(),
    attachSystem::FixedComp(Key,6), attachSystem::CellMap(),
    insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(insIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {}

  BeRefInnerStructure::BeRefInnerStructure(const BeRefInnerStructure& A) : 
    attachSystem::ContainedComp(A),
    attachSystem::FixedComp(A),
    insIndex(A.insIndex),
    cellIndex(A.cellIndex),
    nLayers(A.nLayers)
    /*!
      Copy constructor
      \param A :: BeRefInnerStructure to copy
    */
  {}

  BeRefInnerStructure&
  BeRefInnerStructure::operator=(const BeRefInnerStructure& A)
  /*!
    Assignment operator
    \param A :: BeRefInnerStructure to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedComp::operator=(A);
	cellIndex=A.cellIndex;
	nLayers=A.nLayers;
      }
    return *this;
  }

  BeRefInnerStructure*
  BeRefInnerStructure::clone() const
  /*!
    Clone self 
    \return new (this)
  */
  {
    return new BeRefInnerStructure(*this);
  }

  BeRefInnerStructure::~BeRefInnerStructure()
  /*!
    Destructor
  */
  {}
  

  void
  BeRefInnerStructure::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","populate");

    nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
    ModelSupport::populateDivideLen(Control,nLayers,keyName+"BaseLen", 1.0, baseFrac); // \todo check if 1.0 is correct here
    ModelSupport::populateDivide(Control,nLayers,keyName+"Mat", 0, mat);

    return;
  }

  void
  BeRefInnerStructure::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);

    return;
  }


  void
  BeRefInnerStructure::createSurfaces(const attachSystem::FixedComp& Reflector)
  /*!
    Create planes for the inner structure iside BeRef
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","createSurfaces");

    return; 
  }

  void
  BeRefInnerStructure::createObjects(Simulation& System, const attachSystem::FixedComp& Reflector)
  /*!
    Create the objects
    \param System :: Simulation to add results
    \param Reflector :: Reflector object where the inner structure is to be added
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","createObjects");

    layerProcess(System, Reflector, "topBe", 10, 7);
    layerProcess(System, Reflector, "lowBe", 9, 6);

    return;
  }

  void
  BeRefInnerStructure::createLinks()
  /*!
    Creates a full attachment set
  */
  {  
    ELog::RegMethod RegA("BeRefInnerStructure","createLinks");


    return;
  }

  void
  BeRefInnerStructure::layerProcess(Simulation& System, const attachSystem::FixedComp& Reflector, const std::string& cellName, const int& lpS, const int& lsS)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param Reflector :: FC object
    \param cellName :: top or bottom Be cell
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","layerProcess");
    if (nLayers>1)
      {
	const int pS = Reflector.getLinkSurf(lpS);
	const int sS = Reflector.getLinkSurf(lsS);

	const attachSystem::CellMap* CM = dynamic_cast<const attachSystem::CellMap*>(&Reflector);
	MonteCarlo::Object* beObj(0);
	int beCell(0);

	if (CM)
	  {
	    beCell=CM->getCell(cellName);
	    beObj=System.findQhull(beCell);
	  }

	if (!beObj)
	  throw ColErr::InContainerError<int>(beCell,"Reflector topBe cell not found");

	ModelSupport::surfDivide DA;
	for(size_t i=1;i<nLayers;i++)
	  {
	    DA.addFrac(baseFrac[i-1]);
	    DA.addMaterial(mat[i-1]);
	  }
	DA.addMaterial(mat.back());

	DA.setCellN(beCell);
	DA.setOutNum(cellIndex, insIndex+10000);

	ModelSupport::mergeTemplate<Geometry::Plane,
				    Geometry::Plane> surroundRule;

	surroundRule.setSurfPair(SMap.realSurf(pS),
				 SMap.realSurf(sS));

	std::string OutA = Reflector.getLinkString(lpS);
	std::string OutB = Reflector.getLinkComplement(lsS);

	surroundRule.setInnerRule(OutA);
	surroundRule.setOuterRule(OutB);

	DA.addRule(&surroundRule);
	DA.activeDivideTemplate(System);

	cellIndex=DA.getCellNum();
      }
  }

  void
  BeRefInnerStructure::createAll(Simulation& System,
				 const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
  */
  {
    ELog::RegMethod RegA("BeRefInnerStructure","createAll");

    populate(System.getDataBase());
    createUnitVector(FC);

    createSurfaces(FC);
    createObjects(System, FC);
    createLinks();

    insertObjects(System);       
    return;
  }

}  // NAMESPACE essSystem
