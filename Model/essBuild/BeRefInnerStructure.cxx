/********************************************************************* 
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BeRefInnerStructure.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "ExternalCut.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "BeRefInnerStructure.h"


namespace essSystem
{

BeRefInnerStructure::BeRefInnerStructure(const std::string& Key) :
  attachSystem::FixedComp(Key,0),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}



BeRefInnerStructure::~BeRefInnerStructure()
/*!
   Destructor
 */
{}
  
BeRefInnerStructure::BeRefInnerStructure(const BeRefInnerStructure& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  
  nLayers(A.nLayers),baseFrac(A.baseFrac),mat(A.mat),
  active(A.active)
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
      attachSystem::FixedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      nLayers=A.nLayers;
      baseFrac=A.baseFrac;
      mat=A.mat;
      active=A.active;
    }
  return *this;
}

void
BeRefInnerStructure::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeRefInnerStructure","populate");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"BaseLen", 1.0, baseFrac);
  ModelSupport::populateDivide(Control,nLayers,keyName+"Mat", 0, mat);
  
  active=Control.EvalDefVar<int>(keyName+"Active", 1);
  
  return;
}
  
void
BeRefInnerStructure::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    Requires: 
       ExternalCut: RefBase / RefTop to be single surface (planes)
       CellMap: ReflectorUnit to be cell to divide
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("BeRefInnerStructure","layerProcess");

  if (nLayers>1 && isActive("RefBase") && isActive("RefTop"))
    {
      const int pS =ExternalCut::getRule("RefBase").getPrimarySurface();
      const int sS =ExternalCut::getRule("RefTop").getPrimarySurface();

      // this throws:
      MonteCarlo::Object* beObj=
	CellMap::getCellObject(System,"ReflectorUnit");
      
      ModelSupport::surfDivide DA;
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(baseFrac[i-1]);
	  DA.addMaterial(mat[i-1]);
	}
      DA.addMaterial(mat.back());
      
      DA.setCellN(beObj->getName());
      DA.setOutNum(cellIndex, buildIndex+10000);
      
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      
      surroundRule.setSurfPair(SMap.realSurf(pS),
			       SMap.realSurf(sS));
      
      const HeadRule HRA =ExternalCut::getRule("RefBase");
      const HeadRule HRB =ExternalCut::getRule("RefTop");

      surroundRule.setInnerRule(HRA);
      surroundRule.setOuterRule(HRB);
      
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      
      cellIndex=DA.getCellNum();
    }
}

void
BeRefInnerStructure::createAll(Simulation& System,
			       const attachSystem::FixedComp&,
			       const long int)

  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BeRefInnerStructure","createAll");
  
  populate(System.getDataBase());
  
  if (active)
    layerProcess(System);
  
  return;
}

}  // NAMESPACE essSystem
