#include "message.h"

static const char* g_MessageTypeLabel[] =
{
	"[Error]",
	"[Warning] ",
	"[Information] "
};

static FILE *g_Stream;

/**
 * \brief Open the file where the message will be printed. And set hook to print into a file.
 *
 * \param filename  Name of the file where the message will be printed.
 * \return 0 if an error occured, 1 on success.
*/
int PrintMsgOpenFile(const char* filename)
{
	g_Stream = stderr;
	if(filename != NULL)
	{
		FILE *out;
		out = fopen(filename, "wb+");
		if(out == NULL)
		{
			ERROR_MSG("An error occured while trying to open %s : %s", filename, strerror(errno));
			return 0;
		}
		g_Stream = out;
	}
	return 1;
}

/**
 * \brief Close file used for printing messages.
*/
static void PrintMsgCloseFile()
{
	if((g_Stream != NULL) && (g_Stream != stderr))
	{
		fclose(g_Stream);
	}
}

/**
 * \brief Print message to a file.
 *
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 */
static void PrintMsgFile(MessageType msgType, const char* file, size_t line, const char* function, const char* format, ...)
{
	va_list argList;

#ifdef DEBUG
	fprintf(g_Stream, "%s %s:%d %s : ", g_MessageTypeLabel[msgType], file, line, function);
#else
	fprintf(g_Stream, "%s ", g_MessageTypeLabel[msgType]);
#endif // DEBUG

	va_start(argList, format);
	vfprintf(g_Stream, format, argList);
	va_end(argList);
	
	fputc('\n', g_Stream);
	fflush(g_Stream);
}

PrintMsgProc      PrintMsg      = PrintMsgFile;
PrintMsgCloseProc PrintMsgClose = PrintMsgCloseFile; 