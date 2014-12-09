/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   source/GammaSource.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "WorkData.h"
#include "World.h"
#include "GammaSource.h"

namespace SDef
{

GammaSource::GammaSource() : 
  FixedComp("gammaSource",0),
  cutEnergy(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}


GammaSource::~GammaSource() 
  /*!
    Destructor
  */
{}

void
GammaSource::loadEnergy(const std::string& FName)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
  */
{
  ELog::RegMethod RegA("GammaSource","loadEnergy");

  const double current(60.0);
  const int eCol(1);
  const int iCol(11);
  
  Energy.clear();
  EWeight.clear();
  
  WorkData A;
  if (A.load(FName,eCol,iCol,0))
    {
      ELog::EM<<"Failed to read file:"<<FName<<ELog::endErr;
      return;
    }  

  A.xScale(1e-6);
  A.binDivide(1.0);
  //  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  weight=IV.getVal()/(current*6.24150636309e12);
  // Normalize A:
  A/=IV;

  Energy=A.getXdata();
  if (Energy.size()<2 || weight<1e-12)
    {
      ELog::EM<<"Failed to read energy/data from file:"<<FName<<ELog::endErr;
      return;
    }
  Energy.push_back(2.0*Energy.back()-Energy[Energy.size()-2]);
  const std::vector<DError::doubleErr>& Yvec=A.getYdata();

  std::vector<DError::doubleErr>::const_iterator vc;
  EWeight.push_back(0.0);
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    EWeight.push_back(vc->getVal());
  EWeight.push_back(0.0);

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
  
  xStep=Control.EvalVar<double>(keyName+"XStep"); 
  yStep=Control.EvalVar<double>(keyName+"YStep"); 
  zStep=Control.EvalVar<double>(keyName+"ZStep"); 

  radius=Control.EvalVar<double>(keyName+"Radius"); 
  angleSpread=Control.EvalVar<double>(keyName+"ASpread"); 
  
  double E=Control.EvalVar<double>(keyName+"EStart"); 
  const size_t nE=Control.EvalVar<size_t>(keyName+"NE"); 
  const double EEnd=Control.EvalVar<double>(keyName+"EEnd"); 
  
  const double EStep((EEnd-E)/(nE+1));
  for(size_t i=0;i<nE;i++)
    {
      Energy.push_back(E);
      EWeight.push_back(1.0);
      E+=EStep;
    }
  return;
}

void
GammaSource::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vector
   */
{
  ELog::RegMethod RegA("GammaSource","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  Direction=Y;

  //  applyAngleRotate(xyAngle,zAngle);
  return;
}
  
void
GammaSource::calcPosition()
  /*!
    Calcuate the focus position and other poitns
  */
    
{
  ELog::RegMethod RegA("GammaSource","calcPosition");
  FocusPoint=Origin-Direction*(radius/tan(M_PI*angleSpread/180.0));
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

  
  sourceCard.setActive();
  sourceCard.setComp("vec",Direction);
  sourceCard.setComp("par",2);            /// photon (2)
  sourceCard.setComp("pos",FocusPoint);

  // Direction:
  
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1;
  SI1.addData(-1.0);
  SI1.addData(cos(M_PI*angleSpread/180.0));
  SI1.addData(1.0);

  SDef::SrcProb SP1;
  SP1.addData(0.0);
  SP1.addData(0.0);
  SP1.addData(1.0);
  D1.addUnit(SI1);  
  D1.addUnit(SP1);  
  sourceCard.setData("dir",D1);  

  // Energy:  
  SDef::SrcData D2(2);
  SDef::SrcInfo SI2('A');
  SDef::SrcProb SP2;
  SP2.setData(EWeight);
  SI2.setData(Energy);
  D2.addUnit(SI2);
  D2.addUnit(SP2);
  sourceCard.setData("erg",D2);


  return;
}  

void
GammaSource::createAll(const FuncDataBase& Control,
		       SDef::Source& sourceCard)
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("GammaSource","createAll");
  populate(Control);
  createUnitVector(World::masterOrigin());
  calcPosition();
  createSource(sourceCard);
  return;
}


} // NAMESPACE SDef
