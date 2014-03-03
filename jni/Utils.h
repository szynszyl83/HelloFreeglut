/*
 * Utils.h
 *
 *  Created on: Mar 3, 2014
 *      Author: michal
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <GLES2/gl2.h>

namespace gles {

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define BUFFER_OFFSET_OF_STRUCT(structure, field) ((char *)NULL + (offsetof(struct structure, field)))
#define ARR_SIZE(a) sizeof(a) / sizeof(*a)

void assertNoError();
GLuint loadShaderPair(const char *szVertexProg, const char *szFragmentProg);

} /* namespace gles */

#endif /* UTILS_H_ */
