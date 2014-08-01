/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weights/PointWeights.cxx
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
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>
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
#include "Surface.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "PointWeights.h"

namespace WeightSystem
{

void
setPointWeights(Simulation& System,
		const Geometry::Vec3D& AimPoint,
		const double biasPower)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
    \param AimPoint :: Point to centre around
    \param biasPower :: Bias value (1/r^power)
  */
{
  ELog::RegMethod RegA("F:PointWeights","setPointWeights");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  
  
  WM.addParticle<WeightSystem::WCells>('n');
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","WCell - WM");


  ELog::EM<<"Number of cells "<<WF->getEnergy().size()<<ELog::endDebug;
  const std::vector<double>& EngBin=WF->getEnergy();
  
  const Simulation::OTYPE& Cells=System.getCells();
  Simulation::OTYPE::const_iterator vc;

  // Distance from aim point to target
  const double Factor=1.0+AimPoint.abs();

  // TWO Loop: One to get normalization constant:
  double maxVal(0.0);
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      // Keep currently masked cells:
      if (!vc->second->isPlaceHold() && 
	  !WF->isMasked(vc->first) &&
	  vc->second->getImp())
        {
	  const double D=1.0+AimPoint.Distance(vc->second->getCofM());
	  const double Df=pow(2.0*D/(D+Factor),biasPower);
	  if (Df>maxVal)
	    maxVal=Df;
	}
    }
  ELog::EM<<"Max weight == "<<maxVal<<ELog::endTrace;

  // LOOP TWO:
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      // Keep currently masked cells:
      if (!vc->second->isPlaceHold() && 
	  !WF->isMasked(vc->first) &&
	  vc->second->getImp())
        {
	  const double D=1.0+AimPoint.Distance(vc->second->getCofM());
	  std::vector<double> Eval=WF->getWeights(vc->first);
	  const double Df=pow(2.0*D/(D+Factor),biasPower)/maxVal;

	  for(size_t i=0;i<Eval.size();i++)
	    Eval[i]*=Df;
	  
	  WF->setWeights(vc->first,Eval);
	}
      else if (!vc->second->getImp())
	WF->maskCell(vc->first);
    }
  // ALWAYS Mask outside
  WF->maskCell(1);
  return;
}

}   // NAMESPACE zoomSystem
