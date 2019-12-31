/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CollGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <stack>
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CollGenerator.h"

namespace setVariable
{

CollGenerator::CollGenerator() :
  radius(4.0),minLength(-0.8235),
  AWidth(2.6),minWidth(2.0),
  BWidth(2.6),AHeight(1.8),
  minHeight(1.6),BHeight(1.8),
  mat("Copper"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

CollGenerator::CollGenerator(const CollGenerator& A) : 
  radius(A.radius),minLength(A.minLength),AWidth(A.AWidth),
  minWidth(A.minWidth),BWidth(A.BWidth),AHeight(A.AHeight),
  minHeight(A.minHeight),BHeight(A.BHeight),mat(A.mat),
  voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: CollGenerator to copy
  */
{}

CollGenerator&
CollGenerator::operator=(const CollGenerator& A)
  /*!
    Assignment operator
    \param A :: CollGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      radius=A.radius;
      minLength=A.minLength;
      AWidth=A.AWidth;
      minWidth=A.minWidth;
      BWidth=A.BWidth;
      AHeight=A.AHeight;
      minHeight=A.minHeight;
      BHeight=A.BHeight;
      mat=A.mat;
      voidMat=A.voidMat;
    }
  return *this;
}
  
CollGenerator::~CollGenerator() 
 /*!
   Destructor
 */
{}

void
CollGenerator::setMinSize(const double L,
			  const double W,
			  const double H)
			  
  /*!
    Set min length/width/height
    \param L :: Length [ -ve for fractional ]
    \param W :: width  [ -ve for fractional ]
    \param H :: height [ -ve for fractional ]
   */
{
  minLength=L;
  minWidth=W;
  minHeight=H;
  return;
}

void
CollGenerator::setMinAngleSize(const double L,
			       const double Dist,
			       const double angX,
			       const double angZ)
  
  /*!
    Set min length/width/height
    \param L :: Length 
    \param Dist :: distance from undulator centre 
    \param angX :: angle full opening [urad]
    \param angZ :: angle full opening [urad]
   */
{
  minLength=L;
  minWidth = 2.0*Dist*tan(1e-6*angX/2.0);
  minHeight= 2.0*Dist*tan(1e-6*angZ/2.0);
  return;
}

void
CollGenerator::setFrontAngleSize(const double Dist,
				const double angX,
				const double angZ)
  
  /*!
    Set min length/width/height
    \param Dist :: distance from undulator centre 
    \param angX :: angle full opening [urad]
    \param angZ :: angle full opening [urad]
   */
{
  AWidth= 2.0*Dist*tan(1e-6*angX/2.0);
  AHeight= 2.0*Dist*tan(1e-6*angZ/2.0);
  return;
}

void
CollGenerator::setBackAngleSize(const double Dist,
				const double angX,
				const double angZ)
  
  /*!
    Set min length/width/height
    \param Dist :: distance from undulator centre 
    \param angX :: angle full opening [urad]
    \param angZ :: angle full opening [urad]
   */
{
  BWidth= 2.0*Dist*tan(1e-6*angX/2.0);
  BHeight= 2.0*Dist*tan(1e-6*angZ/2.0);
  return;
}
  
void
CollGenerator::setMain(const double R,const std::string& M,
		       const std::string& VM)
  /*!
    Set the surface radius
    \param R :: surface radius
    \param M :: Material
    \param VM :: Void material
   */
{
  radius=R;
  mat=M;
  voidMat=VM;
  return;
}

void
CollGenerator::setFrontGap(const double W,const double H)
  /*!
    Set the front void window
    \param W :: front width opening
    \param H :: front height opening
   */
{
  AWidth=W;
  AHeight=H;
  return;
}
	
void
CollGenerator::setBackGap(const double W,const double H)
  /*!
    Set the back void window
    \param W :: front width opening
    \param H :: front height opening
   */
{
  BWidth=W;
  BHeight=H;
  return;
}
				  
void
CollGenerator::generateColl(FuncDataBase& Control,
			    const std::string& keyName,
			    const double yStep,
			    const double len) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param len :: length
  */
{
  ELog::RegMethod RegA("CollGenerator","generateColl");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",len);

  const double ML=(minLength<0.0) ? -minLength*len : minLength;
  const double MW=(minWidth<0.0) ? -minWidth*AWidth : minWidth;
  const double MH=(minHeight<0.0) ? -minHeight*AHeight : minHeight;

  if (ML>len+Geometry::zeroTol)
    throw ColErr::SizeError<double>
      (ML,len,"Length/Min Gap lengths wrong order");
  
  Control.addVariable(keyName+"MinLength",ML);
    
  Control.addVariable(keyName+"innerMinWidth",MW);
  Control.addVariable(keyName+"innerMinHeight",MH);

  Control.addVariable(keyName+"innerAWidth",AWidth);
  Control.addVariable(keyName+"innerAHeight",AHeight);

  Control.addVariable(keyName+"innerBWidth",BWidth);
  Control.addVariable(keyName+"innerBHeight",BHeight);

  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"VoidMat",voidMat);
       
  return;

}

  
}  // NAMESPACE setVariable
