/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SourceCreate.h
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
#ifndef SDef_SourceCreate_h
#define SDef_SourceCreate_h

class Simulation;

namespace SDef
{
  class Source;
  class SourceBase;
  
  std::shared_ptr<SourceBase> makeActivationSource(const std::string&);
  
  
  std::string createBeamSource(const FuncDataBase&,const std::string&,
			       const attachSystem::FixedComp&,const long int);

  
  std::string createBilbaoSource(const FuncDataBase&,
				 const attachSystem::FixedComp&,const long int);
  
  std::string createD4CSource(const FuncDataBase&,
			      const attachSystem::FixedComp&,
			      const long int);

  std::string createESSLinacSource(const FuncDataBase&,
				   const attachSystem::FixedComp&,
				   const long int);
  
  std::string createESSSource(const FuncDataBase&,
		       const attachSystem::FixedComp&,const long int);
  
  std::string createESSPortSource(const FuncDataBase&,
			   const attachSystem::FixedComp&,
				  const long int);
  
  std::string createBeamSource(const FuncDataBase&,const std::string&,
			       const attachSystem::FixedComp&,const long int);

  std::string createLaserSource(const FuncDataBase&,const std::string&);
  std::string createGammaSource(const FuncDataBase&,const std::string&,
				const attachSystem::FixedComp&,const long int);
  
  std::string createLensSource(const FuncDataBase&,
			const attachSystem::FixedComp&,
			const long int);
  
  std::string createPointSource(const FuncDataBase&,const std::string&,
				const attachSystem::FixedComp&,
				const long int);
  
  std::string createSinbadSource(const FuncDataBase&,
				 const attachSystem::FixedComp&,
				 const long int);
  
  std::string createTS1Source(const FuncDataBase&,
			      const attachSystem::FixedComp&,
			      const long int);
  
  std::string createTS1GaussianSource(const FuncDataBase&,
				      const attachSystem::FixedComp&,
				      const long int);

    
  std::string createWigglerSource
    (const FuncDataBase&,const attachSystem::FixedComp&,const long int);


  // Goran section:
  std::string createTS1GaussianNewSource(const FuncDataBase&,
				  const attachSystem::FixedComp&,
				  const long int);  
  
  std::string createTS1MuonSource(const FuncDataBase&,
				  const attachSystem::FixedComp&,
				  const long int);
  
  std::string createTS1EPBCollSource(const FuncDataBase&,
				     const attachSystem::FixedComp&,
				     const long int);

  // ---------------------------
  
  std::string createTS2Source(const FuncDataBase&,
			      const attachSystem::FixedComp&,const long int);

  
  std::string createTS3ExptSource(const FuncDataBase&,
			   const attachSystem::FixedComp&,const long int);
}


#endif
 
