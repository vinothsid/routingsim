#include<stdio.h>

float **adjMatrix;
int *flag;

int **forwTable;
float **distVector;

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
			printf(" %f ",distVector[i][j] );
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
                        printf(" %d ",forwTable[i][j] );
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
	printf("NextHop for vertex %d to reach %d is  %d \n" , fromVertex , toVertex , nextHop );
	return nextHop;
}
int main(int argc, char *argv[]) {

	init(5);
	fillMatrix(5);

	initPred(5);
	flag[0] = 1;
	computeRouteDV(5);
	printDistVector(5);

	printForwTable(5);
	findNextHop(0,1);
	findNextHop(0,3);

	findNextHop(1,3);
}
