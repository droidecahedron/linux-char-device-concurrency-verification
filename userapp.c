#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define ASP_CLEAR_BUF _IOW(CDRV_IOC_MAGIC, 1, int)

#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

#define MODE1 1 // define mode here. There can be 2 modes
#define MODE2 2

//case 1
void* thread0(void* threadNum);
void* thread1(void* threadNum);

//case 2
void* thread2(void* threadNum);
void* thread3(void* threadNum);

//case 3
void* thread4(void* threadNum);
void* thread5(void* threadNum);

//case 4
void* thread6(void* threadNum);
void* thread7(void* threadNum);



int main(int argc, char *argv[]) {

	char dev_path[20];
	int i,fd;
	char ch, write_buf[100], read_buf[10];
	int offset, origin;
	sprintf(dev_path, "%s", DEVICE);

	
	//thread stuff
	int max_threads = 8;
	pthread_t threads[max_threads]; // this holds the unique thread identities
	int s; // error check variable
	



	printf("\ncase 1: thread A writes while thread B is writing and changes it to mode 2.\n");
	printf("case 2: open and ioctl. A opens and closes dev, B uses ioctl to change to mode 2. sem2 deadlock\n");
	printf("case 3: ioctl. two threads keep trying to hop between mode 1 and 2 for the device.\n");
	printf("case 4: Try creating two file descriptors to open the device. Doesnt need threads, deadlocks.\n");
	printf("enter x to exit\n");
	printf("\nEnter a test case (1-4,x) : ");
	while(1)
{
		scanf("%c", &ch);
		switch(ch) {
		case '1':
			printf("\nTesting case 1. \n");
			s = pthread_create(&threads[0],NULL,thread0,(void*)(intptr_t)0);	
			if(s != 0)
		  		exit(1); // error check
			s = pthread_create(&threads[1],NULL,thread1,(void*)(intptr_t)1);	
			if(s != 0)
	  			exit(1); // error check
			s = pthread_join(threads[0],NULL);
			if(s != 0)
		  		exit(2); // error check	
			s = pthread_join(threads[1],NULL);
			if(s != 0)
		  		exit(2); // error check
		  			
			printf("Passed case 1\n");
			break;


		case '2':
			printf("\nTesting case 2. \n");

			//open the device
			fd = open(dev_path, O_RDWR);
			if(fd == -1) {
				printf("File %s either does not exist or has been locked by another "
						"process\n", DEVICE);
			}
			
			//change to mode 2
			int rc = ioctl(fd, E2_IOCMODE1, 0);
			if (rc == -1) { 
				perror("\n***error in ioctl***\n");
				return -1;
			}
			
			//close the device
			close(fd);
			
			
			//now two threads
			s = pthread_create(&threads[2],NULL,thread2,(void*)(intptr_t)0);	
			if(s != 0)
		  		exit(1); // error check
			s = pthread_create(&threads[3],NULL,thread3,(void*)(intptr_t)1);	
			if(s != 0)
	  			exit(1); // error check
			s = pthread_join(threads[2],NULL);
			if(s != 0)
		  		exit(2); // error check	
			s = pthread_join(threads[3],NULL);
			if(s != 0)
		  		exit(2); // error check
	  		
	  		printf("Passed case 2\n");
			break;
		case '3':
			//now two threads
			s = pthread_create(&threads[4],NULL,thread4,(void*)(intptr_t)0);	
			if(s != 0)
		  		exit(1); // error check
			s = pthread_create(&threads[5],NULL,thread4,(void*)(intptr_t)1);	
			if(s != 0)
	  			exit(1); // error check
			s = pthread_join(threads[4],NULL);
			if(s != 0)
		  		exit(2); // error check	
			s = pthread_join(threads[5],NULL);
			if(s != 0)
		  		exit(2); // error check
		
			break;

		case '4': 
			fd = open(dev_path, O_RDWR);
			if(fd == -1) {
				printf("File %s either does not exist or has been locked by another "
						"process\n", DEVICE);
			}
			int fd2 = open(dev_path, O_RDWR);
			if(fd == -1) {
				printf("File %s either does not exist or has been locked by another "
						"process\n", DEVICE);
			}
			break;
			
		case 'x':
			return 0;
			break;
		default: 
			
			printf("\nEnter a test case (1-4,x) : ");
			break;

		}
	}
	
	
	
	close(fd);
	return 0; //main thread returns 0
	
	
}

//threads

//TEST CASE 1
void* thread0(void* threadNum)
{
	//can do int x = (int)threadNum; now
	char dev_path[20];
	int fd;
	char ch, write_buf[100], read_buf[10];
	sprintf(dev_path, "%s", DEVICE);
	printf("entered thread %d\n",(int)threadNum);
	
	//open the device
	fd = open(dev_path, O_RDWR);
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
	}
	
	//change to mode 1
	int rc = ioctl(fd, E2_IOCMODE1, 0);
		if (rc == -1) { 
			perror("\n***error in ioctl***\n");
			return -1;
		}
	
	
	//write something to the device
	sprintf(write_buf,"write test0");
	write(fd, write_buf, sizeof(write_buf));
	

	
	
	close(fd);
	printf("exit thread %d\n",(int)threadNum);
	//pthread exit implicitly called here.
}

void* thread1(void* threadNum)
{
	//can do int x = (int)threadNum; now
	char dev_path[20];
	int fd;
	char ch, write_buf[100], read_buf[10];
	sprintf(dev_path, "%s", DEVICE);	
	printf("entered thread %d\n",(int)threadNum);
	
	//open the device
	fd = open(dev_path, O_RDWR);
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
	}
	
	
	//write something to the device.
	int i;
	for(i=0;i<1000;i++){
		sprintf(write_buf,"write test0");
		write(fd, write_buf, sizeof(write_buf));
	}
	
	//change to mode 2
	int rc = ioctl(fd, E2_IOCMODE2, 0);
		if (rc == -1) { 
			perror("\n***error in ioctl***\n");
			return -1;
		}
	
	
	
	
	close(fd);
	printf("exit thread %d\n",(int)threadNum);
	//pthread exit implicitly called here.

}

//TEST CASE 2
void* thread2(void* threadNum)
{
	char dev_path[20];
	int fd;
	char ch, write_buf[100], read_buf[10];
	sprintf(dev_path, "%s", DEVICE);	
	printf("\nentered thread %d\n",(int)threadNum);
	
	//open and close the device a bunch.
	int i;
	for(i=0;i<100;i++)
	{
		fd = open(dev_path, O_RDWR);
		if(fd == -1) {
			printf("File %s either does not exist or has been locked by another "
					"process\n", DEVICE);
		}
		
		close(fd);
	}
	printf("exit thread %d\n",(int)threadNum);
	//pthread exit implicitly called here.

}

void* thread3(void* threadNum)
{
	char dev_path[20];
	int fd;
	char ch, write_buf[100], read_buf[10];
	sprintf(dev_path, "%s", DEVICE);	
	printf("\nentered thread %d\n",(int)threadNum);
	
	//open the device
	fd = open(dev_path, O_RDWR);
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
	}
	
	
	//try to change to mode 2
	int rc = ioctl(fd, E2_IOCMODE2, 0);
		if (rc == -1) { 
			perror("\n***error in ioctl***\n");
			return -1;
		}
	
	
	
	
	
	close(fd);
	printf("exit thread %d\n",(int)threadNum);
	//pthread exit implicitly called here.
}



//case 3
void* thread4(void* threadNum)
{
	char dev_path[20];
	int fd;
	char ch, write_buf[100], read_buf[10];
	sprintf(dev_path, "%s", DEVICE);	
	printf("\nentered thread %d\n",(int)threadNum);
	
	//open the device
	fd = open(dev_path, O_RDWR);
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
	}
	
	int i;
	int rc;
	for(i=0;i<100;i++)
	{
		//try to change to mode 2
		rc = ioctl(fd, E2_IOCMODE2, 0);
			if (rc == -1) { 
				perror("\n***error in ioctl***\n");
				return -1;
			}
		rc = ioctl(fd, E2_IOCMODE1, 0);
			if (rc == -1) { 
				perror("\n***error in ioctl***\n");
				return -1;
			}	
	}
	
	close(fd);
	printf("exit thread %d\n",(int)threadNum);
	//pthread exit implicitly called here.
}

void* thread5(void* threadNum)
{

}

//case 4
void* thread6(void* threadNum);
void* thread7(void* threadNum);








