/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedComp.cxx
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
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

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
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "ContainedComp.h"


namespace attachSystem
{

std::ostream&
operator<<(std::ostream& OX,const ContainedComp& A)
  /*!
    Standard write
    \param OX :: Output stream
    \param A :: ContainedComp to write
   */
{
  A.write(OX);
  return OX;
}

ContainedComp::ContainedComp() 
  /*!
    Constructor 
  */
{}

ContainedComp::ContainedComp(const ContainedComp& A) : 
  boundary(A.boundary),outerSurf(A.outerSurf),
  insertCells(A.insertCells)
  /*!
    Copy constructor
    \param A :: ContainedComp to copy
  */
{}

ContainedComp&
ContainedComp::operator=(const ContainedComp& A)
  /*!
    Assignment operator
    \param A :: ContainedComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      boundary=A.boundary;
      outerSurf=A.outerSurf;
      insertCells=A.insertCells;
    }
  return *this;
}


ContainedComp::~ContainedComp()
  /*!
    Deletion operator
  */
{}

void
ContainedComp::clearRules()
  /*!
    Zero all the rules
   */
{
  boundary.reset();
  outerSurf.reset();
  return;
}

void
ContainedComp::copyRules(const ContainedComp& A) 
  /*!
    Copy all the rules from another object to this object
    \param A :: Object to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","copyRules");
  if (&A != this)
    {
      boundary=A.boundary;
      outerSurf=A.outerSurf;
    }
  return;
}

void
ContainedComp::copyInterObj(const ContainedComp& A) 
  /*!
    Copy all intersect objects from one object to another
    \param A :: Object to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","copyInterObj");
  if (this!=&A)
    insertCells=A.insertCells;
  return;
}

std::vector<Geometry::Surface*>
ContainedComp::getSurfaces() const
  /*!
    Stupidly inefficient method to get the surface cells 
    from the contained boundary
    \return vector of surfaces
  */
{
  ELog::RegMethod RegA("ContainedComp","getSurfaces");
  if (!outerSurf.hasRule())
    {
      std::vector<Geometry::Surface*> Empty;
      return Empty;   // can this be done simpler?
    }
  return  outerSurf.getTopRule()->getSurfVector();
}

std::vector<const Geometry::Surface*>
ContainedComp::getConstSurfaces() const
  /*!
    Stupidly inefficient method to get the surface cells 
    from the contained boundary
    \return vector of surfaces
  */
{
  ELog::RegMethod RegA("ContainedComp","getSurfaces");
  if (!outerSurf.hasRule())
    {
      std::vector<const Geometry::Surface*> Empty;
      return Empty;   // can this be done simpler?
    }
  return outerSurf.getTopRule()->getConstSurfVector();
}
  
void
ContainedComp::addOuterSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterSurf");
  
  outerSurf.addIntersection(StrFunc::makeString(SN));
  outerSurf.populateSurf();
  return;
}

void
ContainedComp::addOuterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addInterSurf(std::string)");
  if (!SList.empty())
    {
      outerSurf.addIntersection(SList);
      outerSurf.populateSurf();
    }
  return;
}

void
ContainedComp::addOuterUnionSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterUnionSurf(std::string)");
  outerSurf.addUnion(SList);
  outerSurf.populateSurf();
  return;
}

void
ContainedComp::addBoundarySurf(const int SN)
  /*!
    Add a set of surfaces to the boundary object [intersection]
    \param SN ::  Signed surface number
  */
{
  ELog::RegMethod RegA("ContainedComp","addBoundarySurf(int)");

  boundary.addIntersection(StrFunc::makeString(SN));
  boundary.populateSurf();
  return;
}

void
ContainedComp::addBoundarySurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the boundary object
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addBoundarySurf(std::string)");
  boundary.addIntersection(SList);
  boundary.populateSurf();

  return;
}


void
ContainedComp::addBoundaryUnionSurf(const int SN)
  /*!
    Add a set of surfaces to the boundary object
    \param SN ::  Signed surface number
  */
{
  ELog::RegMethod RegA("ContainedComp","addBoundaryUnionSurf(int)");
  boundary.addUnion(StrFunc::makeString(SN));
  boundary.populateSurf();
  return;
}

void
ContainedComp::addBoundaryUnionSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the boundary object
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addBoundaryUnionSurf(std::string)");
  boundary.addUnion(SList);
  boundary.populateSurf();
  return;
}

std::string
ContainedComp::getCompExclude() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getExclude");

  return outerSurf.display();
}

std::string
ContainedComp::getExclude() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getExclude");
  
  if (outerSurf.hasRule())
    {
      MonteCarlo::Algebra AX;
      AX.setFunctionObjStr("#("+outerSurf.display()+")");
      return AX.writeMCNPX();
    }
  return "";
}

std::string
ContainedComp::getContainer() const
  /*!
    Calculate the write out the containing surface,
    allows an object to be inserted into this object without
    exceeding the boundary.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getContainer");
  return boundary.display();
}

std::string
ContainedComp::getCompContainer() const
  /*!
    Calculate the write out the containing surface,
    allows an object to be inserted into this object without
    exceeding the boundary.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getCompContainer");

  if (boundary.hasRule())
    {
      MonteCarlo::Algebra AX;
      AX.setFunctionObjStr("#("+boundary.display()+")");
      return AX.writeMCNPX();
    }
  return "";
}

int
ContainedComp::validIntersection(const HeadRule& BObj,
				 const bool inwardCheck,
				 const Geometry::Surface* ASurf,
				 const Geometry::Surface* BSurf)
  /*!
    Using the two surface and each surface in the given rule
    determine if the surf:surf:surf intersection points are 
    within/outside the bounary object (BObj). 
    \param BObj :: Boundary rule to check
    \param inwardCheck :: true is inside
    \param ASurf :: Surface [external]
    \param BSurf :: Surface [external]
    \return true if a valid intersection exists
  */
{
  ELog::RegMethod RegA("ContainedComp","validIntersection");

  if (BObj.hasRule())
    {
      const std::vector<Geometry::Surface*> SurfVec=
	BObj.getTopRule()->getSurfVector();
      std::vector<Geometry::Vec3D> Pts;
      std::vector<Geometry::Surface*>::const_iterator vc;
      std::vector<Geometry::Vec3D>::const_iterator ac;
      for(vc=SurfVec.begin();vc!=SurfVec.end();vc++)
	{
	  Pts=SurInter::processPoint(ASurf,BSurf,*vc);	  
	  for(ac=Pts.begin();ac!=Pts.end();ac++)
	    {
	      if (BObj.isValid(*ac,(*vc)->getName())==inwardCheck)
		return 1;
	    }
	}
    }
  return 0;
}

std::string
ContainedComp::getCompContainer(const Geometry::Surface* ASurf,
				const Geometry::Surface* BSurf) const
  /*!
    Calculate the write out the containing surface,
    allows an object to be inserted into this object without
    exceeding the boundary.
    \param ASurf :: First surf    
    \param BSurf :: Second surf
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getCompContainer");
  if(validIntersection(boundary,1,ASurf,BSurf))
    {
      return getCompContainer();
    }

  return "";
}
  
int
ContainedComp::surfOuterIntersect(const Geometry::Line& LA) const
  /*!
    Get the signed surface that intersects the outer object layer
    \param LA :: Line to calculate from
    \return signed surface
  */
{
  ELog::RegMethod RegA("ContainedComp","surfOuterIntersect");
  // Create object:
  if (outerSurf.hasRule())
    {
      MonteCarlo::Object TObj(1,0,0.0,outerSurf.display());
      TObj.createSurfaceList();
      std::pair<const Geometry::Surface*,double> Res=
	TObj.forwardIntercept(LA.getOrigin(),LA.getDirect());
      
      if (Res.first && Res.second>0.0)
	{
	  // Get point :
	  Geometry::Vec3D Pt=LA.getPoint(Res.second);
	  return Res.first->sideDirection(Pt,LA.getDirect())*
	    Res.first->getName();
	}
    }
  return 0;
}

int
ContainedComp::isBoundaryValid(const Geometry::Vec3D& V) const
  /*!
    Determine if the boundary is valid [reverse test]
    \param V :: Vector to test
    \return true/false
   */
{
  ELog::RegMethod RegA("ContainedComp","isBoundaryValid"); 

  return (boundary.isValid(V)) ? 0 : 1;
}

bool
ContainedComp::isOuterLine(const Geometry::Vec3D& APt,
			   const Geometry::Vec3D& BPt) const
  /*!
    Given a line defined by O,Dir find if the line intersect
    the object and has a valid region within the system
    \param APt :: Origin of line
    \param BPt :: End of line
    \return true if the line intersect the component.
  */
{
  ELog::RegMethod RegA("ContainedComp","isOuterLine");

  // First get surfaces of object:
  if (!outerSurf.hasRule()) return 0;

  std::vector<Geometry::Surface*> SVec=
    outerSurf.getTopRule()->getSurfVector();

  // First if either point within the object return true!
  if (outerSurf.isValid(APt) || outerSurf.isValid(BPt) )
    return 1;

  const double ABDist=APt.Distance(BPt);
  MonteCarlo::LineIntersectVisit LI(APt,BPt-APt);
  std::vector<Geometry::Surface*>::const_iterator vc;
  for(vc=SVec.begin();vc!=SVec.end();vc++)
    {
      LI.clearTrack();
      const std::vector<Geometry::Vec3D>& PVec=
	LI.getPoints(*vc);
      std::vector<Geometry::Vec3D>::const_iterator pc;
      for(pc=PVec.begin();pc!=PVec.end();pc++)
	{
	  const double ADist=APt.Distance(*pc);
	  const double BDist=BPt.Distance(*pc);
	  if (fabs(ADist+BDist-ABDist)<Geometry::zeroTol &&
	      outerSurf.isValid(*pc,(*vc)->getName()))
	    return 1;
	}
    }
  return 0;
}

int
ContainedComp::isOuterValid(const Geometry::Vec3D& V,
			    const std::set<int>& SN) const
  /*!
    Determine if the boundary is valid [reverst test]
    \param V :: Vector to test
    \param SN :: surface numbers to ignore
    \return true/false
   */
{
  ELog::RegMethod RegA("ContainedComp","isOuterValid(set))"); 
  
  return (outerSurf.isValid(V,SN)) ? 0 : 1;
}

int
ContainedComp::isOuterValid(const Geometry::Vec3D& V,const int SN) const
  /*!
    Determine if the boundary is valid [reverst test]
    \param V :: Vector to test
    \param SN :: surface number to ignore
    \return true/false
   */
{
  ELog::RegMethod RegA("ContainedComp","isOuterValid(int))"); 
  
  return (outerSurf.isValid(V,SN)) ? 0 : 1;
}

int
ContainedComp::isOuterValid(const Geometry::Vec3D& V) const
  /*!
    Determine if the boundary is valid
    \param V :: Vector to test
    \return true/false
   */
{
  ELog::RegMethod RegA("ContainedComp","isOuterValid())"); 
  return (outerSurf.isValid(V)) ? 0 : 1;
}

void
ContainedComp::addInsertCell(const ContainedComp& CC)
  /*!
    Adds CC-cells to the insert list
    \param CC :: ContainedComp to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell<CC>");

  addInsertCell(CC.insertCells);
  return;
}

void
ContainedComp::addInsertCell(const std::vector<int>& CVec)
  /*!
    Adds a cell to the insert list
    \param CVec :: Set of cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell<vector>");

  for(const int CN : CVec)
    {
      if (CN==0)
	throw ColErr::EmptyValue<int>("CN index");
      if (std::find(insertCells.begin(),insertCells.end(),CN)
	  ==insertCells.end())
	insertCells.push_back(CN);
    }
  return;
}

void 
ContainedComp::addInsertCell(const int CN)
  /*!
    Adds a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell");
  if (CN==0)
    throw ColErr::EmptyValue<int>("CN index");
  if (std::find(insertCells.begin(),insertCells.end(),CN)
      ==insertCells.end())
    insertCells.push_back(CN);

  return;
}

void 
ContainedComp::setInsertCell(const int CN)
  /*!
    Sets a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","setInsertCell");
  insertCells.clear();
  if (CN)
    insertCells.push_back(CN);
  return;
}

void 
ContainedComp::setInsertCell(const std::vector<int>& CN)
  /*!
    Sets a cell list to the insert list
    \param CN :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","setInsertCell(vec)");
  if (find_if(CN.begin(),CN.end(),
	      std::bind(std::equal_to<int>(),
			std::placeholders::_1,0))!=CN.end())
    throw ColErr::EmptyValue<int>("CN index");

  insertCells=CN;
  return;
}

void
ContainedComp::insertObjects(Simulation& System)
  /*!
    Insert the ContainedComp into the cell list
    \param System :: Simulation to get objects from
  */
{
  ELog::RegMethod RegA("ContainedComp","insertObjects");
  if (!hasOuterSurf()) return;

  for(const int CN : insertCells)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(CN);
      if (outerObj)
	outerObj->addSurfString(getExclude());
      else
	ELog::EM<<"Failed to find outerObject: "<<CN<<ELog::endErr;
    }
  insertCells.clear();
  return;
}

void
ContainedComp::insertInCell(Simulation& System,
			    const int cellN) const
  /*!
    Insert the ContainedComp in a single cell.
    \param System :: Simulation to get objects 
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","insertInCell");
  
  if (!hasOuterSurf()) return;

  MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (outerObj)
    outerObj->addSurfString(getExclude());
  else
    throw ColErr::InContainerError<int>(cellN,"Cell not in Simulation");
  return;
}

void
ContainedComp::insertInCell(Simulation& System,
			    const std::vector<int>& cellVec) const
  /*!
    Insert the ContainedComp in a single cell.
    \param System :: Simulation to get objects 
    \param cellVec :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","insertInCell(Vec)");
  
  if (!hasOuterSurf()) return;
  for(const int cellN : cellVec)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
      if (outerObj)
	outerObj->addSurfString(getExclude());
      else
	throw ColErr::InContainerError<int>(cellN,"Cell not in Simulation");
    }
  return;
}

void
ContainedComp::write(std::ostream& OX) const
  /*!
    Write out the boundar/outer rules
    \param OX :: Output stream
  */
{
  OX<<"\nboundary:";
  StrFunc::writeMCNPX(boundary.display(),OX);
  OX<<std::endl;
  OX<<"outerSurf:";
  StrFunc::writeMCNPX(outerSurf.display(),OX);
  OX<<std::endl;
  return;
}

}  // NAMESPACE attachSystem
