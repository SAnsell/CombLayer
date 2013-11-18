/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   source/SourceCreate.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "ManagedPtr.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "SourceCreate.h"

namespace SDef
{

void
createSimpleSource(Source& sourceCard,
		   const double Einit,const double Eend)
  /*!
    Create a super simple source 
    \param sourceCard :: Source system
    \param Einit :: Energy [MeV]
    \param Eend :: Energy [MeV]
  */
{
  sourceCard.setActive();

  SDef::SrcData E1(1);
  SDef::SrcInfo* SIE1=E1.getInfo();
  SDef::SrcProb* SPE1=E1.getProb();
  
  const int NEPts(30);
  const double ELA=log(Einit);       
  const double ELB=log(Eend);        

  const double Estep=(ELB-ELA)/NEPts;
  double EWeight(0.0);
  double Eval(0.0);
  double width;
  for(int i=0;i<NEPts;i++)
    {
      width= -Eval;
      Eval=exp(ELA+Estep*i);
      width+=Eval;
      SIE1->addData(Eval);
      SPE1->addData(EWeight);
      EWeight=width*(1/Eval);
    }
  sourceCard.setData("erg",E1);
  return;
}

void
createBilbaoSource(const FuncDataBase& Control,Source& sourceCard)
  /*!
    Creates a target 1 proton source:
    FWHM == 1.5*2.35482 ==> 3.53223
    \param Control :: Control system
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SourceCreate","createTS2Source");

  const double E=Control.EvalDefVar<double>("sdefEnergy",50.0);
  const double yStart=Control.EvalDefVar<double>("sdefYPos",-10.0);

  sourceCard.setActive();
  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Geometry::Vec3D(-1,1,0).unit());
  sourceCard.setComp("par",9);
  sourceCard.setComp("erg",E);
  //  sourceCard.setComp("ccc",76);
  sourceCard.setComp("y",yStart);

  SrcData D1(1);
  SrcProb SP1(1);
  SP1.setFminus(-41,5.887,0);
  D1.addUnit(SP1);

  SrcData D2(2);
  SrcProb SP2(1);
  SP2.setFminus(-41,8.326,0);
  D2.addUnit(SP2);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  return;
}

void
createESSSource(const FuncDataBase& Control,Source& sourceCard)
  /*!
    Creates a target 1 proton source:
    FWHM == 1.5*2.35482 ==> 3.53223
    \param Control :: Control system
    \param sourceCard :: Source system
   */
{
  ELog::RegMethod RegA("SourceCreate","createESSSource");

  const double E=Control.EvalDefVar<double>("sdefEnergy",2500.0);
  const double yStart=Control.EvalDefVar<double>("sdefYPos",-10.0);

  sourceCard.setActive();
  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Geometry::Vec3D(0,1,0));
  sourceCard.setComp("par",9);
  sourceCard.setComp("erg",E);
  //  sourceCard.setComp("ccc",76);
  sourceCard.setComp("y",yStart);

  const double xRange=Control.EvalDefVar<double>("sdefWidth",8.0);
  const double step(0.5);  
  std::vector<double> XPts;
  std::vector<double> XProb;
  double XValue= -xRange-step;
  do
    {
      XValue+=step;
      XPts.push_back(XValue);
      XProb.push_back(1.0-(XValue*XValue)/(xRange*xRange));
    } while (XValue<xRange);

  const double zRange=Control.EvalDefVar<double>("sdefHeight",3.0);
  std::vector<double> ZPts;
  std::vector<double> ZProb;
  double ZValue= -zRange-step;
  do
    {
      ZValue+=step;
      ZPts.push_back(ZValue);
      ZProb.push_back(1.0-(ZValue*ZValue)/(zRange*zRange));
    } while (ZValue<zRange);
  
  
  SrcData D1(1);  
  SrcData D2(2);
  
  SrcInfo SI1('A');
  SrcInfo SI2('A');
  SI1.setData(XPts);
  SI2.setData(ZPts);

  SrcProb SP1;
  SrcProb SP2;
  SP1.setData(XProb);
  SP2.setData(ZProb);

  D1.addUnit(SI1);
  D2.addUnit(SI2);
  D1.addUnit(SP1);
  D2.addUnit(SP2);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  return;
}

void
createTS1Source(const FuncDataBase& Control,Source& sourceCard)
  /*!
    Creates a target 1 proton source:
    FWHM == 1.5*2.35482 ==> 3.53223
    \param Control :: Control system
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SourceCreate","createTS2Source");

  const double E=Control.EvalDefVar<double>("sdefEnergy",800.0);
  const double yStart=Control.EvalDefVar<double>("sdefYPos",-10.0);
  const double xShift=Control.EvalDefVar<double>("sdefXOffset",0.0);
  const double zShift=Control.EvalDefVar<double>("sdefZOffset",0.0);

  sourceCard.setActive();
  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Geometry::Vec3D(0,1,0));
  sourceCard.setComp("par",9);
  sourceCard.setComp("erg",E);
  //  sourceCard.setComp("ccc",76);
  sourceCard.setComp("y",yStart);

  const double xRange=Control.EvalDefVar<double>("sdefWidth",4.5);
  const double xStep(xRange/16.0);  
  std::vector<double> XPts;
  std::vector<double> XProb;
  double XValue= -xRange-xStep;
  do
    {
      XValue+=xStep;
      XPts.push_back(XValue+xShift);
      XProb.push_back(1.0-(XValue*XValue)/(xRange*xRange));
    } while (XValue<xRange);

  const double zRange=Control.EvalDefVar<double>("sdefHeight",4.5);
  const double zStep(zRange/16.0);  
  std::vector<double> ZPts;
  std::vector<double> ZProb;
  double ZValue= -zRange-zStep;
  do
    {
      ZValue+=zStep;
      ZPts.push_back(ZValue+zShift);
      ZProb.push_back(1.0-(ZValue*ZValue)/(zRange*zRange));
    } while (ZValue<zRange);

  SrcData D1(1);  
  SrcData D2(2);
  
  SrcInfo SI1('A');
  SrcInfo SI2('A');
  SI1.setData(XPts);
  SI2.setData(ZPts);

  SrcProb SP1;
  SrcProb SP2;
  SP1.setData(XProb);
  SP2.setData(ZProb);

  D1.addUnit(SI1);
  D2.addUnit(SI2);
  D1.addUnit(SP1);
  D2.addUnit(SP2);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  return;
}

void
createTS1GaussianSource(const FuncDataBase& Control,Source& sourceCard)
  /*!
    Creates a target 1 proton source [gaussian]
    \param sourceCard :: Source system
   */
{
  ELog::RegMethod RegA("SourceCreate","createTS1GaussianSource");

  const double E=Control.EvalDefVar<double>("sdefEnergy",800.0);
  const double yStart=Control.EvalDefVar<double>("sdefYPos",-10.0);

  sourceCard.setActive();
  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Geometry::Vec3D(0,1,0));
  sourceCard.setComp("par",9);
  sourceCard.setComp("erg",800.0);
  //  sourceCard.setComp("ccc",76);
  sourceCard.setComp("y",-10.0);

  SrcData D1(1);
  SrcProb SP1(1);
  SP1.setFminus(-41,3.5322,0);  
  D1.addUnit(SP1);

  SrcData D2(2);
  D2.addUnit(SP1);
  sourceCard.setData("x",D1);
  sourceCard.setData("z",D2);

  return;
}

void
createTS2Source(Source& sourceCard)
  /*!
    Creates a target 2 proton source
    \param sourceCard :: Source system
   */
{
  ELog::RegMethod RegA("SourceCreate","createTS2Source");

  sourceCard.setActive();
  sourceCard.setComp("dir",1.0);
  sourceCard.setComp("vec",Geometry::Vec3D(0,0,-1));
  sourceCard.setComp("par",9);
  sourceCard.setComp("erg",800.0);
  //  sourceCard.setComp("ccc",76);
  sourceCard.setComp("z",5.0);

  SrcData D1(1);
  SrcProb SP1(1);
  SP1.setFminus(-41,1.3344,0);
  D1.addUnit(SP1);

  SrcData D2(2);
  D2.addUnit(SP1);
  sourceCard.setData("x",D1);
  sourceCard.setData("y",D2);

  return;
}

}  // NAMESPACE SDef
