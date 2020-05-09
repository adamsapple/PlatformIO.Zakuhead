#include <Arduino.h>
#include <Servo.h>
#include <avr/pgmspace.h>

#include <voice.h>
#include <keyframe.h>
#include <util.h>
#include "defines.h"

unsigned int  now;

KeyFrame srvKeyFrames[] = {
    {0, FRAME_NONE, centerAngle, false}, {1000, FRAME_NONE, 0, true},
};

int prevLedPattern = -1;
unsigned int prevTime = 0;

Servo servo1;
KeyFrameAnimator srvAnimator;
KeyFrameAnimator ledAnimator;


void OnKeyFrameFinished(void* sender, EventArgs* e);
void OnLedKeyFrameFinished(void* sender, EventArgs* e);


//void play() {
//  for (int i = 0; i < zaku8k_raw_len; i++) {
//    OCR2B = pgm_read_byte_near(&zaku8k_raw[i]);
//    delayMicroseconds(125);
//  }
//} 

void play() {
    pinMode(voicePin, OUTPUT);
    for (int i = 0; i < zaku8k_raw_len; i++) {
        OCR2A = pgm_read_byte_near(&zaku8k_raw[i]);
        delayMicroseconds(95);
    }
    pinMode(voicePin, INPUT);
    OCR2A = 0;
} 

int vi;
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

/**
 * non-blocking play.
 */
void playOneImp() {
    if(!isPlay){
        return;
    }
    if(vi >= zaku8k_raw_len) {
        //srvAnimator.Start(servo1.readMicroseconds());
        //OnKeyFrameFinished(NULL, (EventArgs*)(NULL));
        isPlay = false;
        OCR2A = 0;
        // pinMode(voicePin, INPUT);
        servo1.attach(servoPin, minPulse, maxPulse);
        return;
    }
    OCR2A = pgm_read_byte_near(&zaku8k_raw[vi++]);
    delayMicroseconds(70);
}

/**
 * サーボのアニメーション終了Ev
 * 新しいアニメーションを作成し、設定する.
 */
void OnKeyFrameFinished(void* sender, EventArgs* e)
{
    // 次の角度ど時間を決定
    auto span  = (unsigned int)random(1000, 3000);
    auto angle = random(leftAngle, rightAngle);
    auto frz   = (random(0, 8) == 0);

    auto &frame = srvKeyFrames[0];
    // アニメーション割当
    frame.IsFreeze   = frz;
    frame.TimeSpanMS = span;
    frame.DestValue  = map(angle, minAngle, maxAngle, minPulse, maxPulse);
    srvAnimator.SetKeyFrame(frame);

    Serial.print("ctr: ");
    Serial.print(now);
    Serial.print(",  srv update(");
    Serial.print("span: ");     Serial.print(frame.TimeSpanMS);
    Serial.print(", angle: ");  Serial.print(angle);
    Serial.print(", pulse: ");  Serial.print(frame.DestValue);
    Serial.print(", freeze: "); Serial.print(frame.IsFreeze);
    Serial.println(")");

    // LEDのアニメーションも更新する
    OnLedKeyFrameFinished(NULL, (EventArgs*)(NULL));
}

/**
 * Led Animation.
 * 
 */ 
void OnLedKeyFrameFinished(void* sender, EventArgs* e)
{
    ////
    /// Led FSM.
    //
    int next = random(LED_BOOT, LED_PATTERN_NUM);

    if(prevLedPattern == LED_BOOT && next == LED_BOOT) {
        next = LED_KEEP;
    }else
    if(prevLedPattern != LED_OFF && next == LED_BOOT) {
        next = LED_BLINK;
    }else
    if(prevLedPattern == LED_OFF && next == LED_BLINK) {
        next = LED_BOOT;
    }

    if(prevLedPattern == -1){
        next = LED_BOOT;
    }
    
    if(isPlay){
        next = LED_KEEP;
    }

    if(next == LED_KEEP){
        return;
    }
    //a = 1;
    //Serial.print("led pattern: ");
    //Serial.println(a);

    KeyFrame* pKeyFrames  = NULL;
    int       len = 0;
    
    switch(next)
    {
        case LED_BOOT :   pKeyFrames = ledKeyFrames_Boot;      len = array_length(ledKeyFrames_Boot); playStart(); break;
        case LED_BLINK:   pKeyFrames = ledKeyFrames_LED_BLINK; len = array_length(ledKeyFrames_LED_BLINK); break;
        case LED_KEEP :   pKeyFrames = ledKeyFrames_KEEP;      len = array_length(ledKeyFrames_KEEP);      break;
        case LED_OFF  :   pKeyFrames = ledKeyFrames_OFF;       len = array_length(ledKeyFrames_OFF);       break;
    }

    if(len != 0){
        prevLedPattern = next;
        ledAnimator.SetKeyFrames(pKeyFrames, len);
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
    pinMode(spkEnablePin, OUTPUT);
    
    servo1.attach(servoPin, minPulse, maxPulse);
    servo1.write(centerAngle);
    
    delay(3000);
    TCCR2A = _BV(COM2A1)| _BV(WGM20);
    TCCR2B = _BV(CS20);
    
    srvAnimator.Finished += OnKeyFrameFinished;
    srvAnimator.Start(servo1.readMicroseconds());

    ledAnimator.SetKeyFrames(ledKeyFrames_Boot, array_length(ledKeyFrames_Boot));
    ledAnimator.Start(0);

    digitalWrite(spkEnablePin, 1);

    isPlay = false;
    
    Serial.println("setup end.");
}

void loop() {
    // put your main code here, to run repeatedly:
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

    ////
    /// Animation.
    //
    srvAnimator.Update(now);
    ledAnimator.Update(now);
    playOneImp();
    
    ////
    /// update Devices.
    //
    int sv = srvAnimator.GetValue();
    //Serial.print("tservo:"); Serial.println(v);
    if(minPulse <= sv && sv <= maxPulse)
    {
        if (!isPlay) {
            servo1.writeMicroseconds(sv);
        }
    }else{
        Serial.print("illegal pulse width!!!!! : ");
        Serial.println(sv);
    }
    
    int lv = ledAnimator.GetValue();
    analogWrite(ledPin, lv);
    
    prevTime = now;
}
