/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   poly/solveValues.cxx
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
#include <iostream>
#include <fstream>
#include <cmath>
#include <complex>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "vecOrder.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "solveValues.h"

 
namespace mathLevel   
{

solveValues::solveValues() :
  tol(1e-3)
  /*!
    Constructor
   */
{}

solveValues::solveValues(const solveValues& A)  :
  tol(A.tol),Ans(A.Ans)
  /*!
    Copy Constructor
    \param A :: solveValues to copy
   */
{
  for(size_t i=0;i<3;i++)
    Array[i]=A.Array[i];
}

solveValues&
solveValues::operator=(const solveValues& A)  
  /*!
    Assignment operator
    \param A :: solveValues to return
    \return *this
  */
{
  if (this!=&A)
    {
      Ans=A.Ans;
      for(int i=0;i<3;i++)
	Array[i]=A.Array[i];
    }
  return *this;
}

solveValues::~solveValues()
  /*!
    Destructor
  */
{}

size_t
solveValues::finalIndex(const size_t IA,const size_t IB) const
  /*!
    Get the index of the other section
    \param IA :: First index
    \param IB :: Second index
    \return third Index
  */
{
  return (((IA+1) % 3)==IB) ? (IA+2) % 3 : (IA+1) % 3;
}

void
solveValues::backSolve(const size_t IA,const size_t IB,
		       const double VA,const double VB)
  /*!
    Add solutions to the system
    \param IA :: Index of first variable
    \param IB :: Index of second variable
    \param VA :: Value of index A
    \param VB :: Value of index B
  */
{ 
  ELog::RegMethod RegA("solveValues","backSolve(VV)");
  // Determine the best item with the remaining variable:
  const size_t IC=finalIndex(IA,IB);
  const size_t tFlag=(1 << IC);
  int masterIndex;
  for(masterIndex=0;masterIndex<3 && 
	!(Array[masterIndex].getVarFlag() & tFlag);masterIndex++) ;
  if (masterIndex==3) 
    return;

  PolyVar<2> AXY=Array[masterIndex].subVariable(IA,VA);

  PolyVar<1> AZ=(IB>IA) ? AXY.subVariable(IB-1,VB) : 
    AXY.subVariable(IB,VB);

  std::vector<double> Out=AZ.realRoots();
  std::vector<double>::const_iterator vc;
  for(vc=Out.begin();vc!=Out.end();vc++)
    {
      Geometry::Vec3D Point;
      Point[IA]=VA;
      Point[IB]=VB;
      Point[IC]=*vc;
      Ans.push_back(Point);
    }
  return;
}

void
solveValues::backSolve(const size_t IA,const double V)
  /*!
    Process a single known value against the main stack
    Assumes that there are NON singular equations.
    \param IA :: Index of variable
    \param V :: Value of variable
  */
{
  ELog::RegMethod RegA("solveValues","backSolve(V)");
  
  PolyVar<2> SubArray[3];
  for(size_t i=0;i<3;i++)
    SubArray[i]=Array[i].subVariable(IA,V);

  // Check for singular:
  for(size_t i=0;i<3;i++)
    {
      const size_t flagA=SubArray[i].getVarFlag();
      if (countBits(flagA)==1)
	{
	  const size_t xyIndex=lowBitIndex(flagA)-1;
	  const size_t IB=(xyIndex>=IA) ? xyIndex+1 : xyIndex;
	  PolyVar<1> SX=SubArray[i].singleVar();
	  std::vector<double> Yres=SX.realRoots();
	  std::vector<double>::const_iterator vc;
	  for(vc=Yres.begin();vc!=Yres.end();vc++)
	    backSolve(IA,IB,V,*vc);
	  return;
	}
    }
  
  for(size_t i=0;i<3;i++)
    for(size_t j=i+1;j<3;j++)
      {
	PolyVar<1> YRoot=SubArray[i].reduce(SubArray[j]);

	if (YRoot.getDegree()>0)
	  {
	    const size_t IB=(IA) ? 0 :  1;
	    std::vector<double> Yres=YRoot.realRoots();	    
	    std::vector<double>::const_iterator vc;
	    for(vc=Yres.begin();vc!=Yres.end();vc++)
	      backSolve(IA,IB,V,*vc);
	    return;
	  }
      }
  return;
}

void 
solveValues::processResolved(PolyVar<1>& SV,const PolyVar<2>& Other,
			     const size_t subIndex,
			     const size_t Index,const size_t AIndex,
			     const size_t BIndex,const double Value)
  /*!
    Given two polynomials of Parameter: AIndex and A/BIndex
    solve the first and substitute to get the values in the 
    second. The third part of the solution is given by 
    Index/V
    \param SV :: Polynomial of variable AIndex
    \param Other :: Polynomial of variable AIndex
    \param subIndex :: subIndex of solve unit in Other
    \param Index :: Variable for previously solved value
    \param AIndex :: Variable for x value in SV
    \param BIndex :: Variable for y value in Other
    \param Value :: Value for previously solved
  */
{
  const std::vector<double> AResults=SV.realRoots();
  for(size_t kA=0;kA<AResults.size();kA++)
    {
      // Get values [on local index]
      PolyVar<1> TV=Other.subVariable(subIndex,AResults[kA]);
      const std::vector<double> BResults=TV.realRoots();
      for(size_t kB=0;kB<BResults.size();kB++)
	{
	  Geometry::Vec3D Point;
	  Point[Index]=Value;
	  Point[AIndex]=AResults[kA];
	  Point[BIndex]=BResults[kB];
	  Ans.push_back(Point);
	}
    }
  return;
}

void
solveValues::addSolutions(const PolyVar<2>& A,const PolyVar<2>& B,
			  const size_t Index,const double Value)
  /*!
    \param A :: Polynomial of XY
    \param B :: Polynomial of XY
    \param Index :: Variable that has been eliminated
    \param Value :: Variable value
  */
{
  ELog::RegMethod RegA("solveVAlues","addSolutions<2>");

  const PolyVar<2>* EqnArray[]={&A,&B};
  std::sort(EqnArray,EqnArray+2);

  const size_t flagA=EqnArray[0]->getVarFlag();
  if(!flagA) return;  // no solutions to add
  const size_t cntA=countBits(flagA);
  //  const int cntB=countBits(flagB);
  if (cntA==1)           // Fast solution for 1
    { 
      const size_t xyIndex=lowBitIndex(flagA)-1;
      // Note : X always add / Y add if xy !=0 / Z never add
      const size_t AIndex=(Index<=xyIndex) ? xyIndex+1 : xyIndex;
      const size_t BIndex=finalIndex(Index,AIndex);
      
      PolyVar<1> SV=EqnArray[0]->singleVar();
      processResolved(SV,*EqnArray[1],xyIndex,Index,AIndex,BIndex,Value);      
      return;
    }
    
  // Top level case: 
  mathLevel::PolyVar<1> SX=A.reduce(B);
  const size_t AIndex=(!Index) ? 1 : 0;
  const size_t BIndex=((Index+1) % 3 == AIndex) ? (Index+2) % 3 : (Index+1) % 3;
  processResolved(SX,B,0,Index,AIndex,BIndex,Value);

  return;
}

int
solveValues::getSolution(const PolyVar<3>& FXYZ,
			 const PolyVar<3>& GXYZ,
			 const PolyVar<3>& HXYZ)
  /*!
    Get a specific soltion and continue to find others
    \param FXYZ :: First polynominal    
    \param GXYZ :: Second polynominal
    \param HXYZ :: Third polynominal
    \return Number of solutions
  */
{
  ELog::RegMethod RegA("solveValues","getSolution(P,P,P)");
  setEquations(FXYZ,GXYZ,HXYZ);
  return getSolution();
}

int
solveValues::getSolution()
  /*!
    Get a specific soltion and continue to find others
    \return Number of solutions
  */
{
  ELog::RegMethod RegA("solveValues","getSolution()");

  size_t flag=Array[0].getVarFlag();
  // SOLUTION ZYX with single value:
  if (countBits(flag)==1)
    {
      PolyVar<1> SV=Array[0].singleVar();
      const std::vector<double> Out=SV.realRoots();
      const size_t Index=lowBitIndex(flag)-1;
      for(size_t k=0;k<Out.size();k++)
	{
	  PolyVar<2> XYa=Array[1].subVariable(Index,Out[k]);
	  PolyVar<2> XYb=Array[2].subVariable(Index,Out[k]);
	  // ADD SOLUTIONS FOR 2 Equations:
	  addSolutions(XYa,XYb,Index,Out[k]);
	}
      return checkSolutions();
    }	  
  // 
  // Process general case:
  // -- Solution is YX:
  //
  PolyVar<2> SubArray[3];
  SubArray[0]=Array[0].reduce(Array[1]);
  SubArray[1]=Array[0].reduce(Array[2]);
  SubArray[2]=Array[1].reduce(Array[2]);
  std::sort(SubArray,SubArray+3);
  flag=SubArray[0].getVarFlag();
  if (countBits(flag)==1)
    {
      PolyVar<1> SV=SubArray[0].singleVar();
      const std::vector<double> Out=SV.realRoots();      
      const size_t Index=lowBitIndex(flag)-1;   // bit set (count==1)

      std::vector<double>::const_iterator vc;
      for(vc=Out.begin();vc!=Out.end();vc++)
	backSolve(Index,*vc);
      return checkSolutions();
    }	  

  PolyVar<1> OneArray[3];
  OneArray[0]=SubArray[0].reduce(SubArray[1]);
  OneArray[1]=SubArray[0].reduce(SubArray[2]);
  OneArray[2]=SubArray[1].reduce(SubArray[2]);
  std::sort(OneArray,OneArray+3);


  for(size_t i=0;i<3;i++)
    {
      if (i==0 || (OneArray[i]!=OneArray[i-1]))
	{
	  std::vector<double> Out=OneArray[i].realRoots(1e-10);
	  // X coordinates all agree:
	  for(const double D : Out)
	    this->backSolve(0,D);
	}

    }
  // X coordinates all agree:
  return checkSolutions();
}

bool
solveValues::notValid(const Geometry::Vec3D& Pt) const
  /*!
    Test is a value is not valid
    \param Pt :: Point to test
    \return 1 / 0
   */
{
  ELog::RegMethod RegA("solveValues","nonValid");
  for(int i=0;i<3;i++)
    {
      const double resA=Array[i].substitute(Pt[0],Pt[1],Pt[2]);
      if (resA*resA>tol)
	return 1;
    }
  return 0;
}

int
solveValues::checkSolutions() 
  /*!
    Objective is to eliminate the solution based on the master equations
    Also reduces the number of non-unique solutions
    \return Number of real solutions
  */
{
  ELog::RegMethod RegA("solveValues","checkSolutions");

  Ans.erase(remove_if(Ans.begin(),Ans.end(),
		      std::bind(&solveValues::notValid,
				this,std::placeholders::_1)),
	    Ans.end());

  sort(Ans.begin(),Ans.end(),Geometry::vecOrder());
  Ans.erase(unique(Ans.begin(),Ans.end()),Ans.end());
  return static_cast<int>(Ans.size());
}

void 
solveValues::setEquations(const PolyVar<3>& FXYZ,const PolyVar<3>& GXYZ,
			  const PolyVar<3>& HXYZ)
  /*!
    Sets the basic equations
    \param FXYZ :: First polynominal    
    \param GXYZ :: Second polynominal
    \param HXYZ :: Third polynominal
  */
{
  ELog::RegMethod RegA("solveValues","setEquations");

  Ans.clear();
  Array[0]= FXYZ;
  Array[1]= GXYZ;
  Array[2]= HXYZ;
  std::sort(Array,Array+2);
  return;
}

}  // NAMESPACE  mathLevel

