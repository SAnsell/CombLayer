/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/CopiedComp.cxx
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "stringSearch.h"
#include "CopiedComp.h"

namespace attachSystem
{

CopiedComp::CopiedComp()
 /*!
    Constructor 
    Do nothing
  */
{}

CopiedComp::CopiedComp(std::string  baseKey,
		       std::string  newKey) :
  baseName(std::move(baseKey)),newName(std::move(newKey))
 /*!
    Constructor to set base/new name
    \param baseKey :: base key name
    \param newKey :: new keyname
  */
{}


CopiedComp::CopiedComp(const CopiedComp& A) :
  baseName(A.baseName),newName(A.newName)
 /*!
    Constructor to set base/new name
    \param A :: object to copy
  */
{}

CopiedComp&
CopiedComp::operator=(const CopiedComp& A)
 /*!
   Assignment operator
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      baseName=A.baseName;
      newName=A.newName;
    }
  return *this;
}

void
CopiedComp::process(FuncDataBase& Control) const
  /*!
    Modify the control base to keep a copy
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod regA("CopiedComp","process");

  const std::vector<std::string> keyVec=Control.getKeys();

  std::vector<std::string>::const_iterator mc=
    std::lower_bound(keyVec.begin(),keyVec.end(),baseName,
  		     StrFunc::stringSearch());

  StrFunc::stringSearch SItem;
  while(mc!=keyVec.end() && !SItem(baseName,*mc))
    {
      const std::string mainPart=newName+
	mc->substr(baseName.size());
      if (!Control.hasVariable(mainPart))
	Control.copyVar(mainPart,*mc);
      mc++;
    }
  return;
}

 
}  // NAMESPACE attachSystem
