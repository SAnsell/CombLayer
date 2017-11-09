/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightControlHelp.cxx
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cone.h"

#include "WeightControl.h"

namespace WeightSystem
{

  

void
WeightControl::procCalcHelp() 
  /*!
    Write the Calc help
  */
{
  ELog::EM<<"weightCalc ::: \n"
    " TSItem energyCut densityScalar minWeight \n"
    " -- TSItem :: String  [TS](index)P{value} \n"
    "       --- T/S :: tally/source to use \n"
    "       --- index :: index [from zero] of source/tally point\n"
    "       --- P{value} :: Optional value for the power [default 0.5]"
	  <<ELog::endDiag;
  return;
}
    

void
WeightControl::help() 
  /*!
    Write out the help
  */
{
  ELog::RegMethod RegA("WeightControl","help");
  ELog::EM<<"Weight help :: "<<ELog::endDiag;

  ELog::EM<<"-- WeightEnergyType -- ::"<<ELog::endDiag;
  procEnergyTypeHelp();
  ELog::EM<<"-- weightSource --::"<<ELog::endDiag;
  ELog::EM<<"-- weightPlane --::"<<ELog::endDiag;
  ELog::EM<<"-- weightTally --::"<<ELog::endDiag;
  ELog::EM<<"-- weightObject --::"<<ELog::endDiag;
  procObjectHelp();
  ELog::EM<<"-- weightRebase --::"<<ELog::endDiag;
  procRebaseHelp();
  ELog::EM<<"-- wWWG --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgNorm -- minWeight power :: 10^-minWeight and w=w^power "<<ELog::endDiag;
  ELog::EM<<"-- wwgCalc --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgMarkov --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgRPtMesh -- set hte reference point for the mesh ::"<<ELog::endDiag;
  ELog::EM<<"-- wwgVTK --::"<<ELog::endDiag;
  procCalcHelp();

  ELog::EM<<"-- wFCL --:: Set forced collision"<<ELog::endDiag;
  ELog::EM<<"-- wIMP --:: Set Importance"<<ELog::endDiag;

  ELog::EM<<"-- weightTemp --::"<<ELog::endDiag;
  ELog::EM<<"-- tallyWeight --::"<<ELog::endDiag;
  return;
}
  
void
WeightControl::procEnergyTypeHelp() 
  /*!
    Write the Type help
  */
{
  ELog::EM<<"weightEnergyType ::: \n"
    "high : Set High energy default band\n"
    "mid : Set mid energy default band\n"
    "low : Set low energy default band\n"
    "energy [E1 W1 E2 W2 ...] : Set energy bands"<<
    ELog::endDiag;
  return;
}

void
WeightControl::procObjectHelp()
  /*!
    Write the Rebase help
  */
{
  ELog::EM<<"weightObject ::: \n"
    "objectName:{cell name} TSItem energyCut densityScalar minWeight \n"
    " -- TSItem :: String  [TS](index)P{value} \n"
    "       --- T/S :: tally/source to use \n"
    "       --- index :: index [from zero] of source/tally point\n"
    "       --- P{value} :: Optional value for the power [default 0.5]"
	  <<ELog::endDiag;
  return;
}

void
WeightControl::procRebaseHelp()
  /*!
    Write the Rebase help
  */
{
  ELog::EM<<"weightRebase ::: \n"
    "cell cellNumber {index} {eCut} : scale weightObject  \n"
    "                                : cells by WWN[cell/index] to \n"
    "                                : give value in cell.\n"
    "object Name {index} {eCut} : scale object cells by WWN [cell/index] to \n"
    "                                : give value in cell."
	  <<ELog::endDiag;
  return;
}
  
}  // NAMESPACE weightSystem

