/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedRotateGroup.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "inputSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"

namespace attachSystem
{

void
rotate::copy(double& PreXA,double& PreYA,double& PreZA,
             double& XS,double& YS,double& ZS,
             double& XA,double& YA,double& ZA) const
  /*!
    Copy data to external variables
    \param PreXA :: preXAngle value
    \param PreYA :: preYAngle value
    \param PreZA :: preZAngle value
    \param XS :: XStep value
    \param YS :: YStep value    
    \param ZS :: ZStep value
    \param XA :: XAngle value
    \param YA :: XAngle value
    \param ZA :: ZAngle value
  */
{
  PreXA=preXAngle;
  PreYA=preYAngle; 
  PreZA=preZAngle;   
  XS=xStep;
  YS=yStep;
  ZS=zStep;
  XA=xAngle;
  YA=yAngle;
  ZA=zAngle;
  return;
}
  
FixedRotateGroup::FixedRotateGroup(const std::string& mainKey,
				   const std::string& KN,
				   const size_t NL) :
  FixedGroup(mainKey,KN,NL),
  preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xAngle(0.0),yAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param mainKey :: BaseName 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

FixedRotateGroup::FixedRotateGroup(const std::string& mainKey,
				   const std::string& AKey,
				   const size_t ANL,
				   const std::string& BKey,
				   const size_t BNL) :
  FixedGroup(mainKey,AKey,ANL,BKey,BNL),
  preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xAngle(0.0),yAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param mainKey :: KeyName [main system]
    \param AKey :: Key unit
    \param ANL :: A Number of links
    \param BKey :: Key unit
    \param BNL :: B Number of links
  */
{}

FixedRotateGroup::FixedRotateGroup(const std::string& mainKey,
                                   const std::string& AKey,
                                   const size_t ANL,
                                   const std::string& BKey,
                                   const size_t BNL,
                                   const std::string& CKey,
                                   const size_t CNL) :
  FixedGroup(mainKey,AKey,ANL,BKey,BNL,CKey,CNL),
  preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xAngle(0.0),yAngle(0.0),zAngle(0.0)

 /*!
    Constructor 
    \param mainKey :: KeyName [main system]
    \param AKey :: Key unit
    \param ANL :: A Number of links
    \param BKey :: Key unit
    \param BNL :: B Number of links
    \param CKey :: Key unit
    \param CNL :: B Number of links
  */
{}

FixedRotateGroup::FixedRotateGroup(const std::string& mainKey,
                                   const std::string& AKey,
                                   const size_t ANL,
                                   const std::string& BKey,
                                   const size_t BNL,
                                   const std::string& CKey,
                                   const size_t CNL,
                                   const std::string& DKey,
                                   const size_t DNL) :
  FixedGroup(mainKey,AKey,ANL,BKey,BNL,CKey,CNL,DKey,DNL),
  preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xAngle(0.0),yAngle(0.0),zAngle(0.0)
  /*!
    Constructor 
    \param mainKey :: KeyName [main system]
    \param AKey :: Key unit
    \param ANL :: A Number of links
    \param BKey :: Key unit
    \param BNL :: B Number of links
    \param CKey :: Key unit
    \param CNL :: C Number of links
    \param DKey :: Key unit
    \param DNL :: D Number of links
  */
{}


FixedRotateGroup::FixedRotateGroup(const FixedRotateGroup& A) : 
  FixedGroup(A),
  preXAngle(A.preXAngle),preYAngle(A.preYAngle),preZAngle(A.preZAngle),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xAngle(A.xAngle),yAngle(A.yAngle),zAngle(A.zAngle),
  GOffset(A.GOffset)
  /*!
    Copy constructor
    \param A :: FixedRotateGroup to copy
  */
{}

FixedRotateGroup&
FixedRotateGroup::operator=(const FixedRotateGroup& A)
  /*!
    Assignment operator
    \param A :: FixedRotateGroup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedGroup::operator=(A);
      preXAngle=A.preXAngle;
      preYAngle=A.preYAngle;
      preZAngle=A.preZAngle;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xAngle=A.xAngle;
      yAngle=A.yAngle;
      zAngle=A.zAngle;
      GOffset=A.GOffset;
    }
  return *this;
}

void
FixedRotateGroup::populateRotate(const FuncDataBase& Control,
                                 const std::string& keyItem,
                                 rotate& GO)
  /*!
    Populate the variables
    \param Control :: Control data base
    \param keyItem :: name of unit
    \param GO :: offset parameter
   */
{
  ELog::RegMethod RegA("FixedRotateGroup","populateRotate");

  GO.preZAngle=Control.EvalDefVar<double>(keyItem+"PreXYAngle",GO.preZAngle);
  GO.preXAngle=Control.EvalDefVar<double>(keyItem+"PreXAngle",GO.preXAngle);
  GO.preYAngle=Control.EvalDefVar<double>(keyItem+"PreYAngle",GO.preYAngle);
  GO.preZAngle=Control.EvalDefVar<double>(keyItem+"PreZAngle",GO.preZAngle);
  
  GO.xStep=Control.EvalDefVar<double>(keyItem+"XStep",GO.xStep);
  GO.yStep=Control.EvalDefVar<double>(keyItem+"YStep",GO.yStep);
  GO.zStep=Control.EvalDefVar<double>(keyItem+"ZStep",GO.zStep);
  
  GO.zAngle=Control.EvalDefVar<double>(keyItem+"XYAngle",GO.zAngle);
  GO.xAngle=Control.EvalDefVar<double>(keyItem+"XAngle",GO.xAngle);
  GO.yAngle=Control.EvalDefVar<double>(keyItem+"YAngle",GO.yAngle);
  GO.zAngle=Control.EvalDefVar<double>(keyItem+"ZAngle",GO.zAngle);
  
  return;
}

void
FixedRotateGroup::populateRotate
(const std::map<std::string,std::vector<std::string>>& inputMap,
 const std::string& keyItem,
 rotate& GO)
  /*!
    Populate the variables
    \param Control :: Control data base
    \param keyItem :: name of unit
    \param GO :: offset parameter
   */
{
  ELog::RegMethod RegA("FixedRotateGroup","populateRotate(map)");

  GO.preXAngle=mainSystem::findInput(inputMap,keyItem+"PreXAngle",0,GO.preXAngle);
  GO.preYAngle=mainSystem::findInput(inputMap,keyItem+"PreYAngle",0,GO.preYAngle);
  GO.preZAngle=mainSystem::findInput(inputMap,keyItem+"PreXYAngle",0,GO.preZAngle);
  GO.preZAngle=mainSystem::findInput(inputMap,keyItem+"PreZAngle",0,GO.preZAngle);
  GO.xStep=mainSystem::findInput(inputMap,keyItem+"XStep",0,GO.xStep);
  GO.yStep=mainSystem::findInput(inputMap,keyItem+"YStep",0,GO.yStep);
  GO.zStep=mainSystem::findInput(inputMap,keyItem+"ZStep",0,GO.zStep);
  GO.zAngle=mainSystem::findInput(inputMap,keyItem+"XYAngle",0,GO.zAngle);
  GO.xAngle=mainSystem::findInput(inputMap,keyItem+"XAngle",0,GO.xAngle);
  GO.yAngle=mainSystem::findInput(inputMap,keyItem+"YAngle",0,GO.yAngle);
  GO.zAngle=mainSystem::findInput(inputMap,keyItem+"ZAngle",0,GO.zAngle);
  
  return;
}


void
FixedRotateGroup::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    The structure sets global variables based on 
    keyName and FixedGroup based on keyaName+GroupKey
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedRotateGroup","populate");


    
  for(FTYPE::value_type& FCmc : FMap)
    {
      rotate GO;
      populateRotate(Control,FCmc.second->getKeyName(),GO);
      GOffset.emplace(FCmc.first,GO);
    }
  
 
  preXAngle=Control.EvalDefVar<double>(keyName+"PreXAngle",preXAngle);
  preYAngle=Control.EvalDefVar<double>(keyName+"PreYAngle",preYAngle);
  preZAngle=Control.EvalDefVar<double>(keyName+"PreXYAngle",preZAngle);
  preZAngle=Control.EvalDefVar<double>(keyName+"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefVar<Geometry::Vec3D>
    (keyName+"Offset",Geometry::Vec3D(xStep,yStep,zStep));
    
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefVar<double>(keyName+"XStep",xStep);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",yStep);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",zStep);

  xAngle=Control.EvalDefVar<double>(keyName+"XAngle",xAngle);
  yAngle=Control.EvalDefVar<double>(keyName+"YAngle",yAngle);
  zAngle=Control.EvalDefVar<double>(keyName+"XYAngle",zAngle);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",zAngle);
  
  return;
}

void
FixedRotateGroup::populate(const std::string& baseName,
			   const FuncDataBase& Control)
  /*!
    Populate the variables
    The structure sets global variables based on 
    keyName and FixedGroup based on keyaName+GroupKey
    \param baseName :: Second default basename 
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedRotateGroup","populate");

  for(FTYPE::value_type& FCmc : FMap)
    {
      rotate GO;
      populateRotate(Control,FCmc.second->getKeyName(),GO);
      GOffset.emplace(FCmc.first,GO);
    }

  preXAngle=Control.EvalDefTail<double>(keyName,baseName,"PreXAngle",preXAngle);
  preYAngle=Control.EvalDefTail<double>(keyName,baseName,"PreYAngle",preYAngle);
  preZAngle=Control.EvalDefTail<double>(keyName,baseName,"PreXYAngle",preZAngle);
  preZAngle=Control.EvalDefTail<double>(keyName,baseName,"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefTail<Geometry::Vec3D>
    (keyName,baseName,"Offset",Geometry::Vec3D(0,0,0));
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefTail<double>(keyName,baseName,"XStep",xStep);
  yStep=Control.EvalDefTail<double>(keyName,baseName,"YStep",yStep);
  zStep=Control.EvalDefTail<double>(keyName,baseName,"ZStep",zStep);
  
  xAngle=Control.EvalDefTail<double>(keyName,baseName,"XAngle",xAngle);
  yAngle=Control.EvalDefTail<double>(keyName,baseName,"YAngle",yAngle);
  zAngle=Control.EvalDefTail<double>(keyName,baseName,"XYAngle",zAngle);
  zAngle=Control.EvalDefTail<double>(keyName,baseName,"ZAngle",zAngle);
  
  return;
}

void
FixedRotateGroup::populate(const std::map<std::string,
			   std::vector<std::string>>& inputMap)
  /*!
    Populate the variables
    \param inputMap :: Moifications from input stream
   */
{
  ELog::RegMethod RegA("FixedRotate","populate(map)");

  for(FTYPE::value_type& FCmc : FMap)
    {
      rotate GO;
      populateRotate(inputMap,FCmc.second->getKeyName(),GO);
      GOffset.emplace(FCmc.first,GO);
    }

  mainSystem::findInput(inputMap,"preXAngle",0,preXAngle);
  mainSystem::findInput(inputMap,"preYAngle",0,preYAngle);
  mainSystem::findInput(inputMap,"preZAngle",0,preZAngle);
  mainSystem::findInput(inputMap,"xAngle",0,xAngle);
  mainSystem::findInput(inputMap,"yAngle",0,yAngle);
  mainSystem::findInput(inputMap,"zAngle",0,zAngle);
  mainSystem::findInput(inputMap,"xStep",0,xStep);
  mainSystem::findInput(inputMap,"yStep",0,yStep);
  mainSystem::findInput(inputMap,"zStep",0,zStep);

  return;
}

void
FixedRotateGroup::applyOffset()
  /*!
    Apply the rotation/step offset
    Note the rotation/step is apply ON TOP of the full R/Step combination
  */
{
  ELog::RegMethod RegA("FixedRotateGroup","applyOffset");

  for(FTYPE::value_type& FCmc : FMap)
    {
      std::map<std::string,rotate>::const_iterator mc=GOffset.find(FCmc.first);
      if (mc==GOffset.end())
        throw ColErr::InContainerError<std::string>
          (FCmc.first,"Rotate not found");
      
      const rotate& GO=mc->second;
      FCmc.second->applyAngleRotate(preXAngle+GO.preXAngle,
				    preYAngle+GO.preYAngle,
                                    preZAngle+GO.preZAngle);

      FCmc.second->applyShift(xStep+GO.xStep,yStep+GO.yStep,zStep+GO.zStep);

      FCmc.second->applyAngleRotate(xAngle+GO.xAngle,
				    yAngle+GO.yAngle,
				    zAngle+GO.zAngle);

      FCmc.second->reOrientate();
    }
  return;
}

void
FixedRotateGroup::createUnitVector(const attachSystem::FixedComp& FC,
				   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: signed linkpt			
  */
{
  ELog::RegMethod RegA("FixedOffset","createUnitVector");

  FixedGroup::createUnitVector(FC,sideIndex);
  applyOffset();
    
  return;
}
 
}  // NAMESPACE attachSystem
