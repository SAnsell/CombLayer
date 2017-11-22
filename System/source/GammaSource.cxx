/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/GammaSource.cxx
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
#include "Transform.h"
#include "localRotate.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"
#include "SourceBase.h"
#include "GammaSource.h"

namespace SDef
{

GammaSource::GammaSource(const std::string& keyName) : 
  attachSystem::FixedOffset(keyName,0),
  SourceBase(),shape("Circle"),
  width(1.0),height(1.0),radius(1.0),
  angleSpread(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

GammaSource::GammaSource(const GammaSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  shape(A.shape),width(A.width),height(A.height),
  radius(A.radius),angleSpread(A.angleSpread),
  FocusPoint(A.FocusPoint)
  /*!
    Copy constructor
    \param A :: GammaSource to copy
  */
{}

GammaSource&
GammaSource::operator=(const GammaSource& A)
  /*!
    Assignment operator
    \param A :: GammaSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      shape=A.shape;
      width=A.width;
      height=A.height;
      radius=A.radius;
      angleSpread=A.angleSpread;
      FocusPoint=A.FocusPoint;
    }
  return *this;
}

GammaSource::~GammaSource() 
  /*!
    Destructor
  */
{}

GammaSource*
GammaSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new GammaSource(*this);
}
  
void
GammaSource::setPoint()
  /*!
    Set the shape to be point
  */
{
  shape="Point";
  return;
}

void
GammaSource::setRadius(const double R)
  /*!
    Set the shape to be circle
    \param R :: Circle emmision radius
  */
{
  shape="Circle";
  radius=std::abs(R);
  return;
}

void
GammaSource::setRectangle(const double W,const double H)
  /*!
    Set the shape to be circle
    \param W :: Width
    \param H :: Height 
  */
{
  shape="Rectangle";
  width=W;
  height=H;
  return;
}

void
GammaSource::setAngleSpread(const double A)
  /*!
    Set angle spread
    \param A :: Angle to use [deg]
  */
{
  angleSpread=A;
  return;
}
  
void
GammaSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("GammaSource","populate");

  FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);
  
  shape=Control.EvalDefVar<std::string>(keyName+"Shape",shape);
  if (shape=="Circle")   // circle
    radius=Control.EvalDefVar<double>(keyName+"Radius",radius);
  else if (shape=="Rectangle")
    {
      height=Control.EvalDefVar<double>(keyName+"Height",height);
      width=Control.EvalDefVar<double>(keyName+"Width",width);
    }    
  angleSpread=Control.EvalDefVar<double>(keyName+"ASpread",angleSpread); 
  
  return;
}

void
GammaSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("GammaSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();
  return;
}
  
void
GammaSource::calcPosition()
  /*!
    Calcuate the focus position and other points
  */    
{
  ELog::RegMethod RegA("GammaSource","calcPosition");
  if (angleSpread>Geometry::zeroTol)
    FocusPoint=Origin-Y*(radius/tan(M_PI*angleSpread/180.0));
  else
    FocusPoint=Origin;
  return;
}


void
GammaSource::rotate(const localRotate& LR)
  /*!
    Rotate the whole source
    \param LR :: Rotation value
  */
{
  ELog::RegMethod Rega("GammaSource","rotate");
  FixedComp::applyRotation(LR);
  LR.applyFull(FocusPoint);

  if (shape!="Circle" && 
      (!X.masterDir() || !Y.masterDir() || !Z.masterDir() ||
       std::abs(std::abs(FocusPoint.dotProd(Y))-FocusPoint.abs())>Geometry::zeroTol))
    {
      SourceBase::createTransform(FocusPoint,X,Y,Z);
    }
  else
    {
      delete TransPtr;
      TransPtr=0;
    }
  return;
}
  
void
GammaSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createSource");

  ELog::EM<<"Source shape ::"<<shape<<ELog::endDiag;

  sourceCard.setComp("par",particleType);            
  SourceBase::createEnergySource(sourceCard);    

  ELog::EM<<"AngleSPread == "<<angleSpread<<ELog::endDiag;
  if (angleSpread>Geometry::zeroTol)
    {
      SDef::SrcData D2(2);
      SDef::SrcInfo SI2;
      SI2.addData(-1.0);
      SI2.addData(cos(M_PI*angleSpread/180.0));
      SI2.addData(1.0);
      SDef::SrcProb SP2;
      SP2.setData({0.0,0.0,1.0});
      D2.addUnit(SI2);
      D2.addUnit(SP2);
      sourceCard.setData("dir",D2);
    }
  else
    {
      sourceCard.setComp("dir",1.0);
    }

  if (shape=="Circle")
    createRadialSource(sourceCard);
  else if (shape=="Rectangle")
    createRectangleSource(sourceCard);
  else if (shape!="Point")
    {
      ELog::EM<<"GammaSource shape options are :\n";
      ELog::EM<<"   Circle\n";
      ELog::EM<<"   Rectangle\n";
      ELog::EM<<"   Point\n";
      ELog::EM<<ELog::endDiag;
      throw ColErr::InContainerError<std::string>(shape,"Shape not known");
    }
  return;
}


void
GammaSource::createRadialSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma radial source
    - This is non-primary source creation
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createRadialSource");
  
  sourceCard.setComp("ara",M_PI*radius*radius);
  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("pos",FocusPoint);
  return;
}  

void
GammaSource::createRectangleSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createRectangleSource");

  sourceCard.setComp("vec",Y);
  sourceCard.setComp("y",Origin.Y());
  sourceCard.setComp("ara",width*height);

  // are we aligned on the master direction:
  const int aR=(TransPtr) ? 2 : std::abs(Y.masterDir());

  const std::string xyz[]={"x","y","z"};
  if (!TransPtr)
    {
      sourceCard.setComp(xyz[aR-1],Origin.dotProd(Y));
      sourceCard.setComp("vec",Y);
      sourceCard.setComp("axs",Y);
    }
  else
    {
      sourceCard.setComp("vec",Geometry::Vec3D(0,1.0,0));
      sourceCard.setComp("axs",Geometry::Vec3D(0,1.0,0));
    }
  
  

  if (TransPtr)
    sourceCard.setComp("tr",TransPtr->getName());

  
  SDef::SrcData D3(3);
  SDef::SrcInfo SI3;
  SI3.addData(Origin[0]-width/2.0);
  SI3.addData(Origin[0]+width/2.0);

  SDef::SrcData D4(4);
  SDef::SrcInfo SI4;
  SI4.addData(Origin[2]-height/2.0);
  SI4.addData(Origin[2]+height/2.0);

  SDef::SrcProb SP3;
  SP3.addData(0.0);
  SP3.addData(1.0);
  D3.addUnit(SI3);  
  D3.addUnit(SP3);  
  sourceCard.setData(xyz[(aR+1) % 3],D3);

  SDef::SrcProb SP4;
  SP4.addData(0.0);
  SP4.addData(1.0);
  D4.addUnit(SI4);  
  D4.addUnit(SP4);
  sourceCard.setData(xyz[aR % 3],D4);

  return;
}  


void
GammaSource::createAll(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param FC :: FixedComp for origin
    \param linkIndex :: link point
   */
{
  ELog::RegMethod RegA("GammaSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);
  calcPosition();
  return;
}


void
GammaSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("GammaSource","write");

  Source sourceCard;

  createSource(sourceCard);
  sourceCard.write(OX);
  return;

}

void
GammaSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("GammaSource","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}

} // NAMESPACE SDef
