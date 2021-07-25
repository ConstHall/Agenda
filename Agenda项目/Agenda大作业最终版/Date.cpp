#include<iostream>
#include<string>
#include"Date.hpp"
using namespace std;

int daysOfMonth(int year,int month)//Return the number of days in a month of a year(including leap years)
{
	int mon[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	int ans=0;
	if(month!=2){
		ans=mon[month];
	}
	else{
		if(year%4==0&&year%100!=0||year%400==0){
			ans=29;
		}
		else{
			ans=28;
		}
	}
	return ans;
}

Date::Date()
{
    m_year=0;
    m_month=0;
    m_day=0;
    m_hour=0;
    m_minute=0;
}

/**
* @brief constructor with arguments
*/
Date::Date(int t_year,int t_month,int t_day,int t_hour,int t_minute)
{
    m_year=t_year;
    m_month=t_month;
    m_day=t_day;
    m_hour=t_hour;
    m_minute=t_minute;
}

/**
* @brief constructor with a string
*/
Date::Date(const std::string &dateString)
{
    *this=stringToDate(dateString);
}

/**
* @brief return the year of a Date
* @return   a integer indicate the year of a date
*/
int Date::getYear(void) const
{
    return m_year;
}

/**
* @brief set the year of a date
* @param a integer indicate the new year of a date
*/
void Date::setYear(const int t_year)
{
    m_year=t_year;
}

/**
* @brief return the month of a Date
* @return   a integer indicate the month of a date
*/
int Date::getMonth(void) const
{
    return m_month;
}

/**
* @brief set the month of a date
* @param a integer indicate the new month of a date
*/
void Date::setMonth(const int t_month)
{
    m_month=t_month;
}

/**
* @brief return the day of a Date
* @return   a integer indicate the day of a date
*/
int Date::getDay(void) const
{
    return m_day;
}

/**
* @brief set the day of a date
* @param a integer indicate the new day of a date
*/
void Date::setDay(const int t_day)
{
    m_day=t_day;
}

/**
* @brief return the hour of a Date
* @return   a integer indicate the hour of a date
*/
int Date::getHour(void) const 
{
    return m_hour;
}

/**
* @brief set the hour of a date
* @param a integer indicate the new hour of a date
*/
void Date::setHour(const int t_hour)
{
    m_hour=t_hour;
}

/**
* @brief return the minute of a Date
* @return   a integer indicate the minute of a date
*/
int Date::getMinute(void) const
{
    return m_minute;
}

/**
* @brief set the minute of a date
* @param a integer indicate the new minute of a date
*/
void Date::setMinute(const int t_minute)
{
    m_minute=t_minute;
}

/**
*   @brief check whether the date is valid or not
*   @return the bool indicate valid or not
*/
bool Date::isValid(const Date &t_date)
{
    bool flag=true;
	if(t_date.m_year<1000||t_date.m_year>9999){
    	flag=false;
    }
    else if(t_date.m_month<=0||t_date.m_month>12){
    	flag=false;
	}
	else if(t_date.m_day<=0||t_date.m_day>daysOfMonth(t_date.m_year,t_date.m_month)){
		flag=false;
	}
	else if(t_date.m_hour<0||t_date.m_hour>23){
		flag=false;
	}
	else if(t_date.m_minute<0||t_date.m_minute>59){
		flag=false;
	}
	return flag;
}

/**
* @brief convert a string to date, if the format is not correct return
* 0000-00-00/00:00
* @return a date
*/
Date Date::stringToDate(const std::string &t_dateString)//Convert string to Date class
{
    int i;
    bool flag=true;//Determine whether the string conforms to the format of the Date class variable
    for(i=0;i<t_dateString.size();i++)
    {
    	if(i==4||i==7||i==10||i==13){
    		continue;
		}//Skip the characters '-',':' and '/'
		else if(t_dateString[i]<'0'||t_dateString[i]>'9'){
			flag=false;
			break;
		}//If the characters are not 0-9, the loop is broken
    }//Date class format is yyyy-mm-dd/hh:mm
    if(t_dateString[4]!='-'||t_dateString[7]!='-'||t_dateString[10]!='/'||t_dateString[13]!=':'){
    	flag=false;
	}//Determine if the character is valid
    if(flag==false){
    	Date temp(0,0,0,0,0);
      	return temp;
	}//All strings are initialized to 0 if they are not formatted correctly
    else{
    	int year=0,month=0,day=0,hour=0,minute=0;
    	year=(t_dateString[0]-'0')*1000+(t_dateString[1]-'0')*100+(t_dateString[2]-'0')*10+(t_dateString[3]-'0');
    	month=(t_dateString[5]-'0')*10+(t_dateString[6]-'0');
    	day=(t_dateString[8]-'0')*10+(t_dateString[9]-'0');
    	hour=(t_dateString[11]-'0')*10+(t_dateString[12]-'0');
		minute=(t_dateString[14]-'0')*10+(t_dateString[15]-'0'); 
		//Convert year,month,day,hour,and minute into integer form
      	Date final(year,month,day,hour,minute);
      	return final;
	}
}

/**
* @brief convert a date to string, if the format is not correct return
* 0000-00-00/00:00
*/
std::string Date::dateToString(const Date &t_date)
{
    string temp;
    if(isValid(t_date)==false){
    	temp="0000-00-00/00:00";
	} 
    else{
      	char year[5]; 
      	sprintf(year,"%4d-",t_date.m_year);//Written into the year in a prescribed format
      	temp.insert(0,year);//Insert the year into the string temp


      	char month[3];
      	sprintf(month,"%02d-",t_date.m_month);
      	temp.insert(5,month);

      	char day[3];
      	sprintf(day,"%02d/",t_date.m_day);
      	temp.insert(8,day);
      	char hour[3];
      	sprintf(hour,"%02d:",t_date.m_hour);
      	temp.insert(11,hour);

      	char minute[2];
      	sprintf(minute,"%02d",t_date.m_minute);
      	temp.insert(14,minute);
    }
    return temp;
}

/**
*  @brief overload the assign operator
*/
Date &Date::operator=(const Date &t_date)
{
    m_year=t_date.m_year;
    m_month=t_date.m_month;
    m_day=t_date.m_day;
    m_hour=t_date.m_hour;
    m_minute=t_date.m_minute;
    return *this;
}

/**
* @brief check whether the CurrentDate is equal to the t_date
*/
bool Date::operator==(const Date &t_date) const
{
    bool flag=false;
	if((*this).dateToString(*this)==t_date.dateToString(t_date)){//Call the overloaded == in the String library directly
    	flag=true;
	}
    return flag;
}

/**
* @brief check whether the CurrentDate is  greater than the t_date
*/
bool Date::operator>(const Date &t_date) const
{
    bool flag=false;
	if((*this).dateToString(*this)>t_date.dateToString(t_date)){//Call the overloaded > in the String library directly
    	flag=true;
    }
    return flag;
}

/**
* @brief check whether the CurrentDate is  less than the t_date
*/
bool Date::operator<(const Date &t_date) const
{
    bool flag=false;
	if((*this).dateToString(*this)<t_date.dateToString(t_date)){//Call the overloaded < in the String library directly
    	flag=true;
	}
    return flag;
}

/**
* @brief check whether the CurrentDate is  greater or equal than the t_date
*/
bool Date::operator>=(const Date &t_date) const
{
    bool flag=false;
	if((*this).dateToString(*this)>t_date.dateToString(t_date)||(*this).dateToString(*this)==t_date.dateToString(t_date)){
	//Call the overloaded ==&> in the String library directly 
    	flag=true;
	}
    return flag;
}

/**
* @brief check whether the CurrentDate is  less than or equal to the t_date
*/
bool Date::operator<=(const Date &t_date) const
{
    bool flag=false;
	if((*this).dateToString(*this)<t_date.dateToString(t_date)||(*this).dateToString(*this)==t_date.dateToString(t_date)){
	//Call the overloaded ==&< in the String library directly
    	flag=true;
	}
    return flag;
}

