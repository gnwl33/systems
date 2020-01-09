/* This code is my own work, it was written without consulting a tutor or code written by other students - Gene Lee */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ARRSIZE 256 //if you want to change size of segment, you can change # of bits by changing # of ints/array size
#define BITSPERSEG (ARRSIZE*sizeof(int)*8) //# of bits == # of ints * 4 bytes * 8 bits

typedef struct _seg{  //definition of new type "seg"
	int bits[ARRSIZE]; //initial values will be 0
	struct _seg *next; //next points to next seg in linked list
	// int index;      
} seg;

typedef struct{ //coordinate type
	seg *segpt;
	int intnum;
	int bitnum;
} coordinate;


seg *head, *pt; //head stores head of linked list; pt saves last segment pointer
int lastj = 3; //stores last "j" for whichseg; init as 3

int howmanybits(int j){
	return (j - 3)/2; //calculates how many bits needed; bits[0,1,2,...] corresponds to 3,5,9,... 
}

int howmanysegs(int j){ //calculates how many segs based on how many bits
	int Bits = howmanybits(j);
	return (Bits + BITSPERSEG)/BITSPERSEG;
}

seg *whichseg(int j){ //calc ptr to seg containing bit for j
	int i;
	int segNum = howmanysegs(j); //# of segs needed for current j
	int lsegNum = howmanysegs(lastj); //# of segs needed for last j

	if (segNum > lsegNum){ //if # of segs required is greater, point to appropriate seg
		for(i = 0; i < (segNum - lsegNum); i++)
			pt = pt->next;
	}
	else if (segNum < lsegNum){ //if # of segs is less, start pointing from head; otherwise, pt stays the same
		pt = head;
		for(i = 1; i < segNum; i++)
			pt = pt->next;
	}

	lastj = j; //update last j
	return pt;
}

int whichint(int j){ //calc index of array containing bit for j
	int Bits = howmanybits(j);
	return Bits/32 % 256;
}

int whichbit(int j){ //calc 0-31 bit position of bit for j
	int Bits = howmanybits(j);
	return Bits % 32;
}


coordinate getcoord(int j){ //returns coord using "which" funcs
	coordinate c;
	c.segpt = whichseg(j);
	c.intnum = whichint(j);
	c.bitnum = whichbit(j);

	return c;
}

void markcoord(coordinate c){ //sets bit to 1 at coordinate
	seg *ptr;
	ptr = c.segpt;
	ptr->bits[c.intnum] |= 1 << (c.bitnum);
}

int testcoord(coordinate c){ //return 0 or 1 at coordinate
	seg *ptr;
	ptr = c.segpt;
	if ((ptr->bits[c.intnum] & (1 << (c.bitnum)) ) != 0 ){  //(& filters out relevant bit and the result !=0 when bit is 1 and =0 when nth bit is 0)
    	return 1;	//bit at coordinates represents a nonprime #
	}
    else{
    	return 0;	//prime #
    }
}


void marknonprime(int j){ //uses getcoord and markcoord to set nonprimes on (1)
	coordinate c = getcoord(j);
	markcoord(c);
}

int testprime(int j){ //uses getcoord and testcoord to see if prime
	coordinate c = getcoord(j);
	return testcoord(c);
}

coordinate incrcoord(coordinate c, int inc){ //increments coord by inc
	int i;

	for (i = 0; i < inc; i++){
		c.bitnum++;
		if (c.bitnum > 31){ //go to next index in bits[]
			c.bitnum = 0;
			c.intnum++;
			if (c.intnum > 255){ //go to next seg
				c.intnum = 0;
				c.segpt = c.segpt->next;
			}	
		}
	}


	return c;
}

int whichnum(coordinate c){ //compute the integer coresp to coord
	seg *ptr;
	int count = 0;

	ptr = head;
	while (ptr != c.segpt){
		ptr = ptr->next;
		count++; //count how many segs
	}

	return 2*(count*8192 + c.intnum*32 + c.bitnum) + 3;
}

int main(int argc, char *argv[]) {
	int i, N, howmany; //how many segs
	// int n;

	sscanf(argv[1], "%d", &N); //store input N from stdin
	howmany = howmanysegs(N); //calculate how many segs needed

	/*Dynamic allocation of segs*/
	head = (seg*) malloc(sizeof(seg)); //create a head seg of linked list
	// head->index = 1;
	pt = head; //start node pointer at head
	for (i = 1; i < howmany; i++){ //create enough segments according to howmany
		pt->next = (seg*) malloc(sizeof(seg)); //have next pointing to a new seg
		pt = pt->next; //move node pointer to the next seg
		// pt->index = k+1;
	}
	pt = head; //set pointer to head again

	//Sieve Algorithm
	int j, l = 3;
	while (l <= sqrt(N)){
		for(i = l; i <= sqrt(N); i += 2) //starting from k, find next prime number i
			if (!testprime(i)) //break if i is prime (testPrime is 0)
				break;

		for(j = 2 * i; j <= N; j = j + i){ //mark (odd) multiples of prime as nonprime
			if (!(j%2 == 0)){
				marknonprime(j);
			}
		}

		l = i + 2;
	}

	int count = 0;
	for(i = 3; i <= N; i += 2) //go through bits array to count primes
		if (!testprime(i)){
			count++;
		}

	printf("The number of odd primes less than or equal to N is: %d\n", count);

	//TriplePrime Algorithm
	int p, k, m;
	count = 0;
	coordinate d, c = getcoord(3);
	while (scanf("%d %d", &k, &m) == 2){ //keep scanning until EOF
		while (whichnum(c) + m <= N){
			if(!(testcoord(c) || testcoord(incrcoord(c, k/2)) || testcoord(incrcoord(c, m/2)))){ //p, p+k, p+m must all be prime
				count++;
				d = c;
			}
				c = incrcoord(c,1); //check next coordinate
		}
		printf("The number of triple primes is: %d\n", count);
		if (count == 0)
			printf("No solution\n");
		else
			printf("The largest triple prime is: %d %d %d\n", whichnum(d), whichnum(d)+k, whichnum(d)+m);

		c = getcoord(3); //reset c & d & count
		d = c;
		count = 0;
	}
}
