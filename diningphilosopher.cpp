#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <semaphore.h>
#include <sys/select.h>
using namespace std;
const int NumberOfPhilosophers = 5;

int chopsticks[NumberOfPhilosophers]={0,0,0,0,0};

class Semaphore
{
private:
    sem_t _semaphore;

public:
    Semaphore( int value = 1 )	{ (void) sem_init( &_semaphore, 0, value ); };
   ~Semaphore( void )		{ (void) sem_destroy( &_semaphore ); };
    void wait( void )		{ (void) sem_wait( &_semaphore ); };
    void signal( void )		{ (void) sem_post( &_semaphore ); };
};

Semaphore screen;

class Philosopher 
{
	
	pthread_t id;
	int number;
	int time_to_live;
	int eating_time;
	int thinking_time;

	public : Philosopher(int i,int a, int b, int c) 
	{
		number=i;
		thinking_time=a;
		eating_time=b;
		time_to_live=c;
	}

	Philosopher()
	{
		number=-1;
		thinking_time=0;
		eating_time=0;
		time_to_live=0;
	}

	bool getChopsticks(int time);
	void releaseChopsticks(int time);
	friend void run(Philosopher* p);

	public :
	void start();
	void wait();

};

void run(Philosopher* philosopher)
{
	int n=philosopher->number;
	int eat_count=0;
	int total_waiting_time=0;
	int eating_time=philosopher->eating_time;
	int thinking_time=philosopher->thinking_time;
	int time_to_live=philosopher->time_to_live;
	int total_time=0;
	
	while(total_time<=time_to_live)
	{
		int flag=0;
		sleep(thinking_time);
		total_time+=thinking_time;
		while(!philosopher->getChopsticks(total_time) && total_time<time_to_live)
		{
			sleep(1);
			total_time+=1;
			total_waiting_time++;
			if(total_time>=time_to_live)
				{
					flag=1;
					break;
				}

		}
		if(flag==1)
		{
			screen.wait();
			cout<<"\nPhilosopher "<<n<<" has left the Table. Number of meals eaten = "<<eat_count<<" Total time spent waiting = "<<total_waiting_time<<endl;
			screen.signal();
			pthread_exit(NULL);	
		}		
			
			if(total_time>time_to_live)
				break;
			
			else
				{
					sleep(eating_time);
			total_time+=eating_time;
			philosopher->releaseChopsticks(total_time);
			eat_count++;
		}
		

	}
	screen.wait();
	cout<<"\nPhilosopher "<<n<<" has left the Table. Number of meals eaten = "<<eat_count<<" Total time spent waiting = "<<total_waiting_time<<endl;
	screen.signal();
	pthread_exit(NULL);

}

void Philosopher::start()
{
   if ( pthread_create( &id, NULL, (void *(*)(void *)) &run,
			 this ) != 0 )
    {
	screen.wait();
	cout << "could not create thread for philosopher\n";
	screen.signal();
	exit( 1 );
    }
}

void Philosopher::wait()
{
	pthread_join(id,NULL);
}

void Philosopher::releaseChopsticks(int time)
{
	screen.wait();
	chopsticks[number]=0;
	chopsticks[(number+1)%NumberOfPhilosophers]=0;
	
	cout<<"\nPhilosopher "<<number<<" released Chopsticks number "<<(number+1)%NumberOfPhilosophers<<" and "<<number<<" at time - "<<time<<"s"<<endl;
	screen.signal();
}

bool Philosopher::getChopsticks(int time)
{
	screen.wait();
	if(chopsticks[number]==0 && chopsticks[(number+1)%NumberOfPhilosophers]==0)
	{
		
		chopsticks[(number+1)%NumberOfPhilosophers]=1;
		chopsticks[number]=1;
		
		cout<<"\nPhilosopher "<<number<<" picked up Chopsticks number "<<(number+1)%NumberOfPhilosophers<<" and "<<number<<" at time - "<<time<<"s"<<endl;
		screen.signal();
		return true;
	}
	screen.signal();
	return false;
}








int main()
{
	Philosopher p[NumberOfPhilosophers];
	sleep(1);
	for(int i=0;i<NumberOfPhilosophers;i++)
	{
		int a,b,c;
		cout<<"Enter thinking time for Philospher "<<i<<" - ";
		cin>>a;
		cout<<"Enter eating time for Philosopher "<<i<<" - ";
		cin>>b;
		cout<<"Enter total life time for Philosopher "<<i<<" ( philosopher will leave the table if he is not eating at this time instant or just after completion of his meal going on during this time instant) - ";
		cin>>c;
		p[i]=Philosopher(i,a,b,c);
		cout<<"Philospher "<<i<<" created!"<<endl;
		//pthread_create(&p[i]->id,NULL,run, (void *)&p[i]);
	}

	for(int i=0;i<NumberOfPhilosophers;i++)
	{
		p[i].start();
	}

	for(int i=0;i<NumberOfPhilosophers;i++)
	{
		p[i].wait();
	}

	cout<<"PROGRAM TERMINATED !"<<endl;

	return 0;
}