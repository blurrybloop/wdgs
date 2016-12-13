#ifndef _PCH_H
#define _PCH_H

#ifdef WIN32
#include <Windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <gli/gli.hpp>

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <functional>
#include <algorithm>

#ifdef WIN32
#include <dirent/dirent.h>
#else
#include <dirent.h>
#endif

#include "AntTweakBar.h"

#endif
