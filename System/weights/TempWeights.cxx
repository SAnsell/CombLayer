/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/TempWeights.cxx
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
#include "TempWeights.h"

namespace WeightSystem
{

void
scaleTempWeights(Simulation& System,double factor)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
    \param factor :: Weighting scale
  */
{
  ELog::RegMethod RegA("F:TempWeights","scaleTempWeights");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  
  
  WM.addParticle<WeightSystem::WCells>("n");
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle("n"));
  if (!WF)
    {
      ELog::EM<<"Temp weight applied to non cell neutron particles"
	      <<ELog::endWarn;
      ELog::EM<<"NO WEIGHTS APPLIED"<<ELog::endWarn;
      return;
    }

  
  const Simulation::OTYPE& Cells=System.getCells();
  Simulation::OTYPE::const_iterator vc;
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      // Keep currently masked cells:
      if (!vc->second->isPlaceHold() && 
	  !WF->isMasked(vc->first) &&
	  vc->second->getImp())
        {
	  const double T=vc->second->getTemp();
	  if (T<275.0)
	    {
	      const double Df=1.0/(factor*(1.0-T/275));
	      std::vector<double> Eval=WF->getWeights(vc->first);
	      for(size_t i=0;i<Eval.size();i++)
		Eval[i]*=Df;
	      WF->setWeights(vc->first,Eval);
	    }
	}
    }
  // ALWAYS Mask outside
  WF->maskCell(1);
  return;
}

}   // NAMESPACE WeightSystem
