#include"User.hpp"
#include"Date.hpp"
#include"Meeting.hpp"
#include"Storage.hpp"
#include"AgendaService.hpp" 
#include<iostream>
#include<string>
#include<vector>
#include<list>
#include<algorithm>

using namespace std;

/**
* constructor
*/
AgendaService::AgendaService(){
	startAgenda();
}

/**
* destructor
*/
AgendaService::~AgendaService(){
	quitAgenda();
}

/**
* check if the username match password
* @param userName the username want to login
* @param password the password user enter
* @return if success, true will be returned
*/
bool AgendaService::userLogIn(const std::string &userName, const std::string &password){
	std::list<User> temp=listAllUsers();
   	for(auto i=temp.begin();i!=temp.end();i++){//Iterate through the list of users 
   		if(i->getName()==userName&&i->getPassword()==password){//Check if the username and password match
   			return true;	
		}
   }
   return false;
}

/**
* regist a user
* @param userName new user's username
* @param password new user's password
* @param email new user's email
* @param phone new user's phone
* @return if success, true will be returned
*/
bool AgendaService::userRegister(const std::string &userName, const std::string &password,const std::string &email, const std::string &phone){
	std::list<User> temp=listAllUsers();
   	for(auto i=temp.begin();i!=temp.end();i++){//Iterate through the list of users
   		if(i->getName()==userName){//Check whether the user name is registered 
   			return false;	
		}
	} 
   	User user(userName,password,email,phone);//If the username has not been registered,it will be initialized to the User class variable user 
   	m_storage->createUser(user);//Call the create function to create a new user 
   	return true;
}

/**
* delete a user
* @param userName user's username
* @param password user's password
* @return if success, true will be returned
*/
bool AgendaService::deleteUser(const std::string &userName, const std::string &password){
	auto filter=[userName,password](const User &user)->bool{
        if(userName==user.getName()&&password==user.getPassword()){
		//Check whether the current logged in user is consistent with the user to be deleted 
        	return true;
		}
		else{
			return false;
		}
    };
    deleteAllMeetings(userName);//Delete all meetings whose sponsor is the user 
    auto meetings_list=listAllParticipateMeetings(userName);
    for(auto i : meetings_list){//Iterate through the list of meetings 
    	quitMeeting(userName,i.getTitle());//The user is removed from the his all meetings as a participator
	}
    return m_storage->deleteUser(filter)>0; 
}

/**
* list all users from storage
* @return a user list result
*/
std::list<User> AgendaService::listAllUsers(void) const{
	auto filter=[](const User& user)->bool{
		return true;
	};
    return m_storage->queryUser(filter);
}

/**
* create a meeting
* @param userName the sponsor's userName
* @param title the meeting's title
* @param participator the meeting's participator
* @param startData the meeting's start date
* @param endData the meeting's end date
* @return if success, true will be returned
*/
bool AgendaService::createMeeting(const std::string &userName, const std::string &title,const std::string &startDate, 
								  const std::string &endDate,const std::vector<std::string> &participator){
	Date start(startDate);
	Date end(endDate);
	Meeting meeting(userName,participator,start,end,title);
	if(start>=end||start.isValid(start)==false||end.isValid(end)==false){//Check if the date is valid and logical
		return false;
	}
	bool flag=false;
    for(auto i : listAllUsers()){//Iterate through the list of users 
        if(i.getName()==userName){//Check if the sponsor's name is registered 
            flag=true;
            break;
        }
    }
	if(flag==false){//Return false if the sponsor's name does not exist 
		return flag;
	}
	if(participator.size()==0){//Check if the number of participators in the meeting is 0 
		return false;
	} 
    auto filter=[title](const Meeting &meeting)->bool{
    	bool flag1;
        if(meeting.getTitle()==title){//Check if the meeting title already exists
        	flag1=true;
		}
		else{
			flag1=false;
		}
		return flag1;
    };
    if((m_storage->queryMeeting(filter)).size()>0){
    	return false;
	}
    auto filter1=[userName](const Meeting &meeting)->bool{
        bool flag2;
		if(userName==meeting.getSponsor()){
        	flag2=true;
		}
		else{
			flag2=false;
		}
		return flag2;
    };
	std::list<Meeting> meeting1=listAllMeetings(userName);
    for(auto i : meeting1){//Check all the user's meetings(the role as participator or sponsor) 
		if(!(i.getEndDate()<=start||i.getStartDate()>=end)){//Check if the meeting time overlaps with other meeting times
			 return false;
		}
	}
	std::vector<std::string> participators;
    Meeting temp(userName,participators,start,end,title);
    m_storage->createMeeting(temp);
	for(auto i=participator.begin();i!=participator.end();i++){//Iterate through the participators
		bool flag3;
		flag3=addMeetingParticipator(userName,title,*i); 
    	if(flag3==true){
    		participators.push_back(*i);//Add a new participator to the container of participators 
		}
		else{
			m_storage->deleteMeeting(filter);//Delete the user from the meeting 
			return false;
		}
	}
	return true;
}

/**
 * add a participator to a meeting
 * @param userName the sponsor's userName
 * @param title the meeting's title
 * @param participator the meeting's participator
 * @return if success, true will be returned
 */
bool AgendaService::addMeetingParticipator(const std::string &userName,
                            const std::string &title,
                            const std::string &participator){
    bool flag=false;
    for(auto i : listAllUsers()){//Iterate through the users 
        if(i.getName()==participator){//Check whether the participator is registered
            flag=true;
            break;
        }
    }
    if(flag==false){
    	return false;
	}
    auto meeting=m_storage->queryMeeting([&](const Meeting &meeting1)->bool{
    	if(meeting1.getSponsor()==userName&&meeting1.getTitle()==title){//Check if the meeting to join exists
    		return true;
		}
		else{
			return false;
		}
	});
    if(meeting.size()==0){
    	return false;
	}
    auto it=meeting.begin();
    auto temp=*it;
    auto participator_meeting=listAllMeetings(participator);
    for(auto i : participator_meeting){//Iterate all meetings whose sponsor or participator is the participator
        if(!(temp.getEndDate()<=i.getStartDate()||temp.getStartDate()>=i.getEndDate())){
		//Check if the participator attend the meeting,avoid meeting time conflict
        	return false;
		}
    }
    auto filter=[userName,title](const Meeting& meeting)->bool{
    	if(meeting.getSponsor()==userName&&meeting.getTitle()==title){//Check if the meeting to join exists 
    		return true;
		}
		else{
			return false;
		}
    };
    auto switcher=[participator](Meeting& meeting)->bool{
        meeting.addParticipator(participator);//Add the participator to the meeting
    };
    m_storage->updateMeeting(filter,switcher);//Update meeting status
    return true;
}

/**
 * remove a participator from a meeting
 * @param userName the sponsor's userName
 * @param title the meeting's title
 * @param participator the meeting's participator
 * @return if success, true will be returned
 */
bool AgendaService::removeMeetingParticipator(const std::string &userName,
                               const std::string &title,
                               const std::string &participator){
    auto filter=[userName,title,participator](const Meeting& meeting)->bool{
        if(meeting.getSponsor()!=userName||meeting.getTitle()!=title){//Check if the meeting to join exists 
        	return false;
		}
        bool flag=false;
        for(auto i : meeting.getParticipator()){//Iterate the participators 
            if(i==participator){//Check if the user participated in the meeting
                flag=true;
                break;
            }
        }
        return flag;
    };
    auto switcher=[participator](Meeting& meeting)->bool{
        meeting.removeParticipator(participator);//Remove participator 
    };
    int cnt=m_storage->updateMeeting(filter,switcher);//Update meeting status 
    auto filter1=[](const Meeting& temp)->bool{
    	if(temp.getParticipator().size()==0){
    		return true;
		}
		else{
			return false;
		}
    };
    m_storage->deleteMeeting(filter1);//Delete the meeting if the number of participators is 0 
    return cnt>0;
}

/**
* quit from a meeting
* @param userName the current userName. no need to be the sponsor
* @param title the meeting's title
* @return if success, true will be returned
*/
bool AgendaService::quitMeeting(const std::string &userName, const std::string &title){
	auto filter=[userName,title](const Meeting& meeting)->bool{
     	if(meeting.isParticipator(userName)&&meeting.getTitle()==title){//Check if the user participated in the meeting with the title 
     		return true;
	 	}
	 	else{
	 		return false;
	 	}
	};
	auto switcher=[userName](Meeting& meeting)->bool{
        meeting.removeParticipator(userName);//Remove the participator
    };
    int cnt=m_storage->updateMeeting(filter,switcher);
    auto filter1=[](const Meeting& meeting)->bool{
        if(meeting.getParticipator().size()==0){//Check whether the meeting member is 0
        	return true;
		}
		else{
			return false;
		}
    };
    m_storage->deleteMeeting(filter1);
    return cnt>0;
}

/**
* search a meeting by username and title
* @param uesrName the sponsor's userName
* @param title the meeting's title
* @return a meeting list result
*/
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,const std::string &title) const{
	std::list<Meeting> meeting;
	std::list<Meeting> temp;
	std::list<User> user=m_storage->queryUser([userName](const User& temp1)->bool{
		if(temp1.getName()==userName){
			return true;
		}
		else{
			return false;
		}
	});
	if(user.size()==0){
		return temp;
	}
	auto filter=[userName,title](const Meeting& meeting)->bool{
    	if((userName==meeting.getSponsor()||meeting.isParticipator(userName))&&title==meeting.getTitle()){
		//Check if the user is the sponsor or participator of the meeting 
    		return true;
		}
		else{
			return false;
		}
	};
	temp=m_storage->queryMeeting(filter);
	return temp;
}

/**
* search a meeting by username, time interval
* @param uesrName the sponsor's userName
* @param startDate time interval's start date
* @param endDate time interval's end date
* @return a meeting list result
*/
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,const std::string &startDate,const std::string &endDate) const{
	Date start(startDate);
	Date end(endDate);
	std::list<Meeting> temp;
	std::list<Meeting> meeting;
	if(start>end||start.isValid(start)==false||end.isValid(end)==false){//Check if the meeting time is legal 
		return temp;
	}
	std::list<User> user=m_storage->queryUser([userName](const User& temp1)->bool{
		if(temp1.getName()==userName){
			return true;
		}
		else{
			return false;
		}
	});
	if(user.size()==0){
		return meeting;
	}
	auto filter=[userName,start,end](const Meeting &meeting)->bool{
		if((meeting.getSponsor()==userName||meeting.isParticipator(userName))&&meeting.getEndDate()>=start&&meeting.getStartDate()<=end){
		//Search the user who is the sponsor or participator and the meeting time is within the scope
			return true;
		}
	 	else{
	 		return false;
	 	}
	};
	temp=m_storage->queryMeeting(filter);
	return temp;

}

/**
* list all meetings the user take part in
* @param userName user's username
* @return a meeting list result
*/
std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const{
	auto filter=[userName](const Meeting& meeting)->bool{
        if(meeting.getSponsor()==userName||meeting.isParticipator(userName)==true){//Search the user who is the sponsor or participator
        	return true;
		}
		else{
			return false;
		}
    };
    return m_storage->queryMeeting(filter);
}

/**
* list all meetings the user sponsor
* @param userName user's username
* @return a meeting list result
*/
std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const{
 	auto filter=[userName](const Meeting& meeting)->bool{
 			if(meeting.getSponsor()==userName){//Check if the user is the sponsor of the meeting 
 				return true;
			}
			else{
			 	return false;
			}
    };
    return m_storage->queryMeeting(filter);
}

/**
* list all meetings the user take part in and sponsor by other
* @param userName user's username
* @return a meeting list result
*/
std::list<Meeting> AgendaService::listAllParticipateMeetings(const std::string &userName) const{
	auto filter=[userName](const Meeting& meeting)->bool{
		for(auto i : meeting.getParticipator()){//Iterate through the participators of the meeting 
            if(i==userName){//Check whether the user is a participator
            	return true;
			}
        }
        return false;
	};
    return m_storage->queryMeeting(filter);
}

/**
* delete a meeting by title and its sponsor
* @param userName sponsor's username
* @param title meeting's title
* @return if success, true will be returned
*/
bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title){
	auto filter=[userName, title](const Meeting& meeting)->bool{
       if(meeting.getTitle()==title&&meeting.getSponsor()==userName){//Check if the member is the sponsor of a particular meeting 
       		return true;
	   	}
	   	else{
	   		return false;	
		}
    };
    return m_storage->deleteMeeting(filter)>0;
}

/**
* delete all meetings by sponsor
* @param userName sponsor's username
* @return if success, true will be returned
*/
bool AgendaService::deleteAllMeetings(const std::string &userName){
	auto filter=[userName](const Meeting& meeting)->bool{
        if(meeting.getSponsor()==userName){//Check if the member is the sponsor of a meeting 
        	return true;
		}
		else{
			return false;
		}
    };
    return m_storage->deleteMeeting(filter)>0;
}

/**
* start Agenda service and connect to storage
*/
void AgendaService::startAgenda(void)
{
	m_storage=Storage::getInstance();
}

/**
* quit Agenda service
*/
void AgendaService::quitAgenda(void)
{
	m_storage->sync();
} 
