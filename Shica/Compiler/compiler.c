#ifndef COMPILER_C
#define COMPILER_C
#include "./compiler.h"



static int emitOn(oop prog,node vars, node ast, node type);

/* EVENT HANDLER */
int compile_event_init(){
	//lsl event handler
	entryEH = intern("entryEH");
	entryEH->Symbol.value = newEventH(0); // 0 argument
	exitEH = intern("exitEH");
	exitEH->Symbol.value =  newEventH(0); // 0 argument
	//standard event handler
	setEventTable(__EventTable__);
	setStdFuncTable(__StdFuncTable__);
	compile_eh_init();//platform/platformName/library.c

	return 1; // return 1 to indicate success
}
/* STANDARD LIBRARY */
//compile_func_init();//platform/platformName/library.c

/* ==== STATE ==== */

// STATESSm
// s1: pos, name;
// s2: pos, name;
// s3: pos, name;
enum { APSTATE = 0, ATRANSITION = 1 };

static node *states = 0;
static int nstates = 0;
static node *transitions = 0; // transitions between states
static int ntransitions = 0;

// GC
int initSttTrans()
{
	states = NULL;
	nstates = 0;
	transitions = NULL;
	ntransitions = 0;
	return 0; 
}

int collectSttTrans()
{
	if(nstates != 0){
		gc_markOnly(states); // mark the states array itself
		for(int i = 0;  i < nstates;  ++i)
		{
			node state = states[i];
			if (state == NULL) continue; // skip null states
			gc_mark(state); //PAIR
		}
	}
	if(ntransitions != 0){
		gc_markOnly(transitions); // mark the transitions array itself
		for(int i = 0;  i < ntransitions;  ++i)
		{
			node trans = transitions[i];
			if(trans == NULL) continue; // skip null transitions
			gc_mark(trans);//PAIR
		}
	}
	return 0; 
}


static void appendS0T1(node name, int pos,int type)
{
	if(type != 0 && type != 1) {
		dprintf("type must be 0 or 1, got %d\n", type);
		return;//error
	}
	node *lists = type == APSTATE ? states : transitions;
	int *listSize = type == APSTATE ? &nstates : &ntransitions;

	gc_pushRoot((void*)&name);
	switch(type){
		case APSTATE:{
			for (int i = 0;  i < *listSize;  ++i) {
				node stateName = getNode(lists[i], Pair,a);
				if (stateName == name) {
					dprintf("state %s already exists\n", getNode(name, Symbol,name));
					return;
				}
			}
			states = realloc(states, sizeof(node) * (nstates + 1));
			states[nstates] = newPair(name, newInteger(pos));
			nstates++;
			break;
		}
		case ATRANSITION:{
			transitions = realloc(transitions, sizeof(node) * (ntransitions + 1));
			transitions[ntransitions] = newPair(name, newInteger(pos));
			ntransitions++;
			break;
		}
	}
	gc_popRoots(1);
	return;
}

static void setTransPos(oop prog){
	int *code = prog->IntArray.elements;
	for (int i = 0;  i < ntransitions;  ++i) {
		node trans = transitions[i];
		node transName = getNode(trans, Pair,a);
		int transPos = Integer_value(getNode(trans, Pair,b));//pos of after emitII(prog, iTRANSITION, 0);
		for (int j = 0;  j < nstates;  ++j) {
			node state = states[j];
			node stateName = getNode(state, Pair,a);
			if (stateName == transName) {
				int statePos = Integer_value(getNode(state, Pair,b));
				code[transPos-1] = statePos - transPos;//relative position
			}
		}
	}
}

#if DEBUG
//red color for debug print
#define printTYPE(OP) dprintf("\x1b[31memit %s\x1b[0m\n", #OP)
#else
#define printTYPE(OP) ;
#endif


static void emitL (oop array, node object) 	  { intArray_append(array, Integer_value(object)); }
static void emitI (oop array, int i     ) 	  { intArray_append(array, i); }
static void emitII(oop array, int i, int j)      { emitI(array, i); emitI(array, j); }
static void emitIL(oop array, int i, node object) { emitI(array, i); emitL(array, object); }
static void emitIII(oop array, int i, int j, int k)
{
	emitI(array, i);
	emitI(array, j);
	emitI(array, k);
}

static int parseType(node vars, node ast)
{
	switch(getType(ast)){
		case Integer:return Integer;
		case String:return String;
		case GetVar:{
			struct RetVarFunc var = searchVariable(vars, getNode(ast, GetVar,id), NULL);
			if(var.index == -1)return -1; // variable not found
			return GET_OOP_FLAG(var.variable->Variable.type);
		}
		default:
			reportError(DEVELOPER, 0, "unknown AST type %d", getType(ast));
			return -1;
	}
	reportError(DEVELOPER, 0, "unknown AST type %d", getType(ast));
	return -1; // should never reach here
}

// 0: success
// 1: error
static int parseArray(oop prog, cnode array,cnode index,cnode type, cnode vars){
	if(index == nil)return 0;
	const int size = Integer_value(getNode(index, Pair,a));
	if(size!=getNode(array, Array,size)){
		reportError(DEVELOPER, 0, "array size mismatch: expected %d, got %d", size, getNode(array, Array,size));
		return 1;
	}
	cnode *elements = getNode(array, Array,elements);
	for(int i=0;i<size;i++){
		if(getType(elements[i]) == Array){
			if(parseArray(prog, elements[i], getNode(index, Pair,b), type, vars))return 1;
		}else {
			if(emitOn(prog, vars, elements[i], type))return 1;
		}
	}
	emitII(prog, iARRAY, size);
	return 0;
}

static int isAtomicType(node type)
{
	if(type == TYPES[Integer]) return 1;
	if(type == TYPES[Float]) return 1;
	if(type == TYPES[String]) return 1;
	if(type == TYPES[Undefined]) return 1;
	return 0;
}

static node getElementType(cnode emitCTX,cnode id)
{
	assert(getType(getNode(emitCTX, EmitContext, user_types)) == Array);
	cnode *user_types = getNode(emitCTX, EmitContext, user_types)->Array.elements;
	int nUserTypes = getNode(emitCTX, EmitContext, user_types)->Array.size;
	for(int i=0;i<nUserTypes;i++){
		cnode user_type = user_types[i]->Pair.a;
		if(user_type == id){
			return user_types[i]->Pair.b;
		}
	}
	return nil;
}

static int emitOn(oop prog,node vars, node ast, node type)
{
	assert(getType(vars) == EmitContext);
	int ret = 0; /* ret 0 indicates success */
    switch (getType(ast)) {
		case Undefined:
		case Integer:{
			printTYPE(_Integer_);
			if(type == TYPES[Undefined]){
				// allow any type
				dprintf("[C] type undefined, allow any type\n");
			}else if(type != TYPES[Integer]){
				dprintf("[C] type mismatch: expected %d, got Integer\n", GET_OOP_FLAG(type));
				reportError(ERROR, 0, "type mismatch: expected %d, got Integer", GET_OOP_FLAG(type));
				return 1;
			}
			emitIL(prog, iPUSH, ast);
			return 0;
		}
		case String:{
			printTYPE(_String_);
			if(type == TYPES[Undefined]){
				// allow any type
				dprintf("[C] type undefined, allow any type\n");
			}else if(type != TYPES[String]){
				reportError(ERROR, 0, "type mismatch: expected %d, got String", GET_OOP_FLAG(type));
				return 1;
			}
			char* str = getNode(ast, String,value);
			int sLen = strlen(str);
			emitII(prog, sPUSH, sLen); // push the length of the string
			for(int i = 0; i < sLen; i++){
				int data = 0;
				for(int j = 0; j < 4; j++, i++){
					if(i < sLen){
						data |= (str[i] & 0xFF) << (j * 8);
					}else{
						data |= (0 & 0xFF) << (j * 8);
					}
				}
				intArray_append(prog, data);
				--i;
			}
			return 0;
		}
		case Array:{
			printTYPE(_Array_);
			if(isAtomicType(type))
			{
				node *elements = getNode(ast, Array,elements);
				int size = getNode(ast, Array,size);
				emitII(prog, iPUSH, size); // push the size of the array
				for (int i = 0;  i < size;  ++i) {
					if(emitOn(prog, vars, elements[i], type))return 1; // compile each element
				}
			}else{//User-defined type
				assert(getType(type) == Array);
				int arrSize = getNode(ast, Array,size);
				node *arrs = getNode(ast, Array,elements);
				int typeSize= getNode(type, Array,size);
				node *types = getNode(type, Array,elements);//Variable
				if(arrSize != typeSize){
					reportError(ERROR, 0, "element size mismatch: expected %d, got %d", typeSize, arrSize);
					return 1;
				}
				for(int i=0;i<arrSize;i++){
					node t = getNode(types[i], Variable,type);
					if(!isAtomicType(t))stop();//removeme
					if(emitOn(prog, vars, arrs[i], t))return 1;
				}
				emitII(prog, iARRAY, arrSize);//user defined type (that deal as array)
			}
			return 0; 
		}
		case Binop:{
			printTYPE(_Binop_);
			if(emitOn(prog, vars, getNode(ast, Binop,lhs), type)) return 1;
			if(emitOn(prog, vars, getNode(ast, Binop,rhs), type)) return 1;
			switch (getNode(ast, Binop,op)) {
				case NE: emitI(prog, iNE);  return 0; // emit not equal
				case EQ: emitI(prog, iEQ);  return 0;
				case LT: emitI(prog, iLT);  return 0;
				case LE: emitI(prog, iLE);  return 0;
				case GE: emitI(prog, iGE);  return 0;
				case GT: emitI(prog, iGT);  return 0;
				case ADD: emitI(prog, iADD);  return 0;
				case SUB: emitI(prog, iSUB);  return 0;
				case MUL: emitI(prog, iMUL);  return 0;
				case DIV: emitI(prog, iDIV);  return 0;
				case MOD: emitI(prog, iMOD);  return 0;
				default:break;
			}
			fatal("file %s line %d unknown Binop operator %d", __FILE__, __LINE__, getNode(ast, Binop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Unyop:{
			printTYPE(_Unyop_);
			node rhs = getNode(ast, Unyop,rhs);
			switch (getNode(ast, Unyop,op)) {
				case NEG: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs, type); emitI(prog, iSUB); return 0;
				case NOT: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs, type); emitI(prog, iEQ);  return 0;
				default: break;
			}
			struct RetVarFunc var = searchVariable(vars, getNode(rhs, GetVar,id), type);
			if(var.index == -1)return 1; // variable not found
			switch(getNode(ast, Unyop,op)) {
					case BINC: if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, var.index);if(emitOn(prog, vars, rhs, type))return 1; return 0;
					case BDEC: if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, var.index); if(emitOn(prog, vars, rhs, type))return 1;return 0;
					case AINC: if(emitOn(prog, vars, rhs, type))return 1;if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, var.index); return 0;
					case ADEC: if(emitOn(prog, vars, rhs, type))return 1;if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, var.index); return 0;
				default: break;
			}
			fatal("file %s line %d unknown Unyop operator %d", __FILE__, __LINE__, getNode(ast, Unyop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case GetVar: {
			printTYPE(_GetVar_);
			node sym = getNode(ast, GetVar,id);
			struct RetVarFunc var = searchVariable(vars, sym, type);
			if(var.index == -1)return 1; // variable found
			emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the global variable value
			return 0;
		}
		case SetVar: {
			printTYPE(_SetVar_);
			node sym = getNode(ast, SetVar,id);
			node rhs = getNode(ast, SetVar,rhs);
			node declaredType = getNode(ast, SetVar,type);
			switch(getType(rhs)){
				case UserFunc:{
					printTYPE(_Function_);
					if(sym->Symbol.value != FALSE) {
						reportError(ERROR, getNode(ast,SetVar,line), "variable %s is already defined as a function", getNode(sym, Symbol,name));
						return 1;
					}
					node params = getNode(rhs, UserFunc,parameters);
					node body = getNode(rhs, UserFunc,body);
					getNode(vars, EmitContext, local_vars) = newArray(0);
					GC_PUSH(node, closure, newClosure());
					GC_PUSH(int*, argTypes, NULL);
					while(params != nil){
						node sym = getNode(params, Params, id);
						node type = getNode(params, Params, type);
						struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, local_vars), sym, type, NULL); // insert parameter into local variables
						if(var.index == -1){
							GC_POP(argTypes);
							GC_POP(closure);
							return 1; // type error
						}
						appendNewInt(argTypes, ++(closure->Closure.nArgs), GET_OOP_FLAG(type)); // append parameter type to argument types
						params = getNode(params, Pair,b);
					}
					closure->Closure.argTypes = argTypes; // set the argument types
					closure->Closure.retType = GET_OOP_FLAG(declaredType); // set the return type
					emitII(prog, iJUMP, 0); //jump to the end of the function // TODO: once call jump 
					int jump4EndPos = prog->IntArray.size - 1; // remember the position of the jump // TODO: once call jump
					closure->Closure.pos = prog->IntArray.size; // remember the position of the closure
					sym->Symbol.value = closure; // set the closure as the value of the variable
					emitII(prog, iMKSPACE, 0); // reserve space for local variables
					int codePos = prog->IntArray.size - 1; // remember the position of the function code
					if(emitOn(prog, vars, body, declaredType)) {//declated type is the return type
						GC_POP(closure);
						return 1; // compile function body
					}
					prog->IntArray.elements[codePos] = getNode(getNode(vars, EmitContext, local_vars), Array, size); // set the size of local variables
					prog->IntArray.elements[jump4EndPos] = (prog->IntArray.size - 1) - jump4EndPos; // set the jump position to the end of the function // TODO: once call jump
					GC_POP(closure);
					getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
					return 0;
				}
				case Call:{
					printTYPE(_Call_);

					node func = getNode(rhs, Call, function)->GetVar.id->Symbol.value;

					if(func ==  FALSE){
						reportError(ERROR, 
							getNode(ast,SetVar,line), "variable %s is not defined", 
							getNode(rhs, Call,function)->GetVar.id->Symbol.name);
						return 1;
					}
					switch(getType(func)){
						case EventObject:{//var t = timer();
							printTYPE(__EventObject__);
							node args = getNode(rhs, Call, arguments);
							int index = getNode(func, EventObject, index); // get the index of the event object
							struct EventObjectTable eo = EventObjectTable[index];
							node* funcs = getNode(func, EventObject, funcs);
#ifndef NDEBUG
							for(int i = 0; i < eo.nFuncs; i++){
								if(funcs[i] == NULL){
									reportError(DEVELOPER, getNode(ast,SetVar,line), "event object %s function %d is not defined", getNode(sym, Symbol,name), i);
									return 1;
								}
							}
#endif
							int argsCount = 0;
							while(args != nil){
								if(argsCount >= eo.nArgs){
									reportError(ERROR, 
										getNode(ast,SetVar,line), "event object %s expects %d arguments, but got more", 
										getNode(sym, Symbol,name), eo.nArgs);
									dprintf("index %d eo.nArgs %d argsCount %d\n", index, eo.nArgs, argsCount);
									return 1;
								}
								node arg = getNode(args, Args, value);
								if(emitOn(prog, vars, arg, TYPES[eo.argTypes[argsCount]]))return 1; // compile argument
								args = getNode(args, Args, next);
								argsCount++;
							}
							if(eo.nArgs != argsCount){
								reportError(ERROR, getNode(ast,Call,line), "event object %s expects %d arguments, but got %d", getNode(sym, Symbol,name), eo.nArgs, argsCount);
								return 1;
							}
							//initialize the event handler opcode
							emitII(prog, eCALL, index); // call the event object and push it to top of the stack
							switch(getNode(ast, SetVar,scope)) {
								case SCOPE_LOCAL:{
									reportError(ERROR, getNode(ast,SetVar,line), "cannot define event object %s as local variable", getNode(sym, Symbol,name));
									return 1;
								}
								case SCOPE_STATE_LOCAL:{
									struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, state_vars),sym, nil/*Undefined*/, func);
									emitII(prog, iSETSTATEVAR, var.index);
									return 0;
								}
								case SCOPE_GLOBAL:{
									struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, global_vars),sym, nil/*Undefined*/, func);
									emitII(prog, iSETGLOBALVAR, var.index);
									return 0;
								}
							}
							reportError(DEVELOPER,getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
							return 1;
						}
						default:{
							fatal("file %s line %d emitOn: unknown Call type %d", __FILE__, __LINE__, getType(func));
							reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
							return 1;
						}
					}
				}
				case Array:{//UserType
					if(isAtomicType(declaredType)){
						reportError(DEVELOPER, getNode(ast,SetVar,line), "variable %s declared type cannot be atomic type", getNode(sym, Symbol,name));
						return 1;
					}
					assert(getType(declaredType) == Symbol);
					node userTypes = vars->EmitContext.user_types;
					assert(getType(userTypes) == Array);
					int size = userTypes->Array.size;
					node *ele = userTypes->Array.elements;
					for(int i=0;i<size;i++){//search for user type
						if(declaredType == getNode(ele[i], Pair, a)){
							declaredType = getNode(ele[i], Pair, b);
							break;
						}
						if(i==size-1){
							reportError(DEVELOPER, getNode(ast,SetVar,line), "unknown user type %s", getNode(declaredType, Symbol,name));
							return 1;
						}
					}
				}
				case Integer:
				case String:{
					if(declaredType == nil){
						declaredType = TYPES[parseType(vars, rhs)];
					}
				}
				case GetArray:
				case GetField:
				case GetVar:
				case Unyop:
				case Binop:{
					int scope = getNode(ast, SetVar,scope);
					switch(scope) {
						case SCOPE_LOCAL:{
							dprintf("defining local variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = insertVariable(vars, sym, declaredType);
							if(var.index == -1)return 1; //type error
							emitII(prog, iSETGLOBALVAR + var.scope, var.index);
							return 0;
						}
						case SCOPE_STATE_LOCAL:{
							dprintf("defining state variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, state_vars),sym, declaredType, NULL);
							emitII(prog, iSETSTATEVAR, var.index);
							return 0;
						}
						case SCOPE_GLOBAL:{
							dprintf("defining global variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, global_vars),sym, declaredType, NULL);
							emitII(prog, iSETGLOBALVAR, var.index);
							return 0;
						}
					}
					fatal("file %s line %d emitOn: unknown SetVar scope %d", __FILE__, __LINE__, scope);
					reportError(DEVELOPER,getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
				default:{
					fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
					reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
			reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
			return 0;
		}
		case GetArray:{
			printTYPE(_GetArray_);
			node array = getNode(ast, GetArray,array);//Array
			node index = getNode(ast, GetArray,index);//Pair
			struct RetVarFunc var = searchVariable(vars, array->GetVar.id, type);
			if(var.index == -1)return 1; // variable not found
			emitII(prog, iGETGLOBALVAR + var.scope, var.index); 
			while(index != nil){
				node idx = getNode(index, Pair, a);
				if(emitOn(prog, vars, idx, TYPES[Integer]))return 1;					
				index = getNode(index, Pair, b);
				emitI(prog, iGETARRAY);
			}
			printCode(prog);
			stop();
			return 0;
		}
		case SetArray:{
			printTYPE(_SetArray_);
			node setType = getNode(ast, SetArray,type);
			node array = getNode(ast, SetArray,array);//Array
			node index = getNode(ast, SetArray,index);//Pair
			node value = getNode(ast, SetArray,value);//Id
#ifdef DEBUG
			assert(getType(index) == Pair);
			assert(getType(value) == GetVar);
#endif
			int scope = getNode(ast, SetArray,scope);
			if(scope == SCOPE_GLOBAL && setType == nil){
				reportError(ERROR, getNode(ast,SetArray,line), "global variable %s must have a declared type", getNode(value, GetVar,id)->Symbol.name);
				return 1;
			}
			if(setType==nil && getType(value)==Array){
				reportError(ERROR, getNode(ast,SetArray,line), "array variable %s must have a declared type", getNode(value, GetVar,id)->Symbol.name);
				return 1;
			}
			if(setType==nil){
				setType = TYPES[parseType(vars, value)];
				struct RetVarFunc var = searchVariable(vars, value->GetVar.id, setType);
				if(var.index == -1)return 1; // variable not found
				emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the global variable value
				while(index != nil){
					node idx = getNode(index, Pair, a);
					if(emitOn(prog, vars, idx, TYPES[Integer]))return 1;					
					index = getNode(index, Pair, b);
					if(index==nil){
						if(emitOn(prog, vars, array, setType))return 1;
						emitI(prog, iSETARRAY);
						return 0;
					}else{
						emitI(prog, iGETARRAY);
					}
				}
				reportError(DEVELOPER, getNode(ast,SetArray,line), "please contact %s", DEVELOPER_EMAIL);
				return 1;
			}else{// setType is given
				if(parseArray(prog, array, index, setType, vars))return 1;
				struct RetVarFunc var;
				switch(scope) {
					case SCOPE_LOCAL:{
						var = insertVariable(vars, value->GetVar.id, setType);
						if(var.index == -1)return 1; //type error
						emitII(prog, iSETGLOBALVAR, var.index);
						break;
					}
					case SCOPE_STATE_LOCAL:{
						var = appendVariable(getNode(vars, EmitContext, state_vars), value->GetVar.id, setType, NULL);
						emitII(prog, iSETSTATEVAR, var.index);
						break;
					}
					case SCOPE_GLOBAL:{
						var = appendVariable(getNode(vars, EmitContext, global_vars), value->GetVar.id, setType, NULL);
						emitII(prog, iSETGLOBALVAR, var.index);
						break;
					}
				}
				node variable = var.variable;
				int dimIndex = 0;
				while(index != nil){
					node idx = getNode(index, Pair, a);
					variable->Variable.dim[dimIndex++] = Integer_value(idx);
					index = getNode(index, Pair, b);
				}
				return 0;
			}
			reportError(DEVELOPER, getNode(ast,SetArray,line), "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case GetField:{
			printTYPE(_GetField_);
			// id->field
			node id = getNode(ast, GetField, id);
			node field = getNode(ast, GetField, field);
	
			struct RetVarFunc var = searchVariable(vars, id, NULL);//if error happens, `type` is set to NULL to skip type checking
			if(var.index == -1)return 1; // variable not found
			 node varType = getNode(var.variable,Variable, type);
			if(isAtomicType(varType)){
				reportError(ERROR, getNode(ast,GetField,line), "variable %s is of atomic type and has no fields", getNode(id, Symbol,name));
				return 1;
			}
			switch(getType(varType))
			{
				case Undefined:{//Event Object
					node value = var.variable->Variable.value;
					int eoIndex = getNode(value, EventObject,index);
					const node * const funcs = getNode(value, EventObject,funcs);
					//search field in eo.funcs
					int fieldIndex = 0;
					char* funcName = getNode(getNode(field, Call, function), GetVar, id)->Symbol.name;//Stopied at this line
					while(funcs[fieldIndex] != nil){
						if(strcmp(funcs[fieldIndex]->Symbol.name, funcName) == 0){
							emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object
							//chat.send(msg);
							field->Call.function = funcs[fieldIndex];
							if(emitOn(prog, vars, field, type))return 1; // compile the call
							return 0;
						}
						fieldIndex++;
					}
					reportError(ERROR, getNode(ast,GetField,line), "event object %s has no field %s", getNode(id, Symbol,name), getNode(field, Symbol,name));
					return 1;
				}
				case Array:{ //User-defined type
					int fieldSize = getNode(varType, Array,size);
					node *fields = getNode(varType, Array,elements);
					int isGetField = getType(field) == GetField ? 1 : 0;
					node fieldId = isGetField ? getNode(field, GetField, id) : getNode(field, GetVar, id);
					emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the user-defined type variable
					for(int i=0; i<fieldSize; i++){
						if(fieldId == getNode(fields[i], Variable, id)){
							emitII(prog, iPUSH, i); // push the field index
							emitI(prog, iGETARRAY);
							if(isGetField){//agent.pos.x
								i = -1;
								id = getNode(field, GetField, id);
								field = getNode(field, GetField, field);
								var = searchVariable(vars, id, NULL);//if error happens, `type` is set to NULL to skip type checking
								if(var.index == -1)return 1; // variable not found
			 					varType = getNode(var.variable,Variable, type);
								fieldSize = getNode(varType, Array,size);
								fields = getNode(varType, Array,elements);
								i = -1; //restart searching
								continue;
							}else{
								return 0;
							}
						}
						if(i==fieldSize-1){
							reportError(ERROR, getNode(ast,GetField,line), "user-defined type %s has no field %s", getNode(id, Symbol,name), getNode(fieldId, Symbol,name));
							return 1;
						}
					}
					return 0;
				}
				default:{
					fatal("file %s line %d emitOn: GetField is not supported for type %d yet", __FILE__, __LINE__, getType(varType));
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d emitOn: GetField is not supported yet", __FILE__, __LINE__);
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case SetType:{
			printTYPE(_SetType_);
			node id = getNode(ast, SetType,id);
			node rhs = getNode(ast, SetType,rhs);
			assert(getType(rhs) == Array);// variable array typesa
			addUserType(vars, newPair(id, rhs)); 
			return 0;
		}
		case Call:{
			//Standard library functions / user defined functions
			printTYPE(_Call__);
			node id   = getNode(ast, Call, function);
			if(getType(id) == GetVar)id = id->GetVar.id;
			node args = getNode(ast, Call,arguments);

			node func = getNode(id, Symbol, value); // get the function from the variable;

			switch(getType(func)){
				case EventObject:{
					reportError(DEVELOPER, getNode(ast,Call,line), "event object call is not supported yet");
					return -1;
				}
				case EventH:{// event handler
					printTYPE(_EventH_);
					int index = getNode(func, EventH,index);// get the index of the event handler
					struct EventTable eh = EventTable[index];
					int nArgs = eh.nArgs;
					char *argTypes = eh.argTypes;

					int argsCount = 0;
					while(args != nil){
						node arg = getNode(args, Eparams, id);
						if(emitOn(prog, vars, arg, TYPES[argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Eparams, next);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, getNode(ast,Call,line), "event %s expects %d arguments, but got %d", getNode(func, Symbol,name), nArgs, argsCount);
						return 1;
					}
					emitIII(prog, iPCALL,  index, nArgs); // call the event handler
					return 0; 
				}
				case StdFunc:{// standard function
					printTYPE(_StdFunc_);
					int funcIndex = getNode(func, StdFunc, index); // get the index of the standard function
					int argsCount = 0;
					struct StdFuncTable func = StdFuncTable[funcIndex];
					while(args != nil){
						node arg = getNode(args, Args, value);
						dprintf("[C] argument type expected: %d\n", func.argTypes[argsCount]);
						if(emitOn(prog, vars, arg, TYPES[func.argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Args, next);
						argsCount++;
					}
					if(func.nArgs != argsCount){
						reportError(ERROR, getNode(ast,Call,line), "standard function %s expects %d arguments, but got %d", getNode(id, Symbol,name), func.nArgs, argsCount);
						return 1;
					}
					emitII(prog, iSCALL, funcIndex); // call the standard function
					//NOTE: we don't need to clean the stack after the call, because the standard function will do it
					//      BUT we need to check the return value (not implemented yet)
					//emitII(prog, iCLEAN, nReturns); // clean the stack after the call
					return 0; 
				}
				case Closure:{ // user defined function
					printTYPE(_Closure_);
					int nArgs = getNode(func, Closure,nArgs);
					int *argTypes = getNode(func, Closure, argTypes);
					int pos   = getNode(func, Closure,pos);
					int argsCount = 0;
					while(args != nil){
						node arg = getNode(args, Args, value);
						if(emitOn(prog, vars, arg, TYPES[argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Args, next);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, getNode(ast, Call, line), "function %s expects %d arguments, but got %d", getNode(id, Symbol,name), nArgs, argsCount);
						return 1; 
					}
					emitIII(prog, iUCALL, 0, nArgs); // call the function
					int iCallPos = prog->IntArray.size - 2; // remember the position of the call
					prog->IntArray.elements[iCallPos] = pos - (prog->IntArray.size) ; // set the jump position to the function code
					emitII(prog, iCLEAN, nArgs); // clean the stack after the call
					return 0; 
				}
				default:{
					fatal("file %s line %d call function with type %d",__FILE__,__LINE__,getType(func));
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d unknown Call type %d", __FILE__, __LINE__, getType(id));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Pair:{
			printTYPE(_Pair_);
			node a = getNode(ast, Pair,a);
			node b = getNode(ast, Pair,b);
			fatal("file %s line %d emitOn: Pair is not supported yet", __FILE__, __LINE__);
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Print:{
			printTYPE(_Print_);
			node args = getNode(ast, Print,arguments);
			int nArgs = 0;
			//reverse the args order
			node revArgs = nil;
			while(args != nil){
				node copy = getNode(args, Args, next);	
				args->Args.next = revArgs;
				revArgs = args;
				args = copy;	
			}
			while (revArgs != nil) {
				node value = getNode(revArgs, Args, value);
				int argType = parseType(vars, value);
				if(argType == -1) return 1; // type error
				if(emitOn(prog, vars, value, TYPES[argType])) return 1; // compile argument
				switch(argType){
					case Integer: emitI(prog, iPRINT); break;
					case Float:   emitI(prog, fPRINT); break;
					case String:  emitI(prog, sPRINT); break;
					default:
						reportError(ERROR, getNode(ast, Print, line), "print statement does not support type %d", argType);
						return 1;
				}
				revArgs = getNode(revArgs, Args, next);
				nArgs++;
			}
			emitI(prog, flashPRINT); // print the result
			return 0;
		}
		case If:{
			printTYPE(_If_);
			//NEXT-TODO
			int variablesSize = getNode(vars, EmitContext, local_vars) ? getNode(getNode(vars, EmitContext, local_vars), Array, size) : 0; // remember the size of variables
			node condition = getNode(ast, If,condition);
			node statement1 = getNode(ast, If,statement1);
			node statement2 = getNode(ast, If,statement2);

			if(emitOn(prog, vars, condition,TYPES[Integer])) return 1; // compile condition

			emitII(prog, iJUMPIF, 0); // emit jump if condition is TRUE
			int jumpPos = prog->IntArray.size-1; // remember position for jump

			if(emitOn(prog, vars, statement1,type)) return 1; // compile first statement
			if (statement2 !=  FALSE) {
				emitII(prog, iJUMP, 0);
				int jumpPos2 = prog->IntArray.size-1; // remember position for second jump
				if(emitOn(prog, vars, statement2,type)) return 1; // compile second statement
				prog->IntArray.elements[jumpPos] = jumpPos2 - jumpPos; // set jump position for first jump
				prog->IntArray.elements[jumpPos2] = (prog->IntArray.size - 1) - jumpPos2; // set jump position
			} else {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for first jump	
			}
			//NEXT-TODO
			discardVariables(getNode(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Loop:{
			printTYPE(_Looop_);
			node initialization = getNode(ast, Loop,initialization);
			node condition      = getNode(ast, Loop,condition);
			node iteration      = getNode(ast, Loop,iteration);
			node statement      = getNode(ast, Loop,statement);
			//NEXT-TODO
			int variablesSize = getNode(vars, EmitContext, local_vars) ? getNode(getNode(vars, EmitContext, local_vars), Array, size) : 0; // remember the size of variables

			if (initialization !=  FALSE) {
				if(emitOn(prog, vars, initialization, type)) return 1; // compile initialization
			}
			int stLoopPos = prog->IntArray.size; // remember the position of the loop start
			int jumpPos = 0; // position for the jump if condition is TRUE
			if(condition !=  FALSE) {
				if(emitOn(prog, vars, condition, TYPES[Integer])) return 1; // compile condition
				emitII(prog, iJUMPIF, 0); // emit jump if condition is TRUE
				jumpPos = prog->IntArray.size - 1; // remember position for jump
			}

			if(emitOn(prog, vars, statement, type)) return 1; // compile statement

			if (iteration !=  FALSE) {
				if(emitOn(prog, vars, iteration, type)) return 1; // compile iteration
			}
			emitII(prog, iJUMP, 0); // jump to the beginning of the loop
			prog->IntArray.elements[prog->IntArray.size - 1] = stLoopPos - prog->IntArray.size; // set jump position to the start of the loop
			if(condition !=  FALSE) {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for condition
			}
			//NEXT-TODO
			discardVariables(getNode(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Return:{
			printTYPE(_Return_);
			node value = getNode(ast, Return,value);
			if (value !=  FALSE) {
				if(emitOn(prog, vars, value, type)) return 1; // compile return value
			} else {
				emitII(prog, iPUSH, 0); // return 0 if no value is specified
			}
			emitI(prog, iRETURN); // emit return instruction
			return 0;
		}
		case Block:{
			printTYPE(_Block_);
			node *statements = ast->Block.statements;
			int nStatements = ast->Block.size;
			for (int i = 0;  i < nStatements;  ++i) {
				if(emitOn(prog, vars, statements[i], type)) return 1; // compile each statement
			}
			return 0;
		}
		case Transition:{
			printTYPE(_Transition_);
			node id = getNode(ast, Transition,id);
			emitII(prog, iTRANSITION, 0);
			/* WARN: 実際に値を入れるときは、-1して値を挿入する */
			appendS0T1(id, prog->IntArray.size, ATRANSITION); // append state to states
			return 0;
		}
		case State:{
			printTYPE(_State_);
			pushUserTypeIndex(vars);
			node id = getNode(ast, State,id);
			node params = getNode(ast, State,parameters);
			node events = getNode(ast, State,events);
			node *eventList = events->Block.statements;
			int eventsSize = getNode(events, Block, size);
			dprintf("State: %s\n", getNode(id, Symbol,name));

			appendS0T1(id, prog->IntArray.size, APSTATE); // append state to states
			int index = 0;
			int isBreak = 0;
			getNode(vars, EmitContext, state_vars) = newArray(0); // set state variables for the state
			for(;index < eventsSize; index++){
				node ele = eventList[index];
				switch(getType(ele)){
					case Event:{
						if(getNode(ele, Event,id) == entryEH){
							if(emitOn(prog, vars, ele, type)) {
								popUserTypeIndex(vars);
								return 1; // compile event
							}
							index++;
						}
						isBreak = 1;
						break;
					}
					case SetVar:{
						if(emitOn(prog, vars, ele, type)) {
							popUserTypeIndex(vars);
							return 1; // compile variable declarations inside state body
						}
						continue; 
					}
					default:{
						reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
						popUserTypeIndex(vars);
						return 1; // eventList[index] is not an Event
					}
				}
				if(isBreak)break;
			}

			int nEventHandlers = 0;// number of unique event handlers
			int process_sizes[eventsSize - index]; // number of process blocks for each event handler
			node preid =  FALSE;
			emitII(prog, iJUMP, 0);
			int jumpPos = prog->IntArray.size - 1; // remember the position of the jump
			dprintf("Block size: %d\n", eventsSize);
			for (int i = index;  i < eventsSize;  ++i) {
				node ele = eventList[i];
				node id = getNode(ele, Event,id);
				if(id != preid) {
					dprintf("new event\n");
					preid = id;
					process_sizes[nEventHandlers++]=1; // collect unique events
				}else if(id == preid){
					dprintf("same event\n");
					process_sizes[nEventHandlers-1]+=1; // increment number of event handlers block
					dprintf("process_sizes[%d]: %d\n", nEventHandlers-1, process_sizes[nEventHandlers-1]);
				}else if(id == exitEH){
					process_sizes[nEventHandlers]=0; // exitEH has no process block
					continue; // exitEH will be processed later
				}else{
					fatal("file %s line %d emitOn: unreachable code", __FILE__, __LINE__);
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					popUserTypeIndex(vars);
					return 1; // unreachable code
				}
				dprintf("eventList[%d]: %s\n", i, getNode(ele, Event,id)->Symbol.name);
				if(emitOn(prog, vars, ele, type)) {
					popUserTypeIndex(vars);
					return 1; // error compiling event
				}
			}// end for
			prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position to the end of the events

			// emit state event handlers
			emitII(prog, iSETSTATE, nEventHandlers); // set the number of events and position
			dprintf("Number of event handlers: %d\n", nEventHandlers);//remove
			// OPECODE FOR EVENTS
			for(int ei = index ,  ehi =0; ehi < nEventHandlers;  ehi++) {
				if(process_sizes[ehi] == 0){ ei++;continue;} // skip empty events
				dprintf("Make Opcode for Event %d\n", ehi);//remove
				node id = getNode(eventList[ei], Event,id);

				if(getType(id) == GetField)//chat.received()
				{
					struct RetVarFunc var = searchVariable(vars, getNode(id, GetField, id), type);
					if(var.index == -1){
						popUserTypeIndex(vars);
						return 1; // variable not found
					}
					emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object variable value
					node *funcs = getNode(var.variable->Variable.value, EventObject, funcs);
					char * fieldName = getNode(getNode(id, GetField, field),Symbol,name);
					int _i = 0;
					while(funcs[_i] != NULL){
						if(strcmp(getNode(funcs[_i], Symbol,name), fieldName) == 0){
							id = funcs[_i];
							break;
						}
						_i++;
					}
				}//end if

				node eh = getNode(id,Symbol,value);
				int eventID = getNode(eh, EventH, index); // get event ID
				emitIII(prog, iSETEVENT, eventID, process_sizes[ehi]); // emit SETEVENT instruction
#if DEBUG
				if(process_sizes[ehi] <= 0){
					fatal("file %s line %d emitOn: process_sizes[%d] is %d", __FILE__, __LINE__, ehi, process_sizes[ehi]);
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					popUserTypeIndex(vars);
					return 1; // process_sizes[ehi] is invalid
				}
#endif
				dprintf("Number of events for this handler: %d\n", process_sizes[ehi]);//remove
				for(int j = 0; j < process_sizes[ehi]; ++j) {
					dprintf("Event List index: %d\n", ei);//remove
					node event = eventList[ei++];//<< this makes error
					node posPair = getNode(event, Event,block);
					int opPos = prog->IntArray.size;
					int cPos = Integer_value(getNode(posPair,Pair,b));
					dprintf("Action pos: %d, Condition pos: %d\n", Integer_value(getNode(posPair,Pair,a)), cPos);	
					emitIII(prog, iSETPROCESS ,
						Integer_value(getNode(posPair,Pair,a)) - opPos/*rel action pos from op pos*/, 
						cPos == 0 ? 0 : cPos - opPos/*rel condition pos from op pos*/
					); 
				}
			}// end for
			emitI(prog, iIMPL);
			// exitEH
			if(getNode(eventList[eventsSize-1], Event,id) == exitEH){ 
				if(emitOn(prog, vars, eventList[eventsSize-1],type)) {
					popUserTypeIndex(vars);
					return 1; // compile exitEH
				}
			}
			//NEXT-TODO
			// discardVariables(vars->EmitContext.state_vars, 0); // discard variables
			getNode(vars, EmitContext, state_vars) = NULL; // clear state variables
			emitII(prog, iEOS, 0); // emit EOS instruction to mark the end of the state
			popUserTypeIndex(vars);
			return 0;
		}
		case Event:{
			printTYPE(_Event_);
			node id = getNode(ast, Event,id);
			pushUserTypeIndex(vars); // push user type index stack
			if(getType(id) == GetField)
			{
				node parent = getNode(id, GetField, id); // get the variable from the GetField
				struct RetVarFunc var = searchVariable(vars, parent, type);
				if(var.index == -1){
					reportError(ERROR, getNode(ast,Event,line), "variable %s is not defined", getNode(parent, Symbol,name));
					popUserTypeIndex(vars);
					return 1;
				}
				int parentIndex = getNode(var.variable->Variable.value, EventObject, index);
				node* funcs = getNode(var.variable->Variable.value, EventObject, funcs);
				struct EventObjectTable eo = EventObjectTable[parentIndex];
				char* fieldName = getNode(getNode(id, GetField, field),Symbol,name);
				id = NULL;
				for(int i = 0; i < eo.nFuncs; ++i){
					if(strcmp(getNode(funcs[i], Symbol,name), fieldName) == 0){
						id = funcs[i];
						break;
					}
				}
				if(id == NULL){
					reportError(ERROR, getNode(ast,Event,line), "event object %s has no event %s", getNode(parent, Symbol,name), fieldName);
					popUserTypeIndex(vars);
					return 1;
				}
				// emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object variable value
			}
			node params = getNode(ast, Event, parameters);
			node block = getNode(ast, Event, block);
			node eh = getNode(id,Symbol,value);
			if(getType(eh) != EventH) {
				fatal("file %s line %d emitOn: event %s() is not an EventH", __FILE__, __LINE__, getNode(id, Symbol,name));
				reportError(DEVELOPER, getNode(ast,Event,line), "please contact %s", DEVELOPER_EMAIL);
				popUserTypeIndex(vars);
				return 1;
			}
			getNode(vars, EmitContext, local_vars) = newArray(0); // set local variables for the event
			int nArgs = EventTable[eh->EventH.index].nArgs;
			int paramSize =0;
			int cPos      = 0;

			while(params != nil) {//a:id-b:cond
				dprintf("\t appendVarialbe\n");
				struct RetVarFunc var =  appendVariable(
					getNode(vars, EmitContext, local_vars), 
					getNode(params, Eparams, id), 
					getNode(params, Eparams, type),
					NULL); // add parameter to local variables
				if(var.index == -1){
					popUserTypeIndex(vars);
					return 1; // type error
				}
				dprintf("\t finished appendVarialbe\n");
				if(getNode(params, Eparams, cond) !=  FALSE){
					if(cPos==0)cPos = prog->IntArray.size; // remember the position of the condition
					if(emitOn(prog, vars, getNode(params, Eparams, cond), getNode(params, Eparams, type))) return 1; // compile condition if exists
					emitI(prog,iJUDGE); // emit JUDGE instruction
				}
				params = getNode(params, Eparams, next);
				paramSize++;
			}
			if(cPos != 0){
				emitI(prog, iEOC); // emit EOC instruction if condition exists
			}
			if(paramSize != nArgs) {
				reportError(ERROR, getNode(ast,Event,line), "event %s has %d parameters, but expected %d", getNode(id, Symbol,name), paramSize, nArgs);
				getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
				popUserTypeIndex(vars);
				return 1;
			}
			int aPos = prog->IntArray.size; // remember the position of the event handler
			emitII(prog, iMKSPACE, 0); // reserve space for local variables
			int mkspacepos = prog->IntArray.size - 1; // remember the position of MKSPACE

			if(emitOn(prog, vars, block, type))return 1; // compile block
			emitII(prog, iEOE, getNode(getNode(vars, EmitContext, local_vars), Array, size)); // emit EOE instruction to mark the end of the event handler

			prog->IntArray.elements[mkspacepos] = getNode(getNode(vars, EmitContext, local_vars), Array, size); // store number of local variables
			getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
			GC_PUSH(node,apos, newInteger(aPos));
			GC_PUSH(node,cpos,newInteger(cPos));
/* BE CAREFUL */
			ast->Event.block = newPair(apos,cpos); // set the position of the event handler
			GC_POP(cpos);
			GC_POP(apos);
			popUserTypeIndex(vars); // pop user type index stack
			return 0;
		}
		default:break;
	}
    fatal("file %s line %d: emitOn: unimplemented emitter for type %d", __FILE__, __LINE__, getType(ast));
	reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
	popUserTypeIndex(vars);
	return 1;
}

void printCode(oop code){
	for (int i = 0; i < code->IntArray.size; ++i) {
		int op = code->IntArray.elements[i];
		const char *inst = "UNKNOWN";
		switch (op) {
			case iHALT:
				inst = "HALT";
				printf("%03d: %-10s\n", i, inst);
				break;
			case iPUSH:
				inst = "LOAD";
				int value = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, value);
				break;
			case sPUSH:{
				inst = "LOADS";
				int sLen = code->IntArray.elements[++i];
				char sValue[sLen+1];
				memcpy(sValue, &code->IntArray.elements[++i], sLen);
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
				int nElements = code->IntArray.elements[++i];
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
				int nVariables = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iEOS:{
				inst = "EOS";
				int nVariables = code->IntArray.elements[++i];// FIXME: this is not correct instruction
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iGETVAR:
				inst = "GETVAR";
				int varIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndex);
				break;
			case iSETVAR:
				inst = "SETVAR";
				int varIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndexSet);
				break;
			case iGETSTATEVAR:
				inst = "GETSTATEVAR";
				int sVarIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndex);
				break;
			case iSETSTATEVAR:
				inst = "SETSTATEVAR";
				int sVarIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndexSet);
				break;
			case iGETGLOBALVAR:
				inst = "GETGLOBALVAR";
				int gVarIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndex);
				break;
			case iSETGLOBALVAR:
				inst = "SETGLOBALVAR";
				int gVarIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndexSet);
				break;
			case iINITSPACE:
				inst = "INITSPACE";
				int nLocals = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nLocals);
				break;
			case iMKSPACE:
				inst = "MKSPACE";
				int space = code->IntArray.elements[++i];
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
				int offset = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iJUMPIF: {
				inst = "JUMPIF";
				int offset = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iCLEAN: {
				inst = "CLEAN";
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nArgs);
				break;
			}
			case iPCALL: {
				inst = "PCALL";
				int index = code->IntArray.elements[++i];
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, index, index + (i+1), nArgs);
				break;
			}
			case iSCALL: {
				inst = "SCALL";
				int index = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iUCALL: {
				inst = "CALL";
				int pos = code->IntArray.elements[++i];
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, pos, (i+1)+pos, nArgs);
				break;
			}
			case eCALL:{
				inst = "eCall";
				int index = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iTRANSITION: {
				inst = "TRANSITION";
				int pos = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%d)\n", i-1, inst, pos,(i+1)+pos);
				break;
			}
			case iSETSTATE: {
				inst = "SETSTATE";
				int nEvents = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nEvents);
				break;
			}
			case iSETEVENT: {
				inst = "SETEVENT";
				int eventID = code->IntArray.elements[++i];
				int nHandlers = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d %3d\n", i-2, inst, eventID, nHandlers);
				break;
			}
			case iSETPROCESS: {
				inst = "SETPROCESS";
				int pc = i;
				int apos = code->IntArray.elements[++i];
				int cpos = code->IntArray.elements[++i];
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


oop compile()
{
	dprintf("compiling...\n");
    GC_PUSH(oop, prog, intArray_init()); // create a new program code
	emitII(prog, iINITSPACE, 0); // reserve space for local variables
#if DEBUG
int roots = getOriginalGCtxNRoots();
#endif 
	GC_PUSH(node, vars, newEmitContext()); // If something goes wrong in the web parser, move to before changing GC roots
	// compile the AST into the program code
	int line = 1;
	assert(getType(vars) == EmitContext);
	while(yyparse()){
		if (ISTAG_FLAG(result)) {
			break;
		}
		dprintf("compiling statement %d\n", line);
		if(emitOn(prog, vars, result, NULL))return NULL;
		result = parserRetFlags[PARSER_READY];
		dprintf("compiled statement %d [size %4d]\n",++line, (prog->IntArray.size)*4);
	}
	if(result == parserRetFlags[PARSER_FINISH]){
		dprintf("compilation finished\n");
		emitI (prog, iHALT); // end of program
		dprintf("total variable size %d\n", getNode(vars, EmitContext, global_vars)->Array.size);
		prog->IntArray.elements[1] = getNode(vars, EmitContext, global_vars)->Array.size; // store number of variables
		setTransPos(prog); // set transition positions
	}else if(result == parserRetFlags[PARSER_ERROR]){
		dprintf("compilation error\n");
		GC_POP(vars); // pop context variables from GC roots
		return NULL;
	}
	GC_POP(vars); // pop context variables from GC roots
#if DEBUG
	if(roots != getOriginalGCtxNRoots()){
		fatal("file %s line %d: memory leak: roots before compile %d, after compile %d", __FILE__, __LINE__, roots, getOriginalGCtxNRoots());
		reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
		return NULL;
	}
#endif
	GC_POP(prog); // pop program code from GC roots

	dprintf("\ncompile finished, %d statements, code size %d bytes\n\n", line, 4 * prog->IntArray.size);
    return prog;
}


#endif // COMPILER_C