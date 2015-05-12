/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ExtConstructor.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Simulation.h"
#include "inputParam.h"

#include "ExtConstructor.h" 


namespace physicsSystem
{

ExtConstructor::ExtConstructor() 
  /// Constructor
{}

ExtConstructor::ExtConstructor(const ExtConstructor&) 
  /// Copy Constructor
{}

ExtConstructor&
ExtConstructor::operator=(const ExtConstructor&) 
  /// Assignment operator
{
  return *this;
}

void
ExtConstructor::procZone(std::vector<std::string>& StrItem)
  /*!
    Process the zone information
    \param StrItem :: List of item from the input 
  */
{
  ELog::RegMethod RegA("ExtConstuctor","procZone");

  const size_t NS
  if (StrItem[0]=="all")
    Zone.push_back(Range<int>(0,100000000));
  else if (StrItem[0]=="object" && StrItem.size()>1)
    {
      const ModelSupport::objectRegister& OR= 
	ModelSupport::objectRegister::Instance();
      const int cellN=OR.getCell(StrItem[1]);
      const int rangeN=OR.getRange(StrItem[1]);
      if (cellN==0)
	throw ColErr:InContainerError<std::string>(StrItem[1],"Object name");
      Zone.push_back(Range<int>(cellN,rangeN));
    }
  return;
}
  
void
ExtConstructor::processUnit(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
/*!
    Add ext component 
    \param System :: Simulation to get physics/fixed points
    \param IParam :: Main input parameters
    \param Index :: index of the -wExt card
   */
{
  ELog::RegMethod RegA("ExtConstructor","processPoint");

  const size_t NParam=IParam.itemCnt("wExt",Index);
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wExt");
  std::vector<std::string> StrItem;
  
  // Get all values:
  for(size_t j=0;j<NParam;j++)
    StrItem.push_back
      (IParam.getCompValue<std::string>("wExt",Index,j));

  if (StrItem[0]=="help" || StrItem[0]=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  procZone(StrItem);

  /*  
  const std::string PType(IParam.getCompValue<std::string>("tally",Index,1)); 
  
  const masterRotate& MR=masterRotate::Instance();
  std::string revStr;

  if (PType=="free")
    {
      std::vector<Geometry::Vec3D> EmptyVec;
      Geometry::Vec3D PPoint=
	inputItem<Geometry::Vec3D>(IParam,Index,2,"Point for point detector");

      const int flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	PPoint=MR.reverseRotate(PPoint);
      processPointFree(System,PPoint,EmptyVec);
    }

  else if (PType=="freeWindow")
    {
      size_t windowIndex(6);
      Geometry::Vec3D PPoint=
	inputItem<Geometry::Vec3D>(IParam,Index,2,"Point for point detector");
      int flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	{
	  PPoint=MR.reverseRotate(PPoint);
	  windowIndex--;
	}
      
      std::vector<Geometry::Vec3D> WindowPts(4);
      for(size_t i=0;i<4;windowIndex+=3,i++)
	WindowPts[i]=
	  inputItem<Geometry::Vec3D>(IParam,Index,windowIndex,"Window point");
      
      flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	PPoint=MR.reverseRotate(PPoint);
      
      processPointFree(System,PPoint,WindowPts);
    }

  else if (PType=="window")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");
      const double D=
	inputItem<double>(IParam,Index,4,"Distance not given");

      double timeStep(0.0);
      double windowOffset(0.0);

      checkItem<double>(IParam,Index,5,timeStep);
      checkItem<double>(IParam,Index,6,windowOffset);
      const long int linkNumber=getLinkIndex(snd);
      processPointWindow(System,place,linkNumber,D,timeStep,windowOffset);
    }

  else if (PType=="object")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");
      const double D=
	inputItem<double>(IParam,Index,4,"Distance not given");
      const long int linkNumber=getLinkIndex(snd);
      ELog::EM<<"LN == "<<linkNumber<<ELog::endDiag;
	    
      processPointFree(System,place,linkNumber,D);
    }

  else
    {
      ELog::EM<<"Point TallyType "<<PType<<" ignored"<<ELog::endWarn;
    }
  */  

  return;
}

void
ExtConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{

    OX<<"-wExt ZONE : TYPE \n"
      "ZONE :: \n"
      "   All / objectName / CellNumber(s) \n"
      "TYPE :: \n"
      "   simple :: implicit capture \n"
      "   simpleVec [Vec3D] :: implicit capture along a direction\n"
      "   simpleVec Object linkPt :: implicit capture along a direction\n"
      "   scale :: [frac] implicit capture \n"
      "   scaleVec :: [frac] [Vec3D] :: implicit capture along a direction\n"
      "   scaleVec :: [frac] Object linkPt :: "
      " implicit capture along a direction\n";
  return;
}


}  // NAMESPACE physicsSystem
