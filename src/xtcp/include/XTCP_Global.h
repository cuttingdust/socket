#ifndef XTCP_GLOBAL_H
#define XTCP_GLOBAL_H

#include <memory>

#ifdef _WIN32
#ifdef xtcp_EXPORTS
#define XTCP_EXPORT	__declspec(dllexport)
#else
#define XTCP_EXPORT __declspec(dllimport)
#endif
#else
#define XTCP_EXPORT
#endif



#endif // XTCP_GLOBAL_H
