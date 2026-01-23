// Linux Standard Library
#ifndef LIBRARY_C
#define LIBRARY_C
#include "library.h"

//-------------------------------
// COMMON CODE
//-------------------------------
// Event Handler Types
enum{
// Standard Event Handlers
	EVENT_EH = 0x00, // Loop Handler
	TIMER_EH, // Timer Handler
//Event Object Event Handlers
	CHAT_RECEIVED_EH, // WebRTC Broadcast Event Object
	T_TIMER_SEC_EH, // Timer second event handler
	T_TIMER_MIN_EH, // Timer minute event handler
	T_TIMER_HOUR_EH, // Timer hour event handler

	#ifdef RPI // Raspberry Pi specific event handlers (from 0x08:  decimal)
	PI_GPIO_0_31_EH, //= 0x08, // Raspberry Pi GPIO 0-31 Event Handler
	PI_GPIO_EO_N_EH, // Raspberry Pi  GPIO Event object (PIN N) Event Handler
	#endif
	END_EH, /* DO NOT REMOVE THIS LINE */
} EventHandlerType;
// Standard Library Function Types
 enum {
	EXIT_FUNC=0,   // exit function
	MATH_SQRT_FUNC, // math sqrt function
	CHAT_SEND_FUNC, // chat send function
	CHAT_POST_FUNC, // chat post function
	CHAT_SELF_FUNC, // chat self function
	T_TIMER_RESET_FUNC, // timer reset function
	#ifdef RPI // Raspberry Pi specific functions (from 0x08:  decimal)
	PI_GPIO_SET_OUTPUT_FUNC, //= 0x08, // Raspberry Pi GPIO set output function
	PI_GPIO_WRITE_FUNC, // Raspberry Pi GPIO write function
	PI_GPIO_EO_WRITE_FUNC, // Raspberry Pi GPIO Event Object write function
	#endif
} StdLibFuncType;
// Event Object Types
enum {
	CHAT_BROADCAST_EO = 0x00, // WebRTC Broadcast Event Object
	TIME_EO, // Timer Event Object
	END_EO, /* DO NOT REMOVE THIS LINE */
	#ifdef RPI // Raspberry Pi specific event objects (from 0x08:  decimal)
	PI_GPIO_EO, //= 0x08, // Raspberry Pi GPIO Event Object
	#endif
} EventObjectType;



//-------------------------------
// SHICA COMPILER CODE
//-------------------------------
#ifdef SHICACOMP
//-----------------------------------------------
// Standard Event Handlers-------------------------------
//-----------------------------------------------
// Initialize Standard Event Handlers
int compile_eh_init(){
	//standard event handler
	node EH = NULL;
	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH); // 1 argument
    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH); // 1 argument

#ifdef RPI
	EH = intern("pi_gpio_0_31_EH");
	EH->Symbol.value = newEventH(PI_GPIO_0_31_EH); // 2 arguments
#endif
	return 0; // return 0 to indicate success
}
//nArgs, argTypes, nFuncs
//-------------------------------
struct CompEventTable __CompEventTable__[] = {
	[EVENT_EH] = {0, NULL, 0},      // EVENT_EH
	[TIMER_EH] = {1,(char []){Integer}, 2},      // TIMER_EH
	[CHAT_RECEIVED_EH] = {2,(char []){String/*sender*/, String/*msg*/}, 3},      // CHAT_RECEIVED_EH
	[T_TIMER_SEC_EH] = {1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
#ifdef RPI
	[PI_GPIO_0_31_EH] = {1,(char []){Integer}, 1},      // PI_GPIO_0_31_EH
	[PI_GPIO_EO_N_EH] = {1,(char []){Integer}, 1},      // PI_GPIO_EO_N_EH
#endif
};

// Standard Library Functions
int compile_func_init()
{
	node FUNC = NULL;
	FUNC = intern("exit");
	FUNC->Symbol.value = newStdFunc(EXIT_FUNC); // exit function

	FUNC = intern("sqrt");
	FUNC->Symbol.value = newStdFunc(MATH_SQRT_FUNC); // math sqrt function
#ifdef RPI
	FUNC = intern("gpioSetOutput");
	FUNC->Symbol.value = newStdFunc(PI_GPIO_SET_OUTPUT_FUNC); // Raspberry Pi GPIO set output function
	FUNC = intern("gpioWrite");
	FUNC->Symbol.value = newStdFunc(PI_GPIO_WRITE_FUNC); // Raspberry Pi GPIO write function
#endif
	return 0; // return 0 to indicate success
}
struct CompStdFuncTable __CompStdFuncTable__[] = {
	[EXIT_FUNC] = {1, (int[]){Integer}, Undefined}, // exit function takes 1 argument
	[CHAT_SEND_FUNC] = {1, (int[]){String/*msg*/}, Undefined}, // chat send function takes 2 arguments
	[CHAT_POST_FUNC] = {1, (int[]){String/*msg*/}, Undefined}, // chat post function takes 2 arguments
	[CHAT_SELF_FUNC] = {0, NULL, String/*id*/}, // chat self function takes 0 arguments and returns a string
	[MATH_SQRT_FUNC] = {1, (int[]){Integer}, Integer}, // math sqrt function takes 1 argument and returns an integer
	[T_TIMER_RESET_FUNC] = {1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
#ifdef RPI
	[PI_GPIO_SET_OUTPUT_FUNC] = {1, (int[]){Integer/*pin*/}, Undefined}, // Raspberry Pi GPIO set output function takes 2 arguments
	[PI_GPIO_WRITE_FUNC] = {2, (int[]){Integer/*pin*/, Integer/*value*/}, Undefined}, // Raspberry Pi GPIO write function takes 2 arguments
	[PI_GPIO_EO_WRITE_FUNC] = {1, (int[]){Integer/*value*/}, Undefined}, // Raspberry Pi GPIO Event Object write function takes 2 arguments
#endif
};
//-------------------------------
// Event Object Table
//-------------------------------
int compile_eo_init(){
	node sym = NULL;
	node func = NULL;
	node eo = NULL;
// Broadcast Event Object
	sym = intern("broadcastEO");
	eo = newEventObject(sym, CHAT_BROADCAST_EO);// var chat = broadcast(channel, password);
	func = newEventH(CHAT_RECEIVED_EH);sym = newSymbol("receivedEH");
	putFuncToEo(eo, func, sym, 0);// chat.received(sender, msg);
	func = newStdFunc(CHAT_SEND_FUNC);sym = newSymbol("send");
	putFuncToEo(eo, func, sym, 1); // chat.send(msg, recipient);
	func = newStdFunc(CHAT_POST_FUNC);sym = newSymbol("post");
	putFuncToEo(eo, func, sym, 2); // chat.post(msg);
	func = newStdFunc(CHAT_SELF_FUNC);sym = newSymbol("self");
	putFuncToEo(eo, func, sym, 3); // chat.self();
// Timer Event Object
	sym = intern("timerEO");
	eo = newEventObject(sym, TIME_EO);// var t = timer(interval, label);
	func = newEventH(T_TIMER_SEC_EH);sym = newSymbol("secEH");
	putFuncToEo(eo, func, sym, 0);// t.sec(second);
	func = newEventH(T_TIMER_MIN_EH);sym = newSymbol("minEH");
	putFuncToEo(eo, func, sym, 1);// t.min(minute);
	func = newEventH(T_TIMER_HOUR_EH);sym = newSymbol("hourEH");
	putFuncToEo(eo, func, sym, 2);// t.hour(hour);
	func = newStdFunc(T_TIMER_RESET_FUNC);sym = newSymbol("reset");
	putFuncToEo(eo, func, sym, 3);// t.x();
// Other Event Objects can be added here
#ifdef RPI
// Raspberry Pi GPIO Event Object
	sym = intern("gpioEO");
	eo = newEventObject(sym, PI_GPIO_EO);// var gpio = pi_gpio(pin);
	func = newEventH(PI_GPIO_EO_N_EH);sym = newSymbol("readEH");
	putFuncToEo(eo, func, sym, 0);// gpio.readEH(value);
	func = newStdFunc(PI_GPIO_EO_WRITE_FUNC);sym = newSymbol("write");
	putFuncToEo(eo, func, sym, 1);// gpio.write(value);
#endif
	return 0;
}

	// int nArgs; // number of arguments
	// int nFuncs; // number of functions
	// int *argTypes; // types of arguments
struct CompEventObjectTable  __CompEventObjectTable__[] = {
	[CHAT_BROADCAST_EO] = {2, 4, (int[]){String/*channel*/, String/*password*/}}, // WebRTC broadcast event object with 3 arguments and 1 function
	[TIME_EO] = {0, 4, NULL}, // Timer event object with 0 arguments and 4 functions
#ifdef RPI
	[PI_GPIO_EO] = {3, 2, (int[]){Integer/*pin*/, Integer/*mode*/, Integer/*init: pud|vol*/}}, // Raspberry Pi GPIO event object with 3 arguments and 2 functions
#endif
};

//When multiple libraries is needed and think aobut this.
// //Compiler
// //Executor mapping
// const int index_adapt(int index){
// 	switch(index){
// 		case EVENT_EH: return EVENT_EH;
// 		case TIMER_EH: return TIMER_EH;
// 		case T_TIMER_SEC_EH: return T_TIMER_SEC_EH;
// 		case T_TIMER_MIN_EH: return T_TIMER_MIN_EH;
// 		case T_TIMER_HOUR_EH: return T_TIMER_HOUR_EH;
// 		#ifdef RPI
// 		case PI_GPIO_0_31_EH: return PI_GPIO_0_31_EH;
// 		case PI_GPIO_EO_N_EH: return PI_GPIO_EO_N_EH;
// 		#endif
// 		default:
// 			fatal("file %s line %d index_adapt: unknown event handler index %d", __FILE__, __LINE__, index);
// 			return -1;
// 		}
// 	return 0;
// }

#endif // SHICACOMP



//-------------------------------+
// SHICA EXECUTER CODE.          |
//-------------------------------+


// #define SHICAEXEC
#ifdef SHICAEXEC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <errno.h>
#include <netdb.h> // 追加: getnameinfo と NI_NUMERICHOST のため
#include <net/if.h> // IFF_LOOPBACKのため
#include <ifaddrs.h>



// 自身のネットワークインターフェースIPアドレスを取得する関数
int get_network_ip(char *ip_buffer, size_t buffer_size) {
    struct ifaddrs *ifaddr, *ifa;
    int family;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET && !(ifa->ifa_flags & IFF_LOOPBACK)) { // ループバック以外のIPv4アドレス
            int result = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                     ip_buffer, buffer_size, NULL, 0, NI_NUMERICHOST);
            if (result == 0) {
                break; // 最初の有効なネットワークインターフェースを取得
            } else {
                fprintf(stderr, "getnameinfo failed: %s\n", gai_strerror(result));
            }
        }
    }

    freeifaddrs(ifaddr);
    return 0;
}

void set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
    }
}

int create_broadcast_socket(int *sockfd, struct sockaddr_in *broadcast_addr, const char *BROADCAST_ADDR, int BROADCAST_PORT) {
    int yes = 1;

    // ソケット作成
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // ブロードキャスト送信を有効化
    if (setsockopt(*sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) < 0) {
        perror("setsockopt(SO_BROADCAST)");
        close(*sockfd);
        return -1;
    }

    // アドレス設定
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(BROADCAST_PORT);
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_ADDR);

    return 0;
}

void send_broadcast(int sockfd, struct sockaddr_in *broadcast_addr, const char *data, size_t data_size) {
    if (sendto(sockfd, data, data_size, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr)) < 0) {
        perror("sendto");
    } else {
        printf("Broadcast sent successfully\n");
    }
}


int send_broadcast_nonblocking(int sockfd, struct sockaddr_in *broadcast_addr, const char *data, size_t data_size) {
    ssize_t ret = sendto(sockfd, data, data_size, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr));
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {// EAGAIN: リソースが利用可能になるまで待つ必要がある場合
            printf("Send would block, try again later\n");
            return -1;
        } else {                                    // EWOULDBLOCK: リソースが利用可能になるまで待つ必要がある場合
            perror("sendto");
            return -1;
        }
    } else {
        DEBIF printf("Broadcast sent successfully\n");
        return 0;
    }
}


int create_receive_socket(int *sockfd, struct sockaddr_in *recv_addr, int BROADCAST_PORT) {
    // ソケット作成
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // アドレス設定
    memset(recv_addr, 0, sizeof(*recv_addr));
    recv_addr->sin_family = AF_INET;
    recv_addr->sin_port = htons(BROADCAST_PORT);
    recv_addr->sin_addr.s_addr = INADDR_ANY;

    // ポートにバインド
    if (bind(*sockfd, (struct sockaddr *)recv_addr, sizeof(*recv_addr)) < 0) {
        printf("bind error\n");
        return -1;
        perror("bind");
        close(*sockfd);
        return -1;
    }

    return 0;
}

int receive_broadcast(int sockfd, char *buffer, size_t buffer_size) {
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    ssize_t ret = recvfrom(sockfd, buffer, buffer_size, MSG_DONTWAIT, (struct sockaddr *)&sender_addr, &addr_len);
    if (ret > 0) {
        DEBIF printf("Received from %s: %s\n", inet_ntoa(sender_addr.sin_addr), buffer);
        return 0;
    } else {
        DEBIF perror("recvfrom");
        return -1;
    }
}

int receive_broadcast_nonblocking(int sockfd, char *buffer, size_t buffer_size) {
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    ssize_t ret = recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *)&sender_addr, &addr_len);
    if(ret>0){
        DEBIF printf("Received from %s: %s\n", inet_ntoa(sender_addr.sin_addr), buffer);
        return 0;
    }else{
        DEBIF perror("recvfrom");
        return -1;
    }
}



extern enum COMMUNICATE_E{
    COMMUNICATE_WiFi_RECEIVE_E,
    COMMUNICATE_WiFi_BROADCAST_RECEIVE_E,
    COMMUNICATE_WiFi_GROUP_RECEIVE_E,
} COMMUNICATE_E;

extern enum COMMUNICATE_P{
    COMMUNICATE_WiFi_SEND_P,
    COMMUNICATE_WiFi_BROADCAST_P,
    COMMUNICATE_WiFi_GROUP_BROADCAST_P,
    COMMUNICATE_WiFi_BUILD_GROUP_P,
    COMMUNICATE_WiFi_LEAVE_GROUP_P,
} COMMUNICATE_P;

#define MAX_GROUP 8

typedef enum AgentType{
    AgentBase,
    AgentReader,
    AgentMember,
    AgentVisitor,
}agent_t;

typedef union Agent *agent_p;

struct AgentBase{
    enum Type type; //for gc
    agent_t agent_type;
    char myID;
    char groupID;
};

struct AgentReader{
    struct AgentBase base;
    char groupKey[4];//check me
    unsigned char sizeOfMember;
};

struct AgentMember{
    struct AgentBase base;
    char groupKey[4];//check me
};
struct AgentVisitor{
    struct AgentBase base;
};

#define STRUCT_AGENT_TYPE 0b000
union Agent{
    enum Type type;
    struct AgentBase base;
    struct AgentReader reader;
    struct AgentMember member;
    struct AgentVisitor visitor;
};

agent_p _createAgent(agent_t type,int size){
    agent_p agent = (agent_p)gc_alloc(size);
    agent->base.type = registerExternType(STRUCT_AGENT_TYPE) + EXTRA_KIND;
    agent->base.agent_type = type;
    return agent;
}
#define createAgent(TYPE) _createAgent(TYPE,sizeof(struct TYPE))

#if DEBUG
agent_p _check_agent_type(agent_p node,enum AgentType type, char *file, int line)
{
    if (node->base.agent_type != type) {
        printf("%s line %d: expected type %d got type %d\n", file, line, type, node->base.type);
        exit(1);
    }
    return node;
}
#define getA(PTR, TYPE, FIELD)	(_check_agent_type((PTR), TYPE, __FILE__, __LINE__)->TYPE.FIELD)
#else
#define getA(PTR, TYPE, FIELD)  (PTR->TYPE.FIELD)
#endif

#if DEBUG
#define getAgentGroupKey(A) _getAgentGroupKey(__FILE__,__LINE__,A)
char *_getAgentGroupKey(char *file, int line, agent_p agent)
#else
char *getAgentGroupKey(agent_p agent)
#endif
{
    switch(agent->base.agent_type){
        case AgentMember:{
            return agent->member.groupKey;
        }
        case AgentReader:{
            return agent->reader.groupKey;
        }
        case AgentVisitor:
        default:{
#if DEBUG
            DEBUG_LOG_REF("UNKNOWN AGENT TYPE \n");
            exit(1);
#else
            printf("%s line %d UNKNOWN\n",__FILE__,__LINE__);
#endif
        }
    }
    return 0;
}



void setAgentGroupKey(agent_p agent,char *groupKey){
    switch(agent->base.agent_type){
        case AgentMember:{
            memcpy(agent->member.groupKey,groupKey,4);
            break;
        }
        case AgentReader:{
            memcpy(agent->reader.groupKey,groupKey,4);
            break;
        }
        case AgentVisitor:{
            break;
        }
        default:{
            printf("%s line %d UNKNOWN\n",__FILE__,__LINE__);
        }
    }
}


void printAgentData(agent_p agent){
    switch(agent->base.agent_type){
        case AgentMember:{
            printf("AgentMember\n");
            printf("myID:%d\n",agent->base.myID);
            printf("groupID:%d\n",agent->base.groupID);
            printf("groupKey:%*s\n",4,agent->member.groupKey);
            break;
        }
        case AgentReader:{
            printf("AgentReader\n");
            printf("myID:%d\n",agent->base.myID);
            printf("groupID:%d\n",agent->base.groupID);
            printf("groupKey:%*s\n",4,agent->reader.groupKey);
            printf("sizeOfMember:%d\n",agent->reader.sizeOfMember);
            break;
        }
        case AgentVisitor:{
            printf("AgentVisitor\n");
            break;
        }
        default:{
            printf("%s line %d UNKNOWN\n",__FILE__,__LINE__);
        }
    }
}

// request type
#define  REQUEST_UNDEFINED      0x00
#define  REQUEST_JOIN           0x01
#define  REQUEST_LEAVE          0x02
#define  REQUEST_TO_BE_MEMBER   0x03
#define  REQUEST_TO_BE_READER   0x04
#define  REQUEST_SUCCESS        0x05
#define  REQUEST_REJECT         0x06
#define  REQUEST_MOVE           0x07
#define  REQUEST_TRIGER         0x08


// broadcast data index
#define DATA_REQUEST_TYPE        0x00
#define DATA_GROUP_ID            0x01
#define DATA_REQUEST_SENDER_ID   0x02
#define DATA_REQUEST_MEMEBER_ID  0x03
#define DATA_SIZE_OF_MEMBER      0x04
#define DATA_GROUP_KEY           0x05
#define DATA_DATA                0x09

// group key size
#define SIZE_OF_DATA_GROUP_KEY   0x04

// member size
#define MAX_MEMBER_SIZE          0x08
#define ALL_MEMBER_ID            0xFF

#ifdef BUF_SIZE
#undef BUF_SIZE
#endif
#define BUF_SIZE                16


#define STRUCT_SOCKET_INFO_TYPE 0b000
struct SocketInfo{
    enum Type type;
    int recv_sockfd, send_sockfd;
    struct sockaddr_in recv_addr, broadcast_addr, sender_addr;
    socklen_t addr_len;
    char own_ip[INET_ADDRSTRLEN];
};
#define STRUCT_AGENT_INFO_TYPE 0b110
struct AgentInfo{
    enum Type type;
    union Agent* agent;//agent_p is struct Agent*
    struct SocketInfo *socket;
};

//-----------------------------------------------
#include <math.h>
#include <time.h>
// Event Handlers and its initializers
int event_handler(oop exec, oop eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		oop thread = eh->EventHandler.threads[0];
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal(0))); // enqueue a stack with value 0
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(oop eh){
	return 1;
}
int event_object_handler_init(oop eh){
	return 1;
}

int timer_handler(oop exec, oop eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	if(now - IntVal_value(eh->EventHandler.data[0]) >= 1){
		eh->EventHandler.data[0] = newIntVal(now);
		eh->EventHandler.data[1] = newIntVal(IntVal_value(eh->EventHandler.data[1]) + 1); // increment count
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, eh->EventHandler.data[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0; // return 0 to indicate no event
}
int timer_handler_init(oop eh){//normal timer handler (not EO)
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	eh->EventHandler.data[0] = newIntVal(now); //start time
	eh->EventHandler.data[1] = newIntVal(0);   //count
	return 1;
}

//EO Event Handlers
int timer_sec_handler(oop exec, oop eh){
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int interval = IntVal_value(obj);

	int pre = IntVal_value(fields[2]);	
	time_t t = time(NULL);
	int now = (int)(t % 10000);

	if(now - pre >= interval){
		fields[2] = newIntVal(now);
		fields[1] = newIntVal(IntVal_value(fields[1]) + 1); // increment count
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, fields[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
int timer_min_handler(oop exec, oop eh){
	return 0;
}
int timer_hour_handler(oop exec, oop eh){
	return 0;
}

oop eve_wifi_receive(oop exec, oop eh){
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);

	struct AgentInfo *MY_AGENT_INFO = (struct AgentInfo *)obj;
	struct SocketInfo *socketInfo = MY_AGENT_INFO->socket;//remove me after adapt em
	agent_p agent = MY_AGENT_INFO->agent;
	// メッセージ受信
	char buffer[BUF_SIZE];
	ssize_t ret = recvfrom(socketInfo->recv_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&socketInfo->sender_addr, &socketInfo->addr_len);
	if (ret > 0) {
		buffer[ret] = '\0'; // Null終端
		char *sender_ip = inet_ntoa(socketInfo->sender_addr.sin_addr);
		// 自分自身の送信データを無視
		// if (strcmp(sender_ip, socketInfo->own_ip) == 0) {
		// 	return core;
		// }
#if DEBUG
		DEBUG_LOG("\nReceived from %s: %s\n", sender_ip, buffer);
#endif

		if(agent->base.groupID == buffer[DATA_GROUP_ID] && memcmp(getAgentGroupKey(agent), buffer + DATA_GROUP_KEY, SIZE_OF_DATA_GROUP_KEY) == 0){
			switch(buffer[DATA_REQUEST_TYPE]){
				case REQUEST_JOIN:{
					#if DEBUG
					DEBUG_LOG("REQUEST_JOIN");
					printAgentData(agent);
					#endif

					if(agent->base.agent_type == AgentReader){
						int list = agent->reader.sizeOfMember;
						int newId = 1;
						while(list){
							if((list & 0x01) == 0){
								break;
							}
							newId++;
							list >>= 1;
						}
						if(newId > MAX_MEMBER_SIZE){
							//send REJECT  
							buffer[DATA_REQUEST_TYPE] = REQUEST_REJECT;
							int sent = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buffer, BUF_SIZE);
							if (sent < 0) {
								perror("sendto");
							} else {
								SHICA_PRINTF("Replied to %s: REJECT\n", sender_ip);
							}
						}
						else{
							//send TO_BE_MEMBER
							agent->reader.sizeOfMember |= (1U <<(newId-1));
#if DEBUG
							DEBUG_LOG("current Member is %d\n",agent->reader.sizeOfMember);
#endif
							buffer[DATA_REQUEST_TYPE] = REQUEST_TO_BE_MEMBER;
							buffer[DATA_REQUEST_SENDER_ID]        = newId;
							buffer[DATA_REQUEST_MEMEBER_ID] = (1 << (newId-1));
							buffer[DATA_SIZE_OF_MEMBER] = agent->reader.sizeOfMember;
							int sent = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buffer, BUF_SIZE);
							if (sent < 0) {
								perror("sendto");
							} else {
								SHICA_PRINTF("Replied to %s: TO_BE_MEMBER given ID %d\n", sender_ip,newId);
							}
							// printMember(buffer[DATA_SIZE_OF_MEMBER]);
						}
					}
					break;
				}

				case REQUEST_LEAVE:{
					#if DEBUG
					DEBUG_LOG("REQUEST_LEAVE\n");
					#endif
					if(agent->base.agent_type == AgentReader){
						agent->reader.sizeOfMember &= ~(1 << buffer[DATA_REQUEST_SENDER_ID]); 
						//send SUCCESS
						buffer[DATA_REQUEST_TYPE] = REQUEST_SUCCESS;
						buffer[DATA_REQUEST_SENDER_ID] = agent->base.myID;
						int sent = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buffer, BUF_SIZE);
						if (sent < 0) {
							perror("sendto");
						} else {
							SHICA_PRINTF("Replied to %s: SUCCESS\n", sender_ip);
						}
					}
					break;
				}
				case REQUEST_TO_BE_READER:{
					#if DEBUG
					DEBUG_LOG("REQUEST_TO_BE_READER\n");
					#endif
					if(agent->base.agent_type == AgentMember){
						if((buffer[DATA_REQUEST_MEMEBER_ID]>> (agent->base.myID-1) & 1) == 1){//checking for me or not
							agent_p newAgent = createAgent(AgentReader);
							newAgent->reader.sizeOfMember = buffer[DATA_SIZE_OF_MEMBER] & ~(1 << (agent->base.myID-1));//remove my id
							newAgent->base.myID = 0;
							newAgent->base.groupID = buffer[DATA_GROUP_ID];
							printf("current Member is %d\n",newAgent->reader.sizeOfMember);
							setAgentGroupKey(newAgent,agent->reader.groupKey);

							buffer[DATA_REQUEST_TYPE] = REQUEST_SUCCESS;
							buffer[DATA_REQUEST_SENDER_ID]        = agent->base.myID;
							int sent = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buffer, BUF_SIZE);
							if (sent < 0) {
								perror("sendto");
							} else {
								printf("Replied to %s: SUCCESS\n", sender_ip);
							}
							MY_AGENT_INFO->agent = newAgent;
							core->SubCore.any = (void *)MY_AGENT_INFO;
						}
					}
					return core;
				}
				case REQUEST_TRIGER:{
					#if DEBUG
					DEBUG_LOG("REQUEST_TRIGER\n");
					#endif
					if((buffer[DATA_REQUEST_MEMEBER_ID]>> (agent->base.myID-1) & 1) == 1){
						/* trigger data */
						int isOnce = 0;
						evalEventArgsThread->Thread.stack->Array.size = 1;//1:basepoint
						unsigned char value = buffer[DATA_REQUEST_MEMEBER_ID];
						for(int thread_i = 0;thread_i<core->Core.size;thread_i++){
							int isFalse = 0;
							oop thread = core->Core.threads[thread_i];
							//<引数の評価>/<Evaluation of arguments>
							if(thread->Thread.condRelPos != 0){
								if(isOnce == 0){
									Array_push(evalEventArgsThread->Thread.stack,_newInteger(buffer[DATA_DATA]));//arg 3
									if(value== ALL_MEMBER_ID){
										Array_push(evalEventArgsThread->Thread.stack,_newInteger(0));//ALL MEMBER:0
									}else if(value == ((1U) << (agent->base.myID -1))){
										Array_push(evalEventArgsThread->Thread.stack,_newInteger(1));//MYSELF:1
									}else{
										Array_push(evalEventArgsThread->Thread.stack,_newInteger(2));//OTHER:2
									}
									Array_push(evalEventArgsThread->Thread.stack,_newInteger((int)buffer[DATA_REQUEST_SENDER_ID]));//arg1
									isOnce = 1;
								}else{
									evalEventArgsThread->Thread.stack->Array.size = 4;
								}
								evalEventArgsThread->Thread.pc = thread->Thread.base + thread->Thread.condRelPos;
								for(;;){
									FLAG flag = sub_execute(evalEventArgsThread,GM);
									if(flag == F_TRUE){
										break;
									}
									else if(flag == F_FALSE){
										isFalse = 1;
										break;
									}
								}
							}
							
							//<条件が満たされたときの処理>/<Processing when the condition is met>
							if(!isFalse){
								//protect t:thread
								gc_pushRoot((void*)&core);//CHECKME: is it need?
								oop data = newArray(3);
								Array_push(data,_newInteger(buffer[DATA_DATA]));
								if(value == ALL_MEMBER_ID){
									Array_push(data,_newInteger(0));
								}else if(value == ((1U) << (agent->base.myID -1))){
									Array_push(data,_newInteger(1));
								}else{
									Array_push(data,_newInteger(2));
								}
								Array_push(data,_newInteger((int)buffer[DATA_REQUEST_SENDER_ID]));
								gc_popRoots(1);
								enqueue(thread->Thread.queue,data);
							}
						#if DEBUG
							else{
								DEBUG_LOG("not trigger\n");//remove
							}
						#endif
						}

						return core;
					}
				}
				default:{
					#if DEBUG
					DEBUG_LOG("UNSPUPPORTED REQUEST %d\n",buffer[DATA_REQUEST_TYPE]);
					#endif
					break;
				}

			}
		}else{
			printAgentData(agent);
			#if DEBUG
			if(agent->base.groupID != buffer[DATA_GROUP_ID]){
			DEBUG_LOG("UNSPUPPORTED GROUP %d (!= %d)\n",buffer[DATA_GROUP_ID],agent->base.groupID);
			}
			buffer[DATA_GROUP_KEY + SIZE_OF_DATA_GROUP_KEY] = '\0';
			if(memcmp(agent->reader.groupKey, buffer + DATA_GROUP_KEY, SIZE_OF_DATA_GROUP_KEY) != 0){
			DEBUG_LOG("UNSPUPPORTED GROUP KEY %s (!=%s)\n",buffer + DATA_GROUP_KEY,agent->reader.groupKey);
			}
			#endif
		}
	}
return core;
}

#ifdef RPI
#include <pigpio.h>
int rpi_gpioRead_0_31event_handler(oop exec, oop eh){
	uint32_t gpio_states = gpioRead_Bits_0_31();
	if(gpio_states != 0){
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal((int)gpio_states))); // enqueue a stack with gpio states
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
int rpi_gpioRead_N_event_handler_init(oop eh){
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int pin = IntVal_value(obj);

	int level = gpioRead(pin);
	if(level){
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal(level))); // enqueue a stack with gpio level
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
#endif // RPI

// Event Handler Table
	// int (*eh)(oop exec, oop eh); // event handler function
	// int (*init)(oop eh); // initialize function
	// int nArgs; // number of arguments
	// int nData; // number of data fields
struct ExecEventTable  __ExecEventTable__[] = {
	[EVENT_EH] = {event_handler,      event_handler_init,0,0 },      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init,1,2 },      // TIMER_EH
	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_object_handler_init,1,1 },      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {timer_min_handler,    event_object_handler_init,1,1 },      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_object_handler_init,1,1 },      // T_TIMER_HOUR_EH
#ifdef RPI
	[PI_GPIO_0_31_EH] = {rpi_gpioRead_0_31event_handler,      event_handler_init,2,0 },      // PI_GPIO_0_31_EH
	[PI_GPIO_EO_N_EH] = {rpi_gpioRead_N_event_handler,      rpi_gpioRead_N_event_handler_init,1,1 },		  // PI_GPIO_EO_N_EH
#endif
};
//-------------------------------
// Standard Library Functions
//-------------------------------
int lib_exit(oop stack)
{
	int status = intArray_pop(stack); // get exit status from stack
	exit(status); // exit with the given status
	return 0; // return 0 to indicate success
}

int lib_chat_send(oop stack)
{
	oop chat = popStack(stack); // get chat object from stack
	char *msg = StrVal_value(popStack(stack)); // get message from stack
	int recipient = IntVal_value(popStack(stack)); // get recipient from stack
	return 0; // return 0 to indicate success
}

int lib_math_sqrt(oop stack)
{
	int value = IntVal_value(popStack(stack)); // get value from stack
	int result = (int)sqrt((double)value); // calculate square root
	pushStack(stack, newIntVal(result)); // push result back to stack
	return 0; // return 0 to indicate success
}

int lib_timer_reset(oop stack)
{
	GC_PUSH(oop, initVal, popStack(stack)); // get initial value from stack (IntVal)
	oop instance = popStack(stack); // get timer object from stack
	printf("timer reset called with initVal: %d\n", IntVal_value(initVal));
	assert(getKind(instance) == Instance);
	assert(getKind(initVal) == IntVal);
	//DEBUG POINT
	getInstanceField(instance, 0) = newIntVal(1); // reset interval to 0
	getInstanceField(instance, 1) = initVal; // reset the timer to initial value
	time_t t = time(NULL);
	getInstanceField(instance, 2) = newIntVal((int)(t % 10000)); // reset label to current time
	GC_POP(initVal);
	return 0;
}
#ifdef RPI
oop lib_pi_gpio_set_output(oop stack){
	int pin = IntVal_value(popStack(stack));
	gpioSetMode(pin, PI_OUTPUT);
	return 0;
}
oop lib_pi_gpio_write(oop stack){
	int pin = IntVal_value(popStack(stack));
	int value = IntVal_value(popStack(stack));
	gpioWrite(pin, value);
	return 0;
}
oop lib_pi_gpio_eo_write(oop stack){
	oop instance = popStack(stack);
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int pin = IntVal_value(obj);
	int mode = IntVal_value(obj+1);
	if(mode!= PI_OUTPUT){
		reportError(DEVELOPER,0,"lib_pi_gpio_eo_write: GPIO pin %d not set as output mode", pin);
		return 0;
	}
	int value = IntVal_value(popStack(stack));
	gpioWrite(pin, value);
	return 0;
}

void communicate_wifi_group_send(oop process,oop GM){
    oop subcore = Array_pop(mstack);
    void* any = subcore->SubCore.any;
    if(any == 0){
        fatal("should be initialize wifiGroupReceive()\n");
        return;
    }
    getInt(mpc);int size_args = int_value;
    int sendToId = api();
    int value = api();
#if DEBUG
    DEBUG_LOG("sendToId:%d value:%d\n",sendToId,value);
#endif
    struct AgentInfo *MY_AGENT_INFO = (struct AgentInfo *)any;
    if(MY_AGENT_INFO == 0){
        SHICA_PRINTF("It doesn't belong to some group now\n");
        return;
    }
    struct SocketInfo *socketInfo = MY_AGENT_INFO->socket;
    agent_p agent = MY_AGENT_INFO->agent;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    buf[DATA_REQUEST_TYPE] = REQUEST_TRIGER;
    buf[DATA_GROUP_ID] = agent->base.groupID;
    buf[DATA_REQUEST_SENDER_ID] = agent->base.myID;
    printf("myid %d\n",agent->base.myID);
    if(sendToId < 0){
        buf[DATA_REQUEST_MEMEBER_ID] = ALL_MEMBER_ID;//all member: 11111111
    }else{
        buf[DATA_REQUEST_MEMEBER_ID] = (char)sendToId;//0:reader or other: member
    }   
    buf[DATA_SIZE_OF_MEMBER] = 1;
    memcpy(buf + DATA_GROUP_KEY, getAgentGroupKey(agent), 4);
    buf[DATA_DATA] = (char)value;

    // グループ参加リクエストの送信
    int ret = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buf, BUF_SIZE);
    if (ret < 0) {
        perror("send_broadcast_nonblocking");
        close(socketInfo->recv_sockfd);
        close(socketInfo->send_sockfd);
        SHICA_PRINTF("%s line %d\n",__FILE__,__LINE__);
        exit(1);
        MY_AGENT_INFO = 0;
        return;
    }
// CHECK ME:
// if it possible, check the return value of send_broadcast_nonblocking
// it is success, or not 
    return;
}

void communicate_wifi_build_group(oop process,oop GM){
   SHICA_FPRINTF(stderr, "now unsupported function\n");
   exit(1);
    return;
}

#endif // RPI
struct ExecStdFuncTable  __ExecStdFuncTable__[] = {
	[EXIT_FUNC] = {lib_exit}, // exit function
	[CHAT_SEND_FUNC] = {lib_chat_send}, // chat send function
	[MATH_SQRT_FUNC] = {lib_math_sqrt}, // math sqrt function
	[T_TIMER_RESET_FUNC] = {lib_timer_reset}, // timer reset function
#ifdef RPI
	[PI_GPIO_SET_OUTPUT_FUNC] = {lib_pi_gpio_set_output}, // Raspberry Pi GPIO set output function
	[PI_GPIO_WRITE_FUNC] = {lib_pi_gpio_write}, // Raspberry Pi GPIO write function
	[PI_GPIO_EO_WRITE_FUNC] = {lib_pi_gpio_eo_write}, // Raspberry Pi GPIO Event Object write function
#endif
};
//-------------------------------
//Event Object 
//-------------------------------
oop time_eo(oop stack){
/*
0: interval
1: count 
2: label (hold time for comparison)
*/
	GC_PUSH(oop,instance,newInstance(3)); // timer eo has 3 fields: interval, count and label
	oop* fields = getObj(instance, Instance, fields);
	fields[0] = newIntVal(1); // interval
	fields[1] = newIntVal(0); // count
	time_t t = time(NULL);
	fields[2] = newIntVal((int)(t % 10000)); // label
	GC_POP(instance);
	return instance;
}

//-------------------------------
void broadcast_eo(oop stack) {
	GC_PUSH(oop,instance,newInstance(1)); // broadcast eo has 4 fields: ipAddr, portNum, groupID, groupKey
    char *ipAddr  = "255.255.255.255";
    int portNum  = 5000;
    int groupID  = newIntval(popStack(stack));//Shuld be 8 character
    char *groupKey = StrVal_value(popStack(stack));

    // 引数チェック
    if (ipAddr == NULL || groupKey == NULL) {
        SHICA_FPRINTF(stderr, "Invalid argument\n");
        SHICA_PRINTF("%s line %d\n",__FILE__,__LINE__);
        exit(1);
    }
#if DEBUG
    DEBUG_LOG("ipAddr:%s portNum:%d groupID:%d groupKey:%s\n",ipAddr,portNum,groupID,groupKey);
#endif


    // struct ExternMemory *em = newExternMemory(8);
    // subcore->SubCore.em = em;
    // struct SocketInfo *socketInfo = (struct SocketInfo *)gc_extern_alloc(em,sizeof(struct SocketInfo));
    
    struct SocketInfo *socketInfo = (struct SocketInfo *)gc_alloc(sizeof(struct SocketInfo));
    socketInfo->type = registerExternType(STRUCT_SOCKET_INFO_TYPE) + EXTRA_KIND;
    socketInfo->addr_len = sizeof(socketInfo->sender_addr);

    // 自身のネットワークIPアドレスを取得
    if (get_network_ip(socketInfo->own_ip, sizeof(socketInfo->own_ip)) != 0) {
        SHICA_FPRINTF(stderr, "Failed to get own IP address\n");
        SHICA_PRINTF("%s line %d\n",__FILE__,__LINE__);
        exit(1);
		get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
		GC_POP(instance);
        return ;
    }

#if DEBUG
    DEBUG_LOG("Own IP: %s\n", socketInfo->own_ip);
#endif

    // 受信ソケット作成とノンブロッキング化
    if (create_receive_socket(&socketInfo->recv_sockfd, &socketInfo->recv_addr,portNum) == 0) {
        set_nonblocking(socketInfo->recv_sockfd);
    } else {
        SHICA_PRINTF("%s line %d\n",__FILE__,__LINE__);
        exit(1);
		get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
		GC_POP(instance);
        return ;
    }

    // 送信ソケット作成とブロードキャストアドレス設定
    if (create_broadcast_socket(&socketInfo->send_sockfd, &socketInfo->broadcast_addr,ipAddr,portNum) != 0) {
        close(socketInfo->recv_sockfd);
        SHICA_PRINTF("%s line %d\n",__FILE__,__LINE__);
        exit(1);
		get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
		GC_POP(instance);
        return ;
    }


    // struct AgentInfo *MY_AGENT_INFO = (struct AgentInfo *)gc_extern_alloc(em,sizeof(struct AgentInfo));
    struct AgentInfo *MY_AGENT_INFO = (struct AgentInfo *)gc_alloc(sizeof(struct AgentInfo));
    MY_AGENT_INFO->type = registerExternType(STRUCT_AGENT_INFO_TYPE) + EXTRA_KIND;
    MY_AGENT_INFO->socket = socketInfo;
  
#define TIMEOUT 1
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    buf[DATA_REQUEST_TYPE] = REQUEST_JOIN;
    buf[DATA_GROUP_ID] = groupID;
    buf[DATA_REQUEST_SENDER_ID] = 0;
    buf[DATA_REQUEST_MEMEBER_ID] = 0;
    buf[DATA_SIZE_OF_MEMBER] = 1;
    memcpy(buf + DATA_GROUP_KEY, groupKey, 4);
    buf[DATA_DATA] = 0;
    
    // グループ参加リクエストの送信
    int ret = send_broadcast_nonblocking(socketInfo->send_sockfd, &socketInfo->broadcast_addr, buf, BUF_SIZE);
    printf("send_broadcast_nonblocking\n");
    if (ret < 0) {
        printf("send_broadcast_nonblocking\n");
        agent_p agent = createAgent(AgentReader);
        agent->base.myID = 0;
        agent->base.groupID = groupID;
        agent->reader.sizeOfMember = (1U);
        memcpy(agent->reader.groupKey, groupKey,SIZE_OF_DATA_GROUP_KEY + 1);
        MY_AGENT_INFO->agent = agent;//remove me after adapt em
		get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
		GC_POP(instance);
		return;
    }
    // グループ参加リクエストの受信
    time_t start = time(NULL);
    while(time(NULL) - start < TIMEOUT){
        ssize_t ret = recvfrom(socketInfo->recv_sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&socketInfo->sender_addr, &socketInfo->addr_len);
        if (ret > 0) {
            buf[ret] = '\0'; // Null終端
            char *sender_ip = inet_ntoa(socketInfo->sender_addr.sin_addr);
            // 自分自身の送信データを無視
            if (strcmp(sender_ip, socketInfo->own_ip) == 0) {
#if DEBUG
                DEBUG_LOG("same ip\n");
#endif
                continue;
            }
#if DEBUG
            DEBUG_LOG("\nReceived from %s: %s\n", sender_ip, buf);
#else
            SHICA_PRINTF("\nReceived from %s: %s\n", sender_ip, buf);
#endif
            if(buf[DATA_GROUP_ID] == groupID && memcmp(buf + DATA_GROUP_KEY, groupKey, SIZE_OF_DATA_GROUP_KEY) == 0){
                switch(buf[DATA_REQUEST_TYPE]){
                    case REQUEST_TO_BE_MEMBER:{
                        agent_p agent = createAgent(AgentMember);
                        agent->base.myID    = buf[DATA_REQUEST_SENDER_ID];
                        agent->base.groupID = buf[DATA_GROUP_ID];
                        memcpy(agent->member.groupKey,buf + DATA_GROUP_KEY,SIZE_OF_DATA_GROUP_KEY + 1);
#if DEBUG
                        DEBUG_LOG("Join Group Success: my id is %d\n",agent->base.myID);
#endif
                        printf("Join Group Success: my id is %d\n",agent->base.myID);
                        MY_AGENT_INFO->agent = agent;//remove me after adapt em
						get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
						GC_POP(instance);
                        return;
                    }
                    case REQUEST_REJECT:{
#if DEBUG
                        DEBUG_LOG("Join Group Reject\n");
#endif
                        close(socketInfo->recv_sockfd);
                        close(socketInfo->send_sockfd);
                        MY_AGENT_INFO = 0;//remove me after em
                        get(instance,Instance,fields)[0] = 0;
						GC_POP(instance);
                        return;
                    }
                }
                break;
            }
        }
    }
    // タイムアウト
    printf("BUILD GROUP\n");
    agent_p agent = createAgent(AgentReader);
    agent->base.myID = 1;
    agent->base.groupID = groupID;
    agent->reader.sizeOfMember = (1U);
    setAgentGroupKey(agent,groupKey);
    MY_AGENT_INFO->agent = agent;//remove me after adapt em
    get(instance,Instance,fields)[0] = (void *)MY_AGENT_INFO;
	GC_POP(instance);
    return;
#undef TIMEOUT
}


#ifdef RPI
int isInitGPIO = 0;
oop rpi_gpio_eo(oop stack){
/*
0: pin
1: mode
2: init (pud|vol)
*/
	GC_PUSH(oop,instance,newInstance(3)); // gpio eo has 3 fields: pin, mode, init
	oop* fields = getObj(instance, Instance, fields);
	fields[0] = popStack(stack); // pin
	fields[1] = popStack(stack); // mode
	fields[2] = popStack(stack); // init
	if(!isInitGPIO){
		for(int try_init=0; try_init<5; try_init++){
			if(gpioInitialise() >= 0){
				isInitGPIO = 1;
				break;
			}
			else{
				reportError(DEVELOPER,0,"rpi_gpio_eo: gpioInitialise failed, retrying %d/5", try_init+1);
				sleep(1);
			}
		}
		if(!isInitGPIO){
			reportError(DEVELOPER,0,"rpi_gpio_eo: gpioInitialise failed after 5 attempts");
		}
	}
	if(IntVal_value(fields[1]) == PI_INPUT){
		int pud = IntVal_value(fields[2]);
		gpioSetPullUpDown(IntVal_value(fields[0]), pud);
	}else{
		gpioSetMode(IntVal_value(fields[0]), PI_OUTPUT);
		gpioWrite(IntVal_value(fields[0]), IntVal_value(fields[2]));
	}
	GC_POP(instance);
	return instance;
}
#endif // RPI
//-------------------------------

struct ExecEventObjectTable __ExecEventObjectTable__[] = {
	[CHAT_BROADCAST_EO] = {broadcast_eo},
	[TIME_EO] = {time_eo},
#ifdef RPI
	[PI_GPIO_EO] = {rpi_gpio_eo},
#endif
};
#endif // SHICAEXEC

// struct EventTable __EventTable__[] = {
// 	[EVENT_EH] = {event_handler,      event_handler_init, 0, NULL, 0},      // EVENT_EH
// 	[TIMER_EH] = {timer_handler,      timer_handler_init, 1,(char []){Integer}, 2},      // TIMER_EH
// 	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
// 	[T_TIMER_MIN_EH] = {timer_min_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
// 	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
// };

//  struct StdFuncTable __StdFuncTable__[] =
// {
// 	{lib_exit, 1, (int[]){Integer}, Undefined}, // exit function takes 1 argument
// 	{lib_chat_send, 2, (int[]){String, Integer}, Undefined}, // chat send function takes 2 arguments
// 	{lib_math_sqrt, 1, (int[]){Integer}, Integer}, // math sqrt function takes 1 argument and returns an integer
// 	{lib_timer_reset, 1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
// };

// eo_func_t __EventObjectFuncTable__[] = {
// 	[WEB_RTC_BROADCAST_EO] = wifi_udp_broadcast_eo,
// 	[TIME_EO] = time_eo,
// };
// struct EventObjectTable __EventObjectTable__[] = {
// 	[WEB_RTC_BROADCAST_EO] = {3, 1, (int[]){String, Integer, String}}, // WebRTC broadcast event object with 3 arguments and 1 function
// 	[TIME_EO] = {0, 4, NULL}, // Timer event object with 0 arguments and 4 functions
// };

#endif // STDLIB_C