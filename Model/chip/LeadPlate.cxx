/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/LeadPlate.cxx
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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "FixedOffset.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "tubeUnit.h"
#include "LeadPlate.h"

namespace shutterSystem
{


LeadPlate::LeadPlate(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  leadIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(leadIndex+1),activeFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LeadPlate::LeadPlate(const LeadPlate& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  leadIndex(A.leadIndex),cellIndex(A.cellIndex),
  activeFlag(A.activeFlag),thick(A.thick),defMat(A.defMat),
  supportMat(A.supportMat),Centre(A.Centre),radius(A.radius),
  linerThick(A.linerThick),centSpc(A.centSpc)
  /*!
    Copy constructor
    \param A :: LeadPlate to copy
  */
{
  copyHoleCentre(A.HoleCentre);
}

LeadPlate&
LeadPlate::operator=(const LeadPlate& A)
  /*!
    Assignment operator
    \param A :: LeadPlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      activeFlag=A.activeFlag;
      thick=A.thick;
      defMat=A.defMat;
      supportMat=A.supportMat;
      Centre=A.Centre;
      radius=A.radius;
      linerThick=A.linerThick;
      centSpc=A.centSpc;
      copyHoleCentre(A.HoleCentre);
    }
  return *this;
}


LeadPlate::~LeadPlate() 
  /*!
    Destructor
  */
{
  clearHoleCentre();
}

void
LeadPlate::copyHoleCentre(const std::vector<constructSystem::tubeUnit*>& AVec)
  /*!
    Create a copy of the hole centres
    \param AVec :: Vector of holes to copy
  */
{
  clearHoleCentre();
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=AVec.begin();vc!=AVec.end();vc++)
    HoleCentre.push_back(new constructSystem::tubeUnit(**vc));
  
  return;
}

void
LeadPlate::clearHoleCentre() 
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
LeadPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Simulation to use
  */
{
  ELog::RegMethod RegA("LeadPlate","populate");

  FixedOffset::populate(Control);
  
  activeFlag=Control.EvalVar<int>(keyName+"Active");
  if (activeFlag)
    {
      thick=Control.EvalVar<double>(keyName+"Thick");
      
      centSpc=Control.EvalVar<double>(keyName+"CentSpace");
      radius=Control.EvalVar<double>(keyName+"Radius");
      linerThick=Control.EvalVar<double>(keyName+"LinerThick");
      
      defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
      supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
    }
  return;
}

void
LeadPlate::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: FixedComponenT to attach this object to.
    \param sideIndex :: Index
  */
{
  ELog::RegMethod RegA("LeadPlate","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
LeadPlate::calcCentre()
  /*!
    Calculate the centre point
  */
{
  ELog::RegMethod RegA("LeadPlate","calcCentre");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  // Extract Container items
  
  // Initial plane
  const Geometry::Plane* DPlane=
    SMap.realPtr<Geometry::Plane>(leadIndex+1);

  std::string Bnd=getCompContainer();
  int surfItem;
  
  std::vector<const Geometry::Plane*> PVec;
  while(StrFunc::section(Bnd,surfItem))
    {
      const Geometry::Plane* PX=
	dynamic_cast<const Geometry::Plane*>(SurI.getSurf(abs(surfItem)));
      if (PX)
	PVec.push_back(PX);
    }
  // Loop over all pair to find possible surfaces
  std::vector<const Geometry::Plane*>::const_iterator ac;
  std::vector<const Geometry::Plane*>::const_iterator bc;
  std::vector<Geometry::Vec3D> Out;
  Centre=Geometry::Vec3D(0,0,0);
  int cnt(0);
  for(ac=PVec.begin();ac!=PVec.end();ac++)
    for(bc=ac+1;bc!=PVec.end();bc++)
      {
	Out=SurInter::makePoint(DPlane,*ac,*bc);
	if (!Out.empty())
	  {
	    Centre+=Out.front();
	    cnt++;
	  }
      }
  if (!cnt)
    ELog::EM<<"Failed to find centre point "<<ELog::endErr;
  else
    Centre/=cnt;
  return;
}

void 
LeadPlate::createCentres(const Geometry::Plane* PX)
  /*!
    Calculate the centres
    \param PX :: Dividing plane to test against.
  */
{
  ELog::RegMethod RegA("LeadPlate","createCentres");

  int acceptFlag(1);
  int step(0);
  HoleCentre.clear();
  while(acceptFlag)
    {
      acceptFlag=0;
      for(int i=-step;i<=step;i++)
	for(int j=-step;j<=step;j++)
	  {
	    if (abs(i)==step || abs(j)==step)
	      {
		const Geometry::Vec3D C=Centre+X*(i*centSpc)+Z*(j*centSpc);
		MonteCarlo::LineIntersectVisit LI(C,Y);
		const Geometry::Vec3D TPoint = LI.getPoint(PX)+
		  Y*(Geometry::zeroTol*10.0);

		const bool cFlag=isBoundaryValid(TPoint);
		const int bFlag= checkCorners(PX,C);

		if (cFlag || bFlag>=0)
		  {
		    HoleCentre.push_back(new constructSystem::tubeUnit(i,j,TPoint));
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
LeadPlate::checkCorners(const Geometry::Plane* PX,
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
  ELog::RegMethod RegA("LeadPlate","checkCorners");

  int validCnt(0);
  for(int i=-1;i<2;i+=2)
    for(int j=-1;j<2;j+=2)
      {
	const Geometry::Vec3D CX=C+X*(i*centSpc/2.0)+Z*(j*centSpc/2.0);
	MonteCarlo::LineIntersectVisit LI(CX,Y);
	const Geometry::Vec3D TPoint = LI.getPoint(PX)+
	  Y*(Geometry::zeroTol*10.0);
	validCnt+=isBoundaryValid(TPoint);
      }
  if (validCnt==4) return 0;
  return (validCnt) ? 1 : -1;
}

void
LeadPlate::joinHoles()
  /*!
    Create the links between the hole centres
    \todo Optimize by sorting the holeCentre list
   */
{
  ELog::RegMethod RegA("LeadPlate","joinHoles");

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
LeadPlate::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("LeadPlate","createSurface");

  // INNER PLANES
  // Front
  const Geometry::Plane* PX=
    ModelSupport::buildPlane(SMap,leadIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,leadIndex+2,Origin+Y*thick,Y);
  
  // Calc a valid point in the object:
  calcCentre();
  // creat holes:
  createCentres(PX);

  // process the joints
  joinHoles();

  // Create actual holes :
  Geometry::Vec3D Pt,Axis;

  int index(leadIndex);
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
				      (*vc)->getCentre(),Y,radius);
	  (*vc)->setCyl(index+7);
	  if (linerThick>Geometry::zeroTol)
	    {
	      ModelSupport::buildCylinder(SMap,index+8,(*vc)->getCentre(),
					  Y,radius+linerThick);
	      (*vc)->addCyl(index+8);
	    }
	}
      index+=10;
    }
    
  return;
}

void
LeadPlate::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LeadPlate","createObjects");

  std::string Out;
  // Master box: 
  const std::string PlateOut=
    ModelSupport::getComposite(SMap,leadIndex,"1 -2 ");
  addOuterSurf(PlateOut);

  //      Out+=getCompContainer(FrontPtr,CPtr);  

  std::stringstream cx;
  std::vector<constructSystem::tubeUnit*>::const_iterator vc;
  for(vc=HoleCentre.begin();vc!=HoleCentre.end();vc++)
    {
      // First make if not empty:
      const constructSystem::tubeUnit* TUPtr(*vc);
      const std::string Boundary=(TUPtr->isOutside()) ? 
	PlateOut+getCompContainer() : PlateOut; 
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
					       supportMat,0.0,Out));
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

  return;
}


void
LeadPlate::printHoles() const
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
  ELog::EM<<ELog::endDiag;

  return;
}
  
void
LeadPlate::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Twin component to set axis etc    
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("LeadPlate","createAll");
  populate(System.getDataBase());
  if (activeFlag)
    {
      createUnitVector(FC,sideIndex);
      createSurfaces();
      createObjects(System);
      insertObjects(System);
    }
  return;
}
  
}  // NAMESPACE shutterSystem
