/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/surfDivide.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfDivide.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Token.h"
#include "surfDBase.h"
#include "mergeMulti.h"
#include "surfDivide.h"

namespace ModelSupport
{

surfDivide::surfDivide() : 
  cellNumber(0),BaseObj(0),
  outCellN(0),outSurfN(0)
  /*!
    Constructor
  */
{}

surfDivide::surfDivide(const surfDivide& A) : 
  cellNumber(A.cellNumber),BaseObj(A.BaseObj),
  outCellN(A.outCellN),outSurfN(A.outSurfN),
  material(A.material),frac(A.frac)
  /*!
    Copy constructor
    \param A :: surfDivide to copy
  */
{
  PRules.resize(A.PRules.size());
  transform(A.PRules.begin(),A.PRules.end(),
	    PRules.begin(),boost::bind(&surfDBase::clone,_1));
}

surfDivide&
surfDivide::operator=(const surfDivide& A)
  /*!
    Assignment operator
    \param A :: surfDivide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellNumber=A.cellNumber;
      BaseObj=A.BaseObj;
      outCellN=A.outCellN;
      outSurfN=A.outSurfN;
      
      clearRules();
      std::vector<surfDBase*>::const_iterator vc;
      for(vc=A.PRules.begin();vc!=A.PRules.end();vc++)
	PRules.push_back((*vc)->clone());

      material=A.material;
      frac=A.frac;
    }
  return *this;
}

surfDivide::~surfDivide()
  /*!
    Destructor
  */
{
  clearRules();
}

void
surfDivide::clearRules()
  /*!
    Delete the rules : Replace with shared_ptr ASAP
   */
{
  std::vector<surfDBase*>::iterator vc;
  for(vc=PRules.begin();vc!=PRules.end();vc++)
    delete *vc;
  PRules.clear();
  return;
}

// -------------------------------------------------------------------

void
surfDivide::init()
  /*!
    Initialize system
  */
{
  clearRules();
  return;
}

template<typename T>
void
surfDivide::makeSignPair(const int iPt,const int oPt,const int dir)
  /*!
    Creates a simple two surface system with sign type
    \param iPt :: inner Point 
    \param oPt :: outer Point 
    \param dir :: sign direction on replacement [Not sign effected :: Note]
  */
{
  ELog::RegMethod RegA("surfDivide","makeSignPair<T>");

  mergeMulti<T,T>* DR=new mergeMulti<T,T>();
  DR->setPrimarySurf(((iPt>0) ? 1 : 0),iPt);
  DR->addSecondarySurf(oPt);
  DR->setSignReplace(dir);
  PRules.push_back(DR);  
  
  return;
}

template<typename T>
void
surfDivide::makePair(const int iPt,const int oPt)
  /*!
    Creates a simple two surface system
    \param iPt :: inner Point 
    \param oPt :: outer Point 
  */
{
  ELog::RegMethod RegA("surfDivide","makePair");

  mergeMulti<T,T>* DR=new mergeMulti<T,T>();
  DR->setPrimarySurf(((iPt>0) ? 1 : 0),iPt);
  DR->addSecondarySurf(oPt);
  PRules.push_back(DR);  

  return;
}

template<typename T,typename U>
void
surfDivide::makePair(const int iPt,const int oPt)
  /*!
    Creates a simple two surface system
    \param iPt :: inner Point 
    \param oPt :: outer Point 
  */
{
  ELog::RegMethod RegA("surfDivide","makePair");

  mergeMulti<T,U>* DR=new mergeMulti<T,U>();
  DR->setPrimarySurf(((iPt>0) ? 1 : 0),iPt);
  DR->addSecondarySurf(oPt);
  PRules.push_back(DR);  

  return;
}

void
surfDivide::makeRule(const std::string& searchRule,
		       const std::string& outRule)
{
  return;
}

void
surfDivide::addRule(const int,const int)
{
  return;
}

template<typename T>
void
surfDivide::makeMulti(const int iPt,const int oPtA,const int oPtB)
  /*!
    Creates a simple three surface system. All surfaces
    must take a sign
    \param iPt :: inner Point 
    \param oPtA :: outer Point (primary) 
    \param oPtB :: outer Point (secondary)
  */
{
  ELog::RegMethod RegA("surfDivide","makePair");

  mergeMulti<T,T>* DR=new mergeMulti<T,T>();
  DR->setPrimarySurf(((iPt>0) ? 1 : 0),iPt);
  DR->addSecondarySurf(oPtA);
  DR->addSecondarySurf(oPtB);
  PRules.push_back(DR);  
  return;
}

void 
surfDivide::addInnerSingle(const int)
  /*!
    Have an outer surface that is only used in the
    case of the final outer system. i.e it will be deleted
    in all other cases
    \param  :: Inner Surface to delete
  */
{
  ELog::RegMethod RegA("surfDivide","addInnerSingle");
  return;
}

void 
surfDivide::addOuterSingle(const int)
  /*!
    Have an outer surface that is only used in the
    case of the final outer sytem
    \param :: Outer Surface to add
  */
{
  ELog::RegMethod RegA("surfDivide","addOuterSingle");
   
  return;
}

void 
surfDivide::preDivide(Simulation& System)
  /*!
    Preparation work for dividing the cell
    \param System :: Simulation to get objects etc
  */
{
  ELog::RegMethod RegA("surfDivide","preDivide");

  BaseObj=System.findQhull(cellNumber);  
  if (!BaseObj)
    {
      ELog::EM<<"Unable to find "<<cellNumber<<ELog::endErr;
      throw ColErr::ExitAbort(RegA.getFull());
    }
  
  BaseObj->populate();
  BaseObj->createSurfaceList();  
  
  return;
}

void
surfDivide::populateSurfaces()
  /*!
    Create the inner surfaces 
  */
{
  ELog::RegMethod RegA("surfDivide","populateSurfaces");
  
  /// GET Planes to split:
  for(size_t i=0;i<PRules.size();i++)
    PRules[i]->populate();
  return;
}

void
surfDivide::activeDivide(Simulation& System)
  /*!
    Splits an object into two sections 
    - registers new surface 
    - register new object
    - This currently requires that all initial surfaces, in the cell,
    different
    \param System :: Simulation to add to system
   */
{
  ELog::RegMethod RegA("surfDivide","activeDivide");
  // GET CELL:
  
  preDivide(System);
  populateSurfaces();

  std::vector<Token> innerCell=BaseObj->cellVec();;  // Inner Cell 
  std::vector<Token> outerCell;  // outer Cell

  for(size_t i=0;i<=frac.size();i++)
    {      
      // Create outer object
      MonteCarlo::Qhull NewObj(*BaseObj);
      NewObj.setName(outCellN++);
      NewObj.setMaterial(material[i]);
      std::vector<Token> innerCell=NewObj.cellVec();
      if (!i) outerCell=innerCell;
      if (i!=frac.size())
	{
	  // Process Rules:
	  for(size_t rN=0;rN<PRules.size();rN++)
	    {
	      // Process
	      PRules[rN]->createSurf(frac[i],outSurfN);
	      PRules[rN]->processInnerOuter(1,outerCell);
	      PRules[rN]->processInnerOuter(0,innerCell);
	    }
	}
      NewObj.procString(writeToken(outerCell));
      System.addCell(NewObj);
      outerCell=innerCell;
    }
  // Remove Original Cell
  System.removeCell(cellNumber);
  return;  
}


std::string
surfDivide::writeToken(const std::vector<Token>& TVec)
 /*!
   Static function to write out the stream
   \param TVec :: Output Vector
   \return string in normal format.
 */
{
  std::vector<Token>::const_iterator mc;
  std::ostringstream cx;
  for(mc=TVec.begin();mc!=TVec.end();mc++)
    mc->write(cx);
  return cx.str();
}

///\cond TEMPLATE
template void surfDivide::makePair<Geometry::Plane>(const int,const int);
template void surfDivide::makePair<Geometry::Cylinder>(const int,const int);
template void surfDivide::makeMulti<Geometry::Plane>(const int,const int,const int);
template void surfDivide::makeSignPair<Geometry::Plane>(const int,const int,const int);
template void surfDivide::makePair<Geometry::Cylinder,Geometry::Plane>(const int,const int);


///\endcond TEMPLATE


 
}  // NAMESPACE ModelSupport
