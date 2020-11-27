#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_KEY		0

#define ASSERT(assertion, err_desc, instr)									\
	do {																	\
		if (assertion) {													\
			fprintf(stderr, "[%s: %d]: %s\n", __FILE__, __LINE__,			\
				err_desc);													\
			instr;															\
		}																	\
	} while (0)

#ifdef __cplusplus
}
#endif
