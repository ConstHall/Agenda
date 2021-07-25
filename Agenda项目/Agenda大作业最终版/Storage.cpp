#include"Storage.hpp"
#include"Path.hpp"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
using namespace std;

std::shared_ptr<Storage> Storage::m_instance=nullptr;

/**
*   default constructor
*/
Storage::Storage() {
	m_dirty=false;//The data has not been modified
	readFromFile();//Read from .csv file
}

/**
*   disallow the copy constructor and assign operator
*/
bool Storage::readFromFile(void) {
	ifstream users(Path::userPath);
	ifstream meetings(Path::meetingPath);
	int i;
	bool flag=true;
	if(users.is_open()==false||meetings.is_open()==false){
		users.close();
		meetings.close();
		flag=false;
 	}//Check whether both files are open
 	else{
 		string temp1;
		while(getline(users,temp1)){
			string name,password,email,phone;
        	int mark_position[8];
        	int j=0;
			for(i=0;i<temp1.size();i++){
    			if(temp1[i]=='"'){
    				mark_position[j]=i;
    				j++;
				}//Record the location of each ' " '
  			}
			name=temp1.substr(mark_position[0]+1,mark_position[1]-mark_position[0]-1);//Copy the contents between the two ' " ' into the name
			password=temp1.substr(mark_position[2]+1,mark_position[3]-mark_position[2]-1);
			email=temp1.substr(mark_position[4]+1,mark_position[5]-mark_position[4]-1);
			phone=temp1.substr(mark_position[6]+1,mark_position[7]-mark_position[6]-1); 
			User user(name,password,email,phone);//Initialize the user with the above data
			m_userList.push_back(user);//Add user to the m_userList
		}
		users.close();
	
		string temp2;
		while(getline(meetings,temp2)){
			string sponsor,participators,startDate,endDate,title;
			int pos=1,len=0;
			pos=temp2.find("\"")+1;//Record the starting position of sponsor information
			len=temp2.find("\",\"",pos)-pos;//Calculate the length of sponsor
			sponsor=temp2.substr(pos,len);//Copy the contents of the sponsor in temp2 to the sponsor

			pos+=len+3;//Skip the three symbols: ","
			len=temp2.find("\",\"",pos)-pos;
			participators=temp2.substr(pos,len);
			participators+='&';

			pos+=len+3;
			len=temp2.find("\",\"",pos)-pos;
			startDate=temp2.substr(pos,len);

			pos+=len+3;
			len=temp2.find("\",\"",pos)-pos;
			endDate=temp2.substr(pos,len);

			pos+=len+3;
			len=temp2.find("\"",pos)-pos;
			title=temp2.substr(pos,len);
			
			vector<string> No_participators;//Create a container for participators
			string No_i;
			for(i=0;i<participators.size();i++){//Iterate through every character of all participators(including &) 
				if(participators[i]!='&'){
					No_i.push_back(participators[i]);//The current character is added to the string No_i if it is not the delimiter &
				}
				else if(i<participators.size()-1){
					string temp=No_i; 
					No_participators.push_back(temp);
					No_i.clear();
					//If not & and not to the end of the participators,we will copy the current No_i information to temporary variables temp,
					//and then add the temp to participators in the container.Finally the No_i will be cleared.
				}
				else if(i==participators.size()-1){
					No_participators.push_back(No_i);//If not & and arrived at the end of the participators,
					//the direct current No_i's content will be added to the participators in the container
				}
			}
			Date start(startDate);
			Date end(endDate);
			Meeting meeting(sponsor,No_participators,start,end,title);//Initializes the meeting with the above data
			m_meetingList.push_back(meeting);//Add meeting to m_meetingList
		}
		meetings.close();
	} 
	return flag;
}

/**
*   write file content from memory
*   @return if success, true will be returned
*/
bool Storage::writeToFile(void) {
	ofstream users(Path::userPath);
	ofstream meetings(Path::meetingPath);
	int j;
	bool flag=true;
	if(users.is_open()==false||meetings.is_open()==false){
		users.close();
		meetings.close();
		flag=false;
	}//Check whether both files are open
	else{
		for(auto i=m_userList.begin();i!=m_userList.end();i++){//Iterate through the list of users 
		users<<'"'<<i->getName()<<'"'<<','<<'"'<<i->getPassword()<<'"'<<','<<'"'<<i->getEmail()<<'"'<<','<<'"'<<i->getPhone()<<'"'<<"\n";
		//Writes the username,password,email,and phone into the user.csv 
		}
	users.close();
   	for(auto i=m_meetingList.begin();i!=m_meetingList.end();i++){//Iterate through the list of meetings  
       	meetings<<'"'<<i->getSponsor()<<'"'<<','<<'"';//Write the sponsor into the meetings.csv
       	vector<string> participators=i->getParticipator();//Initialized the participators' container with participators of the meeting
       	for(j=0;j<participators.size()-1;j++){//Iterate through the container 
         	meetings<<participators[j]<<'&';//Write the participators of the meeting into meetings.csv and add & at the end of every participators in order. 
    	}
    	meetings<<participators[participators.size()-1]<<'"'<<','<<'"'<<(*i).getStartDate().dateToString((*i).getStartDate())<<'"'<<','<<'"'<<(*i).getEndDate().dateToString((*i).getEndDate())<<'"'<<','<<'"'<<(*i).getTitle()<<'"'<<"\n";
    	//Write the last participator,the start time,end time,and title into meetings.csv
    }
	meetings.close();
	}
	return flag;
}

/**
* get Instance of storage
* @return the pointer of the instance
*/
std::shared_ptr<Storage> Storage::getInstance(void) {
	if(m_instance==nullptr){
		m_instance=shared_ptr<Storage>(new Storage());
	}
	return m_instance;
}

/**
*   destructor
*/
Storage::~Storage() {
	sync();
}

// CRUD for User & Meeting
// using C++11 Function Template and Lambda Expressions

/**
* create a user
* @param a user object
*/
void Storage::createUser(const User &t_user){
	m_dirty=true;
	m_userList.push_back(t_user);
}

/**
* query users
* @param a lambda function as the filter
* @return a list of fitted users
*/
std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const
{
	list<User> users;
	for(auto i=m_userList.begin();i!=m_userList.end();i++){//Iterate through the list of users
		if(filter(*i)==true){
			users.push_back(*i);//If the username is consistent,add it to users
		}
	}
	return users;
}

/**
* update users
* @param a lambda function as the filter
* @param a lambda function as the method to update the user
* @return the number of updated users
*/
int Storage::updateUser(std::function<bool(const User &)> filter,
             std::function<void(User &)> switcher)
{
	int cnt=0;
	m_dirty=true;
	for(auto i=m_userList.begin();i!=m_userList.end();i++){//Iterate through the list of users
		if(filter(*i)==true){
			switcher(*i);
			cnt++;//Count the number of updated users
		}
	}
	return cnt;
}

/**
* delete users
* @param a lambda function as the filter
* @return the number of deleted users
*/
int Storage::deleteUser(std::function<bool(const User &)> filter) {
	int cnt=0;
	m_dirty=true;
	for(auto i=m_userList.begin();i!=m_userList.end();i++){//Iterate through the list of users
		if(filter(*i)==true){
			cnt++;//Calculate the number of users to delete 
		}
	}
	m_userList.erase(std::remove_if(m_userList.begin(),m_userList.end(),filter),m_userList.end());
	//Delete all users in the m_useList that meet the filtering criteria
	return cnt;
}

/**
* create a meeting
* @param a meeting object
*/
void Storage::createMeeting(const Meeting &t_meeting) {
	m_dirty=true;
	m_meetingList.push_back(t_meeting);
}

/**
* query meetings
* @param a lambda function as the filter
* @return a list of fitted meetings
*/
std::list<Meeting> Storage::queryMeeting(
  std::function<bool(const Meeting &)> filter) const
{
	list<Meeting> meetings;
	for(auto i=m_meetingList.begin();i!=m_meetingList.end();i++){//Iterate through the list of meetings
		if(filter(*i)==true){
			meetings.push_back(*i);//if the meeting meets the criteria,add it to the meetings 
		}
	}
	return meetings;
}

/**
* update meetings
* @param a lambda function as the filter
* @param a lambda function as the method to update the meeting
* @return the number of updated meetings
*/
int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                std::function<void(Meeting &)> switcher)
{
	int cnt=0;
	m_dirty=true;
	for(auto i=m_meetingList.begin();i!=m_meetingList.end();i++){//Iterate through the list of meetings
		if(filter(*i)==true){
			switcher(*i);
			cnt++;//Count the number of updated meetings
		}
	}
	return cnt;
}

/**
* delete meetings
* @param a lambda function as the filter
* @return the number of deleted meetings
*/
int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter)
{
	int cnt=0;
	m_dirty=true;
	for(auto i=m_meetingList.begin();i!=m_meetingList.end();i++){//Iterate through the list of meetings 
		if(filter(*i)==true){
			cnt++;//Calculate the number of meetings to delete
		}
	}
	m_meetingList.erase(std::remove_if(m_meetingList.begin(),m_meetingList.end(),filter),m_meetingList.end());
	//Delete all meetings in the meeting list that meet the filtering criteria
	return cnt;
}

/**
* sync with the file
*/
bool Storage::sync(void) {
	m_dirty=false;
	return writeToFile();
} 
