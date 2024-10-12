#ifndef XSOCKET_GLOBAL_H
#define XSOCKET_GLOBAL_H

#include <memory>

#ifdef _WIN32
#ifdef xsocket_EXPORTS
#define XSOCKET_EXPORT __declspec(dllexport)
#else
#define XSOCKET_EXPORT __declspec(dllimport)
#endif
#else
#define XSOCKET_EXPORT
#endif


#endif // XSOCKET_GLOBAL_H
