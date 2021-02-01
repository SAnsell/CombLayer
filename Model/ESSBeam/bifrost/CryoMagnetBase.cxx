/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItem/CryoMagnetBase.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
//#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDIter.h"





#include "support.h"


#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "surfDivide.h"
//#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "surfDBase.h"
#include "mergeTemplate.h"

#include "CryoMagnetBase.h"

namespace cryoSystem
{

      
CryoMagnetBase::CryoMagnetBase(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  //  cellIndex(buildIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}



CryoMagnetBase::CryoMagnetBase(const CryoMagnetBase& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  nLayers(A.nLayers),LRad(A.LRad),LThick(A.LThick),
  LTemp(A.LTemp),outerRadius(A.outerRadius),
  topOffset(A.topOffset),baseOffset(A.baseOffset),
  cutTopAngle(A.cutTopAngle),cutBaseAngle(A.cutBaseAngle),
  topThick(A.topThick), baseThick(A.baseThick),
  apertureWidth(A.apertureWidth), apertureHeight(A.apertureHeight), mat(A.mat), smat(A.smat),vmat(A.vmat)     
  /*!
    Copy constructor
    \param A :: cryoTube to copy
  */
{}

CryoMagnetBase&
CryoMagnetBase::operator=(const CryoMagnetBase& A)
  /*!
    Assignment operator
    \param A :: CryoMagnetBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      nLayers=A.nLayers;      
      LRad=A.LRad;
      LThick=A.LThick;
      LTemp=A.LTemp;
      outerRadius=A.outerRadius;
      topOffset=A.topOffset;
      baseOffset=A.baseOffset;
      cutTopAngle=A.cutTopAngle;
      cutBaseAngle=A.cutBaseAngle;
      topThick=A.topThick;
      baseThick=A.baseThick;
      apertureWidth=A.apertureWidth;
      apertureHeight=A.apertureHeight;
      mat=A.mat ;
      smat = A.smat ;
      vmat = A.vmat ;
    }
  return *this;
}


  
  
CryoMagnetBase::~CryoMagnetBase()
  /*!
    Destructor
  */
{}


void
CryoMagnetBase::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","populate");

  FixedOffset::populate(Control);

  ELog::EM << keyName << ELog::endDiag;
  
  // Master values
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  double T,R,Tmp;
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string KN=std::to_string(i);
      R=Control.EvalVar<double>(keyName+"LRadius"+KN);
      T=Control.EvalPair<double>(keyName+"LThick"+KN,keyName+"LThick"); //If Lthick not given for a specific layer, will be taken default (the one with no number)
      Tmp=Control.EvalPair<double>(keyName+"LTemp"+KN,keyName+"LTemp");
      if  (R>outerRadius-Geometry::zeroTol)
	throw ColErr::OrderError<double>(R,outerRadius,"R / outerRadius");
      if  (R<innerRadius-Geometry::zeroTol)
	throw ColErr::OrderError<double>(R,innerRadius,"R / innerRadius");
      if  (i && R<LRad.back()-Geometry::zeroTol)
	throw ColErr::OrderError<double>(R,LRad.back(),"R / LRad");
      LRad.push_back(R);
      LThick.push_back(T);
      LTemp.push_back(Tmp);
    }
  
  // Master values
  topOffset=Control.EvalVar<double>(keyName+"TopOffset");
  baseOffset=Control.EvalVar<double>(keyName+"BaseOffset");

  cutTopAngle=Control.EvalVar<double>(keyName+"CutTopAngle");
  cutBaseAngle=Control.EvalVar<double>(keyName+"CutBaseAngle");

  topThick=Control.EvalVar<double>(keyName+"TopThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");

  apertureWidth=Control.EvalVar<double>(keyName+"ApertureWidth");
  apertureHeight=Control.EvalVar<double>(keyName+"ApertureHeight");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  smat=ModelSupport::EvalMat<int>(Control,keyName+"SampleMat");
  vmat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  
  
  return;
}

void
CryoMagnetBase::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
CryoMagnetBase::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createSurfaces");

  int SN(buildIndex+100);
  for(size_t i=0;i<LRad.size();i++)
    {
      ModelSupport::buildCylinder(SMap,SN+7,Origin,Z,LRad[i]);
      ModelSupport::buildCylinder(SMap,SN+17,Origin,Z,LRad[i]+LThick[i]);
      SN+=50;
    }
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Z,innerRadius);
  
  ModelSupport::buildCone(SMap,buildIndex+9,
			  Origin-Z*baseOffset,-Z,cutBaseAngle);
  
  ModelSupport::buildCone(SMap,buildIndex+19,
			  Origin+Z*topOffset,Z,cutTopAngle);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*baseOffset,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*topOffset,Z);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(baseOffset+baseThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(topOffset+topThick),Z);

  //Aperture

  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(0.5*apertureHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(0.5*apertureHeight),Z);

  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-X*(0.5*apertureWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+X*(0.5*apertureWidth),X);
  //Plane to separate forward and backward part of the aperture
   ModelSupport::buildPlane(SMap,buildIndex+300,Origin,Y);
   //Plane at the sample position for linking
   ModelSupport::buildPlane(SMap,buildIndex+400,Origin,Z);

  return; 
}

void
CryoMagnetBase::createObjects(Simulation& System)
  /*!
    Create the cryo magnet unit
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createObjects");

  std::string Out,Aperture;
  Aperture = "25 -26 205 -206 ";

  int SN(100); // all indexes for the surfaces given with respect to the buildIndex

  //Void in the middle around the sample
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -8");
    System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));
    CellMap::setCell("InnerSpace",cellIndex-1); //Naming the cell as "InnerSpace"
    
  //Innermost void in the ring structure around the sample position
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " ( (9 -105 ) : (105 -106 ) : (19  106) ) +8 -"
				 +std::to_string(SN+7));
   
   System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));

   Aperture = Aperture + std::to_string(SN+7); //Building the void in aperture  
  //Aluminium rings
  for(size_t i=0;i<LRad.size();i++)
    {
      // The corresponding cylinders were built by:
      //    ModelSupport::buildCylinder(SMap,SN+7,Origin,Z,LRad[i]);
      //    ModelSupport::buildCylinder(SMap,SN+17,Origin,Z,LRad[i]+LThick[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "  (-25 : 26 : -205 : 206) ( (9 -105 ) : (105 -106 ) : (19  106) ) "
				     +std::to_string(SN+7)
				     +" -"+std::to_string(SN+17));
    System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
      SN+=50;
    }
     Aperture = Aperture + " -"+std::to_string(SN-50+17);
  //Void in between Al rings
  SN=100;
  for(size_t i=0;i<LRad.size()-1;i++)
    {      // The corresponding cylinders were built by:
      //    ModelSupport::buildCylinder(SMap,SN+7,Origin,Z,LRad[i]);
      //    ModelSupport::buildCylinder(SMap,SN+17,Origin,Z,LRad[i]+LThick[i]);
      int SN1;
      SN1 = SN+50;
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "(-25 : 26 : -205 : 206)  ( (9 -105 ) : (105 -106 ) : (19  106) ) "
				     +std::to_string(SN+17)+" -"
				     +std::to_string(SN1+7));
      System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));
      SN=SN1;
    }
  //Void around external ring
   Out=ModelSupport::getComposite(SMap,buildIndex,
				  " ( (9 -105 ) : (105 -106 ) : (19  106) ) "
				  +std::to_string(SN+17)+" -7");
   System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));
   //Void in the aperture
   Out=ModelSupport::getComposite(SMap,buildIndex,
				     Aperture+" -300 ");
   System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));
   //
   Out=ModelSupport::getComposite(SMap,buildIndex,
				     Aperture+" 300 ");
   System.addCell(MonteCarlo::Object(cellIndex++,vmat,0.0,Out));
   
  //Cylinder cut by lower cone, base of the magnet
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -7 -9 -105 8");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  
  
  //Cylinder cut by upper cone, top of the magnet
  Out=ModelSupport::getComposite(SMap,buildIndex," -6 -7 -19 106 8");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  

  //Borders and outer surface
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7 "); //cylinder, outside

      
  addOuterSurf(Out); // border will be what is out of cylinder above


  return; 
}

void
CryoMagnetBase::createLinks()
  /*!
    Creates a full attachment set
   Link points and link surfaces
 */
{  
  ELog::RegMethod RegA("CryoMagnetBase","createLinks");
  //Link point at the aperture entrance
  FixedComp::setConnect(0,Origin-Y*(outerRadius),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  //At the aperture exit
  FixedComp::setConnect(1,Origin+Y*(outerRadius),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  //To the right and left
  FixedComp::setConnect(2,Origin-X*(outerRadius),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  
  FixedComp::setConnect(3,Origin+X*(outerRadius),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  // Top and bottom
  FixedComp::setConnect(4,Origin-X*(baseOffset+baseThick),-Z);
    FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  
  FixedComp::setConnect(5,Origin+Y*(topOffset+topThick),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));

  
  return;
}

void
CryoMagnetBase::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("CryoMagnetBase","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
