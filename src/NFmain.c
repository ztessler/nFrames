#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <NFobjects.h>

typedef enum { NFskip, NFsilent, NFreport } NFaction;

int main (int argc, char *argv[])
{
	FILE *inFile = stdin;
	int argPos, argNum = argc, opt;
	float precision;
	const char   *options [] = { "skip",   "silent",   "report", (char *) NULL };
	NFaction opts [] = { NFskip, NFsilent, NFreport };
	NFaction parse   = NFsilent;
	NFaction resolve = NFsilent;
	NFaction execute = NFsilent;
	NFcompModel_p model = (NFcompModel_p) NULL;

	CMmathEqualValues (0.0,0.0); // TODO This is here to make sure that lsmf.bin includes CMmathEqualValues
	for (argPos = 1;argPos < argNum;) {
		if (CMargTest(argv[argPos],"-a","--parse")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) {
				CMmsgPrint (CMmsgUsrError,"Missing parse option!\n");
				goto Help;
			}
			if ((opt = CMoptLookup (options + 1,argv [argPos],true)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed parse option [%s]!\n",argv [argPos]);
				goto Help;
			}
			parse = opts [opt + 1];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-e","--resolve")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) {
				CMmsgPrint (CMmsgUsrError,"Missing resolve option!\n");
				goto Help;
			}
			if ((opt = CMoptLookup (options,argv [argPos],true)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed resolve option [%s]!\n",argv [argPos]);
				goto Help;
			}
			resolve = opts [opt];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-x","--execute")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) {
				CMmsgPrint (CMmsgUsrError,"Missing execute option!\n");
				goto Help;
			}
			if ((opt = CMoptLookup (options,argv [argPos],true)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed execute option [%s]!\n",argv [argPos]);
				goto Help;
			}
			execute = opts [opt];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-p","--precision")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) {
				CMmsgPrint (CMmsgUsrError,"Missing precision!\n");
				goto Help;
			}
			if (sscanf (argv[argPos],"%f",&precision) != 1) {
				CMmsgPrint (CMmsgUsrError,"Invalid precision [%s]!\n",argv[argPos]);
				goto Help;
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-m","--message")) {
			const char *types [] = { "sys_error", "app_error", "usr_error", "debug", "warning", "info", (char *) NULL };
			CMmsgType msgTypes [] = { CMmsgSysError, CMmsgAppError, CMmsgUsrError, CMmsgDebug, CMmsgWarning, CMmsgInfo };
			int type;
			const char *modes [] = { "file:", "on", "off", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing message argument!\n");
				return (CMfailed);
			}

			if ((type = CMoptLookup (types,argv [argPos],false)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!\n",argv [argPos]);
			}
			else {
				switch (CMoptLookup (modes, argv [argPos] + strlen (types [type]) + 1, false)) {
					case 0: CMmsgSetStreamFile (msgTypes [type], argv [argPos] + strlen (types [type]) + 1 + strlen (modes [0]));
					case 1: CMmsgSetStatus     (msgTypes [type], true);  break;
					case 2: CMmsgSetStatus     (msgTypes [type], false); break;
					default:
						CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!\n",argv [argPos]);
						break;
				}
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-h","--help")) goto Help;
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]);
			goto Help;
		}
      argPos++;
	}
	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); goto Help; }

	if ((inFile  = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? fopen (argv [1],"r") : stdin)  == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Input file opening error\n");
		perror (":");
		goto Abort;
	}

	if ((model = NFmodelParse   (inFile, parse   == NFreport)) == (NFcompModel_p) NULL) goto Abort;
	if (resolve != NFskip) {
		if (     NFmodelResolve (model,  resolve == NFreport) == CMfailed) goto Abort;
		if (execute != NFskip) {
			if ( NFmodelExecute (model,  execute == NFreport) == CMfailed) goto Abort;
		}
	}
	NFobjectFree  ((NFobject_p) model);
	if (inFile != stdin) fclose (inFile);
	return (0);
Abort:
	if (model != (NFcompModel_p) NULL) NFobjectFree ((NFobject_p) model);
	if (inFile != stdin) fclose (inFile);
	return (CMfailed);
Help:
	CMmsgPrint (CMmsgUsrError,"%s [options] <model xml>\n",CMprgName(argv[0]));
	CMmsgPrint (CMmsgUsrError,"  -a, --parse   [silent|report]\n");
	CMmsgPrint (CMmsgUsrError,"  -e, --resolve [silent|skip|report]\n");
	CMmsgPrint (CMmsgUsrError,"  -x, --execute [silent|skip|report]\n");
	CMmsgPrint (CMmsgUsrError,"  -p, --precision <value>\n");
	CMmsgPrint (CMmsgUsrError,"  -m, --message [sys_error|app_error|usr_error|debug|warning|info]=[on|off|file=<filename>]\n");
	CMmsgPrint (CMmsgUsrError,"  -h,--help\n");
	return (CMsucceeded);
}
