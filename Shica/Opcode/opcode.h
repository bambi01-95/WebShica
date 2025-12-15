#ifndef OPCODE_H
#define OPCODE_H

enum opcode_t {
    iHALT, 
    iPUSH,
    sPUSH,
    iARRAY,    //initialize array with n elements a[2][2] = {{...}, {...}}
    iSETARRAY, // set array element a[0] = x
    iGETARRAY, // get array element x = a[0]
    iGT, 
    iGE, 
    iEQ, 
    iNE, 
    iLE, 
    iLT,
    iADD, 
    iSUB, 
    iMUL, 
    iDIV, 
    iMOD,

    iGETGLOBALVAR,//don't change the order of these three
    iGETSTATEVAR,
    iGETVAR, 

    iSETGLOBALVAR,// don't change the order of these three
	iSETSTATEVAR,
    iSETVAR,

	iMKSPACE,
    aPRINT,// abstract print
	iPRINT,
    fPRINT,
    sPRINT,
    flashPRINT,
     // print integer, float, string
	iJUMP, // jump to a position
	iJUMPIF, // jump if top of stack is true
	iJUDGE,  //for event condtion
	iPCALL, iUCALL,iSCALL, // call a function
    eCALL, // call an event handler and push the context
	iRETURN, // return from a function
	iCLEAN,//
	iTRANSITION,
	iSETSTATE,//NE: Number of events in the state
	iSETEVENT,//ET: Event type, NP: Number of process
	iSETPROCESS,
	iEOE,
	iEOC,
	iEOS, // end of state
	iIMPL, // aPos, cPos
};
typedef enum opcode_t opcode_t;

#ifdef DEBUG
static const char *_opcode_names[] = {
    "iHALT",
    "iPUSH",
    "sPUSH",
    "iARRAY",
    "iSETARRAY",
    "iGETARRAY",
    "iGT",
    "iGE",
    "iEQ",
    "iNE",
    "iLE",
    "iLT",
    "iADD",
    "iSUB",
    "iMUL",
    "iDIV",
    "iMOD",
    "iGETGLOBALVAR",
    "iGETSTATEVAR",
    "iGETVAR",
    "iSETGLOBALVAR",
    "iSETSTATEVAR",
    "iSETVAR",
    "iMKSPACE",
    "aPRINT",
    "iPRINT",
    "fPRINT",
    "sPRINT",
    "flashPRINT",
    "iJUMP",
    "iJUMPIF",
    "iJUDGE",
    "iPCALL",
    "iUCALL",
    "iSCALL",
    "eCALL",
    "iRETURN",
    "iCLEAN",
    "iTRANSITION",
    "iSETSTATE",
    "iSETEVENT",
    "iSETPROCESS",
    "iEOE",
    "iEOC",
    "iEOS",
    "iIMPL",
};
const char *getOpcodeName(int opcode);
#define printOP(op) printf("%s\n", getOpcodeName(op))
#else
const char *getOpcodeName(int opcode);
#define printOP(op) (void)0
#endif // DEBUG


#endif // OPCODE_H