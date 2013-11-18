/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/oldSimulation.cxx
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
Simulation::removeTS2DeadCells()
  /*!
    Removes those cells that are dead due to 
    variable and interactions etc.
    \return number of cells deleted 
  */
{
  ELog::EMessages.report("Entering TS2 specific components: REMOVECELL",1);
  int cnt(0);
  std::map<int,MonteCarlo::Qhull>::iterator vc=OList.begin();
  TS2remove RBase(DB,SurMap);

  AlterSurf AS(DB);
  try
    {
// SPECIAL FOR SLICE REPORCESSING 
      for(vc=OList.begin();vc!=OList.end();vc++)
	{
	  if (vc->first/100==216)
	    RBase.sliceReprocess(vc->first,vc->second);
	}

// MASTER 
      vc=OList.begin();
// Process cells
      while(vc!=OList.end())     
	{  
	  if (RBase(vc->first,vc->second))
	    {
	      ELog::FMessages.report("Cell destroyed",vc->first);
//	      ELog::CellMessage.report("Cell destroyed",vc->first);
	      OList.erase(vc++);
	      cnt++;
	    }
	  else
	    vc++;
	}

// Process altered surfaces first :: is this correct ??
      
      const std::map<int,int>& Asurf=AS.getMap();
      std::map<int,int>::const_iterator ac;
      for(ac=Asurf.begin();ac!=Asurf.end();ac++)
        {
	  if (!ac->second)
	    removeAllSurface(ac->first);
	  else
	    substituteAllSurface(ac->first,ac->second);
	}

      
// Now process Physics so importance/volume cards can be set
  
      processCellsImp();
    }
  catch (ColErr::ExBase& EType)
    {
      std::cerr<<EType.what()<<std::endl;
      std::cerr<<"Error with cell "<<vc->first<<std::endl;
      exit(1);
    }
//  ELog::CellMessage.processReport(std::cout);
  return cnt;
}

int
Simulation::removeTS1DeadCells()
  /*!
    Removes those cells that are dead due to 
    variable and interactions etc.
    \return number of cells deleted 
  */
{
  ELog::EMessages.report("Entering TS1 specific components: REMOVECELL",0);
  int cnt(0);
  std::map<int,MonteCarlo::Qhull>::iterator vc=OList.begin();
  TS1remove RBase(DB,SurMap);
  AlterTS1Surf AS(DB);
  try
    {
      vc=OList.begin();
// Process cells
      while(vc!=OList.end())     
	{  
	  if (RBase(vc->first,vc->second))
	    {
	      ELog::CellMessage.report("Cell destroyed",vc->first);
	      OList.erase(vc++);
	      cnt++;
	    }
	  else
	    vc++;
	}

// Process altered surfaces first :: is this correct ??

      const std::map<int,int>& Asurf=AS.getMap();
      std::map<int,int>::const_iterator ac;
      for(ac=Asurf.begin();ac!=Asurf.end();ac++)
        {
	  if (!ac->second)
	    removeAllSurface(ac->first);
	  else
	    substituteAllSurface(ac->first,ac->second);
	}
      
// Now process Physics so importance/volume cards can be set
      processCellsImp();
    }
  catch (ColErr::ExBase& EType)
    {
      std::cerr<<EType.what()<<std::endl;
      std::cerr<<"Error with cell "<<vc->first<<std::endl;
      exit(1);
    }
  ELog::CellMessage.processReport(std::cerr);
  return cnt;
}
