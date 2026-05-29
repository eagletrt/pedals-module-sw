#ifndef POST_API_H
#define POST_API_H

#include "post.h"

/*!
 * \brief Initialisation of other module's external functions
 * 
 * \param init structure containing all the external function
 * \return enum PostReturnCode operation status, fails if init is NULL or subroutines fail
 */
enum PostReturnCode post_init(struct PostInit* init);

#endif /* POST_API_H */