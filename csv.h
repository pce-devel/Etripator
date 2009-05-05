#ifndef _CVS_H_

/*
 * - CSV Token -
 * Supported token types are :
 * 		1. 32 bits signed integers
 *      2. ASCII strings
 */
enum CSVTokenType {
	INTEGER = 0,
	STRING,
	TOKENTYPECOUNT
};

struct CSVToken_ {
	unsigned char type;
	union {
		int   integer;
		char* string;
	} value;
};
typedef struct CSVToken_ CSVToken;

/*
 * - CSV State -
 */
struct CSVState_ {
	char* start;		/* CSV buffer start					*/
	char* end;			/* CSV buffer end   				*/
	char* current;		/* current position in CVS buffer	*/
	char* nextLine;		/* pointer to next line				*/
};
typedef struct CSVState_ CSVState;

/*
 * - Initialize state -
 * Arguments :
 * 		1. iStart : Pointer to the begining of the buffer
 * 		2. iEnd	  : Pointer to the end of the buffer
 * 		3. iState : CSV state to set
 */
void CSVSetState(char* iStart, char* iEnd, CSVState* iState);

/* 
 * - Extract tokens from line -
 * Arguments :
 *		1. iState		: CSV file state
 * 		2. iSeparator	: separator
 * 		3. iToken		: pointer to token array
 * 		4. iTokenCount	: number of tokens to extract
 * 
 * Return value:
 * 		number of extracted tokens
 *      0 if an error occured.
 */
int CSVExtractTokensFromLine(CSVState* iState,
							 char  iSeparator, CSVToken* iToken,
							 int   iTokenCount);

/*
 * - Jump to next line -
 * Arguments :
 * 		1. iState	: CSV state
 *
 * Return value:
 * 		1 : Buffer end is not reached 
 * 		0 : Can't jump
 */
int CSVJumpToNextLine(CSVState* iState);

#endif // _CSV_H_
