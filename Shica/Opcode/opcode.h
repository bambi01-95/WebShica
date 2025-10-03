#ifndef OPCODE_H
#define OPCODE_H

enum opcode_t {
    iHALT, 
    iPUSH,
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
	iPRINT,
	iJUMP, // jump to a position
	iJUMPIF, // jump if top of stack is true
	iJUDGE,  //for event condtion
	iPCALL, iUCALL,iSCALL, // call a function
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
    "iGETVAR", 
    "iGETGLOBALVAR",
    "iGETSTATEVAR",
    "iSETVAR",
    "iSETGLOBALVAR",
    "iSETSTATEVAR",
    "iMKSPACE",
    "iPRINT",
    "iJUMP", // jump to a position
    "iJUMPIF", // jump if top of stack is true
    "iJUDGE",  //for event condtion
    "iPCALL", 
    "iUCALL",
    "iSCALL", // call a function
    "iRETURN", // return from a function
    "iCLEAN",//
    "iTRANSITION",
    "iSETSTATE",//NE: Number of events in the state
    "iSETEVENT",//ET: Event type, NP: Number of process
    "iSETPROCESS",
    "iEOE",
    "iEOC",
    "iEOS", // end of state
    "iIMPL", // aPos, cPos
};
const char *getOpcodeName(int opcode);
#define printOP(op) printf("%s\n", getOpcodeName(op))
#else
const char *getOpcodeName(int opcode);
#define printOP(op) (void)0 
// no-op in release mode
#endif // DEBUG


#endif // OPCODE_H