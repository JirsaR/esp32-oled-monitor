#include "NTPClient.h"

NTPClient::NTPClient() {
  // Initialize DST variables
  _dstRulesCount = 0;
  
  // Initialize DST rules array
  for (int i = 0; i < 10; i++) {
    _dstRules[i].day = 0;
    _dstRules[i].month = 0;
    _dstRules[i].year = 0;
    _dstRules[i].offset = 0;
  }
}

void NTPClient::setPoolServerName(const char* poolServerName) {
    this->_poolServerName = poolServerName;
}

// DST Rules Implementation
bool NTPClient::addDSTRule(int day, int month, int year, long offset) {
  // Validate parameters
  if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2020 || _dstRulesCount >= 10) {
    return false;
  }
  
  // Add the rule
  _dstRules[_dstRulesCount].day = day;
  _dstRules[_dstRulesCount].month = month;
  _dstRules[_dstRulesCount].year = year;
  _dstRules[_dstRulesCount].offset = offset;
  _dstRulesCount++;
  
  return true;
}

void NTPClient::clearDSTRules() {
  _dstRulesCount = 0;
  for (int i = 0; i < 10; i++) {
    _dstRules[i].day = 0;
    _dstRules[i].month = 0;
    _dstRules[i].year = 0;
    _dstRules[i].offset = 0;
  }
}

long NTPClient::_calculateCurrentOffset() {
  if (_dstRulesCount == 0) {
    return _timeOffset; // Return default offset if DST is disabled or no rules
  }
  
  // Get current time info
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  int currentDay = timeinfo->tm_mday;
  int currentMonth = timeinfo->tm_mon + 1; // tm_mon is 0-based
  int currentYear = timeinfo->tm_year + 1900; // tm_year is years since 1900
  
  // Find the most recent applicable rule
  long applicableOffset = _timeOffset; // Default to base offset
  int mostRecentYear = 0;
  int mostRecentMonth = 0;
  int mostRecentDay = 0;
  
  for (int i = 0; i < _dstRulesCount; i++) {
    // Check if this rule applies to current year or a past year
    if (_dstRules[i].year <= currentYear) {
      // Check if this rule is more recent than the current applicable rule
      bool isMoreRecent = false;
      
      if (_dstRules[i].year > mostRecentYear) {
        isMoreRecent = true;
      } else if (_dstRules[i].year == mostRecentYear) {
        if (_dstRules[i].month > mostRecentMonth) {
          isMoreRecent = true;
        } else if (_dstRules[i].month == mostRecentMonth) {
          if (_dstRules[i].day >= mostRecentDay) {
            isMoreRecent = true;
          }
        }
      }
      
      // Check if the rule date has passed in the current year
      bool ruleDatePassed = false;
      if (_dstRules[i].year == currentYear) {
        if (_dstRules[i].month < currentMonth) {
          ruleDatePassed = true;
        } else if (_dstRules[i].month == currentMonth && _dstRules[i].day <= currentDay) {
          ruleDatePassed = true;
        }
      } else if (_dstRules[i].year < currentYear) {
        ruleDatePassed = true;
      }
      
      if (isMoreRecent && ruleDatePassed) {
        applicableOffset = _dstRules[i].offset;
        mostRecentYear = _dstRules[i].year;
        mostRecentMonth = _dstRules[i].month;
        mostRecentDay = _dstRules[i].day;
      }
    }
  }
  
  return applicableOffset;
}

void NTPClient::_applyDSTRules() {
  if (_dstRulesCount > 0) {
    _timeOffset = _calculateCurrentOffset();
  }
}

void NTPClient::setTimeOffset(int timeOffset) {
  this->_timeOffset     = timeOffset;
}

void NTPClient::setUpdateInterval(unsigned long updateInterval) {
  this->_updateInterval = updateInterval;
}

bool NTPClient::update() {
  // Apply DST rules before updating
  _applyDSTRules();
  
  // Update if updateIterval is reached or update immediately if there is no lastUpdate
  if ( (millis() - this->_lastUpdate >= this->_updateInterval) || this->_lastUpdate == 0) {                                       
      return this->forceUpdate();
  }

  return false;  // return false if update has not been occurred
}

bool NTPClient::forceUpdate() {
  configTime(this->_timeOffset, 0, this->_poolServerName); 
  if (!getLocalTime(&_timeInfo)) { // get the current time
    return false;
  }

  return true;  // return true after successful update
}

String NTPClient::getFormattedTime() const {
  char timeStringBuff[64];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &_timeInfo);

  return String(timeStringBuff);
}

String NTPClient::getFormattedDate() const {
  char dateStringBuff[64];
  strftime(dateStringBuff, sizeof(dateStringBuff), "%d. %m. %Y", &_timeInfo);

  return String(dateStringBuff);
}
