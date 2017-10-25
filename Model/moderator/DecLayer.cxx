/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/DecLayer.cxx
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
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "Decoupled.h"
#include "DecLayer.h"

namespace moderatorSystem
{

DecLayer::DecLayer(const std::string& Key,const std::string& LKey)  :
  Decoupled(Key),lkeyName(LKey),
  layerIndex(ModelSupport::objectRegister::Instance().cell(LKey)),
  cellIndex(layerIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for base decoupled object
    \param LKey :: Name for layered object
  */
{}

DecLayer::DecLayer(const DecLayer& A) : 
  Decoupled(A),
  layerIndex(A.layerIndex),cellIndex(A.cellIndex)
  /*!
    Copy constructor
    \param A :: DecLayer to copy
  */
{}

DecLayer&
DecLayer::operator=(const DecLayer& A)
  /*!
    Assignment operator
    \param A :: DecLayer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Decoupled::operator=(A);
      layerIndex=A.layerIndex;
      cellIndex=A.cellIndex;
    }
  return *this;
}


DecLayer::~DecLayer() 
  /*!
    Destructor
  */
{}

void
DecLayer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("DecLayer","populate");

  const int nLayers=Control.EvalVar<int>(lkeyName+"NLayers");
  ELog::EM<<"Number of layers == "<<nLayers<<ELog::endTrace;
  for(int i=0;i<nLayers;i++)
     {
       const std::string keyIndex(StrFunc::makeString(lkeyName,i+1));
       const double T=Control.EvalPair<double>(keyIndex,lkeyName,"Thick");
       if (T>Geometry::zeroTol)
	 {
	   lThick.push_back(T);
	   lMat.push_back
	     (ModelSupport::EvalMat<int>(Control,keyIndex+"Mat",
					 lkeyName+"Mat"));
	   lTemp.push_back
	     (Control.EvalPair<double>(keyIndex,lkeyName,"Temp"));
	 }	 
     }
  centMat=ModelSupport::EvalMat<int>(Control,lkeyName+"CentMat");
  centTemp=Control.EvalVar<double>(lkeyName+"CentTemp");
  populated |= 1;  
  return;
}
  
void
DecLayer::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DecLayer","createSurfaces");
  
  int offset(layerIndex);
  double totalThick(0.0);
  for(size_t i=0;i<lThick.size();i++)
    {
      totalThick+=lThick[i];
      ModelSupport::buildCylinder(SMap,offset+7,westCentre-Y*totalThick,
				  Z,westRadius);  // wish
      ModelSupport::buildCylinder(SMap,offset+8,eastCentre+Y*totalThick,
				  Z,eastRadius);  // narrow
      offset+=10;
    }

  return;
}

void
DecLayer::createObjects(Simulation& System)
  /*!
    Create the inner methane object
   */
{
  ELog::RegMethod RegA("DecLayer","createObjects");
  // Outer layer is 7, 8 of old system:
  // WEST SIDE:
  const std::string WWall=ModelSupport::getComposite(SMap,decIndex," 1 3 -4 5 -6 ");
  std::string OutWall=ModelSupport::getComposite(SMap,decIndex," -7 ");
  int offset(layerIndex);
  for(size_t i=0;i<lThick.size();i++)
    {
      const std::string Out=ModelSupport::getComposite(SMap,offset," 7 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],lTemp[i],
				       Out+OutWall+WWall));
      OutWall=ModelSupport::getComposite(SMap,offset," -7 ");
      offset+=10;
    }
  // Inner Half:
  System.addCell(MonteCarlo::Qhull(cellIndex++,centMat,centTemp,
				   OutWall+WWall));      

  // EAST SIDE:
  const std::string EWall=ModelSupport::getComposite(SMap,decIndex," -1 3 -4 5 -6 ");
  OutWall=ModelSupport::getComposite(SMap,decIndex," -8 ");
  offset=layerIndex;
  for(size_t i=0;i<lThick.size();i++)
    {
      const std::string Out=ModelSupport::getComposite(SMap,offset," 8 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],lTemp[i],
				       Out+OutWall+EWall));
      OutWall=ModelSupport::getComposite(SMap,offset," -8 ");
      offset+=10;
    }
  // Inner Half:
  System.addCell(MonteCarlo::Qhull(cellIndex++,centMat,centTemp,
				   OutWall+EWall));
  return;
}


void
DecLayer::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp for axis/origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("DecLayer","createAll");

  // Check that everything from here is called in Decoupled:
  ELog::EM<<"Calling createAll"<<ELog::endTrace;
  Decoupled::createAll(System,FC,sideIndex);
  populate(System.getDataBase());
  createSurfaces();
  createObjects(System);
  System.removeCell(methCell);
  
  System.validateObjSurfMap();
  return;
}
  
}  // NAMESPACE moderatorSystem
