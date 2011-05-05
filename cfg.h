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
	CFG_MISSING_SECTION_DELIMITER,   /*< The closing section delimiter is missing. */
	CFG_INVALID_SECTION_NAME,        /*< The section contains invalid characters. */
	CFG_INVALID_SECTION_DECLARATION, /*< The section declaration is invalid. */ 
	CFG_INVALID_KEY_NAME,            /*< The key contains invalid characters. */
	CFG_INVALID_VALUE_NAME,          /*< The value contains invalid characters. */
	CFG_MEMORY_ISSUE                 /*< Something went wrong with the memory. */
} CFG_ERR;

/**
 * \brief Payload structure for CFG/Ini file parser
 */
struct CFGPayload
{
	void *data; /*!< Use data which will be passed as argument to one the callback function. */

	/**
	 * \brief Section start callback
	 *
	 * This callback will be called when the parser encounters a new section block.
	 *
	 * \param data user provided data 
	 * \param sectionName section name
	 * 
	 * \return
	 *     CFG_MISSING_SECTION_DELIMITER if the closing section delimiter is missing.
	 *     CFG_INVALID_SECTION_NAME if the section name contains a invalid characters.
	 *     CFG_OK on success.
	 */
	CFG_ERR (*sectionCallback) (void *data, const char* sectionName);

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
	 *     CFG_INVALID_KEY_NAME if the key contains an invalid characters.
	 *     CFG_INVALID_VALUE_NAME if the value contains an invalid characters.
	 *     CFG_OK on success.
	 */
	CFG_ERR (*keyValueCallback)    (void *data, const char* key, const char* value);
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