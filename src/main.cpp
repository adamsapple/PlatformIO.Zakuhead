#include <Arduino.h>
#include <Servo.h>
#include <avr/pgmspace.h>

#include <voice.h>
#include <keyframe.h>
#include <util.h>

const int brightDuty  = 255;
const int defaultDuty = 127;
const int darkDuty    = 63;
const int offDuty     = 0;

enum LedMotion {
    LED_OFF,
    LED_DARK,
    LED_BRIGHT,

    LED_FADEOUT,
    LED_DARKEN,
    LED_BRIGHTLY,
    LED_BLINK,

    LED_MOTION_NUM
};

int vi;
Servo servo1;

const int ledPin   = 3;
const int servoPin = 9;
const int voicePin = 11;//3;
unsigned int now;
unsigned long startTime;
unsigned long endTime;

unsigned int startAngle;
unsigned int endAngle;
unsigned int prevAngle;

const int minPulse    = 500;
const int maxPulse    = 2400;
const int minAngle    = 0;
const int centerAngle = 90;
const int maxAngle    = 180;
const int leftAngle   = minAngle + 35;
const int rightAngle  = maxAngle - 35;

//void play() {
//  for (int i = 0; i < zaku8k_raw_len; i++) {
//    OCR2B = pgm_read_byte_near(&zaku8k_raw[i]);
//    delayMicroseconds(125);
//  }
//} 

KeyFrame ledKeyFrames_OFF[] = {
    {500, offDuty, false},
    {500, 0,       true},
};

KeyFrame ledKeyFrames_Boot[] = {
    {0,    offDuty,     false},
    {1000, brightDuty,  false},
    {500,  0,           true},
    {500,  defaultDuty, false},
    {1000, 0,           true},
};

KeyFrame ledKeyFrames_LED_BLINK[] = {
    {20, offDuty,     false}, {105, 0, true},
    {20, defaultDuty, false}, {105, 0, true},
    {20, offDuty,     false}, {105, 0, true},
    {20, defaultDuty, false}, {105, 0, true},
    {20, offDuty,     false}, {105, 0, true},
    {20, defaultDuty, false}, {605, 0, true},
};

KeyFrame ledKeyFrames_KEEP[] = {
    {1000, 0, true},
};

KeyFrame srvKeyFrames[] = {
    {0, centerAngle, false}, {1000, 0, true},
    //{0, leftAngle,   false}, {1000, 0, true},
    //{0, rightAngle,  false}, {1000, 0, true},
    //{0, centerAngle, false}, {3000, 0, true},
    //{100, 20, false},
    //{1000, 0, true},
    //{1000, 150, false}
};

int prevLedPattern = -1;

/*
KeyFrame ledKeyFrames_LED_DARK[1] = {
    {0, darkDuty, false},
};

KeyFrame ledKeyFrames_LED_BRIGHT[1] = {
    {0, brightDuty, false},
};

KeyFrame ledKeyFrames_LED_FADEOUT[1] = {
    {1000, 0, false},
};

KeyFrame ledKeyFrames_LED_DARKEN[1] = {
    {1000, darkDuty, false},
};

KeyFrame ledKeyFrames_LED_BRIGHTLY[1] = {
    {1000, brightDuty, false},
};
*/


KeyFrameAnimator srvAnimator;
KeyFrameAnimator ledAnimator;


void playOneImp();
void OnKeyFrameFinished(void* sender, EventArgs* e);
void OnLedKeyFrameFinished(void* sender, EventArgs* e);

void play() {
    pinMode(voicePin, OUTPUT);
    for (int i = 0; i < zaku8k_raw_len; i++) {
        OCR2A = pgm_read_byte_near(&zaku8k_raw[i]);
        delayMicroseconds(95);
    }
    pinMode(voicePin, INPUT);
    OCR2A = 0;
} 

bool isPlay = false;
void playStart(){
    servo1.detach();
    delay(500);
    pinMode(voicePin, OUTPUT);
    
    vi = 0;
    isPlay = true;
    //srvAnimator.Stop();
    srvKeyFrames[0].IsFreeze   = true;
    srvKeyFrames[0].TimeSpanMS = 3000;
    //srvKeyFrames[0].DestValue  = map(angle, minAngle, maxAngle, minPulse, maxPulse);
    srvAnimator.SetKeyFrame(srvKeyFrames[0]);

}
void playOneImp() {
    if(!isPlay){
        return;
    }
    if(vi >= zaku8k_raw_len) {
        //srvAnimator.Start(servo1.readMicroseconds());
        //OnKeyFrameFinished(NULL, (EventArgs*)(NULL));
        isPlay = false;
        OCR2A = 0;
        pinMode(voicePin, INPUT);
        servo1.attach(servoPin, minPulse, maxPulse);
        return;
    }
    OCR2A = pgm_read_byte_near(&zaku8k_raw[vi++]);
    delayMicroseconds(70);
}



int fctr;
auto angle = 0;
bool debugLed = LOW;


/**
 * サーボのアニメーション終了Ev
 * 新しいアニメーションを作成し、設定する.
 */
void OnKeyFrameFinished(void* sender, EventArgs* e)
{
    //debugLed = !debugLed;
    //digitalWrite(LED_BUILTIN, debugLed);
    //KeyFrame& k= (KeyFrame&)*sender;
    //KeyFrameAnimator s = KeyFrameAnimator(*sender);
    //KeyFrameAnimator* animator = (KeyFrameAnimator*)(sender);
    //animator->SetKeyFrame(keyFrames[fctr]);
    //srvAnimator.SetKeyFrame(keyFrames[fctr]);
    //fctr = (fctr+1) % array_length(keyFrames);
    
    // 次の角度ど時間を決定
    auto span  = random(1000, 4000);
    auto angle = random(leftAngle, rightAngle);
    auto frz   = (random(0, 8) == 0);
    Serial.print("srv update(");
    Serial.print("span: ");     Serial.print(span);
    Serial.print(", angle: ");  Serial.print(angle);
    Serial.print(", freeze: "); Serial.print(frz);
    Serial.println(")");

    // Serial.print("   nv: ");    Serial.println(nowValue);

    // アニメーション割当
    srvKeyFrames[0].IsFreeze   = frz;
    srvKeyFrames[0].TimeSpanMS = span;
    srvKeyFrames[0].DestValue  = map(angle, minAngle, maxAngle, minPulse, maxPulse);
    srvAnimator.SetKeyFrame(srvKeyFrames[0]);
    // if(keyFrames[0].IsFreeze){
    //     servo1.detach();
    // }else{
    //     servo1.attach(servoPin, minPulse, maxPulse);
    // }
    
    // LEDのアニメーションも更新する
    OnLedKeyFrameFinished(NULL, (EventArgs*)(NULL));
}

void OnLedKeyFrameFinished(void* sender, EventArgs* e)
{
    int a = random(0, 4);

    if(prevLedPattern == 0 && a == 0) {
        a = 2;
    }else
    if(prevLedPattern != 3 && a == 0) {
        a = 1;
    }else
    if(prevLedPattern == 3 && a == 1) {
        a = 0;
    }

    if(prevLedPattern == -1){
        a = 0;
    }
    
    if(isPlay){
        a = 2;
    }

    if(a == 2){
        return;
    }
    //a = 1;
    //Serial.print("led pattern: ");
    //Serial.println(a);

    KeyFrame* kf  = NULL;
    int       len = 0;
    
    switch(a){
        case 0:   kf = ledKeyFrames_Boot;      len = array_length(ledKeyFrames_Boot); playStart(); break;
        case 1:   kf = ledKeyFrames_LED_BLINK; len = array_length(ledKeyFrames_LED_BLINK); break;
        case 2:   kf = ledKeyFrames_KEEP;      len = array_length(ledKeyFrames_KEEP);      break;
        case 3:   kf = ledKeyFrames_OFF;       len = array_length(ledKeyFrames_OFF);       break;
    }

    if(len != 0){
        prevLedPattern = a;
        ledAnimator.SetKeyFrames(kf, len);
    }
}


/*
LEDパターン
消灯
明るい
暗い
点滅3回(1-0-1-0-1)
*/

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    int seed = analogRead(2);
    randomSeed(seed);
    Serial.print("seed: ");
    Serial.println(seed);

    pinMode(voicePin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    
    servo1.attach(servoPin, minPulse, maxPulse);
    servo1.write(centerAngle);
    // delay(1500);
    // servo1.write(leftAngle);
    // delay(1500);
    // servo1.write(rightAngle);
    // delay(1500);
    // servo1.write(centerAngle);
    delay(3000);
    //srvAnimator.SetKeyFrames(srvKeyFrames, array_length(srvKeyFrames));

    //srvAnimator.SetKeyFrame(srvKeyFrames);
    //TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    //TCCR2A = _BV(COM2A1)| _BV(COM2B1) | _BV(WGM20);
    TCCR2A = _BV(COM2A1)| _BV(WGM20);
    TCCR2B = _BV(CS20);

    
    //srvAnimator.SetKeyFrames(srvKeyFrames, 2);
    srvAnimator.Finished += OnKeyFrameFinished;
    //ledAnimator.Finished += OnLedKeyFrameFinished;
    //srvAnimator.SetKeyFrame(keyFrames[fctr]);
    int read = servo1.readMicroseconds();
    srvAnimator.Start(read);

    ledAnimator.SetKeyFrames(ledKeyFrames_Boot, array_length(ledKeyFrames_Boot));
    ledAnimator.Start(0);

    isPlay = false;
    //playStart();
    //play();
    
    Serial.println("setup end.");
    Serial.println(read);
}
int tmp = 0;
unsigned int prevTime = 0;
void loop() {
    // put your main code here, to run repeatedly:
    // int us;
    now = millis();
    if (prevTime > now) {
        Serial.print("timer reset!!!!!(");
        Serial.println(")");
        srvAnimator.Update(now);
        ledAnimator.Update(now);
        OnKeyFrameFinished(NULL, (EventArgs*)(NULL));
        prevTime = now;
        return;
    }

    srvAnimator.Update(now);
    ledAnimator.Update(now);
    playOneImp();

    //if (srvAnimator.GetStartValue() != srvAnimator.GetEndValue()) {
    //if (!isPlay) {
        //Serial.println(srvAnimator.GetValue());
    int v = srvAnimator.GetValue();
    if(minPulse <= v && v <= maxPulse)
    {
        if (!isPlay) {
            servo1.writeMicroseconds(srvAnimator.GetValue());
        }
    }else{
        Serial.print("illegal pulse width!!!!! : ");
        Serial.println(v);
    }
        
        
        //Serial.println(srvAnimator.GetValue());
    //}
    
    analogWrite(ledPin, ledAnimator.GetValue());
    

    prevTime = now;
}
