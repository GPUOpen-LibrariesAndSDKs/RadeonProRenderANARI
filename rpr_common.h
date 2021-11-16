#pragma once

#include <assert.h> 
#include <iostream>
#include <vector>
#include "RadeonProRender.h"

#define CHECK(x) { if ( (x) != RPR_SUCCESS ) { ErrorManager(x,__FILE__,__LINE__); } }
#define CHECK_EQ(x,y) { if ( (x) != (y) ) { ErrorManager(0,__FILE__,__LINE__); } }
#define CHECK_NE(x,y) { if ( (x) == (y) ) { ErrorManager(0,__FILE__,__LINE__); } }
#define CHECK_GT(x,y) { if ( (x) <= (y) ) { ErrorManager(0,__FILE__,__LINE__); } }
#define CHECK_GE(x,y) { if ( (x) <  (y) ) { ErrorManager(0,__FILE__,__LINE__); } }

inline void ErrorManager(int errorCode, const char* fileName, int line)
{
	std::cout<<"ERROR detected - program will stop."<<std::endl;
	std::cout<<"file = "<< fileName << std::endl;
	std::cout<<"line = "<< line << std::endl;
	std::cout<<"error code = "<< errorCode << std::endl;
	assert(0);
}


// this is an example a good practice to check RPR memory leak
inline void CheckNoLeak(rpr_context context)
{
	rpr_int status = RPR_SUCCESS;

	std::vector<rpr_context_info> type;
	type.push_back(RPR_CONTEXT_LIST_CREATED_CAMERAS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALNODES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_LIGHTS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_SHAPES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_POSTEFFECTS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_GRIDS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_BUFFERS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_IMAGES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS);
	type.push_back(RPR_CONTEXT_LIST_CREATED_SCENES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_CURVES);
	type.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM);
	type.push_back(RPR_CONTEXT_LIST_CREATED_COMPOSITE);
	type.push_back(RPR_CONTEXT_LIST_CREATED_LUT);

	std::vector<void*> listRemainingObjects;

	for(int iType=0; iType<type.size(); iType++)
	{

		size_t sizeParam = 0;
		status = rprContextGetInfo(context,type[iType],0,0,&sizeParam);CHECK(status);
		
		unsigned int nbObject = sizeParam / sizeof(void*);

		if ( nbObject > 0 )
		{
			std::cout<<"leak of "<< nbObject ;
				 if ( type[iType] == RPR_CONTEXT_LIST_CREATED_CAMERAS ) std::cout<<" cameras\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_MATERIALNODES ) std::cout<<" material nodes\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_LIGHTS ) std::cout<<" lights\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_SHAPES ) std::cout<<" shapes\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_POSTEFFECTS ) std::cout<<" postEffects\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES ) std::cout<<" heteroVolumes\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_GRIDS ) std::cout<<" grids\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_BUFFERS ) std::cout<<" buffers\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_IMAGES ) std::cout<<" images\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS ) std::cout<<" framebuffers\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_SCENES ) std::cout<<" scenes\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_CURVES ) std::cout<<" curves\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM ) std::cout<<" materialsystems\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_COMPOSITE ) std::cout<<" composites\n";
			else if ( type[iType] == RPR_CONTEXT_LIST_CREATED_LUT ) std::cout<<" luts\n";
			else 
			{
				std::cout<<" ???\n"; 
			}

			unsigned int idFirstTime = listRemainingObjects.size();
			listRemainingObjects.assign( idFirstTime + nbObject, nullptr );
			status = rprContextGetInfo(context,type[iType],sizeParam,&listRemainingObjects[idFirstTime],nullptr);CHECK(status);
		}
	}

	if ( listRemainingObjects.size() != 0 )
	{
		std::cout<<"Warning : this context has some leak ("<< listRemainingObjects.size() <<" item(s))\n";
	}

}
