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
		return CFG_FOPEN_ERR;
	}
	
	parser.payload    = payload;
	parser.lineBuffer       = NULL;
	parser.lineBufferSize   = 0;
	parser.lineStringLength = 0;
	
	err = ResizeLineBuffer(&parser, 256);

	if(err == CFG_OK)
	{
		// TODO

		CleanupCFGParser(&parser);
	}

	fclose(input);
	return err;
}