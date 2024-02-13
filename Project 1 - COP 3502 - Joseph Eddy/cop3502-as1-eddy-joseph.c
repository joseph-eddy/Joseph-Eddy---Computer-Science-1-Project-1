/*------------------------------------------------------------------------------------
Name: Joseph Eddy
Program: Assignment 1 Due 6-13-2022
Usage: This assignment does work with dynamic memory allocation, pointers, and pointer
arrays. Several small monster trainers have come to you for advice regarding
expeditions they're planning into various regions.
Objective: program estimates how many monsters of each type they can expect to
capture in each region.
Course Name: COP3502C_CMB-22Summer 00090 - Computer Science 1
-------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cop3502_as1.h"
#include "leak_detector_c.h"

/*---------------------------------------------------------------------------
FUNCTION: remove_crlf
PURPOSE: file input function - used to remove carriage return/line feed
PARAMETERS: char *s - pointer to the file input that is stored in buf
RETURN VALUE: void
PARAMETER REQUIREMENTS: none
ISSUES: none
---------------------------------------------------------------------------*/
void remove_crlf(char *s)
{
    char *end = s + strlen(s);
    end--;

    while((end >= s) && (*end == '\n' || *end == '\r'))
    {
        *end = '\0';
        end--;
    }
}

/*---------------------------------------------------------------------------
FUNCTION: get_next_nonblank_line
PURPOSE:  Gets the next non-blank line from the input file
PARAMETERS:  FILE *ifp - pointer to the input file
             char *buf - pointer for buffer to hold input string
             int max_length - the maximum length the string could be
RETURN VALUE: int (0 - end of file or 1-nonblank line)
PARAMETER REQUIREMENTS: max_length is 255
ISSUES: none
---------------------------------------------------------------------------*/
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length)
{
    buf[0] = '\0'; //start with an empty string
    while(!feof(ifp)&& (buf[0] == '\0'))
    {
        //if not at the eof, and have not read a nonblank line yet then read a line and
        //pull the carriage return line feed off the end.
        fgets(buf, max_length, ifp);
        remove_crlf(buf);
    }

    //checking to see if we read a nonblank line or are at the end of file
    if(buf[0] == '\0')
    {
        //end of file
        return 0;
    }
    else
    {
        //nonblank line
        return 1;
    }
}

/*---------------------------------------------------------------------------
FUNCTION: *new_monster
PURPOSE: constructor for a new monster record
PARAMETERS: int id - unique id for a monster
            char *name - pointer for the monster name
            char * element - pointer for the element the monster is
            int population - how many occurrences are there for this monster
RETURN VALUE: monster pointer
PARAMETER REQUIREMENTS: none
ISSUES: calls malloc so need to free memory later
---------------------------------------------------------------------------*/
monster *new_monster(int id, char *name, char *element, int population)
{
    //heap allocation, allocating space for the structure
    monster *p = malloc(sizeof(monster));

    p->id = id;
    p->name = malloc(strlen(name)+ 1);
    strcpy(p->name, name);
    p->element = malloc(strlen(element)+ 1);
    strcpy(p->element, element);
    p->population = population;

    return p;
}
/*---------------------------------------------------------------------------
FUNCTION: dispose_monster
PURPOSE: destructor for the monster record
PARAMETERS: monster *p - pointer to the monster record to be freed
RETURN VALUE: void
PARAMETER REQUIREMENTS: monster cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void dispose_monster(monster *p)
{
    free(p->element);
    free(p->name);
    free(p);
}

/*---------------------------------------------------------------------------
FUNCTION: print_monster
PURPOSE: prints to the console for debugging
PARAMETERS: monster *m - pointer to the monster record to be printed
RETURN VALUE: void
PARAMETER REQUIREMENTS: monster pointer cannot be null
ISSUES: print to the console, used for debugging only during development
---------------------------------------------------------------------------*/
void print_monster(monster *m)
{
    printf("Monster %s:\n", m->name);
}

/*---------------------------------------------------------------------------
FUNCTION: **new_monster_list
PURPOSE: constructor for monster list
PARAMETERS: int nMonsters - number of monsters that will be added to the monster list
RETURN VALUE: pointer to the monster list
PARAMETER REQUIREMENTS: number should not be null or zero
ISSUES: since this calls calloc need to free this memory by calling
        dispose_monster_list later
---------------------------------------------------------------------------*/
monster **new_monster_list(int nMonsters)
{
    //use the size of the monster not the size of the underlying structure
    //using calloc to force the uninitialized pointers to be null
    monster **ml = calloc(nMonsters, sizeof(monster *));
    return ml;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_monster_list
PURPOSE: shallow destructor for a pointer list of monsters
PARAMETERS: pointer to monster list
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: call this once all of the individual records have been freed
---------------------------------------------------------------------------*/
void dispose_monster_list(monster **ml)
{
    free(ml);
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_monster_list_all
PURPOSE: deep destructor for a pointer list of monsters
PARAMETERS: monster **ml - double pointer to monster list
            int nMonsters - number of Monsters in the Monster List
RETURN VALUE: void
PARAMETER REQUIREMENTS:cannot be null
ISSUES: no issues, frees each monster first by calling dispose_monster,
        then frees the monster list.
---------------------------------------------------------------------------*/
void dispose_monster_list_all(monster **ml, int nMonsters)
{
    int i;
    for(i=0; i< nMonsters; i++)
    {
        if(ml[i] != NULL)
        {
            printf("dispose_monster_list_all:free memory of regionMonster: %s\n", ml[i]->name);
            dispose_monster(ml[i]);
        }
    }
    dispose_monster_list(ml);
}

/*---------------------------------------------------------------------------
FUNCTION: print_monster_list
PURPOSE: prints the monsters in the list, used for debugging
PARAMETERS: monster **ml - double pointer to the monster list
            int nMonsters - number of monsters to be printed
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: print to the console, used for debugging only during development
---------------------------------------------------------------------------*/
void print_monster_list(monster **ml, int nMonsters)
{
    printf("List of %d monsters:\n", nMonsters);
    for(int i=0; i<nMonsters; i++)
    {
        printf(" ");
        print_monster(ml[i]);
    }
}

/*---------------------------------------------------------------------------
FUNCTION: *new_region
PURPOSE: constructor for region this is the list of regions and the monsters belonging
         to them.  It is used later to search and compare with the regions belonging
         to a trainer so the total population can be found and used to calculate
         trainer results.
PARAMETERS: char *name - pointer to region name
            int nmonsters - number of monsters
            int total_population - calculated sum of monsters for the region
            monster **monsters - double pointer to the list of monsters for this region
RETURN VALUE: region pointer
PARAMETER REQUIREMENTS: total_population is calculated later and will be null for
                        the constructor initialization, but will be updated later
                        once calculated.
ISSUES: calls malloc so need to free memory later
---------------------------------------------------------------------------*/
region *new_region(char *name, int nmonsters, int total_population, monster **monsters)
{
     region *p = malloc(sizeof(region));

     p->name = malloc(strlen(name)+ 1);
     strcpy(p->name, name);
     p->nmonsters = nmonsters;
     p->total_population = total_population;
     p->monsters = monsters;

    return p;
}

/*---------------------------------------------------------------------------
FUNCTION: *new_itineraryRegion
PURPOSE: 2nd constructor for region with only name to be used for itinerary regions
         this region is a child of the trainer so we only need to store name.Later
         we will scan the region list matching on region name to calculate the
         trainer results by getting the total population number.
PARAMETERS: char *name - name of the region
RETURN VALUE: pointer to region
PARAMETER REQUIREMENTS: cannot be null
ISSUES: 1)since the file is read in sequentially the data is also stored in this
        manner, 1 struct at a time, and then added to the
        array or list you are currently working with.  The other region elements
        are not included with this constructor because they would be null
        because this region is populated when trainer is read in
        and the trainer record did not have monsters directly tied to it, you have
        to later loop through the independent monsters list matching on name,
        and then get population, to use for calculating trainer results.
        2) using malloc so free memory later
---------------------------------------------------------------------------*/
region *new_itineraryRegion(char *name)
{
    region *p = malloc(sizeof(region));

     p->name = malloc(strlen(name)+ 1);
     strcpy(p->name, name);

    return p;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_region
PURPOSE: destructor - freeing memory for region
PARAMETERS: region *p - pointer to region
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void dispose_region(region *p)
{
    free(p->name);
    free(p);
}

/*---------------------------------------------------------------------------
FUNCTION: print_region
PURPOSE: prints region name, debugging code for region
PARAMETERS: region *r - pointer to region
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: print to the console, used for debugging only during development
---------------------------------------------------------------------------*/
void print_region(region *r)
{
    printf("Region %s:\n", r->name);
}
/*---------------------------------------------------------------------------
FUNCTION: **new_region_list
PURPOSE: constructor - creates the double pointer list of regions,
         the list is used to point to the individual region record.
PARAMETERS: int nRegions - contains number of regions
RETURN VALUE: pointer to region
PARAMETER REQUIREMENTS: cannot be null
ISSUES: using calloc so need to free memory later
---------------------------------------------------------------------------*/
region **new_region_list(int nRegions)
{
    //use the size of the region not the size of the underlying structure
   region **rl = calloc(nRegions, sizeof(region *));
    return rl;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_region_list
PURPOSE: shallow destructor for a pointer list of regions
PARAMETERS: region **rl - double pointer to region
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: call this once all of the individual records have been freed
---------------------------------------------------------------------------*/
void dispose_region_list(region **rl)
{
    free(rl);
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_region_list_all
PURPOSE: deep destructor for a pointer list of regions
PARAMETERS: region **rl - double pointer to region
            int nRegions - number of regions
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: frees memory for monsters then the monster list (if not null)
        first, then regions then the region list
---------------------------------------------------------------------------*/
void dispose_region_list_all(region **rl, int nRegions)
{
    for(int j=0; j<nRegions;j++)
    {
        if(rl[j] != NULL)
        {
            printf("NumberofRegions memory freed: %d\n", nRegions);
            int numMonsters = rl[j]->nmonsters;
            if(rl[j]->monsters != NULL)
            {
                printf("NumberofMonsters memory freed: %d\n", numMonsters);
                dispose_monster_list_all(rl[j]->monsters, numMonsters);
            }
            printf("free memory of region: %s\n", rl[j]->name);
            dispose_region(rl[j]);
        }
    }
    dispose_region_list(rl);
}

/*---------------------------------------------------------------------------
FUNCTION: print_region_list
PURPOSE: printer for a pointer list of regions
PARAMETERS: region **rl - double pointer to region
            int nRegions - number of regions
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: print to the console, used for debugging only during development
---------------------------------------------------------------------------*/
void print_region_list(region **rl, int nRegions)
{
    printf("List of %d regions:\n", nRegions);
    for(int i=0; i<nRegions; i++)
    {
        printf(" ");
        print_region(rl[i]);
    }
}

/*---------------------------------------------------------------------------
FUNCTION: *new_itinerary
PURPOSE: constructor for itinerary
PARAMETERS: int nregions - number of regions associated to the trainer
            region **regions - double pointer to the regions associated to the trainer
            int captures - number of captures the trainer made, used in the trainer
                           results calculation
RETURN VALUE: pointer to itinerary
PARAMETER REQUIREMENTS: cannot be null
ISSUES: calls to malloc, need to free memory later
---------------------------------------------------------------------------*/
itinerary *new_itinerary(int nregions, region **regions, int captures)
{
     itinerary *p = malloc(sizeof(itinerary));

     p->nregions = nregions;
     p->regions = regions;
     p->captures = captures;

    return p;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_itinerary
PURPOSE: destructor freeing memory for itinerary
PARAMETERS: itinerary *p - pointer to itinerary (which is also called visits)
RETURN VALUE:void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void dispose_itinerary(itinerary *p)
{
    free(p);
}

/*---------------------------------------------------------------------------
FUNCTION: print_itinerary
PURPOSE: printer for visit/itinerary
PARAMETERS: itinerary *i = pointer to visit/itinerary
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null, it is s single pointer so it contains 1 value/struct
ISSUES: used for debugging only
---------------------------------------------------------------------------*/
void print_itinerary(itinerary *i)
{
    printf("Itinerary %d:\n", i->captures);
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_visits_all
PURPOSE: deep destructor for a pointer to visits
PARAMETERS: itinerary *vl - single pointer to visit
RETURN VALUE: void
PARAMETER REQUIREMENTS: single pointer to visit is passed, not a list
                        this destructor is not destructing a list, just a
                        single record however it needs to free regions which
                        is a list and that needs to be called from here.
ISSUES: none
---------------------------------------------------------------------------*/
void dispose_visits_all(itinerary *vl)
{
    if(vl != NULL)
    {
        int iRegions = vl->nregions;
        dispose_region_list_all(vl->regions, iRegions);

        dispose_itinerary(vl);
    }
}

/*---------------------------------------------------------------------------
FUNCTION: *new_trainer
PURPOSE: constructor for trainer
PARAMETERS: char *name - pointer to the trainer name
            itinerary *visits - single pointer to visits (there is only 1
                                visits/itinerary record ever)
RETURN VALUE: pointer to trainer
PARAMETER REQUIREMENTS: cannot be null
ISSUES: calls to malloc, free memory later, visits will only have 1 record
---------------------------------------------------------------------------*/
trainer *new_trainer(char *name, itinerary *visits)
{
    trainer *p = malloc(sizeof(trainer));

     p->name = malloc(strlen(name)+ 1);
     strcpy(p->name, name);
     p->visits = visits;

    return p;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_trainer
PURPOSE: destructor - freeing memory for trainer
PARAMETERS: trainer *p - pointer for trainer
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void dispose_trainer(trainer *p)
{
    free(p->name);
    free(p);
}

/*---------------------------------------------------------------------------
FUNCTION: print_trainer
PURPOSE: debugging code for trainer
PARAMETERS: trainer *t - pointer to trainer
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: print to the console the trainers and the regions associated with them
        used for debugging only during development
---------------------------------------------------------------------------*/
void print_trainer(trainer *t)
{
    itinerary *it = t->visits;
    int iRegions = it->nregions;

    printf("trainer is: %s", t->name);
    for(int i=0; i < iRegions; i++)
    {
        printf("Region %s: \n", (it->regions[i]->name));
    }
}

/*---------------------------------------------------------------------------
FUNCTION: **new_trainer_list
PURPOSE: constructor for new trainer list
PARAMETERS: int nTrainers - number of trainers in the list
RETURN VALUE: double pointer to new trainer list
PARAMETER REQUIREMENTS: cannot be null
ISSUES: calls to calloc, later need to free memory
---------------------------------------------------------------------------*/
trainer **new_trainer_list(int nTrainers)
{
    //use the size of the region not the size of the underlying structure
    //using calloc to force the uninitialized pointers to be null
    trainer **tl = calloc(nTrainers, sizeof(trainer *));
    return tl;
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_trainer_list
PURPOSE: shallow destructor for a pointer list of trainers
PARAMETERS: trainer **tl - pointer to the trainer list
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: after all trainers are freed, then free the list.
---------------------------------------------------------------------------*/
void dispose_trainer_list(trainer **tl)
{
   free(tl);
}

/*---------------------------------------------------------------------------
FUNCTION: dispose_trainer_list_all
PURPOSE: deep destructor for a pointer list of trainers
PARAMETERS: trainer **tl - pointer to trainer list
            int nTrainers - number of trainers to be added to the list
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: deletes the children first then rolls up to delete the trainers last
---------------------------------------------------------------------------*/
void dispose_trainer_list_all(trainer **tl, int nTrainers)
{
    int i;
    printf("number of trainers: %d\n", nTrainers);
    for(i=0; i< nTrainers; i++)
    {
        if(tl[i] != NULL)
        {
           //single record, not a list
            dispose_visits_all(tl[i]->visits);

            printf("delete trainers: %s\n", tl[i]->name);

            dispose_trainer(tl[i]);
       }
    }
    dispose_trainer_list(tl);
}

/*---------------------------------------------------------------------------
FUNCTION: print_trainer_list
PURPOSE: printer for a pointer list of trainers
PARAMETERS: trainer **tl - pointer to trainer list
            int nTrainers - number of trainers in the list
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: prints list of trainers, used for debugging purposes only
---------------------------------------------------------------------------*/
void print_trainer_list(trainer **tl, int nTrainers)
{
    for(int i=0; i < nTrainers; i++)
    {
        printf(" ");
        print_trainer(tl[i]);
    }
}

/*---------------------------------------------------------------------------
FUNCTION: calculate_trainer_results
PURPOSE: used to calculate the trainer results number (round (population/total population * captures)
         once calculated the monster record is written to the output file with the trainers results
PARAMETERS: FILE *ofp - output file pointer
            region **regions - double pointer to regions
            int regionsCtr - number of regions used in for loop search
            int itineraryCapturesCtr number of captures for the trainer, used in trainer results calc
            char *trainerRegionName - name of region, used for search
            monster **monsters - double pointer to monsters
            int monstersCtr - number of monsters used in for loop search
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void calculate_trainer_results(FILE *ofp, region **regions, int regionsCtr, int itineraryCapturesCtr,
                               char *trainerRegionName, monster **monsters, int monstersCtr)
{
    int roundedTrainerResults;
    //looping through the regions
    for(int i=0; i<regionsCtr; i++)
    {
        region *r = regions[i];

        //for calculations
        int numMonsters = 0;
        numMonsters = r->nmonsters;

        char *regionName;
        regionName =  r->name;
        int regionTotalPop = 0;
        regionTotalPop = r->total_population;

        //checking for a match
        if (strcmp(regionName,trainerRegionName)==0)
        {
            //looping through the regional monsters
            for(int j = 0;j < numMonsters; j++)
            {
                //regional monster
                monster *rm = r->monsters[j];

                //looping through the monsters for a match
                for (int k = 0; k < monstersCtr; k++)
                {
                    monster *m = monsters[k];
                    char *regionMonsterName;
                    char *monsterName;
                    int monsterPop=0;
                    float temptrainerResults =0;

                    regionMonsterName = rm->name;
                    monsterName = m->name;
                    monsterPop = m->population;

                    if ((strcmp(regionMonsterName, monsterName))== 0)
                    {
                        //calculate
                        temptrainerResults = (((double)monsterPop/regionTotalPop)* itineraryCapturesCtr);
                        roundedTrainerResults = temptrainerResults + 0.5;
                        if (roundedTrainerResults != 0)
                        {
                            fprintf(ofp, "%d %s\n", roundedTrainerResults,monsterName);
                        }
                        break;
                    }
                }
           }
            //break once we have printed all monsters for that region
            //do not need to continue through the regions
            break;
        }
    }
}

/*---------------------------------------------------------------------------
FUNCTION: calculate_total_population
PURPOSE: calculating the total population per region and updating the region record
PARAMETERS: region **regions - double pointer to regions
            int regionsCtr - number of regions
            monster **monsters - double pointer to monsters
            int monstersCtr - number of monsters
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
---------------------------------------------------------------------------*/
void calculate_total_population(region **regions, int regionsCtr, monster **monsters, int monstersCtr)
{
    //loop through the regions list
    //inner loop for monsters of those regions
    //inner loop of monsters looking for a match
    //get the population for each monster in your region add them up
    //take that number and populate 'total population' for that region
    //value to be used later when calculating trainer value

    //looping through the regions
    for(int i=0; i<regionsCtr; i++)
    {
        region *r = regions[i];
        //used to add up the total population for the region
        int intRegionPopulation = 0;
        int numMonsters = 0;
        numMonsters = r->nmonsters;
        //looping through the regional monsters
        for(int j = 0;j < numMonsters; j++)
        {
            //regional monster
            monster *rm = r->monsters[j];

            //looping through the monsters for a match to get the population count
            for (int k = 0; k < monstersCtr; k++)
            {
                monster *m = monsters[k];
                char *regionMonsterName;
                char *monsterName;

                regionMonsterName =  rm->name;
                monsterName = m->name;

                if ((strcmp(regionMonsterName, monsterName))== 0)
                {
                    intRegionPopulation += m->population;
                    break;
                }
            }
        }
        r->total_population = intRegionPopulation;
    }
}

/*---------------------------------------------------------------------------------------------------
FUNCTION: print_Output_File
PURPOSE: print trainer and region name to the output file call trainer results calculation from here
         which prints monster name and trainer number,
PARAMETERS: FILE *ofp - output file pointer
            trainer **trainers - double pointer to trainer
            int iNumTrainerLoops - number of trainers
            region **regions - double pointer to regions
            int iNumRegionLoops - number of regions
            monster **monsters - double pointer to monsters
            int monstersCtr - number of monsters
RETURN VALUE: void
PARAMETER REQUIREMENTS: cannot be null
ISSUES: none
------------------------------------------------------------------------------------------------------*/
void print_Output_File(FILE *ofp, trainer **trainers, int iNumTrainerLoops,region **regions,
                       int iNumRegionLoops, monster **monsters, int monstersCtr)
{
    //for loop for each trainer
    for(int i=0; i < iNumTrainerLoops; i++)
    {
        fprintf(ofp, "%s\n", trainers[i]->name);

        //now get the trainers region
        itinerary *it = trainers[i]->visits;

        int itineraryCapturesCtr = it[0].captures;
        int regionsCtr = it[0].nregions;
        char *trainerRegionName;

        for(int j=0; j < regionsCtr ; j++)
        {
            region *r = it[0].regions[j];
            trainerRegionName = r->name;
            fprintf(ofp, "%s\n", r->name);

            //calculate trainer results
            calculate_trainer_results(ofp, regions, iNumRegionLoops, itineraryCapturesCtr, trainerRegionName, monsters, monstersCtr);
        }
        //blank line between trainers
        fprintf(ofp, "\n");
     }
}
/*-------------------------------------------------------------------------*/
/*   MAIN                                                                  */
/*-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
FUNCTION: main
PURPOSE: memory leak code, reads the input file, populates the structs, calls the calculate functions
         calls the output file writing functions, calls the free memory/dispose functions
PARAMETERS: none
RETURN VALUE: none
PARAMETER REQUIREMENTS: none
ISSUES: none
---------------------------------------------------------------------------*/
int main()
{
   atexit(report_mem_leak);

    //file pointers input and output
    FILE *ifp;
    FILE *ofp;

    //used to store the string I am reading in
    char buf[256];
    char buf2[256];

    //set flags
    bool monsterFlag = false;
    bool regionFlag = false;
    bool trainerFlag = false;

    //variables needed to hold values from file to add to structs
    char monsterName[20];
    char monsterElement[20];
    int monsterPopulation = 0;
    char regionName[20];
    char trainerName[20];
    int itineraryCaptures;
    int itineraryRegions;

    //declaring the lists
    monster **monsters;
    region **regions;
    monster **regionMonsters;
    region **itiRegions;
    itinerary *visits;
    trainer **trainers;

    //opening the file
    ifp = fopen("cop3502-as1-input.txt", "r");

    //variables needed for looping
    int iNumMonsterLoops;
    int iNumRegionLoops;
    int iNumTrainerLoops;
    int iNumRegionMonsterLoops;
    int innerMonsterLoopCtr = 0;
    int innerRegionLoopCtr = 0;
    int innerTrainerLoopCtr = 0;
    int innerItineraryRegionLoopCtr = 0;
    int outerTrainerLoopCtr = 0;

    // reading in the file, series of if/else statements to determine what is in
    //the record (monster/region/trainer/itinerary) and where to store it.
    while(get_next_nonblank_line(ifp, buf, 255))
    {
        //having to do this because the strlwr was changing the values in the buffer
        strcpy(buf2,buf);

        //used to hold return value from strstr if the string was found in the file input record
        char *result;

        //if this is the monsters need to loop that many times
        //looking for the string monster in the file input line
        //converting it to lower case to get correct results.
        if ((result = strstr(strlwr(buf2), "monster")))
        {   //if this is the read for regionmonsters set a different counter
            if(regionFlag)
            {
                //capture the number of monsters
                sscanf(buf, "%d", &iNumRegionMonsterLoops);
                //set flag to true
                monsterFlag = true;
                //constructing the new monster list with the size it should be
                regionMonsters = new_monster_list(iNumRegionMonsterLoops);
            }
            else
            {
                //capture the number of monsters
                sscanf(buf, "%d", &iNumMonsterLoops);
                //set flag to true
                monsterFlag = true;
                //constructing the new monster list with the size it should be
                monsters = new_monster_list(iNumMonsterLoops);
            }
        }
        else if((result = strstr(strlwr(buf2), "region"))&& (!trainerFlag))
        {
            //this is the regions, need the number to loop
            sscanf(buf, "%d", &iNumRegionLoops);
            regionFlag = true;
            regions = new_region_list(iNumRegionLoops);
        }
        else if((result = strstr(strlwr(buf2), "trainer")))
        {
           //this is the trainers, need the number to loop
            sscanf(buf, "%d", &iNumTrainerLoops);
            trainerFlag = true;
            trainers = new_trainer_list(iNumTrainerLoops);
        }
        else
        {
            //main logic for sorting data being read in,checking for true values on flags
            if (monsterFlag)
            {
                if(regionFlag)
                {
                    //if this is a monster in the region portion of the file then it only contains the
                    //monster name setting empty values for other attributes of monster for this
                    //scenario only.
                    sscanf(buf, "%s", monsterName);
                    regionMonsters[innerMonsterLoopCtr] = new_monster(innerMonsterLoopCtr,monsterName,"",0);
                }
                else
                {
                    //if not a region monster then proceed as normal for populating monster list.
                    sscanf(buf, "%s %s %d", monsterName, monsterElement, &monsterPopulation);
                    monsters[innerMonsterLoopCtr] = new_monster(innerMonsterLoopCtr,monsterName,monsterElement,monsterPopulation);
                }

                //incrementing monster counter when equal to how many the file says then...
                innerMonsterLoopCtr++;
                if ((innerMonsterLoopCtr == iNumMonsterLoops) || (innerMonsterLoopCtr == iNumRegionMonsterLoops))
                {
                    //if the region flag is true and we are here then the regionmonsterlist
                    //is to be added to the region struct.
                    if(regionFlag)
                    {
                        //totalPopulation will be calculated later once all records are read in.
                        regions[innerRegionLoopCtr] = new_region(regionName,iNumRegionMonsterLoops,0,regionMonsters);
                        //keeping up with the number of regions being added for use later.
                        innerRegionLoopCtr++;
                    }
                    //setting flag to false, counter to zero, now that we have added all of the monsters
                    monsterFlag = false;
                    innerMonsterLoopCtr = 0;

                  //  print_monster_list(monsters, iNumMonsterLoops);
                    //at the end of the regions portion of the file
                    if (innerRegionLoopCtr == iNumRegionLoops)
                    {
                        regionFlag = false;
                    }
                }
            }
            else if (regionFlag)
            {
                //this is only to retrieve the name of the region
                sscanf(buf, "%s", regionName);
            }
            else if (trainerFlag)
            {
                if (outerTrainerLoopCtr == 0)
                {
                    //getting the trainer name
                    sscanf(buf, "%s", trainerName);
                    outerTrainerLoopCtr++;
                }
                else
                {
                   //getting the counts for captures/regions
                    if ((result = strstr(strlwr(buf2), "capture")))
                    {
                        sscanf(buf, "%d", &itineraryCaptures);
                    }
                    else if ((result = strstr(strlwr(buf2), "region")))
                    {
                        sscanf(buf, "%d", &itineraryRegions);

                        //creating new regionlists for this trainer
                        itiRegions = new_region_list(itineraryRegions);
                    }
                    else
                    {
                        sscanf(buf, "%s", regionName );

                        //establishing new itineraryRegions
                        itiRegions[innerItineraryRegionLoopCtr] = new_itineraryRegion(regionName);
                        innerItineraryRegionLoopCtr++;

                        if(innerItineraryRegionLoopCtr == itineraryRegions)
                        {
                            //this checks to see if this is not your first time in here
                            //and you have read all of the itinerary region names in and
                            //are ready to create the itinerary record and the trainer record.
                            visits = new_itinerary( itineraryRegions, itiRegions, itineraryCaptures);

                            //reset your counter for itinerary regions
                            innerItineraryRegionLoopCtr = 0;

                            //create the trainer record
                            trainers[innerTrainerLoopCtr] = new_trainer(trainerName,visits);
                            innerTrainerLoopCtr++;
                            outerTrainerLoopCtr = 0;

                            //check to see if last trainer and if it is dispose of memory
                            if (innerTrainerLoopCtr == iNumTrainerLoops )
                            {
                                //finishing code for once we have read in all the trainers
                                trainerFlag = false;
                                innerTrainerLoopCtr = 0;
                                //print_trainer_list(trainers,iNumTrainerLoops);
                            }
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------
    // CALCULATIONS/FILE OUTPUT/DISPOSING MEMORY
    //--------------------------------------------

    //closing the input file
    fclose(ifp);

    //opening output file
    ofp = fopen("cop3502-as1-output-eddy-joseph.txt", "w+");

    //calculations for total population for each region
    calculate_total_population(regions, iNumRegionLoops, monsters, iNumMonsterLoops);

    //calculating trainer results and writing to the output file
    print_Output_File(ofp, trainers,iNumTrainerLoops, regions, iNumRegionLoops, monsters, iNumMonsterLoops);

    //closing the output file
    fclose(ofp);

    //memory cleanup
    dispose_trainer_list_all(trainers, iNumTrainerLoops);
    dispose_region_list_all(regions, iNumRegionLoops);

    printf("finished with trainers deleting regions/monsters\n");

    dispose_monster_list_all(monsters, iNumMonsterLoops);

    printf("end of run");
}
