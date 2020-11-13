#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(assertion, src, err_desc, instr)								\
	do {																	\
		if (assertion) {													\
			fprintf(stderr, "[%s: %s, %d]: %s\n", src, __FILE__, __LINE__,	\
				err_desc);													\
			instr;															\
		}																	\
	} while (0)

#ifdef __cplusplus
}
#endif
