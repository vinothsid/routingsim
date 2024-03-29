
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define MYPORT "4950"    // the port users will be connecting to
#define SERVERPORT "4950"    // the port users will be connecting to

#define MAXBUFLEN 100

float *distVector,**tempDistVector,**adjMatrix;
int source,numNodes;
int *forwTable;
pthread_mutex_t dvMutex = PTHREAD_MUTEX_INITIALIZER;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int printVector(float *f,int n) {
	int i=0;
	//printf("Printing vector\n");
	for(i=0;i<n;i++) {
		printf(" %f ",f[i]);
	}
	printf("\n");
	return 1;
}
int printForwTable(int n) {
	int i=0;
	printf("*************************************\n");
	printf("*           Forwarding table        * \n");
	printf("*************************************\n");
	printf("To\t NextHop \t Cost\n");
	for(i=0;i<n;i++) {
		if(i!=source)
			printf("%d %10d %16f \n",i+1,forwTable[i] + 1,distVector[i] );
		else
			printf("%d %10d %16f \n",i+1,forwTable[i],distVector[i] );
			//printf(" %d " , forwTable[i]);
	}
	printf("\n");
	return 1;
}

void readIt(char *file){
        int first =0;
        int dist =0;
        int x,y;
        FILE *fp;
        size_t lsize;
        fp =  fopen(file,"r");
        fseek (fp , 0 , SEEK_END);
        lsize = ftell (fp);
        rewind (fp);
        char *pt,*str;
        int j=0;
        str=(char *)malloc(sizeof(char)*lsize);
        fread(str,lsize,1,fp);
        pt=strtok(str,";, \n");
        while(pt!=NULL){
                dist++;
                if(first==0){
                        first=1;
                        numNodes = atoi(pt);
                //      printf("The number of nodes are %d",NODES);
                        init(numNodes);
                        dist=0;
                }
                if(dist==1){
                        x=atoi(pt);
                //      printf("Argument %d %d\n",j+1,atoi(pt));        
                }
                if(dist==2){
                        y=atoi(pt);
                //      printf("Argument %d %d\n",j+1,atoi(pt));                        
                }
                if(dist==3){
                //      printf("Argument %d %f\n",j+1,atof(pt));                
                        adjMatrix[x-1][y-1]=atof(pt);
                        tempDistVector[x-1][y-1]=adjMatrix[x-1][y-1];
                        adjMatrix[y-1][x-1]=atof(pt);
                        tempDistVector[y-1][x-1]=adjMatrix[y-1][x-1];
                        dist=0;
                }

                //printf("Argument %d %d\n",j+1,atoi(pt));
                pt=strtok(NULL,";, \n");
                j++;
        }

        fclose(fp);

}

char *encode (int src,int n,float *dv) {
	int i=0;
	char *str= (char *)malloc(100) ;

	sprintf(str,"%d",src,n);

	//printf("With only source : %s \n",str);
	for(i=0;i<n;i++) {
		sprintf(str,"%s %f" , str,dv[i]  );
	}

	//printf("encode : %s\n" , str);
	return str;	

}
void *server() {
	float *tmpDV;
	int src;

	while(1) {
		tmpDV = (float *)malloc(sizeof(float) * numNodes);
		src= udpRecv(tmpDV);
//		printVector(tmpDV,numNodes);
		computeRouteDV(src,numNodes,tmpDV);
		//printf("*************************************\n");
		//printf("*           Distance Vector         * \n");
		//printf("*************************************\n");
		//printVector(distVector,numNodes);

		
		printForwTable(numNodes);
	}
}

int udpRecv(float *f)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    //printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n",inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("Received: source [distance vectors]  : \"%s\"\n", buf);

   // float *tmpDv=(float *)malloc(sizeof(float) * numNodes);
    int src;
    src = decode( buf ,f,numNodes );
    printf("Received from : %d \n",src);
    //printVector(tmpDv,numNodes); 
    close(sockfd);

    return src;
}

//Right now it is sending the argument that is being pased with this
//We can make it more generic though
//
void client(char *addr)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char *str;	
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(addr, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

    pthread_mutex_lock(&dvMutex);
    str=encode(source,numNodes,distVector);
    pthread_mutex_unlock(&dvMutex);
    if ((numbytes = sendto(sockfd, str, strlen(str), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);
    //printf("talker: sent %d bytes to %s\n", numbytes, addr);
    //printf("talker: Data %s\n",str);
    free(str);
    close(sockfd);

}

void sendAll(char *allIP) {
	char *tmp;
	char temp[100];
	strcpy(temp,allIP);
        tmp=strtok(temp," ");
        while(tmp!=NULL) {
                client(tmp);
                tmp=strtok(NULL," ");
        }
}

void *clientFunc(void *allIP) {

	while(1) {
		sendAll((char *)allIP);
		sleep(2);
	}
}



int decode( char *str,float *dv,int n ) {
	char *tmp;
	tmp=strtok(str," ");
	int nodeNum =atoi(tmp);
	//printf("Number of nodes : %d \n",numNodes);
	int i=0;
	for(i=0;i<n;i++) {
		tmp=strtok(NULL," ");
		dv[i]=atof(tmp);
	}

	return nodeNum;	

}
/*
int updateVector(int fromVertex , int toVertex , int n) {
        float d;
        d= distVector[toVertex][fromVertex];
        int i=0;

        //printf("Node  %d received distance vector from node %d \n" , toVertex , fromVertex);
        for(i=0; i<n; i++ ) {
                if( distVector[fromVertex][i] != -1) {
                        if( distVector[toVertex][i] == -1 ||  (distVector[fromVertex][i]+d ) < distVector[toVertex][i] ) {
                                distVector[toVertex][i] = distVector[fromVertex][i] + d;
                                forwTable[toVertex][i] = fromVertex;
                                flag[toVertex] = 1; // To say that it has updated its distance vector and need to sent to neighbours again 
                        }
                }
        }
        return 1;
}
*/
int computeRouteDV ( int src,int n,float *tmpDV ) {
        int converged=0,fromVertex;
        int i=0;
        int j=0;
	float d=0;
        int foundFlag=0;
	d=distVector[src];
	
/*
                for(i=0;i<n;i++) {
                        if(flag[i]==1) {
                                foundFlag = 1;
                                fromVertex=i;
                                convergence_count++;
                                //printf("Going to processing node %d \n",fromVertex);
                                break;
                        }
                }
                if (foundFlag == 0 ) {
                        //Distance vectors are Converged
                        break;
                }
*/
  /*              for(j=0;j<n;j++ ) {
                        //Here should it be adjMatrix or distVector ?
                        if ( adjMatrix[j] != -1  ) {
                                updateVector(j,n);
                        }*/
			
	/*	
                }
*/
		//printf("Printing received vector:\n");
		//printVector(tmpDV,n);
		pthread_mutex_lock(&dvMutex);
		for(i=0; i<n; i++ ) { 
				if(tmpDV[i]!=-1 && ( distVector[i] == -1 ||  (tmpDV[i]+d ) < distVector[i]) ) {
					distVector[i] = tmpDV[i] + d;
					forwTable[i] = forwTable[src];
					printf("computeRoutDV \n");
					//flag[toVertex] = 1; // To say that it has updated its distance vector and need to sent to neighbours again 
				}
		}
		pthread_mutex_unlock(&dvMutex);

//                flag[fromVertex] = 0;
  //              foundFlag=0;
        return 1;
}


int init(int n) {
	int i=0;
	distVector = (float *)malloc( sizeof(float) * n );
	forwTable = (int *)malloc(sizeof(int) * n);
	for(i=0;i<n;i++) {
		distVector[i] = -1;
		forwTable[i] = -1;
	}


        adjMatrix = (float **)malloc(sizeof(float *) * n );
        tempDistVector = (float **)malloc(sizeof(float *) * n );


 //       int i=0;
        for(i=0;i<n;i++) {
                *(adjMatrix +i ) = (float *)malloc(sizeof(float ) * n);
                *(tempDistVector+i) = (float *)malloc(sizeof(float ) * n);
        }

        int j=0;
        for (i=0;i<n;i++) {
                for(j=0;j<n;j++) {
                        adjMatrix[i][j] = -1;
                        tempDistVector[i][j] = -1;
                }
        }

}


void initForwTable(int n) {
	int i=0;
	for(i=0;i<n;i++) {
		if(distVector[i]!=-1) {
			forwTable[i] = i;
		}
	}

	forwTable[source]=0;
	
}


void main(int argc, char** argv){
	pthread_t senderThread, receiverThread;
	char ip[200]="";
	//char *ip = "192.168.15.7 192.168.15.11 192.168.15.21";
	int iret1,iret2;
	//printf("Hi the IP to contact is %s",argv[1]);

//        float tmp[4] = {0,10,30,2 };
//	source = 0;
	//str = argv[1];
//	numNodes=4;
//	init(numNodes);
//	numNodes = atoi(argv[1]);
	source  = atoi(argv[1]) -1 ;
	int numAdjNodes=0;
	char *fileName = argv[2];
	numAdjNodes = atoi( argv[3] );
	readIt(fileName);
	printf("Number of nodes : %d . Source node is : %d \n",numNodes,source);

	//Read adjacent node's ip addresses
        int i=0;
	strcat(ip,argv[4]);
	for(i=1;i<numAdjNodes;i++) {
		strcat(ip," ");
		strcat(ip,argv[4+i]);
	}

	printf("Adjacent ip addresses :");
//	for(i=0;i<numAdjNodes;i++) {
		printf(" %s" , ip);
//	}

	printf("\n");	
        for(i=0;i<numNodes;i++) {
                distVector[i] = tempDistVector[source][i];
		if (source==i) {
			distVector[i] = 0;
		}
        }

	//printVector(distVector,numNodes);

	initForwTable(numNodes);
	iret1 = pthread_create( &senderThread, NULL, clientFunc,(void *) ip);
        iret2 = pthread_create( &receiverThread, NULL, server, NULL);

	     pthread_join( receiverThread, NULL); 
	     pthread_join( senderThread, NULL); 
	



}


/*
char *str;


int main() {

	float tmp[5] = {4.5, 5.9, 6.3, 7.2, 10.5  };
	int numNodes = 5;

	init(numNodes);
	int i=0;
	for(i=0;i<numNodes;i++) {
		distVector[i] = tmp[i];
	}

	char temp[30]="4.5";
	sprintf(temp,"%s %f",temp,tmp[2]);

	printf("%s",temp);
*/
/*
	char *pkt = encode(2,5,distVector);
	printf("Pkt : %s\n" , pkt);

	float vector[5];

	decode(pkt,vector,5);

	printVector(vector,5);


}*/
