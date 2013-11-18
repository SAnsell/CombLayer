/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   special/divideManager.cxx
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "KGroup.h"
#include "Source.h"
#include "SurInter.h"
#include "Simulation.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcollect.h"

#include "divideManager.h"

namespace supportSystem
{

divideManager::divideManager()
  /*!
    Constructor
   */
{}

void
divideManager::createAll(const std::string& FName,Simulation& System)
  /*!
    Process an XML file and modify lots of cells
    \param FName :: Files name [xml file]
    \param System :: Simulation to change
   */
{
  ELog::RegMethod RegA("divideManager","createAll");
  const FuncDataBase& Control=System.getDataBase();
  
  if (FName.empty())  return;
  XML::XMLcollect CO;
  if (CO.loadXML(FName))
    ELog::EM<<"Failed to load  == "<<FName<<ELog::endErr;

  // Parse for variables:
  XML::XMLgroup* GR=CO.findGroup("DivideCell");
  std::ofstream cx("testOut.xml");
  CO.writeXML(cx);
  cx.close();
  if (GR)
    {
      // ProcessCells:
      XML::XMLgroup* CellGrp=GR->findGroup("cell");
      ELog::EM<<"Found Divide Cell"<<ELog::endDebug;

      std::vector<double> Frac;
      std::vector<int> Mat;
      while(CellGrp)
        {
	  const int cellN=CellGrp->getItem<int>("name");
	  const int offset=CellGrp->getItem<int>("offset");
	  const std::string keyName=CellGrp->getItem<std::string>("keyname");
	  const size_t NL=
	    Control.EvalVar<size_t>(keyName+"NDivide");
	  ModelSupport::populateDivide(Control,NL,
				       keyName+"Mat_",0,Mat);
	  ModelSupport::populateDivide(Control,NL,
				       keyName+"Frac_",Frac);


	  ModelSupport::surfDivide SDobject;
	  SDobject.setCellN(cellN);	  
	  SDobject.setOutNum(cellN+501,offset+600);
	  // Find pairs
	  XML::XMLobject* OP=CellGrp->findObj("pair");
	  int apt,bpt;
	  while(OP)
	    {
	      std::string Xpair=OP->getItem<std::string>();

	      if (StrFunc::section(Xpair,apt) && 
		  StrFunc::section(Xpair,bpt) )
		{
		  apt+=offset;
		  bpt+=offset;
		  SDobject.makePair<Geometry::Plane>(apt,bpt);
		}
	      CO.deleteObj(OP);
	      OP=CellGrp->findObj("pair");
	    }

	  OP=CellGrp->findObj("inner");
	  while(OP)
	    {
	      apt=OP->getItem<int>();
	      apt+=offset;
	      SDobject.addInnerSingle(apt);
	      CO.deleteObj(OP);
	      OP=CellGrp->findObj("inner");
	    }

	  SDobject.activeDivide(System);
	  CO.deleteObj(CellGrp);
	  CellGrp=GR->findGroup("cell");
	}
    }
  return;
}


  
}  // NAMESPACE supportSystem
