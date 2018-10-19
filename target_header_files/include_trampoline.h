#ifndef INCLUDE_TRAMPOLINE_H
#define INCLUDE_TRAMPOLINE_H

#undef ISR
extern "C" {
	#include "tpl_app_config.h"
	#include "tpl_os.h"
}

#endif //INCLUDE_TRAMPOLINE_H