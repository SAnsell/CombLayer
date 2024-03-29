/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/FocusGenerator.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "FocusGenerator.h"

namespace setVariable
{

FocusGenerator::FocusGenerator() :
  yStepActive(0),yBeamActive(0),yStep(0.0),
  yBeam(0.0),zStep(0.0),
  layerThick({0.5,0.0,1.5}),
  layerMat({"Void","Aluminium","Void","Void"})
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

FocusGenerator::FocusGenerator(const FocusGenerator& A) : 
  yStepActive(A.yStepActive),yBeamActive(A.yBeamActive),
  yStep(A.yStep),yBeam(A.yBeam),zStep(A.zStep),
  layerThick(A.layerThick),layerMat(A.layerMat)
  /*!
    Copy constructor
    \param A :: FocusGenerator to copy
  */
{}

FocusGenerator&
FocusGenerator::operator=(const FocusGenerator& A)
  /*!
    Assignment operator
    \param A :: FocusGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      yStepActive=A.yStepActive;
      yBeamActive=A.yBeamActive;
      yStep=A.yStep;
      yBeam=A.yBeam;
      zStep=A.zStep;
      layerThick=A.layerThick;
      layerMat=A.layerMat;
    }
  return *this;
}

FocusGenerator::~FocusGenerator() 
 /*!
   Destructor
 */
{}

void
FocusGenerator::setLayer(const size_t index,const double T,
			 const std::string& mat)
  /*!
    Set each layer
    \param index :: Index value
    \param T :: Thickness
    \param mat :: material
   */
{
  ELog::RegMethod RegA("FocusGenerator","setLayer");

  if (index>layerThick.size())
    throw ColErr::IndexError<size_t>(index,layerThick.size(),
				     "index/layerThick");
  if (index==layerThick.size())
    {
      layerThick.push_back(T);
      layerMat.push_back(mat);
    }
  else
    {
      layerThick[index]=T;
      layerMat[index]=mat;
    }
  return;
}
  
void
FocusGenerator::writeLayers(FuncDataBase& Control,
			    const std::string& keyName,
			    const double length) const
/*!
    Generate the common layer variables variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
   */
{
  ELog::RegMethod RegA("FocusGenerator","writeLayers");

  
  Control.addVariable(keyName+"Length",length);       
  Control.addVariable(keyName+"XStep",0.0);       

  if (!yStepActive)
    Control.addVariable<double>(keyName+"YStep",-length/2.0);
  else if (yStepActive==1)
      Control.addVariable<double>(keyName+"YStep",yStep);
  else
    Control.addVariable<double>(keyName+"YStep",yStep-length/2.0);


  Control.addVariable(keyName+"ZStep",zStep);       
  Control.addVariable(keyName+"ZAngle",0.0);

  Control.addVariable(keyName+"LayerMat0",layerMat[0]);
  size_t activeLayer(0);
  for(size_t i=1;i<layerThick.size();i++)
    {
      if (layerThick[i]>Geometry::zeroTol)
	{
	  activeLayer++;
	  const std::string SNum(std::to_string(activeLayer));
	  Control.addVariable(keyName+"LayerThick"+SNum,layerThick[i]);
	  Control.addVariable(keyName+"LayerMat"+SNum,layerMat[i]);
	}
    }
  Control.addVariable(keyName+"NShapeLayers",activeLayer+1);

  return;
  
}
  
void
FocusGenerator::generateTaper(FuncDataBase& Control,
			      const std::string& keyName,
			      const double length,
			      const double HS,const double HE,
			      const double VS,const double VE)  const
/*!
    Generate the focus-taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param HS :: Horrizontal start
    \param HE :: Horrizontal end
    \param VS :: Vertical start
    \param VE :: Vertical end
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateTaper");

    
  writeLayers(Control,keyName,length);

  Control.addVariable(keyName+"TypeID","Taper");
  Control.addVariable(keyName+"HeightStart",VS);
  Control.addVariable(keyName+"HeightEnd",VE);
  Control.addVariable(keyName+"WidthStart",HS);
  Control.addVariable(keyName+"WidthEnd",HE);
  Control.addVariable(keyName+"Length",length);

  return;
}

void
FocusGenerator::generateGeneralTaper(FuncDataBase& Control,
				     const std::string& keyName,
				     const double length,
				     const double HSL,const double HEL,
				     const double HSR,const double HER,
				     const double VSB,const double VEB,
				     const double VST,const double VET)  const
  /*!
    Generate the focus-taper variables for general taper
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param HSL :: Distance LEFTWALL-CENTERLINE at start
    \param HEL :: Distance LEFTWALL-CENTERLINE at end
    \param HSR :: Distance RIGHTWALL-CENTERLINE at start
    \param HER :: Distance RIGHTWALL-CENTERLINE at end
    \param VSB :: Distance BOTTOMWALL-CENERLINE at start
    \param VEB :: Distance BOTTOMWALL-CENERLINE at end
    \param VST :: Distance TOPWALL-CENERLINE at start
    \param VET :: Distance TOPWALL-CENERLINE at end
  */
{
  ELog::RegMethod RegA("FocusGenerator","generateGeneralTaper");
    
  writeLayers(Control,keyName,length);

  Control.addVariable(keyName+"TypeID","GeneralTaper");
  Control.addVariable(keyName+"HeightStartTop",VST);
  Control.addVariable(keyName+"HeightEndTop",VET);
  Control.addVariable(keyName+"HeightStartBottom",VSB);
  Control.addVariable(keyName+"HeightEndBottom",VEB);
  Control.addVariable(keyName+"WidthStartLeft",HSL);
  Control.addVariable(keyName+"WidthEndLeft",HEL);
  Control.addVariable(keyName+"WidthStartRight",HSR);
  Control.addVariable(keyName+"WidthEndRight",HER);

  Control.addVariable(keyName+"Length",length);

  return;
}

void
FocusGenerator::generateRectangle(FuncDataBase& Control,
				  const std::string& keyName,
				  const double length,
				  const double H,
				  const double V)  const
/*!
    Generate the focus-taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param H :: Horrizontal 
    \param V :: Vertical 
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateRectangle");
  writeLayers(Control,keyName,length);
  
  Control.addVariable(keyName+"TypeID","Rectangle");
  Control.addVariable(keyName+"Height",V);
  Control.addVariable(keyName+"Width",H);
  Control.addVariable(keyName+"Length",length);

  return;
}

void
FocusGenerator::generateBender(FuncDataBase& Control,
			       const std::string& keyName,
			       const double length,
			       const double HS,const double HE,
			       const double VS,const double VE,
			       const double radius,
			       const double yRotAngle) const
  /*!
    Create bender variables
    \param Control :: FuncDatabase to populate
    \param keyName :: principle name
    \param length :: total length
    \param HS :: Horrizontal start
    \param HE :: Horrizontal end
    \param VS :: Vertical start
    \param VE :: Vertical end
    \param radius :: radius of curvature [cm]
    \param angleDir :: angle to rotate primary plane
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateBender");

  writeLayers(Control,keyName,length);

  Control.addVariable(keyName+"YAngle",yRotAngle);
  Control.addVariable(keyName+"AHeight",VS);
  Control.addVariable(keyName+"BHeight",VE);
  Control.addVariable(keyName+"AWidth",HS);
  Control.addVariable(keyName+"BWidth",HE);

  Control.addVariable(keyName+"Radius",radius);
  return;
}
  
void
FocusGenerator::generateOctagon(FuncDataBase& Control,
				const std::string& keyName,
				const double length,
				const double WS,
				const double WE)  const
/*!
    Generate the focus-Octagon and Taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param WS :: Start Width
    \param WE :: End Width 
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateOctagon");
  writeLayers(Control,keyName,length);

  Control.addVariable(keyName+"TypeID","Octagon");
  Control.addVariable(keyName+"WidthStart",WS);
  Control.addVariable(keyName+"WidthEnd",WE);

  return;
}
  
}  // NAMESPACE setVariable
