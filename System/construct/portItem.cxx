/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/portItem.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <array>

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
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "portItem.h"

namespace constructSystem
{

portItem::portItem(const std::string& Key) :
  attachSystem::FixedComp(Key,2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  statusFlag(0),
  portIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(portIndex+1),radius(0.0),wall(0.0),
  flangeRadius(0.0),flangeLength(0.0),voidMat(0),wallMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

portItem::portItem(const portItem& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  statusFlag(A.statusFlag),portIndex(A.portIndex),
  cellIndex(A.cellIndex),externalLength(A.externalLength),
  radius(A.radius),wall(A.wall),flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),plateThick(A.plateThick),
  voidMat(A.voidMat),wallMat(A.wallMat),plateMat(A.plateMat),
  refComp(A.refComp),exitPoint(A.exitPoint)
  /*!
    Copy constructor
    \param A :: portItem to copy
  */
{}

portItem&
portItem::operator=(const portItem& A)
  /*!
    Assignment operator
    \param A :: portItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      
      statusFlag=A.statusFlag;
      portIndex=A.portIndex;
      cellIndex=A.cellIndex;
      externalLength=A.externalLength;
      radius=A.radius;
      wall=A.wall;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      plateThick=A.plateThick;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      plateMat=A.plateMat;
      refComp=A.refComp;
      exitPoint=A.exitPoint;
    }
  return *this;
}
  
portItem::~portItem() 
  /*!
    Destructor
  */
{}

void
portItem::setMain(const double L,const double R,const double WT)
  /*!
    Set parameters
    \param L :: external length
    \param R :: Internal radius
    \param WT :: Wall thick
    
  */
{
  externalLength=L;
  radius=R;
  wall=WT;
  return;
}

void
portItem::setFlange(const double FR,const double FT)
  /*!
    Set flange parameters
    \param FR :: Flange radius
    \param FT :: Flange thick
  */
{
  flangeRadius=FR;
  flangeLength=FT;
  return;
}

void
portItem::setCoverPlate(const double T,const int M)
  /*!
    Set flange parameters
    \param T :: Plate thickness
    \param M :: cover plate material [-ve for wall Mat]
  */
{
  plateThick=T;
  if (M<0)
    plateMat=wallMat;
  return;
}

void
portItem::setMaterial(const int V,const int W)
  /*!
    Sets the materials
    \param V :: Void mat
    \param W :: Wall Mat.
   */
{
  voidMat=V;
  wallMat=W;
  return;
}

void
portItem::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("portItem","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  refComp=FC.getKeyName();
  statusFlag |= 1;
  return;
}

  
void
portItem::setCentLine(const attachSystem::FixedComp& FC,
		      const Geometry::Vec3D& Centre,
		      const Geometry::Vec3D& Axis)
  /*!
    Set position
    \param FC :: FixedComp to get inital orientation [origin]
    \param Centre :: centre point [in FC basis coordinates]
    \param Axis :: Axis direction
   */
{
  portItem::createUnitVector(FC,0);
  
  Origin+=X*Centre.X()+Y*Centre.Y()+Z*Centre.Z();
  const Geometry::Vec3D DVec=X*Axis.X()+Y*Axis.Y()+Z*Axis.Z();
  FixedComp::reOrientate(1,DVec.unit());
  return;
}

  
void
portItem::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("portItem","createSurfaces");
  // divider surface if needeed :

  ModelSupport::buildPlane(SMap,portIndex+1,Origin,Y);
  if (flangeRadius-Geometry::zeroTol<=radius+wall)
    throw ColErr::SizeError<double>(flangeRadius,wall+radius,
				    "Wall Radius<FlangeRadius");
  ModelSupport::buildCylinder(SMap,portIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,portIndex+17,Origin,Y,radius+wall);
  ModelSupport::buildCylinder(SMap,portIndex+27,Origin,Y,flangeRadius);

  return;
}


void
portItem::createLinks(const ModelSupport::LineTrack& LT,
		      const size_t AIndex,
		      const size_t BIndex)

  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
    \param LT :: Line track
    \param AIndex :: start of high density material 
    \param BIndex :: end of high density material 
  */
{
  ELog::RegMethod RegA("portItem","createLinks");

  if (AIndex)
    {
      FixedComp::setConnect(0,LT.getPoint(AIndex-1),-Y);
      FixedComp::setLinkSurf(0,-LT.getSurfIndex(AIndex-1));
    }
  else
    {
      FixedComp::setConnect(0,Origin,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(portIndex+1));
    }

  const Geometry::Vec3D exitPoint=LT.getPoint(BIndex+1);

  if (plateThick>Geometry::zeroTol)
    {
      FixedComp::setConnect(1,exitPoint+Y*(externalLength+plateThick),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(portIndex+202));
    }
  else
    {
      FixedComp::setConnect(1,exitPoint+Y*externalLength,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(portIndex+2));
    }
  return;
}

void
portItem::constructOuterFlange(Simulation& System,
			       const ModelSupport::LineTrack& LT,
			       const size_t startIndex,
			       const size_t lastIndex)
  /*!
    Find the length and outer fangge
    \parma System :: Simulation to use
    \param LT :: Line track out of object
    \param lastIndex :: index of last point
  */
{
  ELog::RegMethod RegA("portItem","constructOuterFlange");

  const Geometry::Vec3D exitPoint=LT.getPoint(lastIndex+1);

  // Final outer
  ModelSupport::buildPlane(SMap,portIndex+2,
			   exitPoint+Y*externalLength,Y);

  ModelSupport::buildPlane(SMap,portIndex+102,
			   exitPoint+Y*(externalLength-flangeLength),Y);

  if (plateThick>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,portIndex+202,
			     exitPoint+Y*(externalLength+plateThick),Y);
  
  // determine start surface:
  std::string frontSurf;
  if (startIndex!=0)
    frontSurf=std::to_string(LT.getSurfIndex()[startIndex-1]);

  // construct inner volume:
  std::string Out;

  
  Out=ModelSupport::getComposite(SMap,portIndex," 1 -7 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,portIndex," 1 -17 7 -2 ");
  System.addCell(cellIndex++,wallMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,portIndex," 102 -27 17 -2 ");
  System.addCell(cellIndex++,wallMat,0.0,Out);

  if (plateThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,portIndex," -27 202 2 ");
      makeCell("Plate",System,cellIndex++,plateMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,portIndex," -202 -27 (102:-17) 1 ");
    }
  else
    Out=ModelSupport::getComposite(SMap,portIndex," -2 -27 (102:-17) 1 ");
  
  addOuterSurf(Out);

  // Mid port exclude
  const std::string tubeExclude=
    ModelSupport::getComposite(SMap,portIndex," ( 17 : -1 )");
 
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
  const std::vector<double>& Track=LT.getTrack();
  double T(0.0);   // extention base out point
  for(size_t i=startIndex;i<OVec.size() && T<externalLength;i++)
    {
      MonteCarlo::Object* OPtr=OVec[i];
      if (i>lastIndex)   
	T+=Track[i];

      if (keyName == "windowPort" || keyName=="BalderShieldPipePort0")
	ELog::EM<<"P["<<keyName<<"] == "<<OPtr->getName()
		<<":::"<<T<<" "<<externalLength<<ELog::endDiag;

      if (T>=externalLength-flangeLength)
	OPtr->addSurfString(getExclude());
      else 
	OPtr->addSurfString(tubeExclude);
    }
  
  return;
}

void
portItem::calcBoundaryCrossing(const ModelSupport::LineTrack& LT,
			       size_t& AIndex,size_t& BIndex) const
  /*!
    Creates the inner and outer objects of the track in the 
    current ref cell. Base on the idea that the pipe will only
    have to cut solid system [ie. not inner voids]
    \param LT :: Line track
    \param AIndex :: start index
    \param BIndex :: end index
  */
{
  ELog::RegMethod RegA("portItem","calcBoundaryCrossing");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  AIndex=0;
  BIndex=0;
  // no point checking first value
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
    
  for(size_t i=1;i<OVec.size();i++)
    {
      const MonteCarlo::Object* oPtr=OVec[i];
      const int ONum=oPtr->getName();
      if (OR.hasCell(refComp,ONum))
	{
	  if (oPtr->getDensity()>0.01)
	    {
	      if (!AIndex) AIndex=i;
	      BIndex=i;
	    }
	}
    }
  return;
}

  
void
portItem::constructTrack(Simulation& System)
  /*!
    Construct a track system
    \param System :: Simulation of model
   */
{
  ELog::RegMethod RegA("portItem","constructTrack");


  if ((statusFlag & 1)!=1)
    {
      ELog::EM<<"Failed to set in port:"<<keyName<<ELog::endCrit;
      return;
    }
  createSurfaces();
  
  System.populateCells();
  System.validateObjSurfMap();

  ModelSupport::LineTrack LT(Origin,Y,-1.0);
  LT.calculate(System);

  size_t AIndex,BIndex;

  calcBoundaryCrossing(LT,AIndex,BIndex);
  constructOuterFlange(System,LT,AIndex,BIndex);

  createLinks(LT,AIndex,BIndex);
  return;
}
  
}  // NAMESPACE constructSystem
