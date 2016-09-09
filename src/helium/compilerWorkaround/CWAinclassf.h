/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef HE_CWAINCLASSF
#define HE_CWAINCLASSF

#ifdef _WIN32
#define CWAinclassf(a,b) b
#else
#define CWAinclassf(a,b) a::b
#endif

#endif
