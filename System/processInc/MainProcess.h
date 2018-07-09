/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/MainProcess.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef mainSystem_MainProcess_h
#define mainSystem_MainProcess_h

class Simulation;
class SimPHITS;
class SimFLUKA;
class SimPOVRay;
class SimMCNP;
class FuncDataBase;

/*!
  \namespace mainSystem
  \brief Adds a layer for main processing
  \author S. Ansell
  \version 1.0
  \date July 2009
*/

namespace mainSystem
{
  class inputParam;

  void activateLogging(ELog::RegMethod&);

  void getVariables(std::vector<std::string>&,
		    std::map<std::string,std::string>&,
		    std::map<std::string,std::string>&,
		    std::map<std::string,double>&);

  void setRunTimeVariable(FuncDataBase&,
			  const std::map<std::string,std::string>&,
			  const std::map<std::string,std::string>&);

  void incRunTimeVariable(FuncDataBase&,
			  const std::map<std::string,double>&);

  void renumberCells(Simulation&,const inputParam&);

  void setVariables(Simulation&,const inputParam&,std::vector<std::string>&);
  void setMaterialsDataBase(const inputParam&);

  int extractName(std::vector<std::string>&,std::string&);

  Simulation* createSimulation(inputParam&,std::vector<std::string>&,
			       std::string&);

  void TS2InputModifications(Simulation*,inputParam&,std::vector<std::string>&);
  void InputModifications(Simulation*,inputParam&,
			  std::vector<std::string>&);

  void buildFullSimFLUKA(SimPHITS*,const inputParam&,const std::string&);
  void buildFullSimMCNP(SimMCNP*,const inputParam&,const std::string&);
  void buildFullSimPHITS(SimPHITS*,const inputParam&,const std::string&);
  void buildFullSimPOVRay(SimPOVRay*,const inputParam&,const std::string&);
  void buildFullSimulation(Simulation*,const inputParam&,const std::string&);
  void exitDelete(Simulation*);
}


#endif
 
