/**********************************************************************
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/
#pragma once

#include <cassert>
#include <iostream>
#include <vector>
#include "RadeonProRender.h"

#define CHECK(x)                                                                                             \
  {                                                                                                          \
    if ((x) != RPR_SUCCESS)                                                                                  \
    {                                                                                                        \
      ErrorManager(x, __FILE__, __LINE__);                                                                   \
    }                                                                                                        \
  }
#define CHECK_EQ(x, y)                                                                                       \
  {                                                                                                          \
    if ((x) != (y))                                                                                          \
    {                                                                                                        \
      ErrorManager(0, __FILE__, __LINE__);                                                                   \
    }                                                                                                        \
  }
#define CHECK_NE(x, y)                                                                                       \
  {                                                                                                          \
    if ((x) == (y))                                                                                          \
    {                                                                                                        \
      ErrorManager(0, __FILE__, __LINE__);                                                                   \
    }                                                                                                        \
  }
#define CHECK_GT(x, y)                                                                                       \
  {                                                                                                          \
    if ((x) <= (y))                                                                                          \
    {                                                                                                        \
      ErrorManager(0, __FILE__, __LINE__);                                                                   \
    }                                                                                                        \
  }
#define CHECK_GE(x, y)                                                                                       \
  {                                                                                                          \
    if ((x) < (y))                                                                                           \
    {                                                                                                        \
      ErrorManager(0, __FILE__, __LINE__);                                                                   \
    }                                                                                                        \
  }

inline void
ErrorManager(int errorCode, const char *fileName, int line)
{
  std::cout << "ERROR detected - program will stop." << std::endl;
  std::cout << "file = " << fileName << std::endl;
  std::cout << "line = " << line << std::endl;
  std::cout << "error code = " << errorCode << std::endl;
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

  std::vector<void *> listRemainingObjects;

  for (unsigned int iType : type)
  {
    size_t sizeParam = 0;
    status           = rprContextGetInfo(context, iType, 0, nullptr, &sizeParam);
    CHECK(status);

    unsigned int nbObject = sizeParam / sizeof(void *);

    if (nbObject > 0)
    {
      std::cout << "leak of " << nbObject;
      if (iType == RPR_CONTEXT_LIST_CREATED_CAMERAS)
        std::cout << " cameras\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_MATERIALNODES)
        std::cout << " material nodes\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_LIGHTS)
        std::cout << " lights\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_SHAPES)
        std::cout << " shapes\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_POSTEFFECTS)
        std::cout << " postEffects\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES)
        std::cout << " heteroVolumes\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_GRIDS)
        std::cout << " grids\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_BUFFERS)
        std::cout << " buffers\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_IMAGES)
        std::cout << " images\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS)
        std::cout << " framebuffers\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_SCENES)
        std::cout << " scenes\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_CURVES)
        std::cout << " curves\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM)
        std::cout << " materialsystems\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_COMPOSITE)
        std::cout << " composites\n";
      else if (iType == RPR_CONTEXT_LIST_CREATED_LUT)
        std::cout << " luts\n";
      else
      {
        std::cout << " ???\n";
      }

      unsigned int idFirstTime = listRemainingObjects.size();
      listRemainingObjects.assign(idFirstTime + nbObject, nullptr);
      status = rprContextGetInfo(context, iType, sizeParam, &listRemainingObjects[idFirstTime], nullptr);
      CHECK(status);
    }
  }

  if (!listRemainingObjects.empty())
  {
    std::cout << "Warning : this context has some leak (" << listRemainingObjects.size() << " item(s))\n";
  }
}
