#ifndef ETRIPATOR_CFG_H
#define ETRIPATOR_CFG_H

/**
 * \brief CFG parsing errors
 */
typedef enum
{
	CFG_OK = 0,                      /*< Success. */
	CFG_INVALID_PARAMETERS,          /*< At least one of the parameter has an invalid value. */
	CFG_FILE_OPEN_ERR,               /*< An error occured while opening source file. */
	CFG_FILE_READ_ERROR,             /*< An error occured while reading source file. */
	CFG_FILE_EOF,                    /*< The source file end was reached. */
	CFG_EMPTY_STRING,                /*< Empty string. */
	CFG_INVALID_STRING,              /*< Invalid string. */
	CFG_SECTION_MISSING_DELIMITER,   /*< Closing section delimiter is missing. */
	CFG_SECTION_INVALID_DECLARATION, /*< Section declaration is invalid. */
	CFG_SECTION_CALLBACK_FAILED,     /*< Section callback failed. */
	CFG_KEY_VALUE_CALLBACK_FAILED,   /*< Key/Value callback failed. */
	CFG_MISSING_SEPARATOR,           /*< Missing equal separator. */
	CFG_MEMORY_ISSUE                 /*< Something went wrong with the memory. */
} CFG_ERR;

/**
 * \brief Payload structure for CFG/Ini file parser
 */
struct CFGPayload
{
	void *data; /*!< Use data which will be passed as argument to one the callback function. */

	int  line;  /*!< Current line number. */

	/**
	 * \brief Section start callback
	 *
	 * This callback will be called when the parser encounters a new section block.
	 *
	 * \param data user provided data 
	 * \param sectionName section name
	 * 
	 * \return 
	 *     <=0 failure
	 *     >0 success 
	 */
	int (*beginSectionCallback) (void *data, const char* sectionName);

	/**
	 * \brief Section end callback
	 *
	 * This callback will be called when the current section is over (ie at end of file or when a new section starts).
	 *
	 * \param data user provided data 
	 * 
	 * \return 
	 *     <=0 failure
	 *     >0 success 
	 */
	int (*endSectionCallback) (void *data);

	/**
	 * \brief Key value callback
	 *
	 * This callback is called when the parser encounters a new key/value pair.
	 *
	 * \param data user provided data 
	 * \param key key name
	 * \param value value name
	 * 
	 * \return
	 *     <=0 failure
	 *     >0 success 
	 */
	int (*keyValueCallback)    (void *data, const char* key, const char* value);
};

/**
 * \brief Parse CFG file
 *
 * \param filename CFG/Ini filename.
 * \param payload User defined callbacks and data.
 * \return \see CFG_ERR
 */
CFG_ERR ParseCFG(const char* filename, struct CFGPayload* payload);

#endif // ETRIPATOR_CFG_H