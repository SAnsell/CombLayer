/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfDivide.cxx
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
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
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
#include "stringCombine.h"
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
#include "mergeTemplate.h"
#include "ModelSupport.h"
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
	    PRules.begin(),std::bind(&surfDBase::clone,
				     std::placeholders::_1));
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

void
surfDivide::addRule(const surfDBase* SBase)
  /*!
    Adds a rule to the mainsystem
    \param SBase :: Main rule.
  */
{
  ELog::RegMethod REgA("surfDivide","addRule");
  
  if (SBase)
    PRules.push_back(SBase->clone());
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
  ELog::RegMethod RegA("surfDivide","makeMulti");

  mergeMulti<T,T>* DR=new mergeMulti<T,T>();
  DR->setPrimarySurf(((iPt>0) ? 1 : 0),iPt);
  DR->addSecondarySurf(oPtA);
  DR->addSecondarySurf(oPtB);
  PRules.push_back(DR);  
  return;
}

template<typename T>
void
surfDivide::makeTemplate(const int iPt,const int oPtA)
  /*!
    Creates a simple three surface system. All surfaces
    must take a sign
    \param iPt :: inner Point 
    \param oPtA :: outer Point (primary) 
  */
{
  ELog::RegMethod RegA("surfDivide","makeTemplate");

  mergeTemplate<T,T>* DR=new mergeTemplate<T,T>();
  DR->setSurfPair(iPt,oPtA);
  PRules.push_back(DR);  
  return;
}

template<typename T>
void
surfDivide::makeTemplate(const int iPt,const int oPtA,const int oPtB)
  /*!
    Creates a simple three surface system. All surfaces
    must take a sign
    \param iPt :: inner Point 
    \param oPtA :: outer Point (primary) 
    \param oPtB :: outer Point (secondary)
  */
{
  ELog::RegMethod RegA("surfDivide","makeTemplate");

  mergeTemplate<T,T>* DR=new mergeTemplate<T,T>();
  DR->setSurfPair(iPt,oPtA);  
  DR->setSurfPair(iPt,oPtB);

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
surfDivide::activeDivideTemplate(Simulation& System)
  /*!
    This assumes that mergeTemplate objects are being 
    used exclusively.
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("surfDivide","activeDivideTemplate");

  preDivide(System);
  populateSurfaces();

  for(size_t rN=0;rN<PRules.size();rN++)
    PRules[rN]->setOutSurfNumber
      (outSurfN+100*static_cast<int>(rN));

  for(size_t i=0;i<=frac.size();i++)
    {      
      // Create outer object

      // Process Rules:
      const double fA=(i) ? frac[i-1] : -1.0;
      const double fB=(i!=frac.size()) ? frac[i] : 2.0;
      HeadRule cell(BaseObj->getHeadRule());

      for(size_t rN=0;rN<PRules.size();rN++)
	PRules[rN]->process(fA,fB,cell);

      // Set cell:
      MonteCarlo::Qhull NewObj(*BaseObj);
      NewObj.setName(outCellN++);
      NewObj.setMaterial(material[i]);
      NewObj.procString(cell.display());
      System.addCell(NewObj);
      cellBuilt.push_back(NewObj.getName());
    }
  // Remove Original Cell
  System.removeCell(cellNumber);
  return;  
}


void
surfDivide::setBasicSplit(const size_t NDiv,const int matN)
  /*!
    Divide the system by a regular fraction and set one material
    \param NDiv :: number of division
    \param matN :: Material number
   */
{
  ELog::RegMethod RegA("surfDivide","setBasicSplit");

  if (NDiv<2)
    throw ColErr::SizeError<size_t>(NDiv,0,"NDiv zero");
  frac.clear();
  material.clear();
  const double step(1.0/static_cast<double>(NDiv));
  double p(step);
  for(size_t i=1;i<NDiv;i++)   // avoid 0.0 and 1.0
    {
      frac.push_back(p);
      material.push_back(matN);
      p+=step;
    }
  material.push_back(matN);
  return;
}

  
void
surfDivide::addFrac(const double F)
  /*!
    Add a fraction to the stack
    \param F :: Fraction to add [0-1]
    \throw RangeError if F not ordered or between 0/1 limits
   */
{
  ELog::RegMethod RegA("surfDivide","addFrac");

  if (F<Geometry::zeroTol || F>1.0+Geometry::zeroTol)
    throw ColErr::RangeError<double>(F,0,1.0,"Frac out of range");

  if (!frac.empty() &&
      (F-frac.back())<Geometry::zeroTol)
    throw ColErr::RangeError<double>(F,F-frac.back(),1.0,
				     "Frac misorderd");
  
  frac.push_back(F);
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

void
surfDivide::addLayers(const size_t N,
		      const std::vector<double>& fraction,
		      const std::vector<int>& matNum)
  /*!
    Add a double list of layers/materials
    \param N :: Number of layers
    \param fraction :: Fractions [N-1]
    \param matNumber :: Materials
   */
{
  ELog::RegMethod RegA("surfDivide","addLayers");
  if (N)
    {
      for(size_t i=1;i<N;i++)
	{
	  addFrac(fraction[i-1]);
	  addMaterial(matNum[i-1]);
	}
      addMaterial(matNum[N-1]);
    }
  return;
}

int
surfDivide::procSurfDivide(Simulation& System,
			   const ModelSupport::surfRegister& SMap,
			   const int cellNumber,
			   const int moduleIndex,
			   const int surfCreate,
			   const int cellCreate,
			   const std::vector<std::pair<int,int>>& VA,
			   const std::string& Inner,
			   const std::string& Outer)
  /*!
    Process all the basic dividing layers 
    [Copied from GhipIRGuide -- make general??]
    \param System :: Simuation 
    \param SMap :: Surface registration
    \param cellNubmer :: cell number to process
    \param moduleNumber :: offset number from objectRegister
    \param surfCreate :: first surface number for new surface
    \param cellCreate :: first cell number for new object [0 no action]
    \param VA :: Offset vector
    \param Inner :: Inner string [not composed]
    \param Outer :: Outer string [not composed]
    \return cell nubmer
   */
{
  ELog::RegMethod RegA("surfDivide","procSurfDivide");
  
  std::string OutA,OutB;
  // Cell Specific:
  init();
  setCellN(cellNumber);
  if (cellCreate && surfCreate)
    setOutNum(cellCreate,moduleIndex+surfCreate);

  ModelSupport::mergeTemplate<Geometry::Plane,
			      Geometry::Plane> surroundRule;
  for(const std::pair<int,int>& VItem : VA)
    {
      surroundRule.setSurfPair(SMap.realSurf(moduleIndex+VItem.first),
			       SMap.realSurf(moduleIndex+VItem.second));
    }

  OutA=ModelSupport::getComposite(SMap,moduleIndex,Inner);
  OutB=ModelSupport::getComposite(SMap,moduleIndex,Outer);
  surroundRule.setInnerRule(OutA);
  surroundRule.setOuterRule(OutB);


  addRule(&surroundRule);
  activeDivideTemplate(System);
  
  return getCellNum();
}

int
surfDivide::procSimpleDivide(Simulation& System,
                             const ModelSupport::surfRegister& SMap,
			     const int cellNumber,
			     const int moduleIndex,
			     const int surfCreate,
			     const int cellCreate,
			     const std::vector<std::pair<int,int>>& VA)
  /*!
    Process all the basic dividing layers -- simple split
    \param System :: Simuation 
    \param SMap :: Surface registration
    \param cellNubmer :: cell number to split/process
    \param moduleIndex :: offset number for surfaces in VA
    \param surfCreate :: first surface number for new surface
    \param cellCreate :: first cell number for new object [0 no action]
    \param VA :: Pairs of surface number to split
    \return cell nubmer
   */
{
  ELog::RegMethod Rega("surfDivide","procSimpleDivide");
  
  std::string OutA,OutB;
  // Cell Specific:
  init();
  setCellN(cellNumber);
  if (cellCreate && surfCreate)
    setOutNum(cellCreate,moduleIndex+surfCreate);

  for(const std::pair<int,int>& VItem : VA)
    {
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      const int RA=SMap.realSurf(moduleIndex+abs(VItem.first));
      const int RB=SMap.realSurf(moduleIndex+abs(VItem.second));
      const int ASign=(VItem.first>0) ? 1 : -1;
      const int BSign=(VItem.second>0) ? 1 : -1;
      surroundRule.setSurfPair(RA,RB);
      OutA=StrFunc::makeString(RA*ASign);
      OutB=StrFunc::makeString(RB*BSign);
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      addRule(&surroundRule);
    }
  
  activeDivideTemplate(System);  
  return getCellNum();
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


template void surfDivide::makeTemplate<Geometry::Plane>(const int,const int);
template void surfDivide::makeTemplate<Geometry::Plane>(const int,const int,const int);


///\endcond TEMPLATE


 
}  // NAMESPACE ModelSupport
