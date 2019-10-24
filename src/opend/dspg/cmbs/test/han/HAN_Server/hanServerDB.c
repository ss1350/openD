
#include "hanServer.h"


u32 p_HanServer_AddMessageRequiredResponse(s32 responseMessageIndex, u16 u16_clientId, struct sockaddr_in* cliaddres);
u32 p_HanServer_alignTimeStamp();
void p_HanServer_CleanClientPendingMessages(u16 clientId);
s32 p_HanServer_hanMessagesResponseMapping(const char *message);





/////////////////////////////////////////////////////////////////////////////////////////////
// Multiple clients

#define INVALID_CLIENT_ID           				0
#define MAX_CLIENT_REQUEST_PER_MESSAGE       	    100
#define MAX_MULTIPLE_CLIENTS 						10
#define MAX_TIME_STAMP 								0xFFFFFFFF
#define TIME_TO_WAIT_FOR_CLIENTS_TO_REPLY        	10000
u16 u16_currentMaxClientId = 1;



typedef struct{
	u16 u16_clientId;
	struct sockaddr_in cliaddr;
	u8 u8_indicationForKeepAlive;
}s_clientInfo;


typedef struct{
	u16 u16_clientId;
	u32 u32_timeStamp;
}s_pendingClientsForResponse;

typedef struct {
    char *pReqFromClient;
    char *pResToClient;
	s_pendingClientsForResponse clientRequested[MAX_CLIENT_REQUEST_PER_MESSAGE];
}s_MessageRequiredResponse;

s_MessageRequiredResponse g_hanMessagesRequiredResponse[] =
{
	{ "INIT",						"INIT_RES"						},
    { "OPEN_REG", 					"OPEN_RES" 						},
    { "CLOSE_REG", 					"CLOSE_RES" 					},
    { "INIT", 						"INIT_RES" 						},
    { "GET_DEV_TABLE",  			"DEV_TABLE" 					},
    { "DELETE_DEV", 				"DELETE_DEV_RES" 				},
    { "GET_DEV_INFO",   			"DEV_INFO" 						},
    { "GET_EEPROM_PARAM",  		 	"GET_EEPROM_PARAM_RES" 			},
    { "SET_EEPROM_PARAM",   		"SET_EEPROM_PARAM_RES" 			},
    { "GET_EEPROM_BYTES",   		"GET_EEPROM_BYTES_RES" 			},
    { "SET_EEPROM_BYTES",   		"SET_EEPROM_BYTES_RES" 			},
    { "GET_PRODUCTION_PARAM",   	"GET_PRODUCTION_PARAM_RES" 		},
    { "SET_PRODUCTION_PARAM",   	"SET_PRODUCTION_PARAM_RES" 		},
    { "GET_EEPROM_SIZE",   			"GET_EEPROM_SIZE_RES" 			},
    { "GET_TARGET_HW_VERSION",  	"GET_TARGET_HW_VERSION_RES" 	},
    { "GET_SW_VERSION",   			"GET_SW_VERSION_RES" 			},
    { "GET_RAM_BYTES",   			"GET_RAM_BYTES_RES" 			},
    { "SET_RAM_BYTES",   			"SET_RAM_BYTES_RES" 			},
    { "GET_NUM_OF_FUN_MSG_IN_Q",    "GET_NUM_OF_FUN_MSG_IN_Q_RES" 	},
	{ "GET_DEV_TABLE_PHASE_2",  	"DEV_TABLE_PHASE_2" 			},
	{ "GET_DEV_INFO_PHASE_2",   	"DEV_INFO_PHASE_2" 				},
	{ "GET_BLACK_LIST_DEV_TABLE",	"BLACK_LIST_DEV_TABLE"			},
};

#define NUM_OF_MESSAGES_REQUIRED_RESPONSE         sizeof(g_hanMessagesRequiredResponse) / sizeof(s_MessageRequiredResponse)

s_clientInfo g_clientsInfo[MAX_MULTIPLE_CLIENTS];

u32 g_u32_currentNumOfClients;
bool g_cleanNonresponsiveClientsInProgress;

static u32 p_HanServer_AddNewClient(const char *pCommand, u32* pu32_currentClientIndex, struct sockaddr_in* cliaddres);

/////////////////////////////////////////////////////////////////////////////////////////////
extern t_CmdCb g_hanProxyCB[];
extern t_CmdCb g_ServiceProxyCB[];
extern t_CmdCb g_DebugProxyCB[];


void p_HanServer_ServerDbInitialize()
{
	g_u32_currentNumOfClients = 0;
	memset(&(g_clientsInfo),0,sizeof(g_clientsInfo));
	p_HanServer_InitializeMessagesDB();
}





u32 p_HanServer_InitializeMessagesDB()
{
	u32 i;
	for(i=0; i<NUM_OF_MESSAGES_REQUIRED_RESPONSE;i++)
	{
		memset(&(g_hanMessagesRequiredResponse[i].clientRequested),0,sizeof(g_hanMessagesRequiredResponse[i].clientRequested));
	}					
	
	
	return 0;
}



s32 p_HanServer_hanMessagesResponseMapping(const char* message)
{
	s32 index;
	for(index=0; index < NUM_OF_MESSAGES_REQUIRED_RESPONSE; index++)
	{
		if(!strcmp(message,g_hanMessagesRequiredResponse[index].pReqFromClient))
		{
			return index;
		}
	}

	return -1;
}

s32 p_HanSever_Send2RemoteSocket(char *pCommand, struct sockaddr_in* cliaddr)
{
#ifndef HAN_SERVER
    return 0;
#else

    s32 s32_destSocket = -1;

	s32_destSocket = g_destSocket;


    if ( s32_destSocket != -1 )
    {
        //if ( RemoteConnected ) // Removed for now (TB investigated).
        {
            HAN_SERVER_INFO_PRINT("Message to Remote:\n%s\n", pCommand);
        }		
		SocketSendTo(s32_destSocket, (unsigned char *)pCommand, strlen(pCommand), cliaddr);
    }

    return 0;

#endif //HAN_SERVER
}



u32 p_HanServer_alignTimeStamp()
{
	u32 i,j;
	u32 minTimeStamp = MAX_TIME_STAMP;

	//find minimum time stamp
	for (i = 0; i < NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		for (j = 0; j < MAX_CLIENT_REQUEST_PER_MESSAGE; j++)
		{
			if(g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp < minTimeStamp)
			{
				minTimeStamp = g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp;
			}
		}
	}

	//align the time stamp
	for (i = 0; i < NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		for (j = 0; j < MAX_CLIENT_REQUEST_PER_MESSAGE; j++)
		{
			g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp -= minTimeStamp;
		}
	}

	return minTimeStamp;
}




/*
	sends "keep alive" messages to all clients and wait a few secound for response.
	if a client did not respond:
		1) delete every pending message of this client from the queue.
		2) delete the client from the array, and shrink the array.
*/
void* p_HanServer_checkClientsStatus()
{
	u32 i,j;

	if(g_u32_currentNumOfClients == 0)
	{
		g_cleanNonresponsiveClientsInProgress = 0;
		return NULL;
	}

	//clean nonresponsive clients
	j = g_u32_currentNumOfClients - 1;
	for(i = 0; i< MAX_MULTIPLE_CLIENTS && i<=j; i++)
	{
		if(g_clientsInfo[i].u16_clientId != INVALID_CLIENT_ID && g_clientsInfo[i].u8_indicationForKeepAlive == 0)
		{
			//delete pending message
			p_HanServer_CleanClientPendingMessages(g_clientsInfo[i].u16_clientId);
			//delete from the array
			g_clientsInfo[i].u16_clientId = INVALID_CLIENT_ID;
			memset(&(g_clientsInfo[i].cliaddr), 0 , sizeof(g_clientsInfo[i].cliaddr));
			g_u32_currentNumOfClients--;
			
			//then check if we can insert other client from the end of the array						
			for(; j>i; j--)
			{
				if(g_clientsInfo[j].u8_indicationForKeepAlive == 1)
				{
					//copy the client
					g_clientsInfo[i].u16_clientId = g_clientsInfo[j].u16_clientId;
					g_clientsInfo[i].cliaddr = g_clientsInfo[j].cliaddr;
					
					//delete from the array
					g_clientsInfo[j].u16_clientId = INVALID_CLIENT_ID;
					memset(&(g_clientsInfo[j].cliaddr), 0, sizeof(g_clientsInfo[j].cliaddr));

					//update the index
					j--;
					break;
				}
				else
				{
					//delete pending message from the queue
					p_HanServer_CleanClientPendingMessages(g_clientsInfo[j].u16_clientId);
					//delete from the array
					g_clientsInfo[j].u16_clientId = INVALID_CLIENT_ID;
					memset(&(g_clientsInfo[j].cliaddr), 0, sizeof(g_clientsInfo[j].cliaddr));
					g_u32_currentNumOfClients--;
				}
			}
			if (i == j)
				break;
		}

	}
	g_cleanNonresponsiveClientsInProgress = 0;


	return NULL;
	
}

s32 p_HanSever_Send2AllRemoteSockets(char *pCommand)
{
				
#ifndef HAN_SERVER
    return 0;
#else

    s32 s32_destSocket = -1;
	u32 i;
	struct sockaddr_in* cliaddr;

	s32_destSocket = g_destSocket;


    if ( s32_destSocket != -1 )
    {
        //if ( RemoteConnected ) // Removed for now (TB investigated).
        {
            HAN_SERVER_INFO_PRINT("Message to Remote:\n%s\n", pCommand);
        }

			
		for(i = 0; i<g_u32_currentNumOfClients; i++)
		{
			if(g_clientsInfo[i].u16_clientId == INVALID_CLIENT_ID)
				continue;
			cliaddr = &(g_clientsInfo[i].cliaddr);
			SocketSendTo(s32_destSocket, (unsigned char *)pCommand, strlen(pCommand), cliaddr);
		}
    }
	

    return 0;

#endif //HAN_SERVER	
}

// Send HAN message from App. to all connected HAN clients
int p_HanServer_AppSendMessage(const char *pMessage)
{
    return p_HanSever_HandleSendMessageToClient(pMessage);
}

int p_HanSever_HandleSendMessageToClient(const char *pCommand)
{
	t_st_paramList st_paramList;
	
	st_paramList.u16_CurSize = 0;
	st_paramList.u16_MaxSize = INTERNAL_BUFFER_SIZE;
	p_HanServer_serByte(&st_paramList,HAN_SERVER_SEND_MSG_TO_CLIENT);
	p_HanServer_serString(&st_paramList,(unsigned char*)pCommand);
	st_paramList.pu8_Buffer[st_paramList.u16_CurSize] = '\0';
	
	return SocketSendTo(g_internalSocket,st_paramList.pu8_Buffer,sizeof(st_paramList.pu8_Buffer),handle);
}


void p_HanSever_SendMessageToClient(char *pCommand)
{

	u32 i,j;
	bool b_isClientPendingForResponse = 0;
	s32 s32_indexOfFirstPendingClient = -1;
	u32 minTimeStamp = -1;
	u16 pendingClientId;
	char *token;
	char* delimiter = "\r\n";
	char prefix[CMBS_STRING_SIZE + 1];
	char responseMessage[CMBS_STRING_SIZE + 1];

	memcpy(prefix,pCommand, sizeof(prefix));
	token = strtok(prefix, delimiter);
	if(!strncmp(prefix, "[SRV]", strlen("[SRV]")) || !strncmp(prefix, "[DBG]", strlen("[DBG]")))
	{
		token = strtok(NULL, delimiter);
		strcpy(responseMessage, token);
	}
	else
	{
		strcpy(responseMessage, token);
	}


	for(i = 0; i < NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		if(!strcmp(g_hanMessagesRequiredResponse[i].pResToClient,responseMessage))
		{
			for(j=0;j<MAX_CLIENT_REQUEST_PER_MESSAGE;j++)
			{
				if( g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId == INVALID_CLIENT_ID)
				{
					break;
				}
				if(g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp< minTimeStamp)
				{
					minTimeStamp = g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp;
					pendingClientId = g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId;
					s32_indexOfFirstPendingClient = j;
					b_isClientPendingForResponse = 1;
				}
			}

			//insert the last pending client in the array instead of the client we are about to send the response.
			if(b_isClientPendingForResponse)
			{
				//first copy
				g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfFirstPendingClient].u16_clientId = g_hanMessagesRequiredResponse[i].clientRequested[j-1].u16_clientId;
				g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfFirstPendingClient].u32_timeStamp = g_hanMessagesRequiredResponse[i].clientRequested[j-1].u32_timeStamp;

				//then delete
				memset(&(g_hanMessagesRequiredResponse[i].clientRequested[j-1]),0,sizeof(s_pendingClientsForResponse));
			}
			break;
		}
	}


	if(b_isClientPendingForResponse)
	{
		for(i = 0; i<MAX_MULTIPLE_CLIENTS;i++)
		{
			if (g_clientsInfo[i].u16_clientId == pendingClientId)
			{
				p_HanSever_Send2RemoteSocket(pCommand,&(g_clientsInfo[i].cliaddr));
				break;
			}
		}		
	}
	else
	{
		p_HanSever_Send2AllRemoteSockets(pCommand);
	}
}

static u32 p_HanServer_AddNewClient(const char *pCommand,u32* pu32_currentClientIndex,struct sockaddr_in* cliaddres)
{
	u32 i;
	if(g_u32_currentNumOfClients < MAX_MULTIPLE_CLIENTS)
	{//array isn't full
		*pu32_currentClientIndex = g_u32_currentNumOfClients;
		g_clientsInfo[*pu32_currentClientIndex].u16_clientId = u16_currentMaxClientId;
		memcpy(&(g_clientsInfo[*pu32_currentClientIndex].cliaddr), cliaddres, sizeof(*cliaddres));
		g_clientsInfo[*pu32_currentClientIndex].u8_indicationForKeepAlive= 1;
		g_u32_currentNumOfClients++;
				
		//If client array is full - delete all the non-responsive clients.
		if(g_u32_currentNumOfClients == MAX_MULTIPLE_CLIENTS && !g_cleanNonresponsiveClientsInProgress)
		{ //send keep alive for all clients in order to clear the array from unactive clients.
			g_cleanNonresponsiveClientsInProgress = 1;

			//Send keep alive to all clients except the last client.
			for(i = 0; i < MAX_MULTIPLE_CLIENTS - 1; i++)
			{
				g_clientsInfo[i].u8_indicationForKeepAlive = 0; //reset the previos response.
				SocketSendTo(g_destSocket, (unsigned char *)"KEEP_ALIVE", strlen("KEEP_ALIVE"), &(g_clientsInfo[i].cliaddr));
			}

			//use the timer of 'select' for clean non responsive clients.
			u32_HANClientTimeout = TIME_TO_WAIT_FOR_CLIENTS_TO_REPLY;
		}
				
		u16_currentMaxClientId++;
		if(u16_currentMaxClientId == INVALID_CLIENT_ID) // in case we reached to MAX-u16
			u16_currentMaxClientId++;
	}
	else
	{
		//send message to client that we can't support because the array is full.
		//try to delete other clients.
		SocketSendTo(g_destSocket, (unsigned char *)"ERROR/r/n TOO_MANY_CLIENTS/r/n/r/n", strlen("ERROR/r/n TOO_MANY_CLIENTS/r/n/r/n"), cliaddres);
				
		//delete all the non-responsive clients.
		if(!g_cleanNonresponsiveClientsInProgress)
		{ //send keep alive for all clients in order to clear the array from unactive clients.
			g_cleanNonresponsiveClientsInProgress = 1;
			
			//Send keep alive to all clients.
			for(i = 0; i < MAX_MULTIPLE_CLIENTS; i++)
			{
				g_clientsInfo[i].u8_indicationForKeepAlive = 0; //reset the previos response.
				SocketSendTo(g_destSocket, (unsigned char *)"KEEP_ALIVE", strlen("KEEP_ALIVE"), &(g_clientsInfo[i].cliaddr));
			}

			//use the timer of 'select' for clean non responsive clients.
			u32_HANClientTimeout = TIME_TO_WAIT_FOR_CLIENTS_TO_REPLY;
		}
		return 0;
	}

	return 1;
}



u32 p_HanServer_AddMessageRequiredResponse(s32 s32_responseMessageIndex, u16 u16_clientId, struct sockaddr_in* cliaddres)
{
	u32 i;
	//try to find empty slot for the messages
	for(i=0; i < MAX_CLIENT_REQUEST_PER_MESSAGE;i++)
	{
		//empty slot found - save client and timestamp information			
		if(g_hanMessagesRequiredResponse[s32_responseMessageIndex].clientRequested[i].u16_clientId == INVALID_CLIENT_ID)
		{
						
			g_hanMessagesRequiredResponse[s32_responseMessageIndex].clientRequested[i].u16_clientId = u16_clientId;
			if(u32_timeStamp == MAX_TIME_STAMP)
			{ // in case we reached to MAX-u32, try to align the timestamp by subtract the MIN-timestamp in the arrays from all the timestamps in the arrays.
				u32_timeStamp -= p_HanServer_alignTimeStamp() + 1;
			}
								
			g_hanMessagesRequiredResponse[s32_responseMessageIndex].clientRequested[i].u32_timeStamp = u32_timeStamp;
			u32_timeStamp++;

			break;
		}
	}

	//We didn't find empty slot in the array, send message to client that we can't support his message.
	if(i== MAX_CLIENT_REQUEST_PER_MESSAGE)
	{
		SocketSendTo(g_destSocket, (unsigned char *)"ERROR/r/n TOO_MANY_MESSAGES/r/n/r/n", strlen("ERROR/r/n TOO_MANY_MESSAGES/r/n/r/n"), cliaddres);


		return 0;
	}

	return 1;
}





/*
	Search for the message in the queue.
	If found - update "cliaddr" with the address of the pending client, and remove the message from the queue.
*/
void p_HanServer_RemovePendingMessageForClient(char* responseMessage)
{

	u32 i,j;
	bool b_isClientPendingForResponse = 0;
	s32 s32_indexOfFirstPendingClient = -1;
	u32 minTimeStamp = -1;
	for(i = 0; i < NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		if(!strcmp(g_hanMessagesRequiredResponse[i].pResToClient,responseMessage))
		{
			for(j=0;j<MAX_CLIENT_REQUEST_PER_MESSAGE;j++)
			{
				if( g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId == INVALID_CLIENT_ID)
				{
					break;
				}
				if(g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp< minTimeStamp)
				{
					minTimeStamp = g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp;
					s32_indexOfFirstPendingClient = j;
					b_isClientPendingForResponse = 1;
				}
			}

			//insert the last pending client in the array instead of the client we are about to send the response.
			if(b_isClientPendingForResponse)
			{
				//first copy
				g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfFirstPendingClient].u16_clientId = g_hanMessagesRequiredResponse[i].clientRequested[j-1].u16_clientId;
				g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfFirstPendingClient].u32_timeStamp = g_hanMessagesRequiredResponse[i].clientRequested[j-1].u32_timeStamp;

				//then delete
				memset(&(g_hanMessagesRequiredResponse[i].clientRequested[j-1]),0,sizeof(s_pendingClientsForResponse));
				
			}
			break;
		}
	}


	
}

void p_HanServer_CleanClientPendingMessages(u16 u16_clientId)
{
	s32 i,j;
	s32 s32_indexOfLastPendingClient;
	for(i = 0; i < NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		s32_indexOfLastPendingClient= -1;
		//find index of the last pending client for this message
		for(j=0; j< MAX_CLIENT_REQUEST_PER_MESSAGE; j++)
		{
			if( g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId != INVALID_CLIENT_ID)
			{
				s32_indexOfLastPendingClient=j;
				break;
			}
		}


		//check if clientId is pending for this message
		for(j = 0; j < s32_indexOfLastPendingClient; j++)
		{
			if( g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId == u16_clientId)
			{
			
				//insert the last pending client for this message instead of it
				g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId = g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfLastPendingClient].u16_clientId;
				g_hanMessagesRequiredResponse[i].clientRequested[j].u32_timeStamp = g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfLastPendingClient].u32_timeStamp;

				//delete it and update index
				memset(&(g_hanMessagesRequiredResponse[i].clientRequested[s32_indexOfLastPendingClient]),0,sizeof(s_pendingClientsForResponse));
				s32_indexOfLastPendingClient--;

				
				//update this index because we want to check this node again after the switch.
				j--;
			}
			
		}
		
		if(j == s32_indexOfLastPendingClient)
		{
			if( g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId == u16_clientId)
			{//delete the pending message
				memset(&(g_hanMessagesRequiredResponse[i].clientRequested[j]),0,sizeof(s_pendingClientsForResponse));
			}
		}

	}
}

void p_HanServer_MessageFromClient(struct sockaddr_in* cliaddres, const t_CmdCb *g_CBHandler, char *buf)
{
	
	u32 u32_currentClientIndex = 0;
	bool b_foundClient = 0;
	const char *curMessage;
	s32 s32_responseMessageIndex;
	u32 i;
	
	//search client
	for(i=0; i<g_u32_currentNumOfClients; i++)
	{
		if (!memcmp(cliaddres, &(g_clientsInfo[i].cliaddr), sizeof(*cliaddres)))
		//this client already registerd.
		{
			u32_currentClientIndex = i;
			b_foundClient = 1;
			break;
		}
	}

	//add new client to array
	if(!b_foundClient)
	{
		if(strstr(buf, "INIT") != NULL )
		{
			if(!p_HanServer_AddNewClient(buf,&u32_currentClientIndex, cliaddres))
			{
				//could not add the new client
				memset(buf, 0, CMBS_STRING_SIZE);
				return;
			}
		}
		else
		{
			SocketSendTo(g_destSocket, (unsigned char *)"ERROR/r/n REQUIRED_INIT/r/n/r/n", strlen("ERROR/r/n REQUIRED_INIT/r/n/r/n"), cliaddres);
		}
	}

		
	//speicel cases - messages that can ne handeled by this thread
	if (strstr(buf, "KEEP_ALIVE_RES") != NULL)
	{ // receive keep alive message, marked it and move on.
		g_clientsInfo[u32_currentClientIndex].u8_indicationForKeepAlive = 1;
		memset(buf, 0, CMBS_STRING_SIZE);
		return;
	}
	
		

		
    HAN_SERVER_INFO_PRINT("Message from remote: \n%s", buf);
    // parse and execute received command	
    for (i = 0; g_CBHandler[i].pCmd != NULL; i++)
    {
		curMessage = g_CBHandler[i].pCmd;
		if (strstr(buf, curMessage) != NULL)
    	{				
    		//check if this message required response
           	s32_responseMessageIndex = p_HanServer_hanMessagesResponseMapping(curMessage);
			if (s32_responseMessageIndex != -1 )
           	{
				//Add messages that required response.
				if(!p_HanServer_AddMessageRequiredResponse(s32_responseMessageIndex,g_clientsInfo[u32_currentClientIndex].u16_clientId,
														&g_clientsInfo[u32_currentClientIndex].cliaddr))
				{
					break;
				}
           	}

			//HAN_SERVER_DBG("Trying to match %s\n",g_CBHandler[i].pCmd);
			//call handler function
			g_CBHandler[i].pProxyCB(buf);
			memset(buf, 0, CMBS_STRING_SIZE);
			break;
		}
	}
	

		
}

u32 p_HanSever_DeleteClient(struct sockaddr_in* cliaddres)
{
	u32 i;
	s32 s32_currentClientIndex = 0;
	bool b_foundClient = 0;
	
	//search client
	for(i=0; i<g_u32_currentNumOfClients; i++)
	{
		if (!memcmp(cliaddres, &(g_clientsInfo[i].cliaddr), sizeof(*cliaddres)))
		{
			s32_currentClientIndex = i;
			b_foundClient = 1;
			break;
		}
	}

	if(!b_foundClient)
		return 0;

	if(s32_currentClientIndex != g_u32_currentNumOfClients-1)
	{
		memcpy(&(g_clientsInfo[s32_currentClientIndex].cliaddr), &(g_clientsInfo[g_u32_currentNumOfClients-1].cliaddr), sizeof(*cliaddres));
		g_clientsInfo[s32_currentClientIndex].u16_clientId = g_clientsInfo[g_u32_currentNumOfClients-1].u16_clientId;
	}

	//delete pending message
	p_HanServer_CleanClientPendingMessages(g_clientsInfo[g_u32_currentNumOfClients-1].u16_clientId);
	
	//delete from the array
	g_clientsInfo[g_u32_currentNumOfClients-1].u16_clientId = INVALID_CLIENT_ID;
	memset(&(g_clientsInfo[g_u32_currentNumOfClients-1].cliaddr), 0 , sizeof(g_clientsInfo[g_u32_currentNumOfClients-1].cliaddr));
	
	g_u32_currentNumOfClients--;

	return 1;

	

}

void p_HanServer_DBStat()
{
	u32 i, j;
	u32 u32_counter = 0;
	HAN_SERVER_INFO_PRINT("\n");
	HAN_SERVER_INFO_PRINT("**************************************************************************************");
	HAN_SERVER_INFO_PRINT("\nDB Status:\n\n");

	HAN_SERVER_INFO_PRINT("\tNum of Client connected: %d\n", g_u32_currentNumOfClients);

	if (g_u32_currentNumOfClients != 0)
		HAN_SERVER_INFO_PRINT("\n\t\tClient List:\n\n");

	for (i = 0; i < g_u32_currentNumOfClients; i++)
	{
		HAN_SERVER_INFO_PRINT("\t\tClient number #%d with ID: %d\n", i+1, g_clientsInfo[i].u16_clientId);
	}

	HAN_SERVER_INFO_PRINT("\n\tMessages List:\n\n");

	for (i = 0; i< NUM_OF_MESSAGES_REQUIRED_RESPONSE; i++)
	{
		for (j = 0; j<MAX_CLIENT_REQUEST_PER_MESSAGE; j++)
		{
			if (g_hanMessagesRequiredResponse[i].clientRequested[j].u16_clientId != INVALID_CLIENT_ID)
				u32_counter++;
			else
				break;
		}

		HAN_SERVER_INFO_PRINT("\t\tFor Message: %s, There are %d clients that wait for resonse\n", g_hanMessagesRequiredResponse[i].pResToClient, u32_counter);
		u32_counter = 0;
	}
	HAN_SERVER_INFO_PRINT("\n**************************************************************************************");
	HAN_SERVER_INFO_PRINT("\n");

}






