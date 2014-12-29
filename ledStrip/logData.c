// ----------------------------------------------------------------
// -------------------- logData.c --------------------------------
// ----------------------------------------------------------------

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "softKey.h"

void logRunTime (int heatActive, int coolActive, long int *dayRunTime)
{
static int heatActive_old = 0;
static int coolActive_old = 0;
static long int start_time_heat = 0;
static long int start_time_cool = 0;
static long int runTimeHeat = 0;
static long int runTimeCool = 0;
time_t t;
struct tm tm;
static int dayDataLogged;
FILE* fd = NULL;
FILE* fdOnOffTime = NULL;
char buff[81];
struct timespec gettime_now;

  // write run time data to file at one minute to midnight
  t = time(NULL);
  tm = *localtime(&t);
  if (tm.tm_hour == 23 && tm.tm_min == 59) {
  //if (tm.tm_sec == 0) {
    if (dayDataLogged == 0) {
      dayDataLogged = 1;

      // insert date into onOff log
      fdOnOffTime = fopen("/home/pi/c/wiFiThermo/logOnOffTime.txt","a");
      if(fdOnOffTime != NULL) {
        sprintf(buff, "%d-%d-%d\n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
        fwrite(buff,1,strlen(buff),fdOnOffTime);
      }
      fclose (fdOnOffTime);

      //insert date and runTime into data data log
      fd = fopen("/home/pi/c/wiFiThermo/logDayData.txt","a");
      if(fd != NULL) {
        sprintf(buff, "%d-%d-%d ; %d ; %d\n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, (int) runTimeHeat, (int) runTimeCool);
        fwrite(buff,1,strlen(buff),fd);
      }
      fclose (fd);

      // return greater of runTimeHeat or runTimeCool
      if (runTimeHeat > runTimeCool)
        *dayRunTime = runTimeHeat;
      else
        *dayRunTime = runTimeCool;
      runTimeHeat = 0;
      runTimeCool = 0;
    }
  }
  else {
    dayDataLogged = 0;
  }

  // keep track of heat run times
  if (heatActive == 1) {
    if (heatActive_old == 0) {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      start_time_heat = gettime_now.tv_sec;
      fdOnOffTime = fopen("/home/pi/c/wiFiThermo/logOnOffTime.txt","a");
      if(fdOnOffTime != NULL) {
        sprintf(buff, "  Heat ON - %02d:%02d\n", tm.tm_hour, tm.tm_min);
        fwrite(buff,1,strlen(buff),fdOnOffTime);
      }
      fclose (fdOnOffTime);
    }
  }
  else {
    if (heatActive_old == 1) {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      runTimeHeat += gettime_now.tv_sec - start_time_heat;
      fdOnOffTime = fopen("/home/pi/c/wiFiThermo/logOnOffTime.txt","a");
      if(fdOnOffTime != NULL) {
        sprintf(buff, "  Heat OFF - %02d:%02d, cumulative run time = %ds\n", tm.tm_hour, tm.tm_min, (int) runTimeHeat);
        fwrite(buff,1,strlen(buff),fdOnOffTime);
      }
      fclose (fdOnOffTime);
    }
  }


  // keep track of cool run times
  if (coolActive == 1) {
    if (coolActive_old == 0) {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      start_time_cool = gettime_now.tv_sec;
      fdOnOffTime = fopen("/home/pi/c/wiFiThermo/logOnOffTime.txt","a");
      if(fdOnOffTime != NULL) {
        sprintf(buff, "  AC ON - %02d:%02d\n", tm.tm_hour, tm.tm_min);
        fwrite(buff,1,strlen(buff),fdOnOffTime);
      }
      fclose (fdOnOffTime);
    }
  }
  else {
    if (coolActive_old == 1) {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      runTimeCool += gettime_now.tv_sec - start_time_cool;
      fdOnOffTime = fopen("/home/pi/c/wiFiThermo/logOnOffTime.txt","a");
      if(fdOnOffTime != NULL) {
        sprintf(buff, "  AC OFF - %02d:%02d, cumulative run time = %ds\n", tm.tm_hour, tm.tm_min, (int) runTimeHeat);
        fwrite(buff,1,strlen(buff),fdOnOffTime);
      }
      fclose (fdOnOffTime);
    }
  }


  heatActive_old = heatActive;
  coolActive_old = coolActive;
}

