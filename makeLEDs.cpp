#include<iostream>
#include<fstream>
#include<string>
#include<unistd.h>						// for the microsecond sleep function

using namespace std;

#define GPIO		"/sys/class/gpio/"
#define FLASH_DELAY	50000				//50 milliseconds

class LED{
	private:							// the following is part of the implementation
		string	gpioPath;				// private states
		int		gpioNumber;
		void	writeSysfs(string path, string filename, string value);
	public:								// part of the public interface
		LED(int gpioNumber);			// the constructor -- create the object
		virtual void turnOn();			// virtual allows function to be overridden (re-defined)
		virtual void turnOff();			// by child class
		virtual void displayState();
		virtual ~LED();					// the deconstuctor -- called automatically
};

LED::LED(int gpioNumber){				// constructor implementation
	this->gpioNumber = gpioNumber;
	gpioPath = string(GPIO "gpio") + to_string(gpioNumber) + string("/");
	cout << gpioPath << endl;
	writeSysfs(string(GPIO), "export", to_string(gpioNumber));
	usleep(100000);						// ensure GPIO is exported
	writeSysfs(gpioPath, "direction", "out");
}

// This implementation function is "hidden" from outside the class
void LED::writeSysfs(string path, string filename, string value){
	ofstream fs;
	fs.open((path+filename).c_str());
	if (fs.is_open()){
		fs << value;
	}
	else{
		cout << "Couldn't open file." << endl;
	}
	fs.close();
}

void LED::turnOn(){
	writeSysfs(gpioPath, "value", "1");
}

void LED::turnOff(){
	writeSysfs(gpioPath, "value", "0");
}

void LED::displayState(){
	ifstream fs;
	fs.open((gpioPath + "value").c_str());
	if (fs.is_open()){
			string line;
			cout << "The current LED state is: ";
			while(getline(fs,line)) cout << line << endl;
		}
		else{
			cout << "Couldn't open file." << endl;
		}
		fs.close();
}

LED::~LED(){						// the destructor unexports the sysfs entries
	cout << "Destroying the LED with GPIO Number " << gpioNumber << endl;
	writeSysfs(string(GPIO), "unexport", to_string(gpioNumber));
}

// the main function start point

int main(int argc, char* argv[]){
	cout << "Starting the makeLEDs program" << endl;
	LED led1(4), led2(17);			// create 2 LED objects
	led1.displayState();
	led2.displayState();
	cout << "Flashing the LEDs for 5 seconds." << endl;
	for(int i=0; i<50; i++){		// LEDs will alternate
		led1.turnOn();				// turn GPIO4 on
		led2.turnOff();				// turn GPIO17 off
		usleep(FLASH_DELAY);		// sleep for 50ms
		led1.turnOff();				// turn GPIO4 off
		led2.turnOn();				// turn GPIO17 on
		usleep(FLASH_DELAY);		// sleep for 50ms
	}
	led1.displayState();			// display final GPIO4 state
	led2.displayState();			// display final GPIO17 state
	cout << "Finished the makeLEDs program." << endl;
	return 0;
}	