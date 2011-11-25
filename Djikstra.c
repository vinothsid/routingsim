#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct node {
    int nodeNo;
    float distance;
    int pred;
} *elems;

float **adj;

struct node** indexer;
struct node** heap;

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
    if((r<size) && ((*(A+r))->distance < (*(A+smallest))->distance)) {
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

int main() {
    int size=10;  
    float test[10]={8,9,6,3,4,9,2,-1,5,2};
    elems=(struct node*)malloc(size*sizeof(struct node));
    heap=(struct node**)malloc(size*sizeof(struct node*));
    int iter=0;
    while(iter < 10) {
        *(heap+iter)=(elems+iter);
        (elems+iter)->distance=test[iter];
        (elems+iter)->nodeNo=iter;
        iter++;
    }
   int i=0;
   while(i<size) {
        printf("The heap element at : %d is : %f from heap pointers\n",i,(*(heap+i))->distance);
        i++;
   }

   buildMinHeap(heap,10);
   i=0;
   int posMin;
   printf("\n\n");
   while(size !=0) {
        posMin=extractMin(heap,&size);
        printf("The minimum value from extractMin is : %f\n",(elems+posMin)->distance);
   }
   printf("\n\n");
   
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
