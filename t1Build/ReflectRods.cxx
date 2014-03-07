/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/ReflectRods.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "tubeUnit.h"
#include "ReflectRods.h"

namespace ts1System
{

ReflectRods::ReflectRods(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,0),
  rodIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(rodIndex+1),topSurf(0),baseSurf(0),RefObj(0)  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ReflectRods::~ReflectRods() 
  /*!
    Destructor
  */
{
  clearHoleCentre();
}

void
ReflectRods::copyHoleCentre
   (const std::vector<constructSystem::tubeUnit*>& AVec)
  /*!
    Create a copy of the hole centres
    \param AVec :: Vector of holes to copy
  */
{
  ELog::RegMethod RegA("ReflectRods","copyHoleCentre");

  clearHoleCentre();
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=AVec.begin();vc!=AVec.end();vc++)
    HoleCentre.push_back(new constructSystem::tubeUnit(**vc));
  
  return;
}

void
ReflectRods::clearHoleCentre() 
   /*!
     Delete the hole centres
   */
{
  std::vector<constructSystem::tubeUnit*>::iterator vc;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    delete *vc;
  HoleCentre.clear();
  return;
}

void
ReflectRods::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ReflectRods","populate");

  const FuncDataBase& Control=System.getDataBase();
  try
    {
      zAngle=Control.EvalVar<double>(keyName+"ZAngle");
      xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
      
      centSpc=Control.EvalVar<double>(keyName+"CentSpace");
      radius=Control.EvalVar<double>(keyName+"Radius");
      linerThick=Control.EvalVar<double>(keyName+"LinerThick");

      innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
      linerMat=ModelSupport::EvalMat<int>(Control,keyName+"LinerMat");
      defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");


      populated= (radius<Geometry::zeroTol) ? 0 : 1;
    }
  // Exit and don't report if we are not using this scatter plate
  catch (ColErr::InContainerError<std::string>& EType)
    {
      ELog::EM<<"ReflectRods "<<keyName<<" not in use Var:"
	      <<EType.getItem()<<ELog::endWarn;
      populated=0;   
    }
  return;
}

void
ReflectRods::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent to attach this object to.
  */
{
  ELog::RegMethod RegA("ReflectRods","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(FC);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
ReflectRods::getZSurf()
  /*!
    Get extreme Z surfaces
  */
{
  ELog::RegMethod RegA("ReflectRods","getZSurf");
  
  const std::vector<const Geometry::Surface*>& SL=
    RefObj->getSurList();
  
  std::vector<Geometry::Vec3D> Pts;
  Geometry::Line LN(Geometry::Vec3D(0,0,0),Z);
  
  double minDZ(1e38),maxDZ(-1e38);
  
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SL.begin();vc!=SL.end();vc++)
    {
      const Geometry::Plane* PL=
	dynamic_cast<const Geometry::Plane*>(*vc);
      
      if (PL && LN.intersect(Pts,*PL))
	{
	  double dz=Pts.front().abs();
	  if (Pts.front().dotProd(Z)<0.0)
	    dz*=-1;
	  if (minDZ>dz)
	    {
	      minDZ=dz;
	      baseSurf=PL;
	    }
	  else if (maxDZ<dz)
	    {
	      maxDZ=dz;
	      topSurf=PL;
	    }
	}
    }

  if (!topSurf || !baseSurf)
    {
      ELog::EM<<"Failed to find top/Base surface " <<ELog::endCrit;  
      throw ColErr::NumericalAbort("Plane not found in Z intersect");
    }

  ELog::EM<<"Found base == "<<*baseSurf<<ELog::endDiag;
  ELog::EM<<"Found top == "<<*topSurf<<ELog::endDiag;

  return;
}

void
ReflectRods::calcCentre()
  /*!
    Calculate the centre point
  */
{
  ELog::RegMethod RegA("ReflectRods","calcCentre");

  // Extract Container items  
  const std::vector<const Geometry::Surface*>& SL=
    RefObj->getSurList();

  std::vector<const Geometry::Plane*> PVec;  
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SL.begin();vc!=SL.end();vc++)
    {
      const Geometry::Plane* PL=
	dynamic_cast<const Geometry::Plane*>(*vc);
      if (PL && fabs(PL->getNormal().dotProd(Z))<0.5)
	PVec.push_back(PL);
    }

  // Loop over all pair to find possible surfaces
  std::vector<const Geometry::Plane*>::const_iterator ac;
  std::vector<const Geometry::Plane*>::const_iterator bc;
  std::vector<Geometry::Vec3D> Out;
  Centre=Geometry::Vec3D(0,0,0);
  int cnt(0);

  std::set<int> ExSN;
  ExSN.insert(topSurf->getName());  
  for(ac=PVec.begin();ac!=PVec.end();ac++)
    {
      ExSN.insert((*ac)->getName());
      for(bc=ac+1;bc!=PVec.end();bc++)
	{

	  Out=SurInter::makePoint(baseSurf,*ac,*bc);
	  if (!Out.empty())
	    {
	      ExSN.insert((*bc)->getName());
	      const Geometry::Vec3D FP=Out.front();
	      if (RefObj->isValid(FP,ExSN))
		{
		  Centre+=Out.front();
		  cnt++;
		}
	      ExSN.erase((*bc)->getName());
	    }
	  ExSN.erase((*ac)->getName());
	}
    }
  if (!cnt)
    ELog::EM<<"Failed to find centre point "<<ELog::endErr;
  else
    Centre/=cnt;

  ELog::EM<<"Centre == "<<Centre<<ELog::endWarn;

  return;
}

void 
ReflectRods::createCentres(const Geometry::Plane* PX)
  /*!
    Calculate the centres
    \param PX :: Dividing plane to test against.
  */
{
  ELog::RegMethod RegA("ReflectRods","createCentres");

  const Geometry::Vec3D BVecX(X);
  const Geometry::Vec3D BVecY(X*sin(M_PI/6.0)+Y*sin(M_PI/3.0));
  int acceptFlag(1);
  int step(0);
  HoleCentre.clear();
  while(acceptFlag && step<=4)
    {
      acceptFlag=0;
      for(int i=-step;i<=step;i++)
	for(int j=-step;j<=step;j++)
	  {
	    if (abs(i)==step || abs(j)==step) 
	      {
		const Geometry::Vec3D C=Centre+BVecX*(i*centSpc)+
		  BVecY*(j*centSpc);
		MonteCarlo::LineIntersectVisit LI(C,Z);
		const Geometry::Vec3D TPoint = LI.getPoint(PX)-
		  Z*(Geometry::zeroTol*10.0);

		const bool cFlag=RefObj->isValid(TPoint);
		const int bFlag= checkCorners(PX,C);
		if (cFlag || bFlag>=0)
		  {
		    HoleCentre.push_back
		      (new constructSystem::tubeUnit(i,j,TPoint));

		    if (!cFlag) 		    
		      HoleCentre.back()->setEmpty();
		    if (bFlag!=0)
		      HoleCentre.back()->setOutside();
		    if (cFlag) 
		      acceptFlag++;
		  }
	      }
	  }
      step++;
    }
  return;
}

int
ReflectRods::checkCorners(const Geometry::Plane* PX,
			  const Geometry::Vec3D& C) const
  /*!
    Check if the corners of the cube fall in the object
    \param PX :: front/back plane
    \param C :: Centre of cube
    \retval 0 if all inside
    \retval 1 if part outside
    \retval -1 if all outside
   */
{
  ELog::RegMethod RegA("ReflectRods","checkCorners");

  int validCnt(0);
  for(int i=-1;i<2;i+=2)
    for(int j=-1;j<2;j+=2)
      {
	const Geometry::Vec3D CX=C+X*(i*centSpc/2.0)+Y*(j*centSpc/2.0);
	MonteCarlo::LineIntersectVisit LI(CX,Z);
	const Geometry::Vec3D TPoint = LI.getPoint(PX)+
	  Z*(Geometry::zeroTol*10.0);
	validCnt+=RefObj->isValid(TPoint);
      }
  if (validCnt==4) return 0;
  return (validCnt) ? 1 : -1;
}

void
ReflectRods::joinHoles()
  /*!
    Create the links between the hole centres
    \todo Optimize by sorting the holeCentre list
   */
{
  ELog::RegMethod RegA("ReflectRods","joinHoles");

  // Clear all the links first:
  std::vector<constructSystem::tubeUnit*>::iterator vc;
  std::vector<constructSystem::tubeUnit*>::iterator ac;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    (*vc)->clearLinks();
  
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    {
      // Note: addLink carris out isConnected test.
      for(ac=vc+1;ac!=HoleCentre.end();ac++)
	(*vc)->addLink(*ac);  // two way function
    }
  return;
}


void
ReflectRods::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ReflectRods","createSurface");
  
  getZSurf();
  // GET CENTRE POINT OF BOX
  // Calc a valid point in the object:
  calcCentre();
 
  // creat holes:
  createCentres(topSurf);

  // process the joints
  joinHoles();

  // Create actual holes :
  Geometry::Vec3D Pt,Axis;

  int index(rodIndex);
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    {
      for(size_t i=0;i<4;i++)
	{
	  if ((*vc)->midPlane(i,Pt,Axis))
	    {
	      ModelSupport::buildPlane(SMap,index+3+static_cast<int>(i),
				       Pt,Axis);
	      (*vc)->addPlane(i,index+3+static_cast<int>(i));
	    }
	}
      if (!(*vc)->isEmpty())
	{
	  // cylinder
	  ModelSupport::buildCylinder(SMap,index+7,
				      (*vc)->getCentre(),Z,radius);
	  (*vc)->setCyl(index+7);
	  if (linerThick>Geometry::zeroTol)
	    {
	      ModelSupport::buildCylinder(SMap,index+8,(*vc)->getCentre(),
					  Z,radius+linerThick);
	      (*vc)->addCyl(index+8);
	    }
	}
      index+=10;
    }
    
  return;
}

std::string
ReflectRods::plateString() const
  /*!
    Calcuate the string for the +/- top/base surfaces
    - Allows those surfaces to be either direction
    \return surf string 
  */
{
  std::ostringstream cx;
  
  const int tSign= (topSurf->getNormal().dotProd(Z)>0.0) ? -1 : 1;
  const int bSign= (baseSurf->getNormal().dotProd(Z)>0.0) ? 1 : -1;

  cx<<" "<<tSign*topSurf->getName()<<" "
    <<bSign*baseSurf->getName()<<" ";
  return cx.str();
}

void
ReflectRods::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ReflectRods","createObjects");

  std::string Out;
  // Master box: 
  const std::string PlateOut=plateString();
  ELog::EM<<"Plate == "<<Z<<ELog::endDebug;
  ELog::EM<<"Plate == "<<getContainer()<<ELog::endDebug;
  //      Out+=getCompContainer(FrontPtr,CPtr);  

  std::stringstream cx;
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    {
      // First make if not empty:
      const constructSystem::tubeUnit* TUPtr(*vc);
      const std::string Boundary=(TUPtr->isOutside()) ? 
	RefObj->cellCompStr() : PlateOut; 
      size_t cellLayer(0);
      if (!TUPtr->isEmpty())
	{
	  // void in middle:
	  Out=TUPtr->getCell(SMap,cellLayer)+Boundary;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
	  cellLayer++;

	  // Now add liner if present:
	  if (linerThick>Geometry::zeroTol)
	    {
	      Out=TUPtr->getCell(SMap,cellLayer)+Boundary;
	      System.addCell(MonteCarlo::Qhull(cellIndex++,
					       linerMat,0.0,Out));
	      cellLayer++;
	    }
	  // Outer stuff
	  Out=TUPtr->getCell(SMap,cellLayer)+Boundary;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
	}
      else  // EMPTY
	{
	  Out=TUPtr->getCell(SMap,0)+Boundary;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
	  cellLayer++;
	}
    }

  System.removeCell(RefObj->getName());
  RefObj=0;
  return;
}


void
ReflectRods::printHoles() const
  /*!
    Debug function to write out holes
   */
{
  // Clear all the links first:
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    {
      (*vc)->write(ELog::EM.Estream());
    }
  ELog::EM<<ELog::endDebug;

  return;
}
  
void
ReflectRods::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const size_t Index)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Twin component to set axis etc
  */
{
  ELog::RegMethod RegA("ReflectRods","createAll");
  populate(System);
  if (populated)
    {
      createUnitVector(FC);
      createSurfaces();
      createObjects(System);
      insertObjects(System);
    }
  return;
}
  
}  // NAMESPACE shutterSystem
