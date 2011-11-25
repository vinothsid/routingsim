#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct node {
    int nodeNo;
    float distance;
    int pred;
} *elems;

float **adjMatrix;

struct node** indexer;
struct node** heap;

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
        float tmp[5][5] = {{0,3,-1,7,8}, {3,0,2,-1,2},{-1,2,0,1,-1} , {7,-1,1,0,4 },{8,2,-1,4,0} };
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
       float test[10]={-1,-1,-1,-1,0,0,-1,-1,-1,-1};
       while(iter < size) {
                *(index+iter)=(elems+iter);
                (elems+iter)->distance=test[iter];
                (elems+iter)->nodeNo=iter;
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

void relaxNodes(struct node *elems,struct node** index,int pos,int size) {
        int j=0;
        while (j < size) {
                if((adjMatrix[pos][j]!=-1)&&((elems+j)->distance==-1)) {
                        (elems+j)->distance==adjMatrix[pos][j];
                        (elems+j)->pred=pos;
                } 
                else if ((adjMatrix[pos][j]!=-1)&&((elems+pos)->distance+adjMatrix[pos][j] < (elems+j)->distance)) {
                        (elems+j)->distance=(elems+pos)->distance+adjMatrix[pos][j];
                        (elems+j)->pred=pos;
                }
                j++;
        }
        buildMinHeap(index,size);       
}
//(elems+pos)->distance+adjMatrix[pos][j] < (elems+j)->distance

void printHeap(struct node **A,int size) {
        int i=0;
        while(i<size) {
                printf("The heap element at : %d is : %f from heap pointers\n",i,(*(heap+i))->distance);
                i++;
   }     
}

int main() {
        int size=5;  
        elems=(struct node*)malloc(size*sizeof(struct node));
        heap=(struct node**)malloc(size*sizeof(struct node*));
        int iter=0;
        init(size);
        fillMatrix(size);
        initNodes(elems,heap,size,4);
        printHeap(heap,size);
        buildMinHeap(heap,size);
        int posMin;
        printf("\n\n");
        while(size !=0) {
                posMin=extractMin(heap,&size);
                printf("The minimum value from extractMin is : %f at : %d position in the elems array\n",(elems+posMin)->distance,posMin);
                relaxNodes(elems,heap,posMin,size);
        }
        printHeap(heap,size);
        printf("\n\n");
        free(heap);
        free(elems);
        getchar();
     
        /*
        decreaseKey(elems,heap,size,1,1);
        i=0;
        printf("\n\n");
        while(i<size) {
                printf("The min element is : %f from heap pointers\n",(*(heap+i))->distance);
                i++;
        }
        */
}