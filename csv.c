#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "csv.h"

#if !HAVE_STRNDUP
#include "strndup.h"
#endif

/*
 * Token serializers
 */
typedef int (*CSVTokenSerialize) (char*, char*, CSVToken*);

/* Integer */
static int CSVTokenSerializeInteger(char* iStart, char* iEnd, CSVToken* iToken) {
	iEnd = NULL;
	iToken->value.integer = atoi(iStart);
	return 1;
}
 
/* String */
static int CSVTokenSerializeString(char* iStart, char* iEnd, CSVToken* iToken) {
	iToken->value.string = (char*)strndup(iStart, iEnd - iStart);
	return (iToken->value.string != NULL);
}

/*
 * All the token serializers are stored in this array
 */
static CSVTokenSerialize serializer[TOKENTYPECOUNT] = {
	CSVTokenSerializeInteger,
	CSVTokenSerializeString
};

/*
 * Initialize state
 */ 
void CSVSetState(char* iStart, char* iEnd, CSVState* iState) {
	iState->start   = iStart;
	iState->end     = iEnd;
	iState->current = iStart;
	iState->nextLine = (char*)memchr(iState->start, '\n', iState->end - iState->start);
	
	if(iState->nextLine == NULL) {
		iState->nextLine = iState->end;
	}
}

/* 
 * Extract tokens from current line
 */
int CSVExtractTokensFromLine(CSVState* iState,
							 char  iSeparator, CSVToken* iToken,
							 int   iTokenCount) {
	char*  next;
	int	   idx;

	/* For each token found in the line */
	for(idx=0;
	    ((next = (char*)memchr(iState->current, iSeparator, iState->nextLine - iState->current)) != NULL) &&
		(idx < iTokenCount);
  		iState->current=next+1, ++idx) {
  			/* Serialize it according to its type */
  			if(!serializer[iToken[idx].type](iState->current, next, iToken+idx))
				return 0;
  	}
  	
  	/* Extract last token before the end (if needed) */
  	if((iState->current < iState->nextLine) && (idx < iTokenCount)) {
		if(!serializer[iToken[idx].type](iState->current, iState->nextLine, iToken+idx))
			return 0;
		iState->current = iState->nextLine+1;
		++idx;
  	}

  	return idx;
}

/*
 * Jump to next line
 */
int CSVJumpToNextLine(CSVState* iState) {
	iState->current = iState->nextLine+1;
	if(iState->current >= iState->end) return 0;

	// Search for next line
  	if((iState->nextLine = (char*)memchr(iState->current, '\n', iState->end - iState->current)) == NULL) {
  		iState->nextLine = iState->end;
  	}
  	
  	return 1;
}

#ifdef DEBUG_CSV

int main() {
	char* str = "string0;-129;another string;-123;next?; +-123\n"
	"12;13516;line 1;;s\n"
	"abc;42c;\n"
	";;;;";

	CSVState state;
	int i, j, found;
	CSVToken token[4];
	token[0].type = STRING;
	token[1].type = INTEGER;
	token[2].type = STRING;
	token[3].type = STRING;
	
	CSVSetState(str, str + strlen(str), &state);
	
	j = 0;
	do {
		printf("Line : %d\n", j);
		
		found = CSVExtractTokensFromLine(&state, ';', token, 4);
		printf("\tToken found : %d\n", found);
		
		for(i=0; i<found; ++i) {
			if(token[i].type == INTEGER)
				printf("\tinteger : %d\n",token[i].value.integer);
			else
				printf("\tstring : \"%s\"\n",token[i].value.string);
		}

		for(i=0; i<found; ++i) {
			if(token[i].type == STRING)
				free(token[i].value.string);
		}
		
		++j;
	}while(CSVJumpToNextLine(&state));
	
	return 0;
}

#endif
