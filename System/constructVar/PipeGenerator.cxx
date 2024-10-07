/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/PipeGenerator.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "PipeGenerator.h"

namespace setVariable
{

PipeGenerator::PipeGenerator() :
  pipe{0,8.0,8.0,16.0,16.0,0.5,"Aluminium"},
  flangeA{1,12.0,-1.0,24.0,24.0,1.0,"Aluminium"},
  flangeB{1,12.0,-1.0,24.0,24.0,1.0,"Aluminium"},
  windowA{0,6.0,-1.0,14,14.0,0.1,"Silicon300K"},
  windowB{0,6.0,-1.0,14,14.0,0.1,"Silicon300K"},
  claddingThick(0.0),
  voidMat("Void"),claddingMat("B4C"),
  outerVoid(0)
  /*!
    Constructor and defaults
  */
{}

void
PipeGenerator::setPipe(const double R,const double T)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
   */
{
  pipe.type=0;
  pipe.radius=R;
  if (T>0.0)
    pipe.thick=T;
  return;
}

void
PipeGenerator::setRectPipe(const double W,const double H,
                           const double T)
  /*!
    Set all the pipe values
    \param W :: full width of main pipe
    \param H :: full height of main pipe
    \param T :: Thickness
   */
{
  pipe.type=1;
  pipe.width=W;
  pipe.height=H;
  if (T>0.0)
    pipe.thick=T;
  return;
}

void
PipeGenerator::setWindow(const double R,const double T)
  /*!
    Set all the window values
    \param R :: radius of window [-ve to be bigger than main rad]
    \param T :: Thickness
   */
{
  windowA.type=1;
  windowA.radius=R;
  windowA.thick=T;
  windowB.type=1;
  windowB.radius=R;
  windowB.thick=T;
  return;
}

void
PipeGenerator::setRectWindow(const double W,
			     const double H,
			     const double T)
/*!
    Set all the window values for rectangle
    \param W :: full width of main window
    \param H :: full height of main window
    \param T :: Thickness
   */
{
  windowA.type=2;
  windowA.width=W;
  windowA.height=H;
  windowA.thick=T;
  windowB.type=2;
  windowB.width=W;
  windowB.height=H;
  windowB.thick=T;
  return;
}

void
PipeGenerator::setNoWindow()
  /*!
    Remove the window values
   */
{
  windowA.type=0;
  windowB.type=0;
  return;
}


void
PipeGenerator::setFlangeLength(const double A,const double B)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeA.thick=A;
  flangeB.thick=B;
  return;
}

void
PipeGenerator::setFlange(const double R,const double L,const double innerR)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  setAFlange(R,L);
  setBFlange(R,L);
  if (innerR>0.0) {
    flangeA.innerRadius = innerR;
    flangeB.innerRadius = innerR;
  }
  return;
}

void
PipeGenerator::setRectFlange(const double W,
			     const double H,
			     const double L)
/*!
    Set all the flange values (rectangle)
    \param W :: Width of flange
    \param H :: Height of flange
    \param L :: length
   */
{
  setARectFlange(W,H,L);
  setBRectFlange(W,H,L);
  return;
}

void
PipeGenerator::setAFlange(const double R,const double L)
  /*!
    Set all the A-flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeA.type=1;           // front radial
  flangeA.radius=R;
  flangeA.thick=L;
  return;
}

void
PipeGenerator::setARectFlange(const double W,const double H,
			  const double L)
  /*!
    Set all the A-flange values
    \param W :: width (rectangle flange)
    \param L :: length
   */
{
  flangeA.type=2;           // front rect
  flangeA.width=W;
  flangeA.height=H;
  flangeA.thick=L;
  return;
}

void
PipeGenerator::setBFlange(const double R,const double L)
  /*!
    Set all the B-flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeB.type=1;           // front radial
  flangeB.radius=R;
  flangeB.thick=L;

  return;
}

void
PipeGenerator::setBRectFlange(const double W,const double H,
			  const double L)
  /*!
    Set all the A-flange values
    \param W :: width (rectangle flange)
    \param L :: length
   */
{
  flangeB.type=2;           // front rect
  flangeB.width=W;
  flangeB.height=H;
  flangeB.thick=L;
  return;
}
void
PipeGenerator::setFlangePair(const double AR,const double AL,
			     const double BR,const double BL)
  /*!
    Set all the flange values
    \param AR :: radius of front flange
    \param AL :: front flange length
    \param BR :: radius of back flange
    \param BL :: back flange length
   */
{
  setAFlange(AR,AL);
  setBFlange(BR,BL);

  return;
}

void
PipeGenerator::setMat(const std::string& M,
		      const std::string& FM)
  /*!
    Set the pipe/flange materials
    \param M :: Material for main pipe
    \param FM :: material for flange (if different)
  */
{
  pipe.mat = M;
  const std::string FMunit=(FM=="") ? pipe.mat : FM;
  flangeA.mat=FMunit;
  flangeB.mat=FMunit;
  return;
}

void
PipeGenerator::setWindowMat(const std::string& M)
  /*!
    Set the window materials [back/front]
    \param M :: Material
   */
{
  windowA.mat=M;
  windowB.mat=M;
  return;
}

void
PipeGenerator::setWindowMat(const std::string& MA,
                            const std::string& MB)
  /*!
    Set the window materials [back/front]
    \param MA :: Front Material
    \param MB :: Back Material
   */
{
  windowA.mat=MA;
  windowB.mat=MB;
  return;
}

void
PipeGenerator::setCladding(const double T,const std::string& M)
  /*!
    Set the cladding thickess and material
    \param T :: Thickness
    \param M :: Material
   */
{
  claddingThick=T;
  claddingMat=M;
  return;
}

template<typename CF>
void
PipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  pipe.type=0;
  pipe.radius=CF::innerRadius;
  pipe.thick=CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();

  return;
}

template<typename CF>
void
PipeGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeA.type=1;
  flangeA.radius=CF::flangeRadius;
  flangeA.thick=CF::flangeLength;
  return;
}

template<typename CF>
void
PipeGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeB.type=1;
  flangeB.radius=CF::flangeRadius;
  flangeB.thick=CF::flangeLength;
  return;
}

void
PipeGenerator::generatePipe(FuncDataBase& Control,
			    const std::string& keyName,
                            const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("PipeGenerator","generatorPipe");

  const double minRadius(pipe.type ?
                         std::min(pipe.width,pipe.height) :
                         pipe.radius);

  const double realFlangeARadius=(flangeA.radius<0.0) ?
    minRadius-flangeA.radius : flangeA.radius;
  const double realFlangeBRadius=(flangeB.radius<0.0) ?
    minRadius-flangeB.radius : flangeB.radius;

    // VACUUM PIPES:
  if (!pipe.type)
    Control.addVariable(keyName+"Radius",pipe.radius);
  else
    {
      Control.addVariable(keyName+"Width",pipe.width);
      Control.addVariable(keyName+"Height",pipe.height);
    }
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"PipeThick",pipe.thick);

  Control.addVariable(keyName+"FlangeAType",flangeA.type);
  Control.addVariable(keyName+"FlangeBType",flangeB.type);

  if (flangeA.type)
    {
      if (flangeA.type==1) {
	Control.addVariable(keyName+"FlangeARadius",realFlangeARadius);
	if (flangeA.innerRadius>0.0)
	  Control.addVariable(keyName+"FlangeAInnerRadius",flangeA.innerRadius);
      } else
	{
	  Control.addVariable(keyName+"FlangeAWidth",flangeA.width);
	  Control.addVariable(keyName+"FlangeAHeight",flangeA.height);
	}
      Control.addVariable(keyName+"FlangeALength",flangeA.thick);
      Control.addVariable(keyName+"FlangeAMat",flangeA.mat);
    }
  if (flangeB.type)
    {
      if (flangeB.type==1) {
	Control.addVariable(keyName+"FlangeBRadius",realFlangeBRadius);
	if (flangeB.innerRadius>0.0)
	  Control.addVariable(keyName+"FlangeBInnerRadius",flangeB.innerRadius);
      } else
	{
	  Control.addVariable(keyName+"FlangeBWidth",flangeB.width);
	  Control.addVariable(keyName+"FlangeBHeight",flangeB.height);
	}
      Control.addVariable(keyName+"FlangeBLength",flangeB.thick);
      Control.addVariable(keyName+"FlangeBMat",flangeB.mat);
    }

  Control.addVariable(keyName+"WindowAType",windowA.type);
  Control.addVariable(keyName+"WindowBType",windowA.type);
  if (windowA.type)
    {
      Control.addVariable(keyName+"WindowAThick",windowA.thick);
      Control.addVariable(keyName+"WindowAMat",windowA.mat);
      if (windowA.type==1)
	Control.addVariable(keyName+"WindowARadius",windowA.radius);
      else
	{
	  Control.addVariable(keyName+"WindowAHeight",windowA.height);
	  Control.addVariable(keyName+"WindowAWidth",windowA.width);
	}
    }
  if (windowB.type)
    {
      Control.addVariable(keyName+"WindowBThick",windowB.thick);
      Control.addVariable(keyName+"WindowBMat",windowB.mat);
      if (windowB.type==1)
	Control.addVariable(keyName+"WindowBRadius",windowB.radius);
      else
	{
	  Control.addVariable(keyName+"WindowBHeight",windowB.height);
	  Control.addVariable(keyName+"WindowBWidth",windowB.width);
	}
    }

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PipeMat",pipe.mat);

  Control.addVariable(keyName+"CladdingThick",claddingThick);
  Control.addVariable(keyName+"CladdingMat",claddingMat);

  Control.addVariable(keyName+"OuterVoid",outerVoid);

  return;
}

///\cond TEMPLATE
  template void PipeGenerator::setCF<CF8_TDC>();
  template void PipeGenerator::setCF<CF16_TDC>();
  template void PipeGenerator::setCF<CF16>();
  template void PipeGenerator::setCF<CF18_TDC>();
  template void PipeGenerator::setCF<CF25>();
  template void PipeGenerator::setCF<CF26_TDC>();
  template void PipeGenerator::setCF<CF34_TDC>();
  template void PipeGenerator::setCF<CF35_TDC>();
  template void PipeGenerator::setCF<CF37_TDC>();
  template void PipeGenerator::setCF<CF40_22>();
  template void PipeGenerator::setCF<CF40>();
  template void PipeGenerator::setCF<CF50>();
  template void PipeGenerator::setCF<CF63>();
  template void PipeGenerator::setCF<CF66_TDC>();
  template void PipeGenerator::setCF<CF100>();
  template void PipeGenerator::setCF<CF120>();
  template void PipeGenerator::setCF<CF150>();
  template void PipeGenerator::setCF<CF200>();
  template void PipeGenerator::setCF<CF250>();
  template void PipeGenerator::setCF<CF300>();
  template void PipeGenerator::setCF<CF350>();
  template void PipeGenerator::setAFlangeCF<CF25>();
  template void PipeGenerator::setAFlangeCF<CF26_TDC>();
  template void PipeGenerator::setAFlangeCF<CF34_TDC>();
  template void PipeGenerator::setAFlangeCF<CF37_TDC>();
  template void PipeGenerator::setAFlangeCF<CF40_22>();
  template void PipeGenerator::setAFlangeCF<CF40>();
  template void PipeGenerator::setAFlangeCF<CF63>();
  template void PipeGenerator::setAFlangeCF<CF66_TDC>();
  template void PipeGenerator::setAFlangeCF<CF100>();
  template void PipeGenerator::setAFlangeCF<CF120>();
  template void PipeGenerator::setAFlangeCF<CF150>();
  template void PipeGenerator::setAFlangeCF<CF200>();
  template void PipeGenerator::setAFlangeCF<CF250>();
  template void PipeGenerator::setAFlangeCF<CF300>();
  template void PipeGenerator::setAFlangeCF<CF350>();
  template void PipeGenerator::setBFlangeCF<CF25>();
  template void PipeGenerator::setBFlangeCF<CF34_TDC>();
  template void PipeGenerator::setBFlangeCF<CF37_TDC>();
  template void PipeGenerator::setBFlangeCF<CF40_22>();
  template void PipeGenerator::setBFlangeCF<CF40>();
  template void PipeGenerator::setBFlangeCF<CF63>();
  template void PipeGenerator::setBFlangeCF<CF66_TDC>();
  template void PipeGenerator::setBFlangeCF<CF100>();
  template void PipeGenerator::setBFlangeCF<CF120>();
  template void PipeGenerator::setBFlangeCF<CF150>();
  template void PipeGenerator::setBFlangeCF<CF200>();
  template void PipeGenerator::setBFlangeCF<CF250>();
  template void PipeGenerator::setBFlangeCF<CF300>();
  template void PipeGenerator::setBFlangeCF<CF350>();

///\endcond TEMPLATE


}  // NAMESPACE setVariable
