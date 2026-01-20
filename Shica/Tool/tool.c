#ifndef TOOL_C
#define TOOL_C
#include "./tool.h"
#ifdef DEBUG

#ifdef SHICAEXEC
#define ELE (code->IntArray)
#else
#define ELE (code->Intruction)
#endif
void printCode(
#ifdef SHICAEXEC
    oop code
#else
    node code
#endif 
){
	for (int i = 0; i < ELE.size; ++i) {
		int op = ELE.elements[i];
		const char *inst = "UNKNOWN";
		switch (op) {
			case iHALT:
				inst = "HALT";
				printf("%03d: %-10s\n", i, inst);
				break;
			case iPUSH:
				inst = "LOAD";
				int value = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, value);
				break;
			case sPUSH:{
				inst = "LOADS";
				int sLen = ELE.elements[++i];
				char sValue[sLen+1];
				memcpy(sValue, &ELE.elements[++i], sLen);
				sValue[sLen] = '\0';
				printf("%03d: %-10s \"%s\"\n", i-2, inst, sValue);
				if(sLen % sizeof(int) == 0)
					i += sLen/sizeof(int) - 1;
				else
					i += sLen/sizeof(int);
				break;
			}
			case iARRAY: {
				inst = "ARRAY";
				int nElements = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nElements);
				break;
			}
			case iSETARRAY: {
				inst = "SETARRAY";
				printf("%03d: %-10s\n", i, inst);
				break;
			}
			case iGETARRAY: {
				inst = "GETARRAY";
				printf("%03d: %-10s\n", i, inst);
				break;
			}
			case iGT:   inst = "GT"; goto simple;
			case iGE:   inst = "GE"; goto simple;
			case iEQ:   inst = "EQ"; goto simple;
			case iNE:   inst = "NE"; goto simple;
			case iLE:   inst = "LE"; goto simple;
			case iLT:   inst = "LT"; goto simple;
			case iADD:  inst = "ADD"; goto simple;
			case iSUB:  inst = "SUB"; goto simple;
			case iMUL:  inst = "MUL"; goto simple;
			case iDIV:  inst = "DIV"; goto simple;
			case iMOD:  inst = "MOD"; goto simple;
			case iJUDGE:inst = "JUDGE"; goto simple;
			case iEOC:  inst = "EOC"; goto simple;
			case iRETURN: inst = "RETURN"; goto simple;
simple:
				printf("%03d: %-10s\n", i, inst);
				break;
			case iEOE:{
				inst = "EOE";
				int nVariables = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iEOS:{
				inst = "EOS";
				int nVariables = ELE.elements[++i];// FIXME: this is not correct instruction
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iGETVAR:
				inst = "GETVAR";
				int varIndex = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndex);
				break;
			case iSETVAR:
				inst = "SETVAR";
				int varIndexSet = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndexSet);
				break;
			case iGETSTATEVAR:
				inst = "GETSTATEVAR";
				int sVarIndex = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndex);
				break;
			case iSETSTATEVAR:
				inst = "SETSTATEVAR";
				int sVarIndexSet = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndexSet);
				break;
			case iGETGLOBALVAR:
				inst = "GETGLOBALVAR";
				int gVarIndex = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndex);
				break;
			case iSETGLOBALVAR:
				inst = "SETGLOBALVAR";
				int gVarIndexSet = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndexSet);
				break;
			case iINITSPACE:
				inst = "INITSPACE";
				int nLocals = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nLocals);
				break;
			case iMKSPACE:
				inst = "MKSPACE";
				int space = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, space);
				break;
			case aPRINT: {
				inst = "aPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case iPRINT: {
				inst = "PRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case fPRINT: {
				inst = "fPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case sPRINT: {
				inst = "sPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case flashPRINT: {
				inst = "flashPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case iJUMP: {
				inst = "JUMP";
				int offset = ELE.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iJUMPIF: {
				inst = "JUMPIF";
				int offset = ELE.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iCLEAN: {
				inst = "CLEAN";
				int nArgs = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nArgs);
				break;
			}
			case iPCALL: {
				inst = "PCALL";
				int index = ELE.elements[++i];
				int nArgs = ELE.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, index, index + (i+1), nArgs);
				break;
			}
			case iSCALL: {
				inst = "SCALL";
				int index = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iUCALL: {
				inst = "CALL";
				int pos = ELE.elements[++i];
				int nArgs = ELE.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, pos, (i+1)+pos, nArgs);
				break;
			}
			case eCALL:{
				inst = "eCall";
				int index = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iTRANSITION: {
				inst = "TRANSITION";
				int pos = ELE.elements[++i];
				printf("%03d: %-10s %03d(%d)\n", i-1, inst, pos,(i+1)+pos);
				break;
			}
			case iSETSTATE: {
				inst = "SETSTATE";
				int nEvents = ELE.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nEvents);
				break;
			}
			case iSETEVENT: {
				inst = "SETEVENT";
				int eventID = ELE.elements[++i];
				int nHandlers = ELE.elements[++i];
				printf("%03d: %-10s %03d %3d\n", i-2, inst, eventID, nHandlers);
				break;
			}
			case iSETPROCESS: {
				inst = "SETPROCESS";
				int pc = i;
				int apos = ELE.elements[++i];
				int cpos = ELE.elements[++i];
				printf("%03d: %-10s %03d %3d (%3d %3d)\n", i-2, inst, apos, cpos, pc + apos, pc + cpos);
				break;
			}
			case iIMPL:
				inst = "IMPL";
				printf("%03d: %-10s\n", i, inst);
				break;
			default:
				printf("%03d: %-10s %03d\n", i, inst, op);
		}
	}
	printf("\n");
}
#undef ELE

#endif // DEBUG
#endif // TOOL_C