#include "AgendaUI.hpp"
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;

string printParticipators(vector<string> participators){
	int i=0;
	string temp;
	if(participators.size()==0){
		temp="";
	}
	else{
		for(i=0;i<participators.size();i++){
			if(i<participators.size()-1){
				temp+=participators[i];
				temp+=",";
			}
			else{
				temp+=participators[i];
			}
		}	
	}
	return temp;
}

AgendaUI::AgendaUI(){
	startAgenda();
}

void AgendaUI::OperationLoop(void){
	std::string operation;
	while(1){
		cout<<"-----------------------------Agenda-----------------------------"<<endl;
		cout<<"Action :"<<endl;
		cout<<"l    - log in Agenda by user name and password"<<endl;
		cout<<"r    - register an Agenda account"<<endl;
		cout<<"q    - quit Agenda"<<endl;
		cout<<"----------------------------------------------------------------"<<endl;
		cout<<endl;
		cout<<"Agenda :~$ ";
		cin>>operation;
		switch(operation[0])
        {
          case 'l':
		  		userLogIn();
          		break;
          case 'r':
		  		userRegister();
				break;
          case 'q':
		  		quitAgenda();
		  		break;
          default:break;
        }
		if(operation[0]=='q'){
			break;
		}
	}
}

/**
* constructor
*/
void AgendaUI::startAgenda(void){
	m_agendaService.startAgenda();
}

/**
* catch user's operation
* @return the operation
*/
std::string AgendaUI::getOperation(){
	cout<<"-----------------------------Agenda-----------------------------"<<endl;
	cout<<"Action :"<<endl;
	cout<<"o    - log out Agenda"<<endl;
	cout<<"dc   - delete Agenda account"<<endl;
	cout<<"lu   - list all Agenda user"<<endl;
	cout<<"cm   - create a meeting"<<endl;
	cout<<"amp  - add meeting participator"<<endl;
	cout<<"rmp  - remove meeting participator"<<endl;
	cout<<"rqm  - request to quit meeting"<<endl;
	cout<<"la   - list all meetings"<<endl;
	cout<<"las  - list all sponsor meetings"<<endl;
	cout<<"lap  - list all participator meetings"<<endl;
	cout<<"qm   - query meeting by title"<<endl;
	cout<<"qt   - query meeting by time interval"<<endl;
	cout<<"dm   - delete meeting by title"<<endl;
	cout<<"da   - delete all meetings"<<endl;
	cout<<"----------------------------------------------------------------"<<endl;
	cout<<endl;
	cout<<"Agenda@"<<m_userName<<" :~# ";
	string operation;
	cin>>operation;
	return operation;
}

/**
* execute the operation
* @return if the operationloop continue
*/
bool AgendaUI::executeOperation(const std::string &t_operation){
	if(t_operation=="o"){
		userLogOut();
		return false;
	}
	else if(t_operation=="dc"){
		deleteUser();
		return false;
	}
	else if(t_operation=="lu"){
		listAllUsers();
		return true;
	}
	else if(t_operation=="cm"){
		createMeeting();
		return true;
	}
	else if(t_operation=="amp"){
		addMeetingParticipator();
		return true;
	}
	else if(t_operation=="rmp"){
		removeMeetingParticipator();
		return true;
	}
	else if(t_operation=="rqm"){
		quitMeeting();
		return true;
	}
	else if(t_operation=="la"){
		listAllMeetings();
		return true;
	}
	else if(t_operation=="las"){
		listAllSponsorMeetings();
		return true;
	}
	else if(t_operation=="lap"){
		listAllParticipateMeetings();
		return true;
	}
	else if(t_operation=="qm"){
		queryMeetingByTitle();
		return true;
	}
	else if(t_operation=="qt"){
		queryMeetingByTimeInterval();
		return true;
	}
	else if(t_operation=="dm"){
		deleteMeetingByTitle();
		return true;
	}
	else if(t_operation=="da"){
		deleteAllMeetings();
		return true;
	}
	return true;
}

/**
* user Login
*/
void AgendaUI::userLogIn(void){
	cout<<"\n[log in] [username] [password]"<<endl;
	cout<<"[log in] ";
	string t_username, t_password;
	cin>>t_username>>t_password;
	if(m_agendaService.userLogIn(t_username,t_password)==true){
		m_userName=t_username;
		m_userPassword=t_password;
		cout<<"[log in] succeed!"<<endl<<endl;
		while(executeOperation(getOperation()));
	}
	else{
		cout<<"[log in] Password error or user doesn't exist"<<endl<<endl;
	}
}

/**
* user regist
*/
void AgendaUI::userRegister(void){
	cout<<"\n[register] [username] [password] [email] [phone]"<<endl;
	cout<<"[register] ";
	string username,password,email,phone;
	cin>>username>>password>>email>>phone;
	if(m_agendaService.userRegister(username,password,email,phone)==true){
		cout<<"[register] succeed!"<<endl<<endl;
	}	
	else{
		cout<<"[register] This username has been registered!"<<endl<<endl;
	}
}

/**
* user logout
*/
void AgendaUI::userLogOut(void){
	m_userName.clear();
	m_userPassword.clear();
}

/**
* quit the Agenda
*/
void AgendaUI::quitAgenda(void){
	m_agendaService.quitAgenda();
}

/**
* delete a user from storage
*/
void AgendaUI::deleteUser(void){
	if(m_agendaService.deleteUser(m_userName,m_userPassword)==true){
		cout<<"[delete agenda account] succeed!"<<endl<<endl;
		userLogOut();
	}
	else{
		cout<<"[delete agenda account] error!"<<endl<<endl;
	}
}

/**
* list all users from storage
*/
void AgendaUI::listAllUsers(void){
    std::list<User> users=m_agendaService.listAllUsers();
    cout<<"\n[list all users]"<<endl<<endl;
    cout<<left<<setw(15)<<"name"<<left<<setw(25)<<"email"<<"phone"<<endl;
    for(auto i=users.begin();i!=users.end();i++)
    {
    	cout<<left<<setw(15)<<(*i).getName()<<left<<setw(25)<<(*i).getEmail()<<(*i).getPhone()<<endl;
    }
}


/**
* user create a meeting with someone else
*/
void AgendaUI::createMeeting(void){
    int i;
    string title,start,end;
	cout<<"\n[create meeting] [the number of participators]"<<endl;
    cout<<"[create meeting] ";
    int total_participators=0;
    cin>>total_participators;
    std::vector<string> participators;
    for(i=0;i<total_participators;i++)
    {
    	string name;
		cout<<"[create meeting] [please enter the participator "<<i+1<<" ]"<<endl;
    	cin>>name;
    	participators.push_back(name);
    }
    cout<<"[create meeting] [title] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]"<<endl;
    cout<<"[create meeting] ";
    cin>>title>>start>>end;
    if(m_agendaService.createMeeting(m_userName,title,start,end,participators)==true)
    {
    	cout<<"[create meeting] succeed!"<<endl;
    }
    else
    {
    	cout<<"[create meeting] error!"<<endl;
    }
}
/**
* sponsor add a participator to the meeting
*/
void AgendaUI::addMeetingParticipator(void){
	cout<<"\n[add participator] [meeting title] [participator username]"<<endl;
	cout<<"[add participator] ";
	string title,participator;
	cin>>title>>participator;
	if(m_agendaService.addMeetingParticipator(m_userName,title,participator)==true){
		cout<<"[add participator] succeed!"<<endl<<endl;
	}
	else{
		cout<<"[add participator] error!"<<endl<<endl;
	}		
}

/**
* sponsor add a participator to the meeting
*/
void AgendaUI::removeMeetingParticipator(void){
	cout<<"\n[remove participator] [meeting title] [participator username]"<<endl;
	cout<<"[remove participator] ";
	string title, participator;
	cin>>title>>participator;
	if(m_agendaService.removeMeetingParticipator(m_userName,title,participator)==true){
		cout<<"[remove participator] succeed!"<<endl<<endl;
	}
	else{
		cout<<"[remove participator] error!"<<endl<<endl;
	}		
}

/**
* user quit from meeting
*/
void AgendaUI::quitMeeting(void){
	cout<<"\n[quit meeting] [meeting title]"<<endl;
	cout<<"[quit meeting] ";
	string title;
	cin>>title;
	if(m_agendaService.quitMeeting(m_userName,title)==true){
		cout<<"[quit meeting] succeed!"<<endl<<endl;
	}
	else{
		cout<<"[quit meeting] error!"<<endl<<endl;
	}		
}

/**
* list all meetings from storage
*/
void AgendaUI::listAllMeetings(void){
	cout<<"\n[list all meetings]"<<endl;
	printMeetings(m_agendaService.listAllMeetings(m_userName));
}

/**
* list all meetings that this user sponsored
*/
void AgendaUI::listAllSponsorMeetings(void){
	cout<<"\n[list all sponsor meetings]"<<endl;
	printMeetings(m_agendaService.listAllSponsorMeetings(m_userName));
}

/**
* list all meetings that this user take part in
*/
void AgendaUI::listAllParticipateMeetings(void){
	cout<<"\n[list all participator meetings]"<<endl;
	printMeetings(m_agendaService.listAllParticipateMeetings(m_userName));
}

/**
* search meetings by title from storage
*/
void AgendaUI::queryMeetingByTitle(void){
	cout<<"\n[query meetings] [title]"<<endl;
	cout<<"[query meetings] ";
	string title;
	cin>>title;
	printMeetings(m_agendaService.meetingQuery(m_userName,title));
}

/**
* search meetings by timeinterval from storage
*/
void AgendaUI::queryMeetingByTimeInterval(void){
	cout<<"\n[query meetings] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]"<<endl;
	cout<<"[query meetings] ";
	string start,end;
	cin>>start>>end;
	printMeetings(m_agendaService.meetingQuery(m_userName,start,end));
}

/**
* delete meetings by title from storage
*/
void AgendaUI::deleteMeetingByTitle(void){
	cout<<"\n[delete meeting] [title]"<<endl;
	cout<<"[delete meeting] ";
	string title;
	cin>>title;
	if(m_agendaService.deleteMeeting(m_userName,title)==true){
		cout<<"[delete meeting] succeed!"<<endl<<endl;
	}
	else{
		cout<<"[error] delete meeting fail!"<<endl<<endl;
	}
}

/**
* delete all meetings that this user sponsored
*/
void AgendaUI::deleteAllMeetings(void){
	m_agendaService.deleteAllMeetings(m_userName);
	cout<<"\n[delete all meeting] succeed!"<<endl<<endl;
}

/**
* show the meetings in the screen
*/
void AgendaUI::printMeetings(const std::list<Meeting> &t_meetings){
    string participators;
	cout<<left<<setw(12)<<"\ntitle"<<left<<setw(12)<<"sponsor"<<left<<setw(21)<<"start time"<<left<<setw(21)<<"end time"<<left<<setw(14)<<"participators"<<endl;
    for(auto i=t_meetings.begin();i!=t_meetings.end();i++)
    {
    	participators=printParticipators((*i).getParticipator());
    	cout<<left<<setw(11)<<(*i).getTitle()<<left<<setw(12)<<(*i).getSponsor()<<left<<setw(21)<<Date::dateToString((*i).getStartDate())<<left<<setw(21)<<Date::dateToString((*i).getEndDate())<<left<<setw(14)<<participators<<endl;
    }
}
