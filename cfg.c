#include "config.h"
#include "cfg.h"

/**
 * \brief CFG parser helper
 */
struct CFGParser
{
	struct CFGPayload* payload; /*< User data and callbacks. */
	char *lineBuffer;           /*< Char buffer large enough to contains a "chomped" line. */
	size_t lineBufferSize;      /*< Line buffer size. */
	size_t lineStringLength;    /*< Length of the string stored in the line buffer. */
	FILE *input;                /*< File being parser. */
};


#define CFG_FORBIDDEN_CHAR_COUNT 4
static const char g_CFGForbiddenChar[CFG_FORBIDDEN_CHAR_COUNT] = 
{
	'=', ';', ' ', '\t'
};

#define CFG_ESCAPED_CHAR_COUNT 9
static const char g_CFGEscapedChar[CFG_ESCAPED_CHAR_COUNT] = 
{
	'n', 'r', 't', '\'', '"', '\\', '?', '=', ';'
};

static const char g_CFGTranslatedChar[CFG_ESCAPED_CHAR_COUNT] = 
{
	'\n', '\r', '\t', '\'', '"', '\\', '?', '=', ';'
};

/**
 * \brief Increase line buffer size.
 *
 * The request buffer size is adjusted to the closest highest multiple of 256.
 *
 * \param parser CFG parser holding the line buffer to resize.
 * \param newSize New buffer size.
 * \return
 *     CFG_OK if the line buffer was succesfully resized.
 *     CFG_MEMORY_ISSUE if the memory allocation failed.
 */
static CFG_ERR ResizeLineBuffer(struct CFGParser* parser, size_t newSize)
{
	size_t adjustedSize;	
	char *newBuffer;
	
	adjustedSize = (newSize + 0xff) & ~0xff;
	
	newBuffer = (char*)malloc(adjustedSize);
	if(newBuffer == NULL)
	{
		return CFG_MEMORY_ISSUE;
	}

	if(parser->lineBuffer != NULL)
	{
		free(parser->lineBuffer);
	}
	parser->lineBuffer = newBuffer;
	parser->lineBufferSize = adjustedSize;

	return CFG_OK;
}

/**
 * \brief Cleanup CFG parser
 * \param parser CFG parser
 */
static void CleanupCFGParser(struct CFGParser* parser)
{
	parser->payload = NULL;
	parser->lineBufferSize   = 0;
	parser->lineStringLength = 0;

	if(parser->lineBuffer != NULL)
	{
		free(parser->lineBuffer);
		parser->lineBuffer = NULL;
	}
}

/**
 * \brief Read a line from file.
 *
 * The starting and trailing spaces are removed.
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ReadLine(struct CFGParser* parser)
{
	char byte;
	size_t nRead;

	char *ptr = parser->lineBuffer;
	char *end = parser->lineBuffer + parser->lineBufferSize;

	// Skip starting spaces
	do
	{
		nRead = fread(&byte, 1, 1, parser->input);
	} while( (nRead == 1) && 
			 ((byte == ' ') || (byte == '\t')) );

	if(nRead == 0)
	{
		parser->lineStringLength = 0;
		return feof(parser->input) ? CFG_FILE_EOF : CFG_FILE_READ_ERROR;
	}

	// Read the rest of the line
	while( (nRead) && (byte != '\n') && (byte != '\r') )
	{
		*(ptr++) = byte;
		
		if(ptr == end)
		{
			CFG_ERR err = ResizeLineBuffer(parser, parser->lineBufferSize * 2);
			if(err != CFG_OK)
			{
				// We are in big troubles if resize fails.
				return err;
			}
			ptr = parser->lineBuffer;
			end = parser->lineBuffer + parser->lineBufferSize;
		}

		nRead = fread(&byte, 1, 1, parser->input);
	}

	// Check if we read something
	if(ptr == parser->lineBuffer)
	{
		ptr[0] = '\0';
		parser->lineStringLength = 0;
		return feof(parser->input) ? CFG_FILE_EOF : CFG_OK;
	}

	// Remove trailing spaces
	do
	{
		--ptr;
	} while( (ptr != parser->lineBuffer) && ((*ptr == ' ') || (*ptr == '\t')) );

	parser->lineStringLength = ptr - parser->lineBuffer;
	ptr[1] = '\0';

	return feof(parser->input) ? CFG_FILE_EOF : CFG_OK;
}

/**
 * \brief Validate string and transform escaped char.
 *
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ValidateString(char* start, char* end)
{
	char *ptr = NULL;

	// Skip starting spaces
	for( ; (start != end) && ((*start == ' ') || (*start == '\t')); ++start)
	{}

	if(start >= end)
	{
		return CFG_EMPTY_STRING;
	}

	// Skip trailing spaces
	for( ; (end != start) && ((*end == ' ') || (*end == '\t')); --end)
	{}

	// Validate the rest of the string
	for(ptr=start; (ptr <= end); ++ptr)
	{
		if(memchr(g_CFGForbiddenChar, *((unsigned char*)ptr), CFG_FORBIDDEN_CHAR_COUNT) != NULL)
		{
			return CFG_INVALID_SECTION_NAME;		
		}
		if(*ptr == '\\')
		{
			char *escPtr;

			if(ptr > end)
			{
				return CFG_INVALID_SECTION_NAME;
			}

			escPtr = (char*)memchr(g_CFGEscapedChar, *((unsigned char*)ptr+1), CFG_ESCAPED_CHAR_COUNT);
			if(escPtr == NULL)
			{
				return CFG_INVALID_SECTION_NAME;		
			}

			ptr[0] = g_CFGTranslatedChar[escPtr - g_CFGEscapedChar];
			--end;
			memmove(ptr+1, ptr+2, end-ptr);
		}
	}

	end[1] = '\0';

	return CFG_OK;
}

/**
 * \brief Validate section.
 *
 * \param parser CFG parser
 * \return \see CFG_ERR
 */
static CFG_ERR ValidateSection(struct CFGParser* parser)
{
	char *ptr;
	size_t i, end; 
	CFG_ERR err = CFG_OK;

	// Look for closing delimiter
	for( end=parser->lineStringLength, ptr=parser->lineBuffer + end;
		 (end > 0) && (*ptr != ']'); 
		 --ptr, --end )
	{}

	if(end == 0)
	{
		return CFG_MISSING_SECTION_DELIMITER;
	}

	// Validate the remaing chars
	for( i=end+1, ptr=parser->lineBuffer+end+1; 
		 (i<parser->lineStringLength) && ((*ptr == ' ' ) || (*ptr == '\t'));
		 ++ptr, ++i )
	{}

	// The only valid string after a section closing delimiter is a commentary
	if((i < parser->lineStringLength) && (*ptr != ';'))
	{
		return CFG_INVALID_SECTION_DECLARATION;
	}
						
	err = ValidateString(parser->lineBuffer+1, parser->lineBuffer+end-1);
	if(err == CFG_OK)
	{
		// call section callback
	}

	return err;
}

/**
 * \brief Parse CFG file
 *
 * \param filename CFG/Ini filename.
 * \param payload User defined callbacks and data.
 * \return \see CFG_ERR
 */
CFG_ERR ParseCFG(const char* filename, struct CFGPayload* payload)
{
	FILE *input;
	struct CFGParser parser;
	CFG_ERR err;

	if(payload == NULL)
	{
		return CFG_INVALID_PARAMETERS;
	}

	if( (payload->data == NULL) || 
		(payload->sectionCallback == NULL) ||
		(payload->keyValueCallback == NULL) )
	{
		return CFG_INVALID_PARAMETERS;
	}

	input = fopen(filename, "rb");
	if(input == NULL)
	{
		fprintf(stderr, "Unable to open %s : %s", filename, strerror(errno));
		return CFG_FILE_OPEN_ERR;
	}
	
	parser.payload          = payload;
	parser.lineBuffer       = NULL;
	parser.lineBufferSize   = 0;
	parser.lineStringLength = 0;
	parser.input            = input;

	err = ResizeLineBuffer(&parser, 256);

	if(err == CFG_OK)
	{
		while(err == CFG_OK)
		{
			err = ReadLine(&parser);

			if((err == CFG_OK) && (parser.lineStringLength))
			{

				if(parser.lineBuffer[0] == '[')
				{
					err = ValidateSection(&parser);
				}
				else if(parser.lineBuffer[0] != ';')
				{
					// todo : Validate key/value pair
				}
			}
		}

		CleanupCFGParser(&parser);
	}

	fclose(input);
	return err;
}