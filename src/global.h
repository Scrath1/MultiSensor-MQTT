#ifndef GLOBAL_H
#define GLOBAL_H
#include "cfg.h"

// For creating fixed width integer definitions
#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <math.h>
#include <stdint.h>
#endif

// Priority with which the main loop runs in respect to other tasks.
// Higher number => Other tasks preempt the lower number
#define LOOP_TASK_PRIORITY (1)

// Calculates the size of an array
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

// runs code inside following brackets exactly one time, even when called within a loop
#define RUN_ONCE                                        \
    for (static bool _run_already_ = false;             \
         _run_already_ ? false : _run_already_ = true;) \

/**
 * @brief Various return codes to indicate different error conditions
 */
typedef enum
{
    RC_SUCCESS              	=  0,                   /**< \brief Function was successfully completed.                            	*/
    RC_ERROR                	= -1,                   /**< \brief Non specific error.                                             	*/
    RC_ERROR_NULL           	= -2,                   /**< \brief A pointer was NULL when a non-NULL pointer was expected.        	*/
    RC_ERROR_ZERO           	= -3,                   /**< \brief A value was zero when no zero can be accepted.                  	*/
    RC_ERROR_MEMORY         	= -4,                   /**< \brief Out of memory.                                                  	*/
    RC_ERROR_RANGE          	= -5,                   /**< \brief A value was out of Range.                                       	*/
    RC_ERROR_OVERRUN        	= -6,                   /**< \brief A value was too big.                                            	*/
    RC_ERROR_UNDERRUN       	= -7,                   /**< \brief A value was too small                                           	*/
    RC_ERROR_BUFFER_FULL    	= -8,                   /**< \brief A buffer was full.                                              	*/
    RC_ERROR_BUFFER_EMPTY   	= -9,                   /**< \brief A buffer was empty.                                             	*/
    RC_ERROR_BUSY           	= -10,                  /**< \brief A resource was busy.                                            	*/
    RC_ERROR_TIME_OUT       	= -11,                  /**< \brief Something take too much time.                                   	*/
    RC_ERROR_OPEN           	= -12,                  /**< \brief A device can't be opened.                                       	*/
    RC_ERROR_CHECKSUM       	= -13,                  /**< \brief Wrong checksum.                                                 	*/
    RC_ERROR_READ_ONLY      	= -14,                  /**< \brief Object is read only.                                            	*/
    RC_ERROR_WRITE_ONLY     	= -15,                  /**< \brief Object is write only.                                           	*/
    RC_ERROR_INVALID        	= -16,                  /**< \brief Object is invalid.                                              	*/
    RC_ERROR_READ_FAILS     	= -17,                  /**< \brief Could not read from object.                                     	*/
    RC_ERROR_WRITE_FAILS    	= -18,                  /**< \brief Could not write to the object.                                  	*/
    RC_ERROR_NOT_MATCH      	= -19,                  /**< \brief Could not write to the object.								 	    */
    RC_ERROR_BAD_PARAM      	= -20,                  /**< \brief Parameters passed to the function are invalid or illegal.           */
    RC_ERROR_BAD_DATA       	= -21,                  /**< \brief Invalid global data which is required by the function.              */
    RC_ERROR_CANCELED       	= -22,                  /**< \brief operation was canceled.                                             */
    RC_ERROR_INVALID_STATE  	= -23,                  /**< \brief The operation can not be performed because of a FSM-state           */
    RC_ERROR_UNKNOWN        	= -24,                  /**< \brief Unknown error.                                                      */
    RC_ERROR_NOT_IMPLEMENTED    = -25,                  /**< \brief The requested functionality is not implemented.                     */
    RC_ERROR_LIB				= -26,					/**< \brief A library call returned an error                   */
} RC_t;

#endif // GLOBAL_H