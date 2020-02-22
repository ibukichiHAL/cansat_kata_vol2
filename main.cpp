/*
説明
Nucleo-F303K8とGPSモジュールを使ったサンプルプログラム．
 
使用GPS
http://akizukidenshi.com/catalog/g/gK-09991/
 
参考
http://www.hiramine.com/physicalcomputing/general/gps_nmeaformat.html
 
 
以下ピン配置
Nucleo  GPSモジュール
GND-----GND-----------0V
5V------VIN
D0------TX
D1------RX
*/

#include "mbed.h"
 
DigitalOut myled(LED1);
Serial gps(p9, p10);       // tx, rx
Serial pc(USBTX, USBRX);    // tx, rx
int i,rlock,mode;
char gps_data[256];
char ns,ew;
float w_time,hokui,tokei;
float g_hokui,g_tokei;
float d_hokui,m_hokui,d_tokei,m_tokei;
unsigned char c;
int stlgt;
char status;
float velocity, direction;
int date;
 
void getGPS() {
  c = gps.getc();
  if( c=='$' || i == 256){
    mode = 0;
    i = 0;
    for(int j=0; j<256; j++){
        gps_data[j]=NULL;
    }
  }
  if(mode==0){
    if((gps_data[i]=c) != '\r'){
      i++;
    }else{
      gps_data[i]='\0';
      
      pc.printf(gps_data);
      pc.printf("\r\n");
      
      if(sscanf(gps_data, "$GPGGA,%f,%f,%c,%f,%c,%d,%d",&w_time,&hokui,&ns,&tokei,&ew,&rlock,&stlgt) >= 1){
        if(rlock==1){
          //logitude
          d_tokei= int(tokei/100);
          m_tokei= (tokei-d_tokei*100)/60;
          g_tokei= d_tokei+m_tokei;
          //Latitude
          d_hokui=int(hokui/100);
          m_hokui=(hokui-d_hokui*100)/60;
          g_hokui=d_hokui+m_hokui;
          
          float timenow = w_time;
          int hour = timenow / 10000;
          timenow = fmod(timenow, 10000);
          int min = timenow / 100;
          timenow = fmod(timenow, 100);
          int sec = timenow / 1;
          timenow = fmod(timenow, 1);
          
          // /*
          // 10進法（google）
          pc.printf("世界標準時:%02dh%02dm%02ds 北緯:%.8f 東経:%.8f 状態:%d 使用衛星数:%d\r\n",
            hour, min, sec, g_hokui, g_tokei, rlock, stlgt);
          //  */
          /*
          // 60進法（x時y分z秒w）
          pc.printf("ｓ世界標準時:%02dh%02dm%02ds 北緯:%.8f 東経:%.8f 状態:%d 使用衛星数:%d\r\n",
            hour, min, sec, hokui/100, tokei/100, rlock, stlgt);
           */
        }
        else{
          float timenow = w_time;
          int hour = timenow / 10000;
          timenow = fmod(timenow, 10000);
          int min = timenow / 100;
          timenow = fmod(timenow, 100);
          int sec = timenow / 1;
          timenow = fmod(timenow, 1);
          
          pc.printf("# 世界標準時e:%02dh%02dm%02d 状態:%d 使用衛星数:%d\r\n",
            hour, min, sec, rlock, stlgt);
        }
        sprintf(gps_data, "");
      }//if
      
      /* ------------------------------ */
      if(sscanf(gps_data, "$GPRMC,%f,%f,%f,%d",&w_time,&velocity,&direction,&date) >= 1){
        if(rlock==1){
          
          float timenow = w_time;
          int hour = timenow / 10000;
          timenow = fmod(timenow, 10000);
          int min = timenow / 100;
          timenow = fmod(timenow, 100);
          int sec = timenow / 1;
          timenow = fmod(timenow, 1);
          
          float v_mps = velocity * 1852 / 3600;
          
          // /*
          // 10進法（google）
          pc.printf("世界標準時:%02dh%02dm%02ds 速度:%f[mps] 方位:%f, 日付:20%02d年%02d月%02d日\r\n",
            hour,min,sec,v_mps,direction,date%100,(date/100)%100, date/10000);
          //  */
          /*
          // 60進法（x時y分z秒w）
          pc.printf("世界標準時:%02dh%02dm%02ds 速度:%f[mps] 方位:%f, 日付:20%02d年%02d月%02d日\r\n",
            hour,min,sec,velocity,direction,date%100,(date/100)%100, date/10000);
           */
        }
        else{
          float timenow = w_time;
          int hour = timenow / 10000;
          timenow = fmod(timenow, 10000);
          int min = timenow / 100;
          timenow = fmod(timenow, 100);
          int sec = timenow / 1;
          timenow = fmod(timenow, 1);
          
          pc.printf("# 世界標準時:%02dh%02dm%02ds 速度:%f[knot] 方位:%f, 日付:20%02d年%02d月%02d日\r\n",
            hour,min,sec,velocity,direction,date%100,(date/100)%100, date/10000);
        }
        sprintf(gps_data, "");
      }//if
      
      pc.printf("\r\n");
    }
  }
}
 
int main(){
  pc.printf("*** GPS GT-720F ***\r\n");
  gps.baud(9600);
//  pc.baud(115200);
 gps.attach(getGPS,Serial::RxIrq);
  while(1) {/*gps.attach(getGPS,Serial::RxIrq)*/}
}


/* --- 生データ取得用 --- */
/*
#include "mbed.h"

Serial gps(p9, p10);       // TX, RX
Serial pc(USBTX, USBRX);    // TX, RX
DigitalOut led1(LED1);

int main() {
    pc.baud(115200);
    pc.printf("PA6C DEMO\n");
    char gpsout[1024];
    while (1) {
        gpsout[0] = '\0';
        while (1) {
            char c = gps.getc();
            char s[2];
            s[0] = c;
            s[1] = '\0';
            strcat(gpsout, s);
            if (c == '\n') {
                break;
            }
        }
        pc.printf(gpsout);
        led1 = !led1;
    }
}
 */