/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/ReportSelector.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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

#include <boost/format.hpp>

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
#include "support.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Rules.h"
#include "HeadRule.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinkSupport.h"
#include "inputParam.h"
#include "Simulation.h"
#include "masterWrite.h"
#include "ReportSelector.h" 


void
reportSelection(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    A system for reporting values 
    \param System :: Simulation to add tallies
    \param IParam :: InputParam from command line
  */
{
  ELog::RegMethod RegA("ReportSelector[F]","reportSelection");

  masterWrite& MW=masterWrite::Instance();
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const FuncDataBase& Control=System.getDataBase();

  const size_t nP=IParam.setCnt("report");
  //  boost::format FMTvec("%1$=%12.6g%2$=12.6g%3$=12.6g");
  for(size_t index=0;index<nP;index++)
    {
      std::vector<std::string> StrItem;
      const std::string key=
	IParam.getValue<std::string>("report",index,0);

      if(key=="help")
	{
	  ELog::EM<<"report Help "<<ELog::endBasic;
	  ELog::EM<<
	    " -- object [fixedName] linkPoint ::"
	    "    write out the fixedPoint and axis\n"
            " -- variable [variableName] ::"
            "  write out the value of the variable\n";
          ELog::EM<<ELog::endBasic;
	  ELog::EM<<ELog::endErr;
          return;
	}
      else if (key=="var" || key=="variable")
	{
          const std::string VObject=IParam.outputItem<std::string>
            ("report",index,1,"variable not given");
          const std::string varValue=(Control.hasVariable(VObject)) ?
            Control.EvalVar<std::string>(VObject) : "Not-Defined";
          ELog::EM<<"Var["<<VObject<<"] "<<varValue<<ELog::endDiag;
        }
      else if (key=="object")
	{
          boost::format FMTdouble("%12.6f");
          const std::string FObject=IParam.outputItem<std::string>
            ("report",index,1,"objectName not given");
          const std::string linkSide=
            IParam.outputItem<std::string>
            ("report",index,2,"front/back/side not given");
          const long int linkNumber=attachSystem::getLinkIndex(linkSide);
          const attachSystem::FixedComp* TPtr=
            OR.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
          
          const Geometry::Vec3D TPoint=TPtr->getLinkPt(linkNumber);
          const Geometry::Vec3D TAxis=TPtr->getLinkAxis(linkNumber);
          
          ELog::EM<<TPtr->getKeyName()<<"["<<linkNumber<<"] ";
          const size_t len=ELog::EM.Estream().str().size();
          if (len<20)
            ELog::EM<<std::string(20-len,' ');
          
          
          ELog::EM<<MW.padNum(TPoint,12)<<" :: "
                  <<MW.padNum(TAxis,12)<<ELog::endDiag;
	}
      else
        {
          ELog::EM<<"Report token: "<<key<<ELog::endWarn;
        }
    }
  return;
}




