/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/BeRefInnerStructure.cxx
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

namespace essSystem
{

BeRefInnerStructure::BeRefInnerStructure(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,6),
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
  waterDiscThick(A.waterDiscThick),
  waterDiscMat(A.waterDiscMat),
  waterDiscWallMat(A.waterDiscWallMat),
  BeRadius(A.BeRadius),
  BeMat(A.BeMat),
  BeWallThick(A.BeWallThick),
  BeWallMat(A.BeWallMat)
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
      waterDiscThick=A.waterDiscThick;
      waterDiscMat=A.waterDiscMat;
      waterDiscWallThick=A.waterDiscWallThick;
      waterDiscWallMat=A.waterDiscWallMat;
      BeRadius=A.BeRadius;
      BeMat=A.BeMat;
      BeWallThick=A.BeWallThick;
      BeWallMat=A.BeWallMat;
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

  waterDiscThick=Control.EvalVar<double>(keyName+"WaterDiscThick");
  waterDiscMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterDiscMat");
    
  waterDiscWallThick=Control.EvalVar<double>(keyName+"WaterDiscWallThick");
  waterDiscWallMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterDiscWallMat");

  BeRadius=Control.EvalVar<double>(keyName+"BeRadius");
  BeMat=ModelSupport::EvalMat<int>(Control,keyName+"BeMat");

  BeWallThick=Control.EvalVar<double>(keyName+"BeWallThick");
  BeWallMat=ModelSupport::EvalMat<int>(Control,keyName+"BeWallMat");

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
    \param Reflector :: Reflector object
  */
{
  ELog::RegMethod RegA("BeRefInnerStructure","createSurfaces");

  // This is EVIL !!!
  const double BeRefZBottom = Reflector.getLinkPt(7)[2];
  const double BeRefZTop = Reflector.getLinkPt(8)[2];
  
  ModelSupport::buildPlane(SMap,insIndex+5,
                           Origin+Z*(BeRefZBottom+waterDiscThick),Z);
  ModelSupport::buildPlane(SMap,insIndex+6,
                           Origin+Z*(BeRefZTop-waterDiscThick),Z);
  
  ModelSupport::buildPlane(SMap,insIndex+15,
            Origin+Z*(BeRefZBottom+waterDiscThick+waterDiscWallThick),Z);
  ModelSupport::buildPlane(SMap,insIndex+16,
            Origin+Z*(BeRefZTop-waterDiscThick-waterDiscWallThick),Z);

  ModelSupport::buildCylinder(SMap,insIndex+7,Origin,Z,BeRadius);
  ModelSupport::buildCylinder(SMap,insIndex+17,Origin,Z,BeRadius+BeWallThick);
  
  return; 
}

void
BeRefInnerStructure::createObjects(Simulation& System,
                                   const attachSystem::FixedComp& Reflector)
  /*!
    Create the objects
    \param System :: Simulation to add results
    \param Reflector :: Reflector object where the inner structure is to be added
  */
{
  ELog::RegMethod RegA("BeRefInnerStructure","createObjects");
  
  const attachSystem::CellMap* CM =
    dynamic_cast<const attachSystem::CellMap*>(&Reflector);
  MonteCarlo::Object* LowBeObj(0);
  MonteCarlo::Object* TopBeObj(0);
  int lowBeCell(0);
  int topBeCell(0);
  
  if (CM)
    {
      lowBeCell=CM->getCell("lowBe");
      LowBeObj=System.findQhull(lowBeCell);
      
      topBeCell=CM->getCell("topBe");
      TopBeObj=System.findQhull(topBeCell);
    }
  if (!LowBeObj)
    throw ColErr::InContainerError<int>(topBeCell,"Reflector lowBe cell not found");
  if (!TopBeObj)
    throw ColErr::InContainerError<int>(topBeCell,"Reflector topBe cell not found");
  
  std::string Out;
  const std::string lowBeStr = Reflector.getLinkString(7);
  const std::string topBeStr = Reflector.getLinkString(8);
  const std::string sideBeStr = Reflector.getLinkString(9);
  HeadRule HR, LowBeExclude, TopBeExclude;
  
  // Bottom Be cell
  Out = ModelSupport::getComposite(SMap, insIndex, " -5 ");
  HR.procString(lowBeStr);
  HR.makeComplement();
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterDiscMat,0,
                                   Out+sideBeStr+HR.display()));
  
  Out = ModelSupport::getComposite(SMap, insIndex, " 5 -15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++, waterDiscWallMat, 0,
                                   Out+sideBeStr));

  Out = ModelSupport::getComposite(SMap, insIndex, " -7 15");
  System.addCell(MonteCarlo::Qhull(cellIndex++, BeMat, 0,
                                   Out+Reflector.getLinkString(10)));
  
  Out = ModelSupport::getComposite(SMap,insIndex," -17 7 15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,BeWallMat,0,
                                   Out+Reflector.getLinkString(10)));
  
  Out = ModelSupport::getComposite(SMap,insIndex," -17 15 ");
  LowBeExclude.procString(Out);
  
  Out = ModelSupport::getComposite(SMap,insIndex," -15 ");
  LowBeExclude.addUnion(Out);
  LowBeExclude.makeComplement();
  LowBeObj->addSurfString(LowBeExclude.display());
  
  // Top Be cell
  Out = ModelSupport::getComposite(SMap,insIndex," 6 ");
  HR.procString(topBeStr);
  HR.makeComplement();
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterDiscMat,0,
                                   Out+sideBeStr+HR.display()));
  
  Out = ModelSupport::getComposite(SMap,insIndex," 16 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterDiscWallMat,
                                   0,Out+sideBeStr));
  
  Out = ModelSupport::getComposite(SMap,insIndex," -7 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++, BeMat,0,
                                   Out+Reflector.getLinkString(11)));
  
  Out = ModelSupport::getComposite(SMap, insIndex, " -17 7 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,BeWallMat,0,
                                   Out+Reflector.getLinkString(11)));
  
  Out = ModelSupport::getComposite(SMap,insIndex," -17 -16 ");
  TopBeExclude.procString(Out);
  
  Out = ModelSupport::getComposite(SMap,insIndex," 16 ");
  TopBeExclude.addUnion(Out);
  TopBeExclude.makeComplement();
  TopBeObj->addSurfString(TopBeExclude.display());
  
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
