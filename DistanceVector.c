#include<stdio.h>
#include<string.h>
#include<stdlib.h>

float **adjMatrix;
int *flag;
int NODES =0;
int **forwTable;
float **distVector;
int convergence_count=0;
int node1,node2;
int init( int n  ) {
	adjMatrix = (float **)malloc(sizeof(float *) * n );
	distVector = (float **)malloc(sizeof(float *) * n );
	forwTable = (int **)malloc(sizeof(int *) * n );
	flag = (int *)malloc(sizeof(int) * n);
	int i=0;
	for(i=0;i<n;i++) {
		*(adjMatrix +i ) = (float *)malloc(sizeof(float ) * n);
		*(distVector+i) = (float *)malloc(sizeof(float ) * n);
		*(forwTable+i) = (int *)malloc(sizeof(int) * n );
	}

	int j=0;
	for (i=0;i<n;i++) {
		for(j=0;j<n;j++) {
			adjMatrix[i][j] = -1;
			distVector[i][j] = -1;
			forwTable[i][j] = -1;
		}
		flag[i]=0;	
	}
	
	return 1;	

}

int finish( int n  ) {
	int i=0;
	int j=0;
	for (i=0;i<n;i++) {
		for(j=0;j<n;j++) {
			if(i==j){
				
				distVector[i][j] = 0;
				forwTable[i][j] = -1;
			}		
		}
	}
	
	return 1;	

}


int fillMatrix(int n) {
	float tmp[5][5] = {{0,3,-1,7,8}, {3,0,2,-1,2},{-1,2,0,1,-1} , {7,-1,1,0,4 },{8,2,-1,4,0} };
	int i=0;
	int j=0;
	for(i=0;i<n;i++) {
		for(j=0;j<n;j++) {
			adjMatrix[i][j]=tmp[i][j];
			distVector[i][j]=adjMatrix[i][j];	
		}

	}
	return 1;	

}

int updateVector(int fromVertex , int toVertex , int n) {
	float d;
	d= distVector[toVertex][fromVertex];
	int i=0;

	printf("Node  %d received distance vector from node %d \n" , toVertex , fromVertex);
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

int computeRouteDV ( int n ) {
	int converged=0,fromVertex;
	int i=0;
	int j=0;
	int foundFlag=0;
	while(1) {
		for(i=0;i<n;i++) {
			if(flag[i]==1) {
				foundFlag = 1;
				fromVertex=i;
				convergence_count++;
				printf("Going to processing node %d \n",fromVertex);
				break;
			}
		}
		if (foundFlag == 0 ) {
			//Distance vectors are Converged
			break;
		}
		for(j=0;j<n;j++ ) {
			//Here should it be adjMatrix or distVector ?
			if ( adjMatrix[fromVertex][j] != -1  ) {
				updateVector(fromVertex,j,n);
			}
		}
		flag[fromVertex] = 0;
		foundFlag=0;
	}
	return 1;
}

int printDistVector(int n) {
	int i=0,j=0;
	printf("Distance Vector:\n");
	for(i=0;i<n;i++) {
		for(j=0;j<n;j++) {
			printf(" %6.2f ",distVector[i][j] );
		}
		printf("\n");
	}
	return 1;
}

int printForwTable(int n) {
	printf("Forwarding table of all vertices : \n");
        int i=0,j=0;
        for(i=0;i<n;i++) {
                for(j=0;j<n;j++) {
                        printf(" %d ",forwTable[i][j]+1 );
                }
                printf("\n");
        }
        return 1;

}

int initPred(int n) {
	int i=0;
	int j=0;
	for(i=0;i<n;i++) {
		for(j=0;j<n;j++){
			if(adjMatrix[i][j] != -1 ) {
				forwTable[i][j]=j;
			}
		}
	}
}

int findNextHop(int fromVertex,int toVertex) {
	int nextHop = -1;
/*	int parent=-1;
	int tmp=toVertex;
	while( parent!=fromVertex ) {
		parent=forwTable[fromVertex][tmp];
		nextHop =  tmp;
		tmp = parent;
	}
*/	
	nextHop = forwTable[fromVertex][toVertex];
	printf("NextHop for vertex %d to reach %d is  %d \n" , fromVertex+1 , toVertex+1 , nextHop+1 );
	return nextHop;
}
int printRoutingTable(int k,int n) {
	int i;
	printf("\n*********************************");
	printf("\n ROUTING TABLE FOR NODE %d", k+1);
	printf("\n*********************************");
	printf("\nNext Hop \t Cost");
        
	for(i=0;i<n;i++) {
		
		printf("\n%d\t\t%6.2f" , forwTable[k][i]+1 , distVector[k][i]);
        }
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
			NODES = atoi(pt);
		//	printf("The number of nodes are %d",NODES);
			init(NODES);		
			dist=0;		
		}
		if(dist==1){		
			x=atoi(pt);
		//	printf("Argument %d %d\n",j+1,atoi(pt));	
		}
		if(dist==2){
			y=atoi(pt);
		//	printf("Argument %d %d\n",j+1,atoi(pt));			
		}
		if(dist==3){
		//	printf("Argument %d %f\n",j+1,atof(pt));		
			adjMatrix[x-1][y-1]=atof(pt);
			distVector[x-1][y-1]=adjMatrix[x-1][y-1];
			adjMatrix[y-1][x-1]=atof(pt);
			distVector[y-1][x-1]=adjMatrix[y-1][x-1];	
			dist=0;		
		}

		//printf("Argument %d %d\n",j+1,atoi(pt));
		pt=strtok(NULL,";, \n");
		j++;
	}

	fclose(fp);
	
}

int main(int argc, char *argv[]) {
 
	if(argc !=5){
	printf("\n\nPlease enter the command as\n");
	printf("\n\n./<executable> <initial node> <file name> <node 1> <node 2> \n");
	exit(0);	
	}	
	printf("The file name is %s",*(argv+2));
	readIt(*(argv+2));

	//fillMatrix(5);

	initPred(NODES);
	printForwTable(NODES);
	printDistVector(NODES);
	
	flag[atoi(*(argv+1))-1] = 1;
	computeRouteDV(NODES);
	finish(NODES);
	printDistVector(NODES);
	node1=atoi(*(argv+3))-1;
	node2=atoi(*(argv+4))-1;
	printForwTable(NODES);
	printRoutingTable(node1,NODES);
	printRoutingTable(node2,NODES);
	printf("\n");
	printf("Least Cost Distance from Node %d to Node %d is %6.2f" , node1+1 , node2+1, distVector[node1][node2]);
	printf("\nThe number of iterations taken to converge = %d\n" , convergence_count);

//	findNextHop(0,1);
//	findNextHop(0,3);

//	findNextHop(1,3);
}
