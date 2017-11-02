/********************************************************************* 
  CombLayer : MCNPX Input builder
 
 * File:   essBuild/ConicModerator.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "LayerComp.h"
#include "ModBase.h"
#include "ConicModerator.h"

namespace essSystem
{

ConicModerator::ConicModerator(const std::string& Key)  :
  constructSystem::ModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


ConicModerator::ConicModerator(const ConicModerator& A) : 
  ModBase(A),IWidth(A.IWidth),
  IHeight(A.IHeight),OWidth(A.OWidth),OHeight(A.OHeight),
  length(A.length),innerAngle(A.innerAngle),
  topAngle(A.topAngle),baseAngle(A.baseAngle),thick(A.thick),
  faceThick(A.faceThick),alThick(A.alThick),
  modTemp(A.modTemp),modMat(A.modMat),alMat(A.alMat),
  HCell(A.HCell)
  /*!
    Copy constructor
    \param A :: ConicModerator to copy
  */
{}

ConicModerator&
ConicModerator::operator=(const ConicModerator& A)
  /*!
    Assignment operator
    \param A :: ConicModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::ModBase::operator=(A);
      IWidth=A.IWidth;
      IHeight=A.IHeight;
      OWidth=A.OWidth;
      OHeight=A.OHeight;
      length=A.length;
      innerAngle=A.innerAngle;
      topAngle=A.topAngle;
      baseAngle=A.baseAngle;
      thick=A.thick;
      faceThick=A.faceThick;
      alThick=A.alThick;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      HCell=A.HCell;
    }
  return *this;
}

ConicModerator*
ConicModerator::clone() const
  /*!
    Clone copy constructor
    \return new this
  */
{
  return new ConicModerator(*this); 
}

ConicModerator::~ConicModerator() 
  /*!
    Destructor
  */
{}

void
ConicModerator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data base for variable
  */
{
  ELog::RegMethod RegA("ConicModerator","populate");
  
  ModBase::populate(Control);
  
  IWidth=Control.EvalVar<double>(keyName+"IWidth");
  IHeight=Control.EvalVar<double>(keyName+"IHeight");
  OWidth=Control.EvalVar<double>(keyName+"OWidth");
  OHeight=Control.EvalVar<double>(keyName+"OHeight");
  length=Control.EvalVar<double>(keyName+"Length");

  thick=Control.EvalVar<double>(keyName+"Thick");
  faceThick=Control.EvalVar<double>(keyName+"FaceThick");
  alThick=Control.EvalVar<double>(keyName+"AlThick");

  vacGap=Control.EvalVar<double>(keyName+"VacGap");
  waterAlThick=Control.EvalVar<double>(keyName+"WaterAlThick");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  voidGap=Control.EvalVar<double>(keyName+"VoidGap");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  nLayers=4;     // set for LayerComp
  return;
}
 
void
ConicModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConicModerator","createSurfaces");


  Geometry::Vec3D IVec[4]=
    { Origin-X*(IWidth/2.0), Origin+X*(IWidth/2.0),
      Origin-Z*(IHeight/2.0),Origin+Z*(IHeight/2.0) };
  Geometry::Vec3D OVec[4]=
    { Origin-X*(OWidth/2.0)+Y*length, Origin+X*(OWidth/2.0)+Y*length,
      Origin-Z*(OHeight/2.0)+Y*length,Origin+Z*(OHeight/2.0)+Y*length };

  // Inner H2 Planes:

  const double WD=(OWidth-IWidth)/2.0;
  const double HD=(OHeight-IHeight)/2.0;
  const double xRatio=length / sqrt(WD*WD+length*length);
  const double zRatio=length / sqrt(HD*HD+length*length);

  const double HT[]=
    {-alThick,0.0,thick,alThick+thick,
     alThick+thick+vacGap,
     alThick+thick+vacGap+waterAlThick,
     alThick+thick+vacGap+waterAlThick+waterThick,
     alThick+thick+vacGap+2*waterAlThick+waterThick,
     alThick+thick+vacGap+2*waterAlThick+waterThick+voidGap
    };
  // Inner AL Planes:
  int CI(modIndex);
  ModelSupport::buildPlane(SMap,CI+2,Origin+Y*length,Y);  
  ModelSupport::buildPlane(SMap,CI+22,Origin+Y*(length-alThick),Y);  
  for(int i=0;i<9;i++)
    {
      ModelSupport::buildPlane(SMap,CI+1,Origin-Y*HT[i],Y);  
      ModelSupport::buildPlane(SMap,CI+3,
			       IVec[0]-X*(xRatio*HT[i]),
			       OVec[0]-X*(xRatio*HT[i]),
			       OVec[0]-X*(xRatio*HT[i])+Z,X);
      ModelSupport::buildPlane(SMap,CI+4,
			       IVec[1]+X*(xRatio*HT[i]),
			       OVec[1]+X*(xRatio*HT[i]),
			       OVec[1]+X*(xRatio*HT[i])+Z,X);
      ModelSupport::buildPlane(SMap,CI+5,
			       IVec[2]-Z*(zRatio*HT[i]),
			       OVec[2]-Z*(zRatio*HT[i]),
			       OVec[2]-Z*(zRatio*HT[i])+X,X);
      ModelSupport::buildPlane(SMap,CI+6,
			       IVec[3]+Z*(zRatio*HT[i]),
			       OVec[3]+Z*(zRatio*HT[i]),
			       OVec[3]+Z*(zRatio*HT[i])+X,X);
      CI+=10;
    }
  FixedComp::setConnect(2,Origin+(IVec[0]+OVec[0])/2.0-
			X*(xRatio*HT[8]),-X);
  FixedComp::setConnect(3,Origin+(IVec[1]+OVec[1])/2.0+
			X*(xRatio*HT[8]),X);
  FixedComp::setConnect(4,Origin+(IVec[1]+OVec[1])/2.0-
			Z*(zRatio*HT[8]),-Z);
  FixedComp::setConnect(5,Origin+(IVec[1]+OVec[1])/2.0+
			Z*(zRatio*HT[8]),Z);

  return;
}

void
ConicModerator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("ConicModerator","createLinks");

  const double TT(alThick+thick);
  FixedComp::setConnect(0,Origin-Y*TT,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+81));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(modIndex+2));

  FixedComp::setLinkSurf(2,-SMap.realSurf(modIndex+83));
  FixedComp::setLinkSurf(3,SMap.realSurf(modIndex+84));
  FixedComp::setLinkSurf(4,-SMap.realSurf(modIndex+85));
  FixedComp::setLinkSurf(5,SMap.realSurf(modIndex+86));

  return;
}
  
void
ConicModerator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ConicModerator","createObjects");

  std::string Out;  
  Out=ModelSupport::getComposite(SMap,modIndex," 81 -2 83 -84 85 -86 ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,modIndex," 1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 11 13 -14 15 -16 (-1:-3:4:-5:6 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-22 21 23 -24 25 -26 (-11:-13:14:-15:16 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 31 33 -34 35 -36 (-21:-23:24:-25:26 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Head piece
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 22 23 -24 25 -26 (-13:14:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Vac layer:
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 41 43 -44 45 -46 (-31:-33:34:-35:36 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Water Inner Al
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 51 53 -54 55 -56 (-41:-43:44:-45:46 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // Water Inner
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 61 63 -64 65 -66 (-51:-53:54:-55:56 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  // Water Inner
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 71 73 -74 75 -76 (-61:-63:64:-65:66 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Final void
  Out=ModelSupport::getComposite(SMap,modIndex,
				 "-2 81 83 -84 85 -86 (-71:-73:74:-75:76 )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

Geometry::Vec3D
ConicModerator::getSurfacePoint(const size_t layerIndex,
                                const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-6]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylModerator","getSurfacePoint");

  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));


  if (SI>5) 
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
  if (layerIndex>=4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");

  // Modification map:
  switch(SI)
    {
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

std::string
ConicModerator::getLayerString(const size_t layerIndex,
			       const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylModerator","getLayerString");

  if (layerIndex>=4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");

  //  const int SI(modIndex+static_cast<int>(layerIndex)*10);
  switch(sideIndex)
    {
    }
  throw ColErr::IndexError<long int>(sideIndex,5,"sideIndex");
}

int
ConicModerator::getLayerSurf(const size_t layerIndex,
			     const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLayerSurf");

  if (layerIndex>=4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");
  
  //  const int SI(modIndex+static_cast<int>(layerIndex)*10);
  switch(sideIndex)
    {
      
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

  
void
ConicModerator::createAll(Simulation& System,
			  const attachSystem::FixedComp& axisFC,
			  const attachSystem::FixedComp* orgFC,
			  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("ConicModerator","createAll");
  populate(System.getDataBase());

  ModBase::createUnitVector(axisFC,orgFC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE essSystem
