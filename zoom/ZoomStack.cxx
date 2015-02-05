/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomStack.cxx
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
#include <numeric>
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "varBlock.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ZoomStack.h"

namespace zoomSystem
{

ZoomStack::ZoomStack(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,0),
  stackIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(stackIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ZoomStack::ZoomStack(const ZoomStack& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  stackIndex(A.stackIndex),cellIndex(A.cellIndex),
  nItem(A.nItem),posIndex(A.posIndex),length(A.length),
  height(A.height),Items(A.Items),voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: ZoomStack to copy
  */
{}

ZoomStack&
ZoomStack::operator=(const ZoomStack& A)
  /*!
    Assignment operator
    \param A :: ZoomStack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      nItem=A.nItem;
      posIndex=A.posIndex;
      length=A.length;
      height=A.height;
      Items=A.Items;
      voidCell=A.voidCell;
    }
  return *this;
}


ZoomStack::~ZoomStack() 
 /*!
   Destructor
 */
{}

void
ZoomStack::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("ZoomStack","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values

  nItem=Control.EvalVar<size_t>(keyName+"NItem");

  const size_t DSize(6);
  const size_t ISize(1);
  const std::string sndKey[DSize+ISize]=
    {"Width","StepLift","StepDist","Clear","PathWidth","PathHeight",
     "Mat"};
  
  for(size_t i=0;i<nItem;i++)
    {
      Items.push_back(varBlock(DSize,sndKey,ISize,sndKey+DSize));
      for(size_t j=0;j<DSize+ISize;j++)
	{
	  const std::string KX=
	    StrFunc::makeString(keyName+sndKey[j],i);
	  std::ostringstream cx;
	  if (Control.hasVariable(KX))
	    {
	      if (j<DSize)
		Items[i].setVar(sndKey[j],Control.EvalVar<double>(KX));
	      else
		Items[i].setVar(sndKey[j],
				ModelSupport::EvalMat<int>(Control,KX));
	    }
	  else if (!i)
	    {
	      ELog::EM<<"Failed to connect on first component:: "
		      <<sndKey[j]<<ELog::endErr;
	    }
	  else
	    Items[i].setVar(sndKey[j],Items[i-1]);
	}  
    }

  posIndex=Control.EvalVar<size_t>(keyName+"Index");
  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");

  return;
}
  
void
ZoomStack::createUnitVector(const attachSystem::TwinComp& TT)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param TT :: Twin item 
  */
{
  ELog::RegMethod RegA("ZoomStack","createUnitVector");
  TwinComp::createUnitVector(TT);
  
  Origin=bEnter+Y*10.0;
  for(size_t i=0;i<posIndex;i++)
    Origin+=X*(Items[i].getVar<double>("Width")+
	       Items[i].getVar<double>("Clear"));

  return;
}

void
ZoomStack::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomStack","createSurface");

  // Create Outer surfaces
  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,stackIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,stackIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,stackIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,stackIndex+6,Origin+Z*height/2.0,Z);
  
  int offset(stackIndex);
  Geometry::Vec3D OE(Origin);
  for(size_t i=0;i<nItem;i++)
    {
      const varBlock& LI=Items[i];
      // front metal [base]
      if (!i)
	{
	  ModelSupport::buildPlane(SMap,offset+3,
				   OE-X*LI.getVar<double>("Width"),X);
	  ModelSupport::buildPlane(SMap,offset+13,
				   OE-X*(LI.getVar<double>("Width")-
					 LI.getVar<double>("StepLift")),X);
	  ModelSupport::buildPlane(SMap,offset+12,OE+
				   Y*LI.getVar<double>("StepDist"),Y);
	}
      ModelSupport::buildPlane(SMap,offset+4,
			       OE+X*LI.getVar<double>("Width"),X);
      ModelSupport::buildPlane(SMap,offset+24,
			       OE+X*(LI.getVar<double>("Width")
				     +LI.getVar<double>("Clear")),X);
			       
      ModelSupport::buildPlane(SMap,offset+14,
			       OE+X*(LI.getVar<double>("Width")+
				     LI.getVar<double>("StepLift")),X);

      ModelSupport::buildPlane(SMap,offset+34,
			      OE+X*(LI.getVar<double>("Width")
				    +LI.getVar<double>("Clear")
				    +LI.getVar<double>("StepLift")),X);
			       
       // DIVIDE:
      ModelSupport::buildPlane(SMap,offset+22,OE+
			       Y*LI.getVar<double>("StepDist"),Y);
      ModelSupport::buildPlane(SMap,offset+32,
			       OE+Y*(LI.getVar<double>("StepDist")
				     -LI.getVar<double>("Clear")),Y);
      // BeamLine:
      ModelSupport::buildPlane(SMap,offset+503,OE-X*
			       LI.getVar<double>("PathWidth")/2.0,X);
      ModelSupport::buildPlane(SMap,offset+504,OE+X*
			       LI.getVar<double>("PathWidth")/2.0,X);
      ModelSupport::buildPlane(SMap,offset+505,OE-
			       Z*LI.getVar<double>("PathHeight")/2.0,Z);
      ModelSupport::buildPlane(SMap,offset+506,OE+
			       Z*LI.getVar<double>("PathHeight")/2.0,Z);
      
      // Increment:
      OE+=X*(LI.getVar<double>("Clear")+2.0*LI.getVar<double>("Width"));
      offset+=50;
    }      
  return;
}

void
ZoomStack::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZoomStack","createObjects");
  
  std::string Out;
  const std::string FB=ModelSupport::getComposite(SMap,stackIndex,"1 -2 ");
  const std::string Sides=ModelSupport::getComposite(SMap,stackIndex,
						     "1 -2 5 -6 ");
  std::string basePlate=ModelSupport::getComposite(SMap,stackIndex,
						   "3 (13 : -12) ");
  int index(stackIndex);
  for(size_t i=0;i<nItem;i++)
    {
      // Path Void
      Out=FB+ModelSupport::getComposite(SMap,index," 503 -504 505 -506 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      // Main material
      Out=Sides+basePlate;
      Out+=ModelSupport::getComposite(SMap,index," -14 (-4 : 22) ");
      Out+=ModelSupport::getComposite(SMap,index," (-503:504:-505:506) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,
				       Items[i].getVar<int>("Mat"),0.0,Out));
      
      
      // Void on top
      Out=Sides+ModelSupport::getComposite(SMap,index," 4 (14 : -22) ");
      Out+=ModelSupport::getComposite(SMap,index," -34 (-24 : 32) ");
      basePlate=ModelSupport::getComposite(SMap,index," 24 (34 : -32) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
      index+=50;
    }


  if( nItem)
    {
      Out=Sides+ModelSupport::getComposite(SMap,stackIndex," 3 (13 : -12) ");
      Out+=ModelSupport::getComposite(SMap,index-50," -34 (-24 : 32) ");
      addOuterSurf(Out);
    }

  
  return;
}

void
ZoomStack::createAll(Simulation& System,const attachSystem::TwinComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Beamline track to place object within
  */
{
  ELog::RegMethod RegA("ZoomStack","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);

  insertObjects(System);       

  return;
}

  
}  // NAMESPACE moderatorSystem
