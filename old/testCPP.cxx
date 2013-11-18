/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/testCPP.cxx
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


int
flagExtract(std::vector<int>& MArg)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \retval 1 :: short success
    \retval 1 :: long success
    \retval 0 :: nothing found
  */
{
  std::vector<int>::iterator vc;
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      if (*vc==1)
	{
	  MArg.erase(vc);
	  return 1;
	}
      std::cout<<"V == "<<*vc<<std::endl;
    }
  return 0;
}
