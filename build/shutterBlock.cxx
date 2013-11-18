/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/shutterBlock.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "shutterBlock.h"

namespace shutterSystem
{

const int shutterBlock::Size(7);

void
shutterBlock::setVar(const FuncDataBase& Control,
		     const int Item,
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
      flag=Control.EvalVar<int>(VarStr);
      return;
    case 1:
      centY=Control.EvalVar<double>(VarStr);
      return;
    case 2:
      length=Control.EvalVar<double>(VarStr);
      return;
    case 3:
      height=Control.EvalVar<double>(VarStr);
      return;
    case 4:
      edgeVGap=Control.EvalVar<double>(VarStr);
      return;
    case 5:
      edgeHGap=Control.EvalVar<double>(VarStr);
      return;
    case 6:
      matN=Control.EvalVar<int>(VarStr);
      return;
    }
  return;
}

int
shutterBlock::setFromControl(const FuncDataBase& Control,
			     const std::string& primName,
			     const int shutterID,
			     const int Index,
			     const int Item)
  /*!
    Set the value in the structure from control
    \param Control :: Data Base to get variables from
    \param primName :: first primary name
    \param shutterID :: shutter number [+1]
    \param Index :: Block number
    \param Item :: Key variable
    \return variable exists
   */
{
  ELog::RegMethod RegA("shutterBlock","setFromControl");

  static const char* sndKey[Size]=
    {"Flag","Cent","Len","Height","VGap","HGap","Mat"};

  if (Item<0 || Item>shutterBlock::Size)
    {
      ELog::EM<<"Item out of range "<<ELog::endErr;
      return 0;
    }
  std::ostringstream cx,dx,ex,fx;

  cx<<primName<<shutterID<<sndKey[Item]<<Index+1;
  dx<<primName<<shutterID<<sndKey[Item];
  ex<<primName<<sndKey[Item]<<Index+1;
  fx<<primName<<sndKey[Item];
  if (Control.hasVariable(cx.str()))
    setVar(Control,Item,cx.str());
  else if (Control.hasVariable(dx.str()))
    setVar(Control,Item,dx.str());
  else if (Control.hasVariable(ex.str()))
    setVar(Control,Item,ex.str());
  else if (Control.hasVariable(fx.str()))
    setVar(Control,Item,fx.str());
  else 
    return 0;
  
  return 1;
}


} // NAMESPACE shutterSystem
