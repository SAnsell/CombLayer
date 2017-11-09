/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/FocusGenerator.cxx
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
    \param Key :: Name for item in search
  */
{}

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
    Control.addParse<double>(keyName+"YStep","-"+keyName+"Length/2.0");
  else if (yStepActive==1)
    Control.addVariable<double>(keyName+"YStep",yStep);
  else
    Control.addVariable<double>(keyName+"YStep",yStep-length/2.0);
  if (!yBeamActive)
    Control.addParse<double>(keyName+"BeamYStep","-"+keyName+"Length/2.0");
  else if (yBeamActive==1)
    Control.addVariable<double>(keyName+"BeamYStep",yBeam);
  else
    Control.addVariable<double>(keyName+"BeamYStep",yStep-length/2.0);

  Control.addVariable(keyName+"ZStep",zStep);       
  Control.addVariable(keyName+"XYAngle",0.0);       
  Control.addVariable(keyName+"ZAngle",0.0);
  Control.addVariable(keyName+"BeamXYAngle",0.0);       

  Control.addVariable(keyName+"NShapes",1);
  Control.addVariable(keyName+"ActiveShield",0);

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
  Control.addVariable(keyName+"NShapeLayers",activeLayer);

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
  
  Control.addVariable(keyName+"0TypeID","Taper");
  Control.addVariable(keyName+"0HeightStart",VS);
  Control.addVariable(keyName+"0HeightEnd",VE);
  Control.addVariable(keyName+"0WidthStart",HS);
  Control.addVariable(keyName+"0WidthEnd",HE);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}

void
FocusGenerator::generateRectangle(FuncDataBase& Control,
				  const std::string& keyName,
				  const double length,
				  const double H,const double V)  const
/*!
    Generate the focus-taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param H :: Horrizontal 
    \param V :: Vertical 
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateTaper");
  writeLayers(Control,keyName,length);
  
  Control.addVariable(keyName+"0TypeID","Rectangle");
  Control.addVariable(keyName+"0Height",V);
  Control.addVariable(keyName+"0Width",H);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}

void
FocusGenerator::generateBender(FuncDataBase& Control,
			       const std::string& keyName,
			       const double length,
			       const double HS,const double HE,
			       const double VS,const double VE,
			       const double radius,const double angleDir) const
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

  Control.addVariable(keyName+"0TypeID","Bend");
  Control.addVariable(keyName+"0AHeight",VS);
  Control.addVariable(keyName+"0BHeight",VE);
  Control.addVariable(keyName+"0AWidth",HS);
  Control.addVariable(keyName+"0BWidth",HE);
  Control.addVariable(keyName+"0Length",length);
  Control.addVariable(keyName+"0AngDir",angleDir);
  Control.addVariable(keyName+"0Radius",radius);
  return;
}
  
void
FocusGenerator::generateOctagon(FuncDataBase& Control,
				const std::string& keyName,
				const double length,
				const double WS,const double WE)  const
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
  
  Control.addVariable(keyName+"0TypeID","Octagon");
  Control.addVariable(keyName+"0WidthStart",WS);
  Control.addVariable(keyName+"0WidthEnd",WE);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}
  
}  // NAMESPACE setVariable
