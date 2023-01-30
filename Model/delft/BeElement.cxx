/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeElement.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <climits>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "LayerDivide1D.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"
#include "BeElement.h"

namespace delftSystem
{


BeElement::BeElement(const size_t XI,const size_t YI,
		     const std::string& Key) :
  RElement(XI,YI,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X index
    \param YI :: Y index
    \param Key :: KeyName
  */
{}

BeElement::BeElement(const BeElement& A) : 
  RElement(A),
  Width(A.Width),Depth(A.Depth),TopHeight(A.TopHeight),
  nLayer(A.nLayer),beMat(A.beMat)
  /*!
    Copy constructor
    \param A :: BeElement to copy
  */
{}
  
BeElement&
BeElement::operator=(const BeElement& A)
  /*!
    Assignment operator
    \param A :: BeElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RElement::operator=(A);
      Width=A.Width;
      Depth=A.Depth;
      TopHeight=A.TopHeight;
      nLayer=A.nLayer;
      beMat=A.beMat;
    }
  return *this;
}

void
BeElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BeElement","populate");

  Width=ReactorGrid::getElement<double>
    (Control,baseName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,baseName+"Depth",XIndex,YIndex);
  TopHeight=ReactorGrid::getElement<double>
    (Control,baseName+"TopHeight",XIndex,YIndex);

  beMat=ReactorGrid::getMatElement
    (Control,baseName+"Mat",XIndex,YIndex);

  nLayer=ReactorGrid::getElement<size_t>
    (Control,baseName+"NLayer",XIndex,YIndex);

  return;
}

void
BeElement::createSurfaces()
  /*!
    Creates/duplicates the surfaces for this block
  */
{  
  ELog::RegMethod RegA("BeElement","createSurface");

  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Z*TopHeight,Z);

  return;
}

void
BeElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("BeElement","createObjects");

  const HeadRule& baseHR=getRule("BasePlate");
  HeadRule HR;
  // Outer Layers
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -6");
  makeCell("Main",System,cellIndex++,beMat,0.0,HR*baseHR);

  addOuterSurf(HR);      
  return;
}


void
BeElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
BeElement::layerProcess(Simulation& System,
			const FuelLoad& FuelSystem)
  /*!
    Layer all the be elements.
    Note all calls go to FuelSystem.getMaterial

    \param System :: Simulation
    \param FuelSystem :: Fuel Load system for materials
  */
{
  ELog::RegMethod RegA("BeElement","layerProcess");
  
  if (nLayer<2)
    {
      const int MatN=
	FuelSystem.getMaterial(XIndex+1,YIndex+1,0,1,beMat);
      if (MatN!=beMat)
	{
	  const int CN=getCell("Main");
	  MonteCarlo::Object* OPtr=System.findObject(CN);
	  if (!OPtr)
	    throw ColErr::InContainerError<int>
	      (CN,"["+keyName+"] Main Be Cell");
	  
	  OPtr->setMaterial(MatN);
	}
      return;
    }

  ModelSupport::LayerDivide1D LD1(keyName+"Main");

  LD1.setSurfPair(SMap.realSurf(buildIndex+5),-SMap.realSurf(buildIndex+6));
  LD1.setFractions(nLayer);  

  std::vector<int> DefMat;
  for(size_t i=0;i<nLayer;i++)
    {
      const int MatN=
	FuelSystem.getMaterial(XIndex+1,YIndex+1,0,i+1,beMat);
      DefMat.push_back(MatN);
    }
  LD1.setMaterials(DefMat);
  LD1.divideCell(System,getCell("Main"));
  return;
}

void
BeElement::createAll(Simulation& System,
                     const attachSystem::FixedComp& RG,
		     const long int sideIndex)
  /*!
    Creation of the Be-Reflector unit
    \param System :: Simulation to add component to
    \param RG :: Fixed Unit
    \param sideIndex :: Link index
  */
{
  ELog::RegMethod RegA("BeElement","createAll");

  populate(System.getDataBase());

  createUnitVector(RG,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  if (FuelPtr)
    layerProcess(System,*FuelPtr);
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
