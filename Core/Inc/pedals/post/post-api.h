#ifndef POST_API_H
#define POST_API_H

#include "post.h"

/*!
 * \brief Initialisation for modules
 * 
 * \param init Contains info that must be provided to the init function of other modules
 * \return enum PostReturnCode status of the operation
 */
enum PostReturnCode post_api_init(struct PostInit *init);

#endif //POST_API_H