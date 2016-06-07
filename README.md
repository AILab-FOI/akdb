# Kalashnikov DB 

AK (Автомат Калашникова) DB is a simple relational database management system 
developed by students of the Faculty of Organization and Informatics in 
Varaždin, Croatia.

http://kalashnikovdb.sourceforge.net/

The system is currently in alpha development status and NOT fully functional!

## Getting Started

To get you started you can simply clone the repository and install the dependencies:

### Prerequisites
```
sudo apt-get install git build-essential swig make
```
### Clone Kalashnikov DB

Clone the repository using [git][git]:

```
git clone https://github.com/mschatten/akdb
cd akdb
```
### Install Dependencies
```
sudo apt-get install python2.7
```
### Setting up virtualenv
Install [virtualenv](http://docs.python-guide.org/en/latest/dev/virtualenvs/) via [pip](https://pip.pypa.io/en/stable/installing/):
```
pip install virtualenv
```
You can also use a Python interpreter of your choice.
```
cd my_project_folder
virtualenv -p /usr/bin/python2.7 venv
```
To begin using the virtual environment, it needs to be activated:
```
source venv/bin/activate
```
If you are done working in the virtual environment for the moment, you can deactivate it:
```
deactivate
```
## Build the Application

Build (builds bin/akdb, documentation in doc/ and swig interface)
```
cd src
make
make doc
make swig
```
## Run the Application
```
cd bin/
./akdb help
./akdb <option>
```
## Clean project
Cleaning the project from files generated while compiling use this:
```
make clean
make clean-d
```

## Directory Layout
## Testing
1) Main engine
```
cd bin
./akdb test
```
On first use DB file will be initialized. In case it hangs press 1 and Enter
(don't ask why).

2) Server (read README in src/srv)
```
cd src/srv
python server.py
```

3) Client
```
cd src/srv
python akdb_cli.py
```
