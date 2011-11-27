#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int NODES=0;
struct node {
    int nodeNo;
    float distance;
    int pred;
    int nextHop;
} *elems;

float **adjMatrix;

struct node** indexer;
struct node** heap;
int source , dest;

int init( int n ) {
        adjMatrix = (float **)malloc(sizeof(float *) * n );
        int i=0;
        for(i=0;i<n;i++) {
                *(adjMatrix +i ) = (float *)malloc(sizeof(float ) * n);
        }

        int j=0;
        for (i=0;i<n;i++) {
                for(j=0;j<n;j++) {
                        adjMatrix[i][j] = -1;
                }  
        }
        return 1;       
}

int fillMatrix(int n) {
        float tmp[5][5] = {{0,3,-1,7,8}, {3,0,2,-1,2},{-1,2,0,2,-1} , {7,-1,2,0,1 },{8,2,-1,1,0} };
        int i=0;
        int j=0;
        for(i=0;i<n;i++) {
                for(j=0;j<n;j++) {
                        adjMatrix[i][j]=tmp[i][j];
                }

        }
        return 1;       
}

int initNodes (struct node* elems,struct node** index,int size,int source) {
       int iter=0;
       float test[10]={0,-1,-1,-1,-1,0,-1,-1,-1,-1};
       while(iter < size) {
                *(index+iter)=(elems+iter);
		if(iter!=source) {
                	(elems+iter)->distance=-1;
		} else {
			(elems+iter)->distance=0;
		}
                (elems+iter)->nodeNo=iter;
                (elems+iter)->pred=-1;
		(elems+iter)->nextHop=source;
                iter++;
       }      
}

int parent(int i) {
       if (i==0)
               return 0;
       else
               return (int)((i-1)/2);
}

int left(int i) {
       return 2*i+1;
}

int right(int i) {
       return 2*i+2;
}

void minHeapify(struct node **A,int i,int size) {
       int l=left(i);
       int r=right(i);
       int smallest;
       struct node* temp;
       if ((l<size)&& ((((*(A+l))->distance !=-1) && ((*(A+l))->distance < (*(A+i))->distance)) || ((*(A+i))->distance == -1))) {
               smallest=l;
       } else {
               smallest=i;
       }
       //((*(A+r))->distance < (*(A+smallest))->distance)
       if((r<size) && ((((*(A+r))->distance !=-1) && ((*(A+r))->distance < (*(A+smallest))->distance)) || ((*(A+smallest))->distance == -1)) ) {
               smallest=r;
       }
       if(smallest!=i) {
               temp=*(A+i);
               *(A+i)=*(A+smallest);
               *(A+smallest)=temp;
               minHeapify(A,smallest,size);
       }
}

void buildMinHeap(struct node **A,int size) {
        int i=size/2;
        while(i>=0) {
                minHeapify(A,i,size);       
                i--;
        }   
}

int extractMin(struct node **A,int* size) {
        struct node* temp;
        int m=*size-1;
        temp=*(A+m);
        *(A+m)=*(A);
        *(A)=temp;
        (*size)--;
        minHeapify(A,0,*size);
        return (*(A+m))->nodeNo;

}


void decreaseKey(struct node *A,struct node **index,int size,int key,int pos) {
        (A+pos)->distance=key;
        buildMinHeap(index,size);
}

void relaxNodes(struct node *elems,struct node** index,int pos,int verCount,int size) {
        int j=0;
        while (j < verCount) {
                if( (adjMatrix[pos][j]!=-1) && ((elems+j)->distance==-1) ) {
                        (elems+j)->distance=adjMatrix[pos][j]+(elems+pos)->distance;
                        (elems+j)->pred=pos;
                } 
                else if ( (adjMatrix[pos][j]!=-1) && ((elems+pos)->distance+adjMatrix[pos][j] < (elems+j)->distance) ) {
                        (elems+j)->distance=(elems+pos)->distance+adjMatrix[pos][j];
                        (elems+j)->pred=pos;
                }
                j++;
        }
        buildMinHeap(index,size);       
}
//(elems+pos)->distance+adjMatrix[pos][j] < (elems+j)->distance

void printPath(struct node* elems,int nodeNo) {
	if((elems+nodeNo)->pred!=-1)
		printPath(elems,(elems+nodeNo)->pred);
		printf("---> %d",nodeNo+1);
}

void printHeap(struct node **A,int size) {
        int i=0;
        while(i<size) {
                printf("The heap element at : %d is : %f from heap pointers\n",i,(*(heap+i))->distance);
                i++;
   	}     
}
int printpath1(struct node* elems,int nodeNo) {
	  if((elems+nodeNo)->pred == -1) return -1;
	while((elems+nodeNo)->pred!=source) {
	   
	  	
            nodeNo=(elems+nodeNo)->pred;
	}
        return (elems+nodeNo)->nodeNo;

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
			
			adjMatrix[y-1][x-1]=atof(pt);
				
			dist=0;		
		}

		//printf("Argument %d %d\n",j+1,atoi(pt));
		pt=strtok(NULL,";, \n");
		j++;
	}

	fclose(fp);
	
}


int main(int argc,char** argv) {
	if(argc < 3) {
		printf("usage ./exec [filename] [sourceNode] [destNode]\nspecify filename as X till read-graph is written\n");
		exit(1);
	}
        //int size=5; 
         source=atoi(argv[2])-1;
         dest=atoi(argv[3])-1;
	readIt(argv[1]);        
	int verCount=NODES; 
	printf("%d",verCount);
        elems=(struct node*)malloc(NODES*sizeof(struct node));
        heap=(struct node**)malloc(NODES*sizeof(struct node*));
        
        //fillMatrix(size);
        initNodes(elems,heap,NODES,source);
        printHeap(heap,NODES);
        buildMinHeap(heap,NODES);
        int posMin;
        printf("\n\n");
        while(NODES !=0) {
                posMin=extractMin(heap,&NODES);
                printf("The minimum value from extractMin is : %f at : %d position in the elems array\n",(elems+posMin)->distance,posMin);
                relaxNodes(elems,heap,posMin,verCount,NODES);
        }
	int j=0;
	printf("Destination \t Cost \t Next Hop");
	for(j=0;j<verCount;j++) {
           printf("\n %d \t\t %6.2f \t %d" , j+1 , (elems+j)->distance, printpath1(elems,j)+1);
	   
         }
        
        
        printf("\n\n");
        printPath(elems,dest);
        printHeap(heap,NODES);
	
        printf("\n\n");
        free(heap);
        free(elems);
        getchar();
     
}
