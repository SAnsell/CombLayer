/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/beamBlock.cxx
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
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MaterialSupport.h"
#include "beamBlock.h"

namespace hutchSystem
{

/// Number of active names
const size_t beamBlock::Size(3);

void
beamBlock::setVar(const FuncDataBase& Control,
		     const size_t Item,
		     const std::string& VarStr)
/*!
  Given a value set the item
  \param Control :: Function Data Base
  \param Item :: Index value to variable
  \param VarStr :: Name of variable in the system
*/
{
  switch(Item)
    {
    case 0:
      thickness=Control.EvalVar<double>(VarStr);
      return;
    case 1:
      angle=Control.EvalVar<double>(VarStr);
      return;
    case 2:
      matN=ModelSupport::EvalMat<int>(Control,VarStr);
      return;
    }
  return;
}

int
beamBlock::setFromControl(const FuncDataBase& Control,
			  const std::string& primName,
			  const size_t Index,
			  const size_t Item)
  /*!
    Set the value in the structure from control
    \param Control :: Data Base to get variables from
    \param primName :: first primary name
    \param Index :: Block number
    \param Item :: Key variable
    \return variable exists
  */
{
  ELog::RegMethod RegA("beamBlock","setFromControl");
  static const char* sndKey[Size]={"Thick","Angle","Mat"};

  if (Item>=beamBlock::Size)
    throw ColErr::IndexError<size_t>(Item,beamBlock::Size,
				     "beamBlock::size/Item");

  const std::string AKey=
    StrFunc::makeString(primName+sndKey[Item],Index+1);
  const std::string BKey=
    StrFunc::makeString(primName+sndKey[Item]);

  if (Control.hasVariable(AKey))
    setVar(Control,Item,AKey);
  else if (Control.hasVariable(BKey))
    setVar(Control,Item,BKey);
  else 
    return 0;

  return 1;
}


} // NAMESPACE hutchSystem
