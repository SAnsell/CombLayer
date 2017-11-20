/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/objectSupport.cxx
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
#include "support.h"
#include "stringCombine.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Object.h"
#include "Qhull.h"
#include "inputParam.h"
#include "objectSupport.h" 


namespace objectSupport
{

template<>
void addItem(std::set<int>& V,const int I)
  /*!
    Add item to a vector
    \param V :: Vector to add 						
    \param I :: Item
   */
{
  V.insert(I);
  return;
}

template<>
void addItem(std::vector<int>& V,const int I)
  /*!
    Add item to a vector
    \param V :: Vector to add 						
    \param I :: Item
   */
{
  V.push_back(I);
  return;
}
 
template<typename T>
T
cellSelection(const Simulation& System,
	      const std::string& matName,
	      const T& inputGrp)
  /*!
    Select cells and return set/vector
    \param System :: simualtion to use
    \param matName :: Material Zaid/Name/All/AllNonVoid
    \param inputGrp :: Inptu group
   */
{
  ELog::RegMethod RegA("objectSupport[F]","cellSelection<T>");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();
  
  if (matName=="All" || matName=="all")
    return inputGrp;

  T cellOut;
  
  int matFlag(0);
  int matNumber(0);

  if (matName=="AllNonVoid" || matName=="allNonVoid")
    matFlag= -1;
  else if(DB.hasKey(matName))
    {
      matFlag=1;
      matNumber=DB.getIndex(matName);
    }
  else // zaid
    {
      if (!StrFunc::convert(matName,matNumber))
	throw ColErr::InContainerError<std::string>(matName,"matName");
    }
	  

  for(const int CN : inputGrp)
    {
      const MonteCarlo::Object* OPtr=System.findQhull(CN);
      if (OPtr)
	{
	  const int matN=OPtr->getMat();
	  const MonteCarlo::Material& cellMat=DB.getMaterial(matN);
	  switch (matFlag)
	    {
	    case -1:       // allnonvoid
	      if (matN) addItem(cellOut,CN);
	      break;
	    case 0:        // ?
	      if (cellMat.hasZaid(static_cast<size_t>(matNumber),0,0))
		addItem(cellOut,CN);
	      break;
	    case 1:        // named material
	      if (matN==matNumber) addItem(cellOut,CN);
	      break;
	    }
	}
    }
  return cellOut;

}

std::vector<int>
getCellSelection(const Simulation& System,
		 const int matN,
		 const std::string& keyName) 
  /*!
    Extract all the cells with a material based on matN and keyName
    \param System :: Simulation for build [needed for nonVoidcells ] 
    \param matN :: Material number
           -1 : all materials 
	   -2 : all non zero materials
	   > 1000 : materials containing zaid
    \param keyName :: keyName
    \return vector of cell indexes
   */
{
  ELog::RegMethod RegA("objectSupport[F]","getCellSelection");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::vector<int> cells;
  // NOTE that getting all the cells from OR is insane
  if (keyName=="allNonVoid" || keyName=="AllNonVoid")
    cells=System.getNonVoidCellVector();
  else if (keyName=="All" || keyName=="all")
    cells=System.getCellVector();
  else
    cells=OR.getObjectRange(keyName);

  // PROCESS mat:
  std::vector<int> Out;
  std::vector<int> matCell;
  
  if (matN>1000)
    matCell=System.getCellWithZaid(static_cast<size_t>(matN));
  else  
    matCell=System.getCellWithMaterial(matN);

  if (!cells.size())
    throw ColErr::InContainerError<std::string>
      (keyName,"cell emepty for mat:"+StrFunc::makeString(matN));

  std::set_intersection(cells.begin(),cells.end(),
                        matCell.begin(),matCell.end(),
                        std::back_inserter(Out));
  return Out;
}

template std::vector<int>
cellSelection(const Simulation&,const std::string&,const std::vector<int>&);

}  // NAMESPACE objectSupport
