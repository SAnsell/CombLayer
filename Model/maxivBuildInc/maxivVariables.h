/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuildInc/maxivVariables.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_maxivVariables_h
#define setVariable_maxivVariables_h

class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace setVariable
{
  void MaxIVVariables(FuncDataBase&,
		      const std::string&,
		      const std::set<std::string>&);

  void maxivInstrumentVariables
    (const std::set<std::string>&,
     const std::string&,
     FuncDataBase&);
    
  // MAXIV Beamlines 1.5GeV Ring
  void R1RingVariables(FuncDataBase&);
  void FLEXPESvariables(FuncDataBase&);
  void MAXPEEMvariables(FuncDataBase&);
  void SPECIESvariables(FuncDataBase&);

  // MAXIV Beamlines 3.0GeV Ring
  void R3RingVariables(FuncDataBase&);
  void BALDERvariables(FuncDataBase&);
  void COSAXSvariables(FuncDataBase&);
  void SOFTIMAXvariables(FuncDataBase&);
  void DANMAXvariables(FuncDataBase&);
  void FORMAXvariables(FuncDataBase&);
  void MICROMAXvariables(FuncDataBase&);

  void R3FrontEndVariables(FuncDataBase&,const std::string&);
  void R1FrontEndVariables(FuncDataBase&,const std::string&,
			   const double);

  void LINACvariables(FuncDataBase&);
  void LINACmagnetVariables(FuncDataBase&,const std::string&);
}


#endif
